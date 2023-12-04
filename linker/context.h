#ifndef BRILINKER_CONTEXT_H
#define BRILINKER_CONTEXT_H

#include "util.h"
#include "machinetype.h"

typedef struct {
    char* Output;
    MachineType Emulation;
    char** LibraryPaths;
    int LibraryPathsCount;
} ContextArgs;

typedef struct {
    ContextArgs Args;
} Context;

Context* NewContext();
void appendLibraryPath(Context* ctx, const char* arg);

#endif //BRILINKER_CONTEXT_H
