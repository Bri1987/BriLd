#include "merge.h"

SectionFragment* NewSectionFragment(MergedSection* m) {
    SectionFragment* fragment = (SectionFragment*)malloc(sizeof(SectionFragment));
    if (fragment != NULL) {
        fragment->OutputSection = m;
        fragment->Offset = UINT32_MAX;
    }
    return fragment;
}

//uint64_t SectionFragment_GetAddr(const SectionFragment* s) {
//    return s->OutputSection->Shdr.Addr + (uint64_t)(s->Offset);
//}