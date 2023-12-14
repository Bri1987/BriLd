#include "chunk.h"

OutputPhdr *NewOutputPhdr(){
    OutputPhdr *outputPhdr = (OutputPhdr*) malloc(sizeof (OutputPhdr));
    outputPhdr->chunk = NewChunk();
    outputPhdr->chunk->shdr.Flags = SHF_ALLOC;
    outputPhdr->chunk->shdr.AddrAlign = 8;
    outputPhdr->chunk->chunkType = ChunkTypePhdr;
    return outputPhdr;
}

bool isTls(Chunk* c){
    return (GetShdr(c)->Flags & SHF_TLS) != 0;
}

bool isBss(Chunk* c){
    return GetShdr(c)->Type == SHT_NOBITS && !isTls(c);
}

bool isNote(Chunk* c){
    Shdr *shdr = GetShdr(c);
    return (shdr->Type == SHT_NOTE) && ((shdr->Flags & SHF_ALLOC) != 0);
}

bool isTbss_(Chunk* chunk){
    Shdr *shdr = GetShdr(chunk);
    return (shdr->Type == SHT_NOBITS) && ((shdr->Flags & SHF_TLS) !=0);
}

uint32_t toPhdrFlags(Chunk* c){
    uint32_t ret = PF_R;
    int write = (GetShdr(c)->Flags & SHF_WRITE) != 0;
    if(write)
        ret |= PF_W;
    if((GetShdr(c)->Flags & SHF_EXECINSTR) != 0)
        ret |= PF_X;
    return ret;
}

Phdr* define(uint64_t typ,uint64_t flags,int64_t minAlign, Chunk* c){
    Phdr *phdr = (Phdr*) malloc(sizeof (Phdr));
    phdr->Type = typ;
    phdr->Flags = flags;
    phdr->Align = max_(minAlign,GetShdr(c)->AddrAlign);
    phdr->Offset = GetShdr(c)->Offset;
    if(GetShdr(c)->Type == SHT_NOBITS)
        phdr->FileSize = 0;
    else
        phdr->FileSize = GetShdr(c)->Size;
    phdr->VAddr = GetShdr(c)->Addr;
    phdr->PAddr = GetShdr(c)->Addr;
    phdr->MemSize = GetShdr(c)->Size;
    return phdr;
}

void push(Chunk* c,Phdr* vec,int lenVec){
    Phdr *phdr = &vec[lenVec - 1];
    phdr->Align = max_(phdr->Align,GetShdr(c)->AddrAlign);
    if(c->shdr.Type != SHT_NOBITS){
        phdr->FileSize = GetShdr(c)->Addr + GetShdr(c)->Size - phdr->VAddr;
       // printf("(name) :%s , phdr size %lu\n",c->name,phdr->FileSize);
    }
    phdr->MemSize = GetShdr(c)->Addr + GetShdr(c)->Size - phdr->VAddr;
}

Chunk** Chunk_RemoveIf(Chunk** elems, int* count) {
    int num = *count;
    size_t i = 0;
    for (size_t j = 0; j < num; j++) {
        if (isTbss_(elems[j])) {
            (*count)--;
            continue;
        }
        elems[i] = elems[j];
        i++;
    }

    return elems;
}

Phdr *createPhdr(Context* ctx, int *num){
    Phdr *vec = NULL;

    Phdr *phdr = define(PT_PHDR,PF_R,8,ctx->phdr->chunk);
    vec = realloc(vec,sizeof (Phdr) * (*num + 1));
    vec[*num] = *phdr;
    *num += 1;

    for(int i=0; i<ctx->chunkNum;){
        Chunk *first = ctx->chunk[i];
        i++;
        if(!isNote(first))
            continue;

        uint32_t flags = toPhdrFlags(first);
        uint64_t alignment = first->shdr.AddrAlign;
        Phdr *p1 = define(PT_NOTE,flags,(int64_t )alignment,first);
        vec = realloc(vec,sizeof (Phdr) * (*num + 1));
        vec[*num] = *p1;
        *num += 1;
        while (i < ctx->chunkNum && isNote(ctx->chunk[i]) && toPhdrFlags(ctx->chunk[i]) == flags){
            push(ctx->chunk[i],vec,*num);
            i++;
        }
    }

    {
        Chunk **chunks = (Chunk**) malloc(sizeof (Chunk*) * ctx->chunkNum);
        for(int i=0; i<ctx->chunkNum;i++){
            chunks[i] = ctx->chunk[i];
        }

        int count = ctx->chunkNum;
        chunks = Chunk_RemoveIf(chunks,&count);
        for(int i=0; i<count;){
            Chunk *first = chunks[i];
            i++;

            if((GetShdr(first)->Flags & SHF_ALLOC) == 0)
                break;

            uint32_t flags = toPhdrFlags(first);
            Phdr *p2 = define(PT_LOAD,flags,4096,first);
            vec = realloc(vec,sizeof (Phdr) * (*num + 1));
            vec[*num] = *p2;
            *num += 1;

            if(!isBss(first)){
                while (i < count && !isBss(chunks[i]) && toPhdrFlags(chunks[i]) == flags){
                    push(chunks[i],vec,*num);
                    i++;
                }
            }

            while (i <count && isBss(chunks[i]) && toPhdrFlags(chunks[i]) == flags){
                push(chunks[i],vec,*num);
                i++;
            }
        }
    }

    for(int i=0; i<ctx->chunkNum;i++){
        if(!isTls(ctx->chunk[i]))
            continue;

        Phdr *p3 = define(PT_TLS, toPhdrFlags(ctx->chunk[i]),1,ctx->chunk[i]);
        vec = realloc(vec,sizeof (Phdr) * (*num + 1));
        vec[*num] = *p3;
        *num += 1;
        i++;

        while (i < ctx->chunkNum && isTls(ctx->chunk[i])){
            push(ctx->chunk[i],vec,*num);
            i++;
        }

        Phdr *ph = &vec[*num-1];
        ctx->TpAddr = ph->VAddr;
    }
    return vec;
}

void Phdr_UpdateShdr(Chunk* c,Context* ctx){
    c->phdrS.phdrNum = 0;
    c->phdrS.phdrs = NULL;
    c->phdrS.phdrs = createPhdr(ctx,&c->phdrS.phdrNum);
    c->shdr.Size = sizeof (Phdr)*c->phdrS.phdrNum;
   // printf("size %lu\n",c->shdr.Size);
}

void Phdr_CopyBuf(Chunk* c,Context* ctx){
    //TODO check 这样写对不对
    Write(ctx->buf+c->shdr.Offset,sizeof (Phdr)*c->phdrS.phdrNum,c->phdrS.phdrs);
}