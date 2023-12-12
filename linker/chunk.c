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
    chunk->chunkType  = 0;

    chunk->outpuSec.members = NULL;
    chunk->outpuSec.memberNum = 0;
    chunk->outpuSec.idx = 0;
    return chunk;
}

Shdr *GetShdr(Chunk* c){
    return &c->shdr;
}

void CopyBuf(Chunk* c,Context* ctx){
    if(c->chunkType == ChunkTypeEhdr)
        Ehdr_CopyBuf(c,ctx);
    else if(c->chunkType == ChunkTypeShdr)
        Shdr_CopyBuf(c,ctx);
    else if(c->chunkType == ChunkTypeOutputSection)
        OutputSec_CopyBuf(c,ctx);
}

void Update(Chunk* c,Context* ctx){
    if(c->chunkType == ChunkTypeEhdr)
        ;
    else if(c->chunkType == ChunkTypeShdr)
        Shdr_UpdateShdr(c,ctx);
    else if(c->chunkType == ChunkTypeOutputSection)
        ;
}

char* GetName(Chunk* c){
    return c->name;
}