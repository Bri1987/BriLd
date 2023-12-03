#include <stdio.h>
#include "file.h"
#include "elf_std.h"
#include "inputfile.h"

int main(int argc, char* argv[]) {
    if(argc < 2)
        fatal("less args\n");
    File *file = NewFile(argv[1]);

    InputFile *inputFile = NewInputFile(file);
    for(int i=0;i<inputFile->sectionNum;i++){
        Shdr shdr = inputFile->ElfSections[i];
        printf("%s\n", ElfGetName(inputFile->ShStrtab,inputFile->ElfSections[i].Name));
    }
    return 0;
}
