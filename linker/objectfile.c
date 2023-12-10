#include "union.h"

extern HashMap *name_map;

ObjectFile *NewObjectFile(File* file,bool isAlive){
    ObjectFile *objectFile = (ObjectFile*) malloc(sizeof (ObjectFile));
    objectFile->inputFile = NewInputFile(file);
    objectFile->inputFile->isAlive = isAlive;
    objectFile->Sections = NULL;
    objectFile->SymtabSec = NULL;
    objectFile->SymtabShndxSec = NULL;
    objectFile->isecNum = 0;
    return objectFile;
}

void Parse(Context *ctx,ObjectFile* o){
    o->SymtabSec = FindSection(o->inputFile,2);  //SHT_SYMTAB
    if(o->SymtabSec != NULL){
        o->inputFile->FirstGlobal = o->SymtabSec->Info;
        FillUpElfSyms(o->inputFile,o->SymtabSec);
        o->inputFile->SymbolStrtab = GetBytesFromIdx(o->inputFile,o->SymtabSec->Link);
    }
    InitializeSections(o);
    InitializeSymbols(ctx,o);
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

//TODO 不造正确性
void FillUpSymtabShndxSec(ObjectFile* o,Shdr* s){
    char* bs = GetBytesFromShdr(o->inputFile,s);
    int num = s->Size / s->EntSize;
    o->SymtabShndxSec = malloc(sizeof(uint32_t)*num);
    uint32_t *t = o->SymtabShndxSec;
    while (num > 0){
        uint32_t *sy = malloc(sizeof (uint32_t));
        Read(sy,bs,sizeof (uint32_t));
        memcpy(t,sy,sizeof (uint32_t));
        bs += sizeof (uint32_t);
        t +=sizeof (uint32_t);
        num--;
    }
}

void InitializeSections(ObjectFile* o){
    o->Sections = (InputSection **)calloc(o->inputFile->sectionNum, sizeof(InputSection *));

    for (int i = 0; i < o->inputFile->sectionNum; ++i) {
        Shdr *shdr = &o->inputFile->ElfSections[i];
        switch (shdr->Type) {
            //不需要放入可执行文件
            case SHT_GROUP:
            case SHT_SYMTAB:
            case SHT_STRTAB:
            case SHT_REL:
            case SHT_RELA:
            case SHT_NULL:
                break;
            case SHT_SYMTAB_SHNDX:
                printf("SHT_SYMTAB_SHNDX\n");
                FillUpSymtabShndxSec(o,shdr);
                break;
            default:
                o->Sections[i] = NewInputSection(o,i);
                break;
        }
    }
}

int64_t GetShndx(ObjectFile* o, Sym* esym, int idx) {
    // 假设你有一个类似的 Assert 函数用于检查条件
    assert(idx >= 0 && idx < o->inputFile->symNum );

    // 如果 esym 的 Shndx 是 SHN_XINDEX，则返回 SymtabShndxSec 中对应索引的值
    if (esym->Shndx == SHN_XINDEX) {
        return o->SymtabShndxSec[idx];
    }

    // 否则，返回 esym 的 Shndx
    return esym->Shndx;
}

void InitializeSymbols(Context *ctx,ObjectFile* o){
    if(o->SymtabSec == NULL)
        return;

    o->inputFile->LocalSymbols = (Symbol*) malloc(sizeof (Symbol)*o->inputFile->FirstGlobal);
    for(int i=0; i< o->inputFile->FirstGlobal;i++){
        o->inputFile->LocalSymbols[i]= *NewSymbol("");
    }
    o->inputFile->LocalSymbols[0].file = o;
    for(int i=1; i< o->inputFile->FirstGlobal;i++){
        Sym* esym = &o->inputFile->ElfSyms[i];
        Symbol *sym = &o->inputFile->LocalSymbols[i];
        sym->name  = ElfGetName(o->inputFile->SymbolStrtab,esym->Name);
        sym->file = o;
        sym->value = esym->Val;
        sym->symIdx = i;

        if(!IsAbs(esym))
            SetInputSection(sym,o->Sections[GetShndx(o,esym,i)]);
    }

    o->inputFile->Symbols = (Symbol **)calloc(o->inputFile->symNum, sizeof(Symbol *));
    for (int i = 0; i < o->inputFile->symNum; ++i) {
        o->inputFile->Symbols[i] = (Symbol*) malloc(sizeof (Symbol));
    }
    for(int i=0;i<o->inputFile->FirstGlobal;i++){
        o->inputFile->Symbols[i] = &o->inputFile->LocalSymbols[i];
    }
    for(int i=o->inputFile->FirstGlobal;i<o->inputFile->symNum;i++){
        Sym* esym = &o->inputFile->ElfSyms[i];
        char* name = ElfGetName(o->inputFile->SymbolStrtab,esym->Name);
        o->inputFile->Symbols[i] = GetSymbolByName(ctx,name);
    }
}


InputSection *GetSection(ObjectFile* o,Sym* esym,int idx){
    return o->Sections[GetShndx(o,esym,idx)];
}

void ResolveSymbols(ObjectFile* o){
    for(int i=o->inputFile->FirstGlobal;i<o->inputFile->symNum;i++){
        Sym* esym = &o->inputFile->ElfSyms[i];
        Symbol *sym = o->inputFile->Symbols[i];

        if(IsUndef(esym)){
            continue;
        }

        InputSection *isec = NULL;
        if(!IsAbs(esym)){
            isec = GetSection(o,esym,i);
            if(isec == NULL)
                continue;
        }
        if(sym->file == NULL){
            sym->file = o;
            SetInputSection(sym,isec);
            sym->value = esym->Val;
            sym->symIdx = i;
        }
    }
}

void markLiveObjs(ObjectFile* o,ObjectFile***roots,int *rootSize){
    assert(o->inputFile->isAlive);
    for(int i=o->inputFile->FirstGlobal;i<o->inputFile->symNum;i++){
        Symbol *sym = o->inputFile->Symbols[i];
        Sym *esym = &o->inputFile->ElfSyms[i];

        if(sym->file == NULL){
            continue;
        }

        if(IsUndef(esym) && !sym->file->inputFile->isAlive){
            sym->file->inputFile->isAlive = true;
            *roots = realloc(*roots, (*rootSize + 1) * sizeof(ObjectFile*));
            (*roots)[*rootSize] = sym->file;
            *rootSize += 1;
        }
    }
}

void ClearSymbols(ObjectFile* o){
    for(int i=o->inputFile->FirstGlobal;i < o->inputFile->symNum;i++){
        Symbol *sym = o->inputFile->Symbols[i];
        if(sym->file == o)
            clear(sym);
    }
}