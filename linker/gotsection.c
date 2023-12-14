#include "chunk.h"

//thread local storage TLS段也会把数据存到.got , libC中很多

GotSection *NewGotSection(){
    GotSection *gotSection = (GotSection*) malloc(sizeof (GotSection));
    gotSection->chunk = NewChunk();
    gotSection->chunk->name = malloc(sizeof (".got") + 1);
    strcpy(gotSection->chunk->name,".got");
    gotSection->chunk->shdr.Type = SHT_PROGBITS;
    gotSection->chunk->shdr.Flags = SHF_ALLOC | SHF_WRITE;
    gotSection->chunk->shdr.AddrAlign = 0;
    gotSection->chunk->chunkType = ChunkTypeGotSection;
    return gotSection;
}

//向GotTpSyms中增加一个元素
void AddGotTpSymbol(Chunk* chunk, Symbol* sym){
    sym->gotTpIdx = chunk->shdr.Size / 8;
    chunk->shdr.Size += 8;
   // printf("%lu\n",chunk->shdr.Size);
    chunk->gotSec.GotTpSyms = realloc(chunk->gotSec.GotTpSyms,sizeof (Symbol*) * (chunk->gotSec.TpSymNum + 1));
    chunk->gotSec.GotTpSyms[chunk->gotSec.TpSymNum] = sym;
    chunk->gotSec.TpSymNum++;
}

GotEntry *GetEntries(Chunk *chunk,Context* ctx,int* num){
    GotEntry *entries = NULL;
    for(int i =0; i< chunk->gotSec.TpSymNum;i++){
        Symbol *sym = chunk->gotSec.GotTpSyms[i];
        uint32_t idx = sym->gotTpIdx;
        entries = (GotEntry*) realloc(entries,sizeof (GotEntry) * ((*num) + 1));
        GotEntry gotEntry ;
        gotEntry.idx = idx;
        gotEntry.val = Symbol_GetAddr(sym) -ctx->TpAddr;
        entries[*num] = gotEntry;
        (*num)++;
    }
    return entries;
}

void GotSec_CopyBuf(Chunk* c,Context* ctx){
  //  printf("in !!\n");
    int num =0 ;
    GotEntry *entries = GetEntries(c,ctx,&num);
    for(int i=0; i< num;i++){
        GotEntry ent = entries[i];
        Write(ctx->buf + c->shdr.Offset + ent.idx*8 ,sizeof (uint64_t),&ent.val);
    }
}