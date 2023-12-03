#ifndef BRILINKER_FILE_H
#define BRILINKER_FILE_H

#include "util.h"

typedef struct File {
    char* Name;
    char* Contents;
    struct File* Parent;
} File;

File* NewFile(const char* name);

#endif //BRILINKER_FILE_H
