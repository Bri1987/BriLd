#include "union.h"

ObjectFile *NewObjectFile(File* file,bool isAlive){
    ObjectFile *objectFile = (ObjectFile*) malloc(sizeof (ObjectFile));
    objectFile->inputFile = NewInputFile(file);
    objectFile->inputFile->isAlive = isAlive;
    objectFile->Sections = NULL;
    objectFile->SymtabSec = NULL;
    objectFile->SymtabShndxSec = NULL;
    objectFile->isecNum = 0;
    objectFile->mergeableSections = NULL;
    objectFile->mergeableSectionsNum = 0;
    return objectFile;
}

//解析目标文件
void Parse(Context *ctx,ObjectFile* o){
    o->SymtabSec = FindSection(o->inputFile,2);  //SHT_SYMTAB
    if(o->SymtabSec != NULL){
        o->inputFile->FirstGlobal = o->SymtabSec->Info;
        FillUpElfSyms(o->inputFile,o->SymtabSec);
        o->inputFile->SymbolStrtab = GetBytesFromIdx(o->inputFile,o->SymtabSec->Link);
    }
    InitializeSections(o,ctx);
    InitializeSymbols(ctx,o);
    InitializeMergeableSections(o,ctx);
    SkipEhframeSections(o);
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

void InitializeSections(ObjectFile* o,Context* ctx){
    o->Sections = (InputSection **)calloc(o->inputFile->sectionNum, sizeof(InputSection *));
    o->isecNum = o->inputFile->sectionNum;

    for (int i = 0; i < o->inputFile->sectionNum; ++i) {
        Shdr *shdr = &o->inputFile->ElfSections[i];
        char* name = NULL;
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
                name = ElfGetName(o->inputFile->ShStrtab,shdr->Name);
                o->Sections[i] = NewInputSection(ctx,name,o,i);
                break;
        }
    }

    //处理重定向的section
    for(int i=0; i< o->inputFile->sectionNum;i++){
        Shdr *shdr = &o->inputFile->ElfSections[i];
        if(shdr->Type != SHT_RELA)
            continue;

        assert(shdr->Info < o->isecNum);
        InputSection *target = o->Sections[shdr->Info];
        if(target != NULL){
            //没有两个relocation指向同一个section的情况
            //重定向section的section header的info指向 重定位所适用的节区的节区头部索引，比如.rel.text指向.text
            assert(target->RelsecIdx == UINT32_MAX);
            target->RelsecIdx = i;
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

        //绝对符号没有对应的inputSection
        if(!IsAbs(esym))
            SetInputSection(sym,o->Sections[GetShndx(o,esym,i)]);
    }

    o->inputFile->Symbols = (Symbol **)calloc(o->inputFile->symNum, sizeof(Symbol *));
    for (int i = 0; i < o->inputFile->symNum; ++i) {
        o->inputFile->Symbols[i] = (Symbol*) malloc(sizeof (Symbol));
        o->inputFile->numSymbols++;
    }
    ////填充上所有localSym
    for(int i=0;i<o->inputFile->FirstGlobal;i++){
        o->inputFile->Symbols[i] = &o->inputFile->LocalSymbols[i];
    }
    //填充其他非local的symbols , 在初始化阶段填入的值还是默认初值
    for(int i=o->inputFile->FirstGlobal;i<o->inputFile->symNum;i++){
        Sym* esym = &o->inputFile->ElfSyms[i];
        char* name = ElfGetName(o->inputFile->SymbolStrtab,esym->Name);
        o->inputFile->Symbols[i] = GetSymbolByName(ctx,name);
    }
}

void InitializeMergeableSections(ObjectFile * o,Context* ctx){
    o->mergeableSectionsNum = o->isecNum;
    o->mergeableSections = (MergeableSection**)malloc(o->mergeableSectionsNum * sizeof(MergeableSection*));
    if(o->mergeableSections == NULL)
        fatal("null mergeableSections init\n");

    for (int i = 0; i < o->isecNum; i++) {
        InputSection * isec = o->Sections[i];
        if (isec != NULL && isec->isAlive &&
            (shdr_(isec)->Flags & SHF_MERGE) != 0) {
            o->mergeableSections[i] = splitSection(ctx, isec);
            isec->isAlive = false;      //被拆了，不再需要了
        } else {
            o->mergeableSections[i] = NULL;
        }
    }
}

// 找到字符串结束即all zeors的位置
int findNull(const char* data, uint64_t data_len, int entSize) {
    if (entSize == 1) {
        const char* result = memchr(data, 0, data_len);
        if (result != NULL) {
            return (int)(intptr_t)(result - data);
        }
    } else {
        for (int i = 0; i <= data_len - entSize; i += entSize) {
            const char* bs = data + i;
            int allZeros = 1;
            for (int j = 0; j < entSize; j++) {
                if (bs[j] != 0) {
                    allZeros = 0;
                    break;
                }
            }
            if (allZeros) {
                return i;
            }
        }
    }

    return -1;
}

MergeableSection *splitSection(Context* ctx,InputSection* isec){
    MergeableSection *m = NewMergeableSection();
    Shdr *shdr = shdr_(isec);
    m->parent = GetMergedSectionInstance(ctx,Name(isec),shdr->Type,shdr->Flags);
    m->p2align = isec->P2Align;

    char *data = isec->contents;
    uint64_t offset = 0;
    uint64_t data_len = shdr->Size;
    if((shdr->Flags & SHF_STRINGS) != 0){
        //strs
        //每项数据不是固定大小的
        while (data_len > 0) {
            //如果entsize是1 : "hello,world"
            //如果entsize是4 : "H\0\0\0e\0\0\0...." 将每个数据对齐到4
            //处理对齐 , 找到结束位置
            int end = findNull(data,data_len,shdr->EntSize);
            if(end == -1){
                fatal("string is not null terminated");
            }

            uint64_t sz = (uint64_t)end + shdr->EntSize;
            //char* substr = strndup(data, sz);
            char* substr = malloc(sz +1);
            memcpy(substr,data,sz);
            substr[sz] = '\0';
            //printBytes(substr,sz);

            m->strslen = realloc(m->strslen,sizeof (int) * (m->strNum + 1));
            m->strs = realloc(m->strs, (m->strNum + 1) * sizeof(char*));
            m->fragOffsets = realloc(m->fragOffsets, (m->fragOffsetNum + 1) * sizeof(uint32_t));

            if (substr == NULL || m->strs == NULL || m->fragOffsets == NULL) {
                fatal("null !\n");
            }

            m->strs[m->strNum] = substr;
            m->strslen[m->strNum] = sz;
            m->fragOffsets[m->fragOffsetNum] = offset;

            data += sz;
            offset += sz;
            data_len -= sz;

            m->strNum++;m->fragOffsetNum++;
        }
    }else {
        //const数据
        //一项一项数据进行处理，每个数据固定EntSize大
        if (shdr->Size % shdr->EntSize != 0) {
            fatal("section size is not multiple of entsize");
        }

        while (data_len > 0) {
            char* substr = malloc((shdr->EntSize + 1) * sizeof(char));
            //strncpy(substr, data, shdr->EntSize);
            memcpy(substr, data, shdr->EntSize);
            substr[shdr->EntSize] = '\0';

            m->strslen = realloc(m->strslen,sizeof (int) * (m->strNum + 1));
            m->strs = realloc(m->strs, (m->strNum + 1) * sizeof(char*));
            m->fragOffsets = realloc(m->fragOffsets, (m->fragOffsetNum + 1) * sizeof(uint32_t));

            if (substr == NULL || m->strs == NULL || m->fragOffsets == NULL) {
                fatal("null !\n");
            }

            m->strs[m->strNum] = substr;
            m->fragOffsets[m->fragOffsetNum] = offset;
            m->strslen[m->strNum] = shdr->EntSize;

            offset += shdr->EntSize;
            data += shdr->EntSize;
            data_len -= shdr->EntSize;
            m->strNum++;m->fragOffsetNum++;
        }
    }
    return m;
}

InputSection *GetSection(ObjectFile* o,Sym* esym,int idx){
    return o->Sections[GetShndx(o,esym,idx)];
}

void ResolveSymbols(ObjectFile* o){
    //localSymbol是不需要resolve的,从第一个全局符号开始解析就行
    for(int i=o->inputFile->FirstGlobal;i<o->inputFile->symNum;i++){
        Sym* esym = &o->inputFile->ElfSyms[i];
        Symbol *sym = o->inputFile->Symbols[i];

        //printf那些不也是undef吗，是不是也还是得处理 ; 不是，这些会在别的objFile处理到
        if(IsUndef(esym)){
            continue;
        }

        InputSection *isec = NULL;
        if(!IsAbs(esym)){
            isec = GetSection(o,esym,i);
            if(isec == NULL)
                continue;
        }
        //读到不同文件时，会将每一个global符号应该在的文件赋到对应的file
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

void registerSectionPieces(ObjectFile* o){
    //printf("new obj num %zu\n",o->mergeableSectionsNum);
    for(int i=0; i< o->mergeableSectionsNum;i++){
        MergeableSection * m = o->mergeableSections[i];
        if(m == NULL)
            continue;

        m->fragments = (SectionFragment**) malloc(sizeof (SectionFragment*) * m->strNum);
        m->fragmentNum = m->strNum;

//        printf("same m1\n");
//        printf("addr %p\n",(void*)m->parent);
        for(int j = 0; j<m->strNum ;j++){
            m->fragments[j] = Insert(m->parent,m->strs[j],m->p2align,m->strslen[j]);
        }
        //printf("same m2\n");
    }

    //因为symbol可能要改成属于一个fragment , 进行处理
    for(int i = 1;i<o->inputFile->symNum;i++){
        Symbol *sym = o->inputFile->Symbols[i];
        Sym *esym = &o->inputFile->ElfSyms[i];

        if(IsAbs(esym) || IsUndef(esym) || IsCommon(esym))
            continue;

        MergeableSection *m = o->mergeableSections[GetShndx(o,esym,i)];
        if(m == NULL)
            continue;

        uint32_t fragOffset;
        SectionFragment *frag = GetFragment(m,esym->Val,&fragOffset);

        if (frag == NULL) {
            fatal("bad symbol value");
        }

        SetSectionFragment(sym,frag);
        sym->value = fragOffset;
    }
}

//跳过.eh_frame，就不处理异常了
void SkipEhframeSections(ObjectFile* o){
    for(int i=0;i < o->isecNum;i++){
        InputSection *isec = o->Sections[i];
        if(isec != NULL && isec->isAlive && strcmp(Name(isec),".eh_frame")==0){
            isec->isAlive = false;
        }
    }
}

void ScanRelocations_(ObjectFile* o){
    for(int i=0;i < o->isecNum;i++) {
        InputSection *isec = o->Sections[i];
        if(isec != NULL && isec->isAlive && (shdr_(isec)->Flags & SHF_ALLOC) != 0){
            ScanRelocations__(isec);
        }
    }
}