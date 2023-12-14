#include "chunk.h"
#include "output.h"

OutputSection *NewOutputSection(char* name,uint32_t typ, uint64_t flags, uint32_t idx){
    OutputSection *outputSection = (OutputSection*) malloc(sizeof (OutputSection));
    outputSection->chunk = NewChunk();
    outputSection->chunk->name = name;
    outputSection->chunk->shdr.Type = typ;
    outputSection->chunk->shdr.Flags = flags;

    outputSection->chunk->outpuSec.idx = idx;
    outputSection->chunk->chunkType = ChunkTypeOutputSection;
    return outputSection;
}

void OutputSec_CopyBuf(Chunk* c,Context* ctx){
    if(c->shdr.Type == SHT_NOBITS)
        return;

    char* base = ctx->buf + c->shdr.Offset;
    for(int i=0; i< c->outpuSec.memberNum;i++){
        struct InputSection_* isec = c->outpuSec.members[i];
        //printf("isec offset %d\n",isec->offset);
        WriteTo(isec,base+isec->offset,ctx);
    }
}

OutputSection *findOutputSection(Context* ctx,const char* name,uint64_t typ,uint64_t flags){
    for(int i = 0; i<ctx->outputSecNum;i++){
        OutputSection *osec = ctx->outputSections[i];
        if(strcmp(osec->chunk->name,name)==0 && osec->chunk->shdr.Type == typ && osec->chunk->shdr.Flags == flags)
            return osec;
    }
    return NULL;
}

// GetOutputSection 单例模式返回一个input section对应的output section
OutputSection *GetOutputSection(Context* ctx,char* name,uint64_t typ,uint64_t flags){
    char* outputName = GetOutputName(name,flags);
    flags = flags & ~((uint64_t)SHF_GROUP) & ~((uint64_t)2048 /*SHF_COMPRESSED*/) & ~((uint64_t)SHF_LINK_ORDER);

    OutputSection *osec = findOutputSection(ctx,outputName,typ,flags);
    if(osec!=NULL)
        return osec;

    osec = NewOutputSection(outputName,typ,flags,ctx->outputSecNum);
    ctx->outputSections = (OutputSection**) realloc(ctx->outputSections,sizeof (OutputSection*) * (ctx->outputSecNum+1));
    ctx->outputSections[ctx->outputSecNum] = osec;
    ctx->outputSecNum++;
    return osec;
}