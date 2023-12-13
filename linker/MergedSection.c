#include "merge.h"
#include "output.h"
#include "mergedSection.h"

extern HashMap *mergedMap;

MergedSection *NewMergedSection(char* name , uint64_t flags , uint32_t typ){
    MergedSection *mergedSection = (MergedSection*) malloc(sizeof (MergedSection));
    //mergedSection->map = HashMapInit();
    mergedSection->chunk = NewChunk();

    //TODO 直接赋值
    mergedSection->chunk->name = name;
    mergedSection->chunk->shdr.Flags = flags;
    mergedSection->chunk->shdr.Type = typ;
    mergedSection->chunk->chunkType = ChunkTypeMergedSection;
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

    //TODO 打印awesomeLd的go，为什么有两个1两个2，两个December这种
    if(!HashMapContain(m->chunk->mergedSec.map,key_)){
        frag = NewSectionFragment(m);
        HashMapPut(m->chunk->mergedSec.map,key_,frag);
        //TODO 只拿一个map装所有有没有问题
        HashMapPut(mergedMap,key_,key);
        //printf("sss %d\n", HashMapSize(m->chunk->mergedSec.map));
    } else {
        frag = HashMapGet(m->chunk->mergedSec.map,key_);
    }

    //printf("%d\n",frag->P2Align);
    if(frag->P2Align < p2align){
        frag->P2Align = p2align;
    }

    return frag;
}

void AssignOffsets(MergedSection* m){
    Fragment **fragments;
    fragments = (Fragment**) malloc(sizeof (Fragment*) * HashMapSize(m->chunk->mergedSec.map));
    HashMapFirst(m->chunk->mergedSec.map);

    int numFragments = 0;
    for(Pair* p = HashMapNext(m->chunk->mergedSec.map); p!=NULL; p = HashMapNext(m->chunk->mergedSec.map)){
        Fragment *fragment = (Fragment*) malloc(sizeof (Fragment));
        fragment->key = HashMapGet(mergedMap,p->key);
        fragment->val = p->value;
        fragments[numFragments] = fragment;
        numFragments++;
    }

    //sort.SliceStable(fragments, func(i, j int) bool {
    //		x := fragments[i]
    //		y := fragments[j]
    //		if x.Val.P2Align != y.Val.P2Align {
    //			return x.Val.P2Align < y.Val.P2Align
    //		}
    //		if len(x.Key) != len(y.Key) {
    //			return len(x.Key) < len(y.Key)
    //		}
    //
    //		return x.Key < y.Key
    //	})

    printf("n %d\n",numFragments);
    for (int i = 0; i < numFragments - 1; i++) {
        for (int j = 0; j < numFragments - i - 1; j++) {
            Fragment* x = fragments[j];
            Fragment* y = fragments[j + 1];

            if (x->val->P2Align != y->val->P2Align) {
                if (x->val->P2Align > y->val->P2Align) {
                    // 交换两个元素的位置
                    Fragment* temp = fragments[j];
                    fragments[j] = fragments[j + 1];
                    fragments[j + 1] = temp;
                }
            }
            else if (strlen(x->key) != strlen(y->key)) {
                if (strlen(x->key) > strlen(y->key)) {
                    // 交换两个元素的位置
                    Fragment* temp = fragments[j];
                    fragments[j] = fragments[j + 1];
                    fragments[j + 1] = temp;
                }
            }
            else if (strcmp(x->key, y->key) > 0) {
                // 交换两个元素的位置
                Fragment* temp = fragments[j];
                fragments[j] = fragments[j + 1];
                fragments[j + 1] = temp;
            }

        }
    }

    uint64_t offset = 0;
    uint64_t p2align = 0;
    printf("num %d\n",numFragments);
    for(int i=0; i< numFragments;i++){
        Fragment *frag = fragments[i];
        offset = AlignTo(offset, 1 << frag->val->P2Align);
        //printf("name %s , offset %lu\n",m->chunk->name,offset);
        frag->val->Offset = offset;
        offset += strlen(frag->key);
        if(p2align < frag->val->P2Align){
            p2align = frag->val->P2Align;
        }
    }

    m->chunk->shdr.Size = AlignTo(offset,1<<p2align);
    m->chunk->shdr.AddrAlign = 1<<p2align;
}

void MergedSec_CopyBuf(Chunk* c,Context* ctx){
    char* buf = ctx->buf + c->shdr.Offset;

    HashMapFirst(c->mergedSec.map);
    for(Pair* p = HashMapNext(c->mergedSec.map); p!=NULL; p = HashMapNext(c->mergedSec.map)){
        char* key = HashMapGet(mergedMap,p->key);
        SectionFragment *frag = p->value;
        //TODO check 可以用strcpy吧
        strcpy(buf + frag->Offset,key);
    }
}