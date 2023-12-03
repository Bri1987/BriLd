#include "elf_std.h"
#include "file.h"

typedef struct {
    File *file;
    //TODO check数组 ElfSyms 是一个 Sym 结构体的数组
    Shdr* ElfSections;
    int sectionNum;
    char* ShStrtab;
}InputFile;

InputFile* NewInputFile(File* file);
char* GetBytesFromIdx(InputFile* inputFile, int64_t idx);
char* GetBytesFromShdr(InputFile* inputFile, Shdr* shdr);