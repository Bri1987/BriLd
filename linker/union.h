#ifndef BRILINKER_UNION_H
#define BRILINKER_UNION_H

#include "utils.h"
#include "file.h"
#include "merge.h"

typedef struct ObjectFile_ ObjectFile;
typedef struct InputSection_ InputSection;
typedef struct InputFile_ InputFile;

typedef struct Symbol_{
    ObjectFile *file;
    char* name;
    uint64_t value;
    int32_t symIdx;

    //union
    InputSection * inputSection;
    SectionFragment *sectionFragment;
}Symbol;


struct ObjectFile_{
    InputFile *inputFile;    //这样表示继承
    Shdr *SymtabSec;
    InputSection ** Sections;
    int64_t isecNum;
    uint32_t* SymtabShndxSec;    //
    MergeableSection **mergeableSections;
    size_t mergeableSectionsNum;
};

struct InputSection_{
    ObjectFile *objectFile;
    char* contents;
    uint32_t shndx;    //在section header中的下标值
    uint32_t shsize;
    bool isAlive;      //看看这个inputsection是否放到最终可执行文件中
    uint8_t P2Align;
};

struct InputFile_{
    File *file;
    //TODO check数组 ElfSyms 是一个 Sym 结构体的数组
    Shdr* ElfSections;
    int64_t sectionNum;

    char* ShStrtab;
    int64_t FirstGlobal;

    Sym *ElfSyms;
    int64_t symNum;

    char* SymbolStrtab;
    bool isAlive;

    //TODO 好怪
    Symbol* LocalSymbols;    //只对内部可见
    int64_t numLocalSymbols;

    Symbol** Symbols;       //可能und等全部symbol，会指向别处
    int64_t numSymbols;

};

ObjectFile *NewObjectFile(File* file,bool isAlive);
void Parse(Context *ctx,ObjectFile* o);
void FillUpSymtabShndxSec(ObjectFile* o,Shdr* s);
void InitializeSections(ObjectFile* o);
void InitializeSymbols(Context *ctx,ObjectFile* o);
void InitializeMergeableSections(ObjectFile * o,Context* ctx);
MergeableSection *splitSection(Context* ctx,InputSection* isec);
int64_t GetShndx(ObjectFile* o, Sym* esym, int idx);
void ResolveSymbols(ObjectFile* o);
//typedef void (*FeederFunc)(ObjectFile*);
void markLiveObjs(ObjectFile* o,ObjectFile*** roots,int *rootSize);
void ClearSymbols(ObjectFile* o);
void registerSectionPieces(ObjectFile* o);

//-----------------------
void AddObjectFile(ObjectFile*** Objs, int* ObjsCount, ObjectFile* newObj);
//----------------inputsection
InputSection *NewInputSection(ObjectFile* file,uint32_t shndx);
Shdr *shdr_(InputSection* i);
char* Name(InputSection* inputSection);

Symbol *NewSymbol(char* name);
Symbol *GetSymbolByName(Context* ctx,char* name);
void SetInputSection(Symbol *s,InputSection* isec);
void SetSectionFragment(Symbol* s,SectionFragment* frag);
Sym *ElfSym_(Symbol* s);
void clear(Symbol* s);

InputFile* NewInputFile(File* file);
char* GetBytesFromIdx(InputFile* inputFile, int64_t idx);
char* GetBytesFromShdr(InputFile* inputFile, Shdr* shdr);
Shdr* FindSection(InputFile* f, uint32_t ty);
void FillUpElfSyms(InputFile* inputFile,Shdr* s);

#endif //BRILINKER_UNION_H
