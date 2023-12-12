#ifndef BRILINKER_MERGE_H
#define BRILINKER_MERGE_H

#include "utils.h"
#include "context.h"

typedef struct SectionFragment_ SectionFragment;
typedef struct MergedSection_ MergedSection;

struct SectionFragment_ {
    MergedSection* OutputSection;
    uint32_t Offset;      //在section中的offset
    uint32_t P2Align;
    bool IsAlive;
} ;

typedef struct MergeableSection{
    MergedSection * parent;
    uint8_t p2align;
    char** strs;
    int strNum;
    uint32_t* fragOffsets;
    int fragOffsetNum;
    SectionFragment ** fragments;
    int fragmentNum;
}MergeableSection;

//SectionFragment
SectionFragment* NewSectionFragment(MergedSection* m);
uint64_t SectionFragment_GetAddr(const SectionFragment* s);

//mergedSection
MergedSection *NewMergedSection(char* name , uint64_t flags , uint32_t typ);
MergedSection *GetMergedSectionInstance(Context* ctx, char* name,uint32_t typ,uint64_t flags);
SectionFragment *Insert(MergedSection* m,char* key,uint32_t p2align);

//mergeableSection
MergeableSection *NewMergeableSection();
//根据偏移，找到它属于哪个sectionFragment
SectionFragment* GetFragment(const MergeableSection* m, uint32_t offset, uint32_t* fragOffset);

#endif //BRILINKER_MERGE_H
