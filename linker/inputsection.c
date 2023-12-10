#include "union.h"

Shdr *shdr_(InputSection* i){
    assert(i->shndx < i->objectFile->inputFile->sectionNum);
    return &i->objectFile->inputFile->ElfSections[i->shndx];
}

InputSection *NewInputSection(ObjectFile* file,uint32_t shndx){
    InputSection *inputSection = (InputSection*) malloc(sizeof (InputSection));
    inputSection->objectFile = file;
    inputSection->shndx = shndx;

    Shdr *shdr = shdr_(inputSection);
    inputSection->contents = malloc(shdr->Size);
    memcpy(inputSection->contents, file->inputFile->file->Contents + shdr->Offset, shdr->Size);
    //TODO 加不加
    //inputSection->contents[shdr->Size] = '\0';
    return inputSection;
}

char* Name(InputSection* inputSection){
    return ElfGetName(inputSection->objectFile->inputFile->ShStrtab,shdr_(inputSection)->Name);
}