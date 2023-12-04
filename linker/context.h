#ifndef BRILINKER_CONTEXT_H
#define BRILINKER_CONTEXT_H

#include "util.h"
#include "machinetype.h"
//#include "objectfile.h"

//解决一下嵌套包含
struct ObjectFile_;

typedef struct {
    char* Output;
    MachineType Emulation;
    char** LibraryPaths;
    int LibraryPathsCount;
} ContextArgs;

typedef struct {
    ContextArgs Args;

    struct ObjectFile_** Objs;
    int ObjsCount;

} Context;

Context* NewContext();
void appendLibraryPath(Context* ctx, const char* arg);

#endif //BRILINKER_CONTEXT_H
