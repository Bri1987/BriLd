#include "union.h"
#include "chunk.h"

Shdr *shdr_(InputSection* i){
    assert(i->shndx < i->objectFile->inputFile->sectionNum);
    return &i->objectFile->inputFile->ElfSections[i->shndx];
}

uint8_t toP2Align(uint64_t align) {
    if (align == 0) {
        return 0;
    }
    return __builtin_ctzll(align);
}

InputSection *NewInputSection(Context *ctx,char* name,ObjectFile* file,uint32_t shndx){
    InputSection *inputSection = (InputSection*) malloc(sizeof (InputSection));
    inputSection->objectFile = file;
    inputSection->shndx = shndx;

    Shdr *shdr = shdr_(inputSection);
    inputSection->contents = malloc(shdr->Size);
    memcpy(inputSection->contents, file->inputFile->file->Contents + shdr->Offset, shdr->Size);
    //TODO 不能加，原因是 ?
    //inputSection->contents[shdr->Size] = '\0';

    assert((shdr->Flags & (uint64_t )2048) == 0);  //SHF_COMPRESSED
    inputSection->shsize = shdr->Size;
    inputSection->isAlive = true;
    inputSection->P2Align = toP2Align(shdr->AddrAlign);

    inputSection->outputSection = GetOutputSection(ctx,name,shdr->Type,shdr->Flags);
    return inputSection;
}

char* Name(InputSection* inputSection){
    return ElfGetName(inputSection->objectFile->inputFile->ShStrtab,shdr_(inputSection)->Name);
}


void  CopyContents(InputSection* i,char* buf){
    buf = malloc(i->shsize);
    memcpy(buf,i->contents,i->shsize);
}

void WriteTo(InputSection *i,char* buf){
    if(shdr_(i)->Type == SHT_NOBITS || i->shsize == 0)
        return;

    CopyContents(i,buf);
}