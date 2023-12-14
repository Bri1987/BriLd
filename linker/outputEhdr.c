#include "chunk.h"

OutputEhdr *NewOutputEhdr(){
    OutputEhdr *outputEhdr = (OutputEhdr*) malloc(sizeof (OutputEhdr));
    outputEhdr->chunk = NewChunk();
    outputEhdr->chunk->shdr.Flags = SHF_ALLOC;
    outputEhdr->chunk->shdr.Size = sizeof (Ehdr);
    outputEhdr->chunk->shdr.AddrAlign = 8;
    outputEhdr->chunk->chunkType = ChunkTypeEhdr;
    return outputEhdr;
}

// 找到第一条执行代码地址
uint64_t getEntryAddr(Context* ctx){
    for(int i=0; i<ctx->outputSecNum;i++){
        OutputSection *osec = ctx->outputSections[i];
        if(strcmp(osec->chunk->name,".text")==0){
            return osec->chunk->shdr.Addr;
        }
    }
    return 0;
}

uint32_t getFlags(Context* ctx){
    assert(ctx->ObjsCount > 0);
    uint32_t flags = GetEhdr(ctx->Objs[0]->inputFile).Flags;
    for(int i=1; i< ctx->ObjsCount;i++){
        ObjectFile *obj = ctx->Objs[i];
        if((GetEhdr(obj->inputFile).Flags & 1/*EF_RISCV_RVC*/) !=0){
            flags |= 1/*EF_RISCV_RVC*/;
            break;
        }
    }

    return flags;
}

void Ehdr_CopyBuf(Chunk *c,Context* ctx){
    Ehdr *ehdr = (Ehdr*) malloc(sizeof (Ehdr));
    WriteMagic(ehdr->Ident);
    ehdr->Ident[EI_CLASS] = ELFCLASS64;
    ehdr->Ident[EI_DATA] = ELFDATA2LSB;
    ehdr->Ident[EI_VERSION] = EV_CURRENT;
    ehdr->Ident[EI_OSABI] = 0;
    ehdr->Ident[EI_ABIVERSION] = 0;
    ehdr->Type = ET_EXEC;
    ehdr->Machine = 243;      //EM_RISCV
    ehdr->Version = EV_CURRENT;
    ehdr->Entry = getEntryAddr(ctx);
    ehdr->ShOff = ctx->shdr->chunk->shdr.Offset;
    ehdr->PhOff = ctx->phdr->chunk->shdr.Offset;
    ehdr->Flags = getFlags(ctx);
    ehdr->EhSize = sizeof (Ehdr);
    ehdr->PhEntSize = sizeof (Phdr);
    ehdr->PhNum = ctx->phdr->chunk->shdr.Size / sizeof (Phdr);
    ehdr->ShEntSize = sizeof(Shdr);
    ehdr->ShNum = ctx->shdr->chunk->shdr.Size / sizeof (Shdr);
    ehdr->ShStrndx = 0;

    char* buf = malloc(sizeof (Ehdr));
    memcpy(buf,ehdr,sizeof (Ehdr));
   // printf("ehdr offset %lu\n",c->shdr.Offset);
    memcpy(ctx->buf+c->shdr.Offset,buf,sizeof (Ehdr));
}