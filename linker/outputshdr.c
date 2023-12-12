#include "chunk.h"

OutputShdr *NewOutputShdr(){
    OutputShdr *outputShdr = (OutputShdr*) malloc(sizeof (OutputShdr));
    outputShdr->chunk = NewChunk();
    outputShdr->chunk->shdr.AddrAlign = 8;
    outputShdr->chunk->chunkType = ChunkTypeShdr;
    return outputShdr;
}

void Shdr_UpdateShdr(Chunk* c,Context* ctx){
    c->shdr.Size = 1 * sizeof (Shdr);
}

void Shdr_CopyBuf(Chunk* c,Context* ctx){
    //base := ctx.Buf[o.Shdr.Offset:]
    //utils.Write[Shdr](base, Shdr{})
    Shdr shdr;
    Write(ctx->buf+c->shdr.Offset,sizeof (Shdr),&shdr);
}