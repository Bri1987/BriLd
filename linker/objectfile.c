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

// 添加 ObjectFile 到 Objs 数组
void AddObjectFile(ObjectFile*** Objs, int* ObjsCount, ObjectFile* newObj) {
    (*ObjsCount)++; // 增加数组元素个数

    // 动态调整 Objs 数组的大小
    *Objs = (ObjectFile**)realloc(*Objs, (*ObjsCount) * sizeof(ObjectFile*));

    if (*Objs != NULL) {
        // 将新的 ObjectFile 指针添加到数组
        (*Objs)[(*ObjsCount) - 1] = newObj;
    } else {
        // 处理内存分配错误
        printf("Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
}