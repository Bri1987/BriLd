#ifndef BRILINKER_CHUNK_H
#define BRILINKER_CHUNK_H

#include "elf_std.h"

typedef struct Chunk{
    char* name;
    Shdr shdr;
}Chunk;

Chunk *NewChunk();

#endif //BRILINKER_CHUNK_H
