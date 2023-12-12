#include "passes.h"

ObjectFile** RemoveIf(ObjectFile** elems, int* count) {
    int num = *count;
    size_t i = 0;
    for (size_t j = 0; j < num; j++) {
        if (!elems[j]->inputFile->isAlive) {
            (*count)--;
            continue;
        }
        elems[i] = elems[j];
        i++;
    }

    return elems;
}

void ResolveSymbols_pass(Context* ctx){
    for(int i=0;i<ctx->ObjsCount;i++){
        ObjectFile *objectFile = ctx->Objs[i];
        ResolveSymbols(objectFile);
    }

    MarkLiveObjects(ctx);

    for(int i=0;i<ctx->ObjsCount;i++){
        ObjectFile *objectFile = ctx->Objs[i];
        if(!objectFile->inputFile->isAlive)
            ClearSymbols(objectFile);
    }

    ctx->Objs = RemoveIf(ctx->Objs,&ctx->ObjsCount);
}

void MarkLiveObjects(Context* ctx) {
    ObjectFile **roots = NULL;
    int rootSize = 0;
    for (int i = 0; i < ctx->ObjsCount; i++) {
        if (ctx->Objs[i]->inputFile->isAlive) {
            roots = realloc(roots, (rootSize + 1) * sizeof(ObjectFile*));
            roots[rootSize] = ctx->Objs[i];
            rootSize++;
        }
    }

    int num = 0;
    while (num < rootSize) {
        ObjectFile *file = roots[num];
        if (!file->inputFile->isAlive)
            continue;

        markLiveObjs(file, &roots, &rootSize);
        num++;
    }
}


void RegisterSectionPieces(Context* ctx){
    for (int i = 0; i < ctx->ObjsCount; i++) {
        ObjectFile *file = ctx->Objs[i];
        registerSectionPieces(file);
    }
}

uint64_t SetOutputSectionOffsets(Context* ctx){
    uint64_t addr = 0x200000;
    for(int i=0; i< ctx->chunkNum;i++){
        Chunk *chunk = ctx->chunk[i];
        if((GetShdr(chunk)->Flags & SHF_ALLOC) == 0)
            continue;

        addr = AlignTo(addr,GetShdr(chunk)->AddrAlign);
        GetShdr(chunk)->Addr = addr;

        if(!isTbss(chunk)){
            addr += GetShdr(chunk)->Size;
        }
    }

    size_t i = 0;
    Chunk *first = ctx->chunk[0];

    while (1) {
        Shdr* shdr = GetShdr(ctx->chunk[i]);
        shdr->Offset = shdr->Addr - GetShdr(first)->Addr;
        i++;

        if (i >= ctx->chunkNum ||
                ((GetShdr(ctx->chunk[i])->Flags & SHF_ALLOC) == 0)) {
            break;
        }
    }

    Shdr *lastShdr = GetShdr(ctx->chunk[i-1]);
    uint64_t fileoff = lastShdr->Offset + lastShdr->Size;

    for(; i< ctx->chunkNum;i++){
        fileoff = AlignTo(fileoff,ctx->chunk[i]->shdr.AddrAlign);
        GetShdr(ctx->chunk[i])->Offset = fileoff;
        fileoff += ctx->chunk[i]->shdr.Size;
    }
    return fileoff;
}

void CreateSyntheticSections(Context* ctx){
    struct OutputEhdr_* outputEhdr = NewOutputEhdr();
    ctx->ehdr = outputEhdr;
    ctx->chunk = realloc(ctx->chunk,sizeof (Chunk*) * (ctx->chunkNum+1));
    ctx->chunk[ctx->chunkNum] = outputEhdr->chunk;
    ctx->chunkNum++;

    struct OutputShdr_* outputShdr = NewOutputShdr();
    ctx->shdr = outputShdr;
    ctx->chunk = realloc(ctx->chunk,sizeof (Chunk*) * (ctx->chunkNum+1));
    ctx->chunk[ctx->chunkNum] = outputShdr->chunk;
    ctx->chunkNum++;
}

