#ifndef BRILINKER_INPUT_H
#define BRILINKER_INPUT_H

#include "utils.h"
#include "union.h"
#include "context.h"
#include "archive.h"

ObjectFile *CreateObjectFile(Context *ctx,File* file,bool inLib);
void readFile(Context *ctx,File* file);
void ReadInputFiles(Context* ctx,char** remaining);

#endif //BRILINKER_INPUT_H
