#include "objectfile.h"

ObjectFile *NewObjectFile(File* file){
    ObjectFile *objectFile = (ObjectFile*) malloc(sizeof (ObjectFile));
    objectFile->inputFile = NewInputFile(file);
    return objectFile;
}

void Parse(ObjectFile* o){
    o->SymtabSec = FindSection(o->inputFile,2);  //SHT_SYMTAB
    if(o->SymtabSec != NULL){
        o->inputFile->FirstGlobal = o->SymtabSec->Info;
        FillUpElfSyms(o->inputFile,o->SymtabSec);
        o->inputFile->SymbolStrtab = GetBytesFromIdx(o->inputFile,o->SymtabSec->Link);
    }
}