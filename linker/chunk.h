#ifndef BRILINKER_CHUNK_H
#define BRILINKER_CHUNK_H

#include "elf_std.h"
#include "context.h"
#include "utils.h"
#include "union.h"

#define max_(a, b) ((a) > (b) ? (a) : (b))

typedef uint8_t ChunkType;
#define ChunkTypeUnknown ((ChunkType)0)
#define ChunkTypeEhdr ((ChunkType)1)
#define ChunkTypeShdr ((ChunkType)2)
#define ChunkTypePhdr ((ChunkType)3)
#define ChunkTypeOutputSection ((ChunkType)4)
#define ChunkTypeMergedSection ((ChunkType)5)
#define ChunkTypeGotSection ((ChunkType)6)

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

    struct {
        HashMap *map;    //string - sectionFragment
    }mergedSec;

    struct {
        Phdr *phdrs;
        int phdrNum;
    }phdrS;

    struct {
        Symbol ** GotTpSyms;
        int TpSymNum;
    }gotSec;
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

typedef struct OutputPhdr_{
    Chunk *chunk;
}OutputPhdr;

typedef struct GotSection_{
    Chunk *chunk;
}GotSection;

// .got 表中的每个条目对应一个全局变量或函数的地址 , 针对tp_addr的偏移量
typedef struct GotEntry_{
    int64_t idx;
    uint64_t val;
}GotEntry;

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

//-------------------phdr
OutputPhdr *NewOutputPhdr();
void Phdr_CopyBuf(Chunk* c,Context* ctx);
void Phdr_UpdateShdr(Chunk* c,Context* ctx);

//-------------------got section
GotSection *NewGotSection();
void AddGotTpSymbol(Chunk* chunk, Symbol* sym);
void GotSec_CopyBuf(Chunk* c,Context* ctx);
GotEntry *GetEntries(Chunk *chunk,Context* ctx,int* num);

#endif //BRILINKER_CHUNK_H
