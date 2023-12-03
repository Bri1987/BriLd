#include <stdio.h>
#include "file.h"
#include "elf_std.h"
#include "inputfile.h"

int main(int argc, char* argv[]) {
    if(argc < 2)
        fatal("less args\n");
    File *file = NewFile(argv[1]);

    InputFile *inputFile = NewInputFile(file);
    Shdr shdr = inputFile->ElfSections[4];
    printf("%lu",inputFile->ElfSections->Size / sizeof (Shdr));
    return 0;
}
