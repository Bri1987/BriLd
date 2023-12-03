#include <elf.h>
#include "inputfile.h"

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

    inputFile->ElfSections[0] = *shdr;
    inputFile->sectionNum = numSections;
    for (; numSections > 1; --numSections) {
        contents += sizeof(Shdr);
        Read(&inputFile->ElfSections[numSections - 1], contents,sizeof (Shdr));
    }

    int64_t shstrndx = ehdr->ShStrndx;
    if (ehdr->ShStrndx == SHN_XINDEX) {
        shstrndx = shdr->Link;
    }

    return inputFile;
}