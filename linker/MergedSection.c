#include "merge.h"
#include "output.h"

MergedSection *NewMergedSection(char* name , uint64_t flags , uint32_t typ){
    MergedSection *mergedSection = (MergedSection*) malloc(sizeof (MergedSection));
    mergedSection->map = HashMapInit();
    mergedSection->chunk = NewChunk();

    //TODO 直接赋值
    mergedSection->chunk->name = name;
    mergedSection->chunk->shdr.Flags = flags;
    mergedSection->chunk->shdr.Type = typ;
    return mergedSection;
}

//单例
MergedSection* find(Context* ctx,char* name,uint32_t typ,uint64_t flags) {
    for (size_t i = 0; i < ctx->mergedSectionNum; i++) {
        MergedSection* osec = ctx->mergedSections[i];
        if (strcmp(name, osec->chunk->name) == 0 && flags == osec->chunk->shdr.Flags &&
            typ == osec->chunk->shdr.Type) {
            return osec;
        }
    }
    return NULL;
}

MergedSection *GetMergedSectionInstance(Context* ctx, char* name,uint32_t typ,uint64_t flags){
    name = GetOutputName(name,flags);

    flags = flags & ~(SHF_GROUP) & ~(SHF_MERGE) &
            ~(SHF_STRINGS) & ~(2048);

    MergedSection *osec = find(ctx,name,typ,flags);
    if(osec != NULL)
        return osec;

    osec = NewMergedSection(name,flags,typ);
    ctx->mergedSections = (struct MergedSection_**)realloc(ctx->mergedSections, (ctx->mergedSectionNum + 1) * sizeof(struct MergedSection_*));
    if(ctx->mergedSections == NULL)
        fatal("null mergedSection");
    ctx->mergedSections[ctx->mergedSectionNum] = osec;
    ctx->mergedSectionNum++;
    return osec;
}

SectionFragment *Insert(MergedSection* m,char* key,uint32_t p2align){
    SectionFragment *frag = NULL;
    uint32_t hashKey = hash(key);
    char* key_ = convertHashToKey(hashKey);

    if(!HashMapContain(m->map,key)){
        frag = NewSectionFragment(m);
        HashMapPut(m->map,key_,frag);
    } else {
        frag = HashMapGet(m->map,key_);
    }

    if(frag->P2Align < p2align){
        frag->P2Align = p2align;
    }

    return frag;
}