
#ifndef BRILINKER_PASSES_H
#define BRILINKER_PASSES_H

#include "union.h"

void MarkLiveObjects(Context* ctx);
void ResolveSymbols_pass(Context* ctx);
void RegisterSectionPieces(Context* ctx);

#endif //BRILINKER_PASSES_H
