#include "elf_std.h"
#include "file.h"

typedef struct {
    File *file;
    //TODO check数组 ElfSyms 是一个 Sym 结构体的数组
    Shdr* ElfSections;
    int sectionNum;
}InputFile;

InputFile* NewInputFile(File* file);