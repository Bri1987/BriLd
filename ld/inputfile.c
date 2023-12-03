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
    return inputFile;
}

char* GetBytesFromShdr(InputFile* inputFile, Shdr* shdr){
    uint64_t length = shdr->Size;
    char* contents = malloc(length+1);
    memcpy(contents, inputFile->file->Contents + shdr->Offset, length);
    contents[length] = '\0';

    return contents;
}

char* GetBytesFromIdx(InputFile* inputFile, int64_t idx){
    return GetBytesFromShdr(inputFile,&inputFile->ElfSections[idx]);
}