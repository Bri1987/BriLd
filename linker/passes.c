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

uint64_t GetFileSize(Context* ctx){
    uint64_t fileoff = 0;
    for(int i=0; i< ctx->chunkNum;i++){
        fileoff = AlignTo(fileoff,ctx->chunk[i]->shdr.AddrAlign);
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
}

