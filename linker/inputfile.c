#include <elf.h>
#include "union.h"

InputFile* NewInputFile(File* file){
    InputFile *inputFile = (InputFile*) malloc(sizeof (InputFile));
    inputFile->file = file;
    if(!CheckMagic(file->Contents))
        fatal("not an elf file\n");

    Ehdr *ehdr = (Ehdr*) malloc(sizeof (Ehdr));
    Read(ehdr,file->Contents,sizeof (Ehdr));
    char *contents = file->Contents + ehdr->ShOff;
    Shdr *shdr = (Shdr*) malloc(sizeof (Shdr));
    Read(shdr,contents,sizeof (Shdr));

    int numSections = ehdr->ShNum;
    if(numSections == 0)
        numSections = (int)shdr->Size;

    inputFile->ElfSections = (Shdr*)malloc(numSections * sizeof(Shdr));
    inputFile->ElfSections[0] = *shdr;

    inputFile->sectionNum = numSections;
    for (; numSections > 1; --numSections) {
        contents += sizeof(Shdr);
        Read(&inputFile->ElfSections[inputFile->sectionNum - numSections + 1], contents,sizeof (Shdr));
    }

    int64_t shstrndx = ehdr->ShStrndx;
    if (ehdr->ShStrndx == SHN_XINDEX) {
        shstrndx = shdr->Link;
    }

    //注意不能用strcpy，因为全是'\0'
    char* c = GetBytesFromIdx(inputFile,shstrndx);
    uint64_t len = inputFile->ElfSections[shstrndx].Size;
    inputFile->ShStrtab = malloc(len);
    memcpy(inputFile->ShStrtab,c, len);

    //其他的初始化
    inputFile->FirstGlobal = 0;
    inputFile->numLocalSymbols = 0;
    inputFile->LocalSymbols = NULL;
    inputFile->numSymbols = 0;
    inputFile->Symbols = NULL;
    inputFile->symNum = 0;
    inputFile->ElfSyms = NULL;
    inputFile->SymbolStrtab = NULL;
    return inputFile;
}

// GetBytesFromShdr 返回一个section的数据内容
char* GetBytesFromShdr(InputFile* inputFile, Shdr* shdr){
    uint64_t length = shdr->Size;
    char* contents = malloc(length+1);
    memcpy(contents, inputFile->file->Contents + shdr->Offset, length);
    contents[length] = '\0';

    return contents;
}

// GetBytesFromIdx 根据一个section的index拿到对应section的数据内容
char* GetBytesFromIdx(InputFile* inputFile, int64_t idx){
    return GetBytesFromShdr(inputFile,&inputFile->ElfSections[idx]);
}

// FindSection 返回第一个Section type对应的section
Shdr* FindSection(InputFile* f, uint32_t ty) {
    for (int i = 0; i < f->sectionNum; i++) {
        Shdr* shdr = &(f->ElfSections[i]);
        if (shdr->Type == ty) {
            return shdr;
        }
    }
    return NULL;
}

// FillUpElfSyms 填充符号表表项到inputfile的ElfSyms数组
void FillUpElfSyms(InputFile* inputFile,Shdr* s){
    char *bs = GetBytesFromShdr(inputFile,s);
    int numbs = s->Size / sizeof (Sym);
    inputFile->ElfSyms = (Sym*) malloc(numbs* sizeof(Sym));
    inputFile->symNum = numbs;

    while (numbs > 0){
        Read(&inputFile->ElfSyms[inputFile->symNum - numbs],bs,sizeof(Sym));
        bs += sizeof(Sym);
        numbs--;
    }
}

Ehdr GetEhdr(InputFile* f){
    Ehdr ehdr;
    Read(&ehdr,f->file->Contents,sizeof (Ehdr));
    return ehdr;
}