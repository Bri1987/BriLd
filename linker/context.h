#ifndef BRILINKER_CONTEXT_H
#define BRILINKER_CONTEXT_H

#include "utils.h"
#include "machinetype.h"
//#include "objectfile.h"

//解决一下嵌套包含
struct ObjectFile_;
struct Symbol_;

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

    HashMap *SymbolMap;  //char*,Symbol*
} Context;

Context* NewContext();
void appendLibraryPath(Context* ctx, char* arg);

#endif //BRILINKER_CONTEXT_H
