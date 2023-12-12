
#ifndef BRILINKER_PASSES_H
#define BRILINKER_PASSES_H

#include "union.h"
#include "chunk.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

void MarkLiveObjects(Context* ctx);
void ResolveSymbols_pass(Context* ctx);
void RegisterSectionPieces(Context* ctx);
void CreateSyntheticSections(Context* ctx);
uint64_t SetOutputSectionOffsets(Context* ctx);
void BinSections(Context* ctx);
void CollectOutputSections(Context* ctx);
void ComputeSectionSizes(Context* ctx);

#endif //BRILINKER_PASSES_H
