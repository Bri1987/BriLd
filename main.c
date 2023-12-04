#include <stdio.h>
#include "file.h"
#include "elf_std.h"
#include "objectfile.h"

int main(int argc, char* argv[]) {
    if(argc < 2)
        fatal("less args\n");
    File *file = NewFile(argv[1]);

    ObjectFile *objectFile = NewObjectFile(file);
    Parse(objectFile);
    for(int i=0;i<objectFile->inputFile->symNum;i++){
        printf("%s\n", ElfGetName(objectFile->inputFile->SymbolStrtab,objectFile->inputFile->ElfSyms[i].Name));
    }
    return 0;
}
