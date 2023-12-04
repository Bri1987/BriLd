#include "context.h"

Context* NewContext(){
    Context* ctx = (Context*)malloc(sizeof(Context));
    if (ctx == NULL) {
        return NULL;
    }

    ctx->Args.Output = "a.out";
    ctx->Args.Emulation = 0;
    ctx->Args.LibraryPathsCount=0;
    return ctx;
}

// 实现追加字符串的函数
void appendLibraryPath(Context* ctx, const char* arg) {
    // 分配内存来存储新字符串
    char* newStr = strdup(arg);

    // 重新分配内存来存储扩展后的数组
    ctx->Args.LibraryPaths = (char**)realloc(ctx->Args.LibraryPaths, (ctx->Args.LibraryPathsCount + 1) * sizeof(char*));

    // 将新字符串添加到数组中
    ctx->Args.LibraryPaths[ctx->Args.LibraryPathsCount] = newStr;

    // 更新数组元素的数量
    ++ctx->Args.LibraryPathsCount;
}