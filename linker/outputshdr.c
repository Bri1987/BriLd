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
    shdr.Offset = 0;
    shdr.Name = 0;
    shdr.Type = 0;
    shdr.Flags = 0;
    shdr.Addr = 0;
    shdr.Size = 0;
    shdr.Info = 0;
    shdr.AddrAlign = 0;
    shdr.EntSize = 0;
    shdr.Link =0;
    Write(ctx->buf+c->shdr.Offset,sizeof (Shdr),&shdr);
}