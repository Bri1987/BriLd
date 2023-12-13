
#ifndef BRILINKER_PASSES_H
#define BRILINKER_PASSES_H

#include "union.h"
#include "mergedSection.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

void MarkLiveObjects(Context* ctx);
void ResolveSymbols_pass(Context* ctx);
void RegisterSectionPieces(Context* ctx);
void CreateSyntheticSections(Context* ctx);
uint64_t SetOutputSectionOffsets(Context* ctx);
void BinSections(Context* ctx);
void CollectOutputSections(Context* ctx);
void ComputeSectionSizes(Context* ctx);
void SortOutputSections(Context* ctx);
bool isTbss(Chunk* chunk);
void ComputeMergedSectionSizes(Context* ctx);

#endif //BRILINKER_PASSES_H
