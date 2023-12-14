#ifndef BRILINKER_FILE_H
#define BRILINKER_FILE_H

#include "utils.h"
#include "context.h"

typedef struct File {
    char* Name;
    char* Contents;
    uint64_t contents_len;   //由于有'\0'，添加这个
    struct File* Parent;     //表示它来自于哪个.a文件 , 如果直接.o就是空
} File;

File* NewFile(const char* name);
File* OpenLibrary(const char* filepath);
File* FindLibrary(Context* ctx, const char* name);

#endif //BRILINKER_FILE_H
