#include "chunk.h"

OutputEhdr *NewOutputEhdr(){
    OutputEhdr *outputEhdr = (OutputEhdr*) malloc(sizeof (OutputEhdr));
    outputEhdr->chunk = NewChunk();
    outputEhdr->chunk->shdr.Flags = SHF_ALLOC;
    outputEhdr->chunk->shdr.Size = sizeof (Ehdr);
    outputEhdr->chunk->shdr.AddrAlign = 8;
    outputEhdr->chunk->chunkType = ChunkTypeEhdr;
    return outputEhdr;
}

void Ehdr_CopyBuf(Chunk *c,Context* ctx){
    Ehdr *ehdr = (Ehdr*) malloc(sizeof (Ehdr));
    WriteMagic(ehdr->Ident);
    ehdr->Ident[EI_CLASS] = ELFCLASS64;
    ehdr->Ident[EI_DATA] = ELFDATA2LSB;
    ehdr->Ident[EI_VERSION] = EV_CURRENT;
    ehdr->Ident[EI_OSABI] = 0;
    ehdr->Ident[EI_ABIVERSION] = 0;
    ehdr->Type = ET_EXEC;
    ehdr->Machine = 243;      //EM_RISCV
    ehdr->Version = EV_CURRENT;
    //TODO
    ehdr->EhSize = sizeof (Ehdr);
    ehdr->PhEntSize = sizeof (Phdr);
    //TODO
    ehdr->ShEntSize = sizeof(Shdr);
    ehdr->ShNum = 0;
    ehdr->ShStrndx = 0;

    char* buf = malloc(sizeof (Ehdr));
    memcpy(buf,ehdr,sizeof (Ehdr));
    memcpy(ctx->buf+c->shdr.Offset,buf,sizeof (Ehdr));
}