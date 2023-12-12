#ifndef BRILINKER_CHUNK_H
#define BRILINKER_CHUNK_H

#include "elf_std.h"
#include "context.h"
#include "utils.h"
#include "union.h"

typedef uint8_t ChunkType;
#define ChunkTypeUnknown ((ChunkType)0)
#define ChunkTypeEhdr ((ChunkType)1)
#define ChunkTypeShdr ((ChunkType)2)
#define ChunkTypePhdr ((ChunkType)3)
#define ChunkTypeOutputSection ((ChunkType)4)

typedef struct Chunk_{
    char* name;
    Shdr shdr;
    ChunkType chunkType;
    int32_t rank;

    struct {
        struct InputSection_** members;
        int memberNum;
        uint32_t idx;
    }outpuSec;
}Chunk;

typedef struct OutputEhdr_{
    Chunk *chunk;
}OutputEhdr;

typedef struct OutputShdr_{
    Chunk *chunk;
}OutputShdr;

typedef struct OutputSection_{
    Chunk * chunk;
}OutputSection;

Chunk *NewChunk();
Shdr *GetShdr(Chunk* c);
void CopyBuf(Chunk* c,Context* ctx);
char* GetName(Chunk* c);
void Update(Chunk* c,Context* ctx);

//-------------ehdr
OutputEhdr *NewOutputEhdr();
void Ehdr_CopyBuf(Chunk *c,Context* ctx);

//----------------shdr
OutputShdr *NewOutputShdr();
void Shdr_UpdateShdr(Chunk* c,Context* ctx);
void Shdr_CopyBuf(Chunk* c,Context* ctx);

//--------------outputsection
OutputSection *GetOutputSection(Context* ctx,char* name,uint64_t typ,uint64_t flags);
OutputSection *NewOutputSection(char* name,uint32_t typ, uint64_t flags, uint32_t idx);
void OutputSec_CopyBuf(Chunk* c,Context* ctx);

#endif //BRILINKER_CHUNK_H
