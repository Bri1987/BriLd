#include "chunk.h"

Symbol *NewSymbol(char* name){

    Symbol *symbol = (Symbol*) malloc(sizeof (Symbol));
    symbol->name = name;
    symbol->symIdx = -1;
    symbol->file = NULL;
    symbol->inputSection = NULL;
    symbol->value = 0;
    symbol->sectionFragment = NULL;
    symbol->flags = 0;
    symbol->gotTpIdx = 0;
    return symbol;
}

void SetInputSection(Symbol *s,InputSection* isec){
    s->inputSection = isec;
    s->sectionFragment = NULL;
}

void SetSectionFragment(Symbol* s,SectionFragment* frag){
    s->sectionFragment = frag;
    s->inputSection = NULL;
}

Symbol *GetSymbolByName(Context* ctx,char* name){
    //如果symbolMap中已存，直接拿
    if(HashMapContain(ctx->SymbolMap,name)){
        return HashMapGet(ctx->SymbolMap,name);
    }

    //否则创建一个新symbol，目前还是初始化状态
    HashMapPut(ctx->SymbolMap,name, NewSymbol(name));
    return HashMapGet(ctx->SymbolMap,name);
}

//返回这个symbol对应的一个Elf32_Sym条目
Sym *ElfSym_(Symbol* s){
    assert(s->symIdx < s->file->inputFile->symNum);
    return &s->file->inputFile->ElfSyms[s->symIdx];
}

void clear(Symbol* s){
    s->file = NULL;
    s->symIdx = -1;
    s->inputSection = NULL;
    s->sectionFragment = NULL;
}

uint64_t Symbol_GetAddr(Symbol* s){
    if(s->sectionFragment != NULL)
        return SectionFragment_GetAddr(s->sectionFragment) + s->value;

    if(s->inputSection != NULL)
        return InputSec_GetAddr(s->inputSection) + s->value;

    return s->value;
}

uint64_t GetGotTpAddr(Context* ctx,Symbol* s){
    return ctx->got->chunk->shdr.Addr + s->gotTpIdx * 8;
}
