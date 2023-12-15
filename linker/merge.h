#ifndef BRILINKER_MERGE_H
#define BRILINKER_MERGE_H

#include "utils.h"
#include "context.h"

typedef struct SectionFragment_ SectionFragment;
typedef struct MergedSection_ MergedSection;

//将merge-able section分成小的数据块
struct SectionFragment_ {
    MergedSection* OutputSection;   //进行一个双向关联吧
    uint32_t Offset;      //在section中的offset
    uint32_t P2Align;
    bool IsAlive;
    int strslen;            //保存这个sectionFragment的长度
} ;

//input section拆成一个(? y)包含多个sectionFragment的merge-able section , 再放入merged section
typedef struct MergeableSection{
    MergedSection * parent;
    uint8_t p2align;
    char** strs;            //fragments的原始数据, 是数据，不一定是字符串 , 还有const原始数据
    int strNum;
    int* strslen;
    uint32_t* fragOffsets;
    int fragOffsetNum;
    SectionFragment ** fragments;
    int fragmentNum;
}MergeableSection;

//SectionFragment
SectionFragment* NewSectionFragment(MergedSection* m);
uint64_t SectionFragment_GetAddr(SectionFragment* s);

//mergedSection
MergedSection *NewMergedSection(char* name , uint64_t flags , uint32_t typ);
MergedSection *GetMergedSectionInstance(Context* ctx, char* name,uint32_t typ,uint64_t flags);
SectionFragment *Insert(MergedSection* m,char* key,uint32_t p2align,int strslen);

//mergeableSection
MergeableSection *NewMergeableSection();
//根据偏移，找到它属于哪个sectionFragment
SectionFragment* GetFragment(const MergeableSection* m, uint32_t offset, uint32_t* fragOffset);

#endif //BRILINKER_MERGE_H
