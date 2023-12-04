#ifndef BRILINKER_INPUT_H
#define BRILINKER_INPUT_H

#include "util.h"
#include "objectfile.h"
#include "context.h"
#include "archive.h"

ObjectFile *CreateObjectFile(File* file);
void readFile(Context *ctx,File* file);
void ReadInputFiles(Context* ctx,char** remaining);

#endif //BRILINKER_INPUT_H
