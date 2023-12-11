#include "context.h"

Context* NewContext(){
    Context* ctx = (Context*)malloc(sizeof(Context));
    if (ctx == NULL) {
        return NULL;
    }

    ctx->Args.Output = "a.out";
    ctx->Args.Emulation = 0;
    ctx->Args.LibraryPathsCount=0;
    ctx->SymbolMap = HashMapInit();
    ctx->Args.LibraryPaths = NULL;
    ctx->ObjsCount = 0;
    ctx->Objs = NULL;
    ctx->mergedSectionNum = 0;
    ctx->mergedSections = NULL;

    ctx->chunk = NULL;
    ctx->chunkNum = 0;
    ctx->buf = NULL;
    ctx->ehdr = NULL;
    return ctx;
}

// 实现追加字符串的函数
void appendLibraryPath(Context* ctx, char* arg) {

    ctx->Args.LibraryPaths = (char**)realloc(ctx->Args.LibraryPaths, (ctx->Args.LibraryPathsCount + 1) * sizeof(char*));

    ctx->Args.LibraryPaths[ctx->Args.LibraryPathsCount] = (char*)malloc((strlen(arg) + 1) * sizeof(char));

    strcpy(ctx->Args.LibraryPaths[ctx->Args.LibraryPathsCount], arg);

    ++ctx->Args.LibraryPathsCount;
}