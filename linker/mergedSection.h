#ifndef BRILINKER_MERGEDSECTION_H
#define BRILINKER_MERGEDSECTION_H

#include "utils.h"
#include "chunk.h"

//合并后的section
struct MergedSection_{
    Chunk *chunk;
    HashMap *map;    //string - sectionFragment
};

#endif //BRILINKER_MERGEDSECTION_H
