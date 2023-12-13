#ifndef BRILINKER_MERGEDSECTION_H
#define BRILINKER_MERGEDSECTION_H

#include "utils.h"
#include "chunk.h"

//合并后的section
struct MergedSection_{
    Chunk *chunk;
};

typedef struct Fragment_ {
    char* key;
    SectionFragment* val;
}Fragment;

void AssignOffsets(MergedSection* m);
void MergedSec_CopyBuf(Chunk* c,Context* ctx);

#endif //BRILINKER_MERGEDSECTION_H
