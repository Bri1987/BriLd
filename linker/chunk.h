#ifndef BRILINKER_CHUNK_H
#define BRILINKER_CHUNK_H

#include "elf_std.h"
#include "context.h"

typedef uint8_t ChunkType;
#define ChunkTypeUnknown ((ChunkType)0)
#define ChunkTypeEhdr ((ChunkType)1)
#define ChunkTypePhdr ((ChunkType)2)

typedef struct Chunk_{
    char* name;
    Shdr shdr;
    ChunkType chunkType;
}Chunk;

typedef struct OutputEhdr_{
    Chunk *chunk;
}OutputEhdr;

Chunk *NewChunk();
Shdr *GetShdr(Chunk* c);
void CopyBuf(Chunk* c,Context* ctx);

//-------------ehdr
OutputEhdr *NewOutputEhdr();
void Ehdr_CopyBuf(Chunk *c,Context* ctx);

#endif //BRILINKER_CHUNK_H
