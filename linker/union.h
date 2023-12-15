#ifndef BRILINKER_UNION_H
#define BRILINKER_UNION_H

#include "utils.h"
#include "file.h"
#include "merge.h"

typedef struct ObjectFile_ ObjectFile;
typedef struct InputSection_ InputSection;
typedef struct InputFile_ InputFile;

struct OutputSection_;

typedef struct Symbol_{
    ObjectFile *file;
    char* name;
    uint64_t value;
    int32_t symIdx;

    //union
    InputSection * inputSection;
    SectionFragment *sectionFragment;

    int32_t gotTpIdx;
    uint32_t flags;
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
    struct ObjectFile_ *objectFile;     //来自于某个文件
    char* contents;
    uint32_t shndx;    //在section header数组中的下标值，为了找到它的section header信息
    uint32_t shsize;
    bool isAlive;      //看看这个inputsection是否放到最终可执行文件中
    uint8_t P2Align;    //power to align , 等于log2的addrAlign

    //在outputsection中的偏移
    uint32_t  offset;       //这个inputsection在它对应的outputsection中的偏移
    struct OutputSection_* outputSection;   //记录一下这个input section属于哪个output section

    uint32_t RelsecIdx;
    Rela* rels;
    int relNum;
};

// InputFile 包含obj file或so file, 作为一个基类
// 用于解析elf文件后存储信息用
struct InputFile_{
    File *file;
    //ElfSyms 是一个 Sym 结构体的数组
    Shdr* ElfSections;
    int64_t sectionNum;

    char* ShStrtab;
    int64_t FirstGlobal;

    Sym *ElfSyms;
    int64_t symNum;

    char* SymbolStrtab;
    bool isAlive;

    Symbol* LocalSymbols;    //只对内部可见

    Symbol** Symbols;       //可能und等全部symbol，会指向别处
    int64_t numSymbols;

};

ObjectFile *NewObjectFile(File* file,bool isAlive);
void Parse(Context *ctx,ObjectFile* o);
void FillUpSymtabShndxSec(ObjectFile* o,Shdr* s);
void InitializeSections(ObjectFile* o,Context* ctx);
void InitializeSymbols(Context *ctx,ObjectFile* o);
void InitializeMergeableSections(ObjectFile * o,Context* ctx);
MergeableSection *splitSection(Context* ctx,InputSection* isec);
int64_t GetShndx(ObjectFile* o, Sym* esym, int idx);
void ResolveSymbols(ObjectFile* o);
//typedef void (*FeederFunc)(ObjectFile*);
void markLiveObjs(ObjectFile* o,ObjectFile*** roots,int *rootSize);
void ClearSymbols(ObjectFile* o);
void registerSectionPieces(ObjectFile* o);
void SkipEhframeSections(ObjectFile* o);
void ScanRelocations_(ObjectFile* o);

//-----------------------
void AddObjectFile(ObjectFile*** Objs, int* ObjsCount, ObjectFile* newObj);
//----------------inputsection
InputSection *NewInputSection(Context *ctx,char* name,ObjectFile* file,uint32_t shndx);
Shdr *shdr_(struct InputSection_* i);
char* Name(struct InputSection_* inputSection);
void WriteTo(struct InputSection_ *i,char* buf,Context* ctx);
Rela *GetRels(InputSection* i);
uint64_t InputSec_GetAddr(InputSection* i);
void ScanRelocations__(InputSection* isec);
void ApplyRelocAlloc(InputSection* i,Context* ctx,char* buf);
void writeItype(void* loc, uint32_t val);
void writeStype(void* loc, uint32_t val);
void writeBtype(void* loc, uint32_t val);
void writeUtype(void* loc, uint32_t val);
void writeJtype(void* loc, uint32_t val);
void setRs1(void* loc,uint32_t rs1);

Symbol *NewSymbol(char* name);
Symbol *GetSymbolByName(Context* ctx,char* name);
void SetInputSection(Symbol *s,InputSection* isec);
void SetSectionFragment(Symbol* s,SectionFragment* frag);
Sym *ElfSym_(Symbol* s);
void clear(Symbol* s);
uint64_t Symbol_GetAddr(Symbol* s);
uint64_t GetGotTpAddr(Context* ctx,Symbol* s);

InputFile* NewInputFile(File* file);
char* GetBytesFromIdx(InputFile* inputFile, int64_t idx);
char* GetBytesFromShdr(InputFile* inputFile, Shdr* shdr);
Shdr* FindSection(InputFile* f, uint32_t ty);
void FillUpElfSyms(InputFile* inputFile,Shdr* s);
Ehdr GetEhdr(InputFile* f);

#endif //BRILINKER_UNION_H
