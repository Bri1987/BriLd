#include "merge.h"
#include "mergedSection.h"

SectionFragment* NewSectionFragment(MergedSection* m) {
    SectionFragment* fragment = (SectionFragment*)malloc(sizeof(SectionFragment));
    if (fragment != NULL) {
        fragment->OutputSection = m;
        fragment->Offset = UINT32_MAX;
        fragment->P2Align = 0;
        //TODO is alive的初始化
        fragment->strslen = 0;
    }
    return fragment;
}

uint64_t SectionFragment_GetAddr(SectionFragment* s) {
    //printf("offset %ld\n",s->OutputSection->chunk->shdr.Addr + s->Offset);
    return s->OutputSection->chunk->shdr.Addr + s->Offset;
}