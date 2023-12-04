#include "elf_std.h"
#include "file.h"

typedef struct InputFile_{
    File *file;
    //TODO check数组 ElfSyms 是一个 Sym 结构体的数组
    Shdr* ElfSections;
    int64_t sectionNum;

    char* ShStrtab;
    int64_t FirstGlobal;

    Sym *ElfSyms;
    int64_t symNum;

    char* SymbolStrtab;

}InputFile;

InputFile* NewInputFile(File* file);
char* GetBytesFromIdx(InputFile* inputFile, int64_t idx);
char* GetBytesFromShdr(InputFile* inputFile, Shdr* shdr);
Shdr* FindSection(InputFile* f, uint32_t ty);
void FillUpElfSyms(InputFile* inputFile,Shdr* s);