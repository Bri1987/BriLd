#include "merge.h"

MergeableSection *NewMergeableSection(){
    MergeableSection *mergeableSection = (MergeableSection*) malloc(sizeof (MergeableSection));
    mergeableSection->fragOffsets = NULL;
    mergeableSection->fragments = NULL;
    mergeableSection->fragmentNum = 0;
    mergeableSection->p2align = 0;
    mergeableSection->parent = NULL;
    mergeableSection->strs = NULL;
    mergeableSection->strNum = 0;
    mergeableSection->strslen = NULL;
    mergeableSection->fragOffsetNum = 0;
    return mergeableSection;
}

// GetFragment 获取到某个offset下的fragment
// 注意要返回offset距离所在fragment起始处的距离
SectionFragment* GetFragment(const MergeableSection* m, uint32_t offset, uint32_t* fragOffset) {
    size_t pos = 0;
    for (size_t i = 0; i < m->fragmentNum; i++) {
        if (offset < m->fragOffsets[i]) {
            break;
        }
        pos++;
    }

    if (pos == 0) {
        return NULL;
    }

    size_t idx = pos - 1;
    *fragOffset = offset - m->fragOffsets[idx];
    return m->fragments[idx];
}