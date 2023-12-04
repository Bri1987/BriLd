#ifndef BRILINKER_OBJECTFILE_H
#define BRILINKER_OBJECTFILE_H

#include "util.h"
#include "inputfile.h"

typedef struct ObjectFile_{
    InputFile *inputFile;    //这样表示继承
    Shdr *SymtabSec;
}ObjectFile;

ObjectFile *NewObjectFile(File* file);
void Parse(ObjectFile* o);
#endif //BRILINKER_OBJECTFILE_H