//members填充
void BinSections(Context* ctx){
    InputSection ***group = (InputSection***) malloc(sizeof (InputSection**) * ctx->outputSecNum);
    // 初始化 group 数组的每个元素为 NULL
    for (size_t i = 0; i < ctx->outputSecNum; i++) {
        group[i] = NULL;
    }

    for(int i=0;i<ctx->ObjsCount;i++){
        ObjectFile *file = ctx->Objs[i];
        for(int j=0; j< file->isecNum;j++){
            InputSection *isec = file->Sections[j];
            if(isec == NULL || !isec->isAlive){
                continue;
            }

            int64_t idx = isec->outputSection->chunk->outpuSec.idx;
            size_t prevSize = 0;
            if (group[idx] != NULL) {
                while (group[idx][prevSize] != NULL) {
                    prevSize++;
                }
            }

            // 分配新的 group[idx] 大小
            //TODO +2
            group[idx] = (InputSection**)realloc(group[idx], (prevSize + 2) * sizeof(InputSection*));
            // 将 isec 添加到 group[idx] 的末尾
            group[idx][prevSize] = isec;
            group[idx][prevSize + 1] = NULL;
        }
    }

    for(int idx=0;idx< ctx->outputSecNum;idx++){
        OutputSection *osec = ctx->outputSections[idx];
        osec->chunk->outpuSec.members = group[idx];
        int len = 0;
        if(group[idx] != NULL){
            while (group[idx][len] != NULL) {
                len++;
            }
            osec->chunk->outpuSec.memberNum = len;
        }
    }
}

void CollectOutputSections(Context* ctx){
    for(int i =0; i< ctx->outputSecNum;i++){
        OutputSection * osec = ctx->outputSections[i];
        if(osec->chunk->outpuSec.memberNum > 0){
            ctx->chunk = realloc(ctx->chunk,sizeof (Chunk*) * (ctx->chunkNum + 1));
            ctx->chunk[ctx->chunkNum] = osec->chunk;
            ctx->chunkNum++;
        }
    }
}

void ComputeSectionSizes(Context* ctx){
    for(int i =0; i< ctx->outputSecNum;i++) {
        OutputSection *osec = ctx->outputSections[i];
        uint64_t offset = 0;
        int64_t p2align = 0;

        for(int j=0; j<osec->chunk->outpuSec.memberNum; j++){
            InputSection *isec = osec->chunk->outpuSec.members[j];
            offset = AlignTo(offset, 1<<isec->P2Align);
            isec->offset = offset;
            offset += isec->shsize;
            p2align = max(p2align,isec->P2Align);
        }

        osec->chunk->shdr.Size = offset;
        osec->chunk->shdr.AddrAlign = 1 << p2align;
    }
}

int b2i(bool b){
    if(b)
        return 1;
    return 0;
}

void getRank(Chunk *chunk,Context* ctx){
    uint32_t typ = GetShdr(chunk)->Type;
    uint32_t flags = GetShdr(chunk)->Flags;
    if((flags & SHF_ALLOC) == 0)
        chunk->rank = INT32_MAX - 1;
    else if(chunk == ctx->shdr->chunk)
        chunk->rank = INT32_MAX;
    else if(chunk == ctx->ehdr->chunk)
        chunk->rank = 0;
    else if(typ == SHT_NOTE)
        chunk->rank = 2;
    else {
        int writeable = b2i((flags & SHF_WRITE) != 0);
        int notExec = b2i((flags & SHF_EXECINSTR) == 0);
        int notTls = b2i((flags & SHF_TLS) == 0);
        int isBss = b2i(typ == SHT_NOBITS);

        chunk->rank = writeable << 7 | notExec << 6 | notTls << 5 | isBss << 4;
    }
    //printf("name %s , rank %d\n",chunk->name,chunk->rank);
}

void SortOutputSections(Context* ctx){
    for(int i=0;i<ctx->chunkNum;i++){
        getRank(ctx->chunk[i],ctx);
    }

    //qsort(ctx->chunk, ctx->chunkNum, sizeof(Chunk), compareChunks);
    for (size_t i = 0; i < ctx->chunkNum - 1; i++) {
        for (size_t j = 0; j < ctx->chunkNum - i - 1; j++) {
            // 比较相邻的两个元素的 rank 值
            if (ctx->chunk[j]->rank > ctx->chunk[j + 1]->rank) {
                // 交换两个元素的位置
                Chunk *temp = ctx->chunk[j];
                ctx->chunk[j] = ctx->chunk[j + 1];
                ctx->chunk[j + 1] = temp;
            }
        }
    }
}

bool isTbss(Chunk* chunk){
    Shdr *shdr = GetShdr(chunk);
    return (shdr->Type == SHT_NOBITS) && ((shdr->Flags & SHF_TLS) !=0);
}