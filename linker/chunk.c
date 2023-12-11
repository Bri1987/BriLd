#include "chunk.h"

Chunk *NewChunk(){
    Chunk *chunk = (Chunk*) malloc(sizeof (Chunk));
    chunk->shdr.AddrAlign = 1;
    chunk->shdr.Addr = 0;
    chunk->shdr.Name = 0;
    chunk->shdr.Size = 0;
    chunk->shdr.EntSize = 0;
    chunk->shdr.Type = 0;
    chunk->shdr.Offset = 0;
    chunk->shdr.Link = 0;
    chunk->shdr.Info = 0;
    chunk->shdr.Flags = 0;
    return chunk;
}