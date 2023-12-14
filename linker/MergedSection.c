#include "merge.h"
#include "output.h"
#include "mergedSection.h"

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

    //v printf("nnnnname %s\n",name);
    osec = NewMergedSection(name,flags,typ);
    ctx->mergedSections = (struct MergedSection_**)realloc(ctx->mergedSections, (ctx->mergedSectionNum + 1) * sizeof(struct MergedSection_*));
    if(ctx->mergedSections == NULL)
        fatal("null mergedSection");
    ctx->mergedSections[ctx->mergedSectionNum] = osec;
    ctx->mergedSectionNum++;
    return osec;
}

void printBytes(const char* str, size_t length) {
    printf("%s\t",str);
    printf("Bytes__: ");
    for (size_t i = 0; i < length; i++) {
        printf("%02X ", (unsigned char)str[i]);
    }
    printf("finish\n");
}

char* checke(HashMap* map,char* key,int len){
    HashMapFirst(map);
    for(Pair *p = HashMapNext(map); p!=NULL; p= HashMapNext(map)){
        SectionFragment *frag = p->value;
        if(frag->strslen == len && memcmp(p->key,key,len)==0)
            return p->key;
    }
    return NULL;
}

SectionFragment *Insert(MergedSection* m,char* key,uint32_t p2align,int strslen){
    SectionFragment *frag = NULL;

    char* kk = checke(m->chunk->mergedSec.map,key,strslen);
//    printf("before \t");
//    printBytes(key,strslen);
    if(kk == NULL){
        frag = NewSectionFragment(m);
        frag->strslen = strslen;
        HashMapPut(m->chunk->mergedSec.map,key,frag);
        //printBytes(key,strslen);
        //printf("key is %s , file %s\n",key,m->chunk->name);
    } else {
        frag = HashMapGet(m->chunk->mergedSec.map,kk);
    }

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
        //fragment->key = HashMapGet(mergedMap,p->key);
        fragment->key = p->key;
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

   // printf("n %d\n",numFragments);
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
            else if (x->val->strslen != y->val->strslen) {
                if (x->val->strslen > y->val->strslen) {
                    // 交换两个元素的位置
                    Fragment* temp = fragments[j];
                    fragments[j] = fragments[j + 1];
                    fragments[j + 1] = temp;
                }
            }
            else if (memcmp(x->key, y->key,x->val->strslen) > 0) {
                // 交换两个元素的位置
                Fragment* temp = fragments[j];
                fragments[j] = fragments[j + 1];
                fragments[j + 1] = temp;
            }

        }
    }

    uint64_t offset = 0;
    uint64_t p2align = 0;
    //printf("num %d\n",numFragments);
    for(int i=0; i< numFragments;i++){
        Fragment *frag = fragments[i];
        offset = AlignTo(offset, 1 << frag->val->P2Align);
        //printf("name %s , offset %lu\n",m->chunk->name,offset);
        frag->val->Offset = offset;
        //printf("name %s , offset %d\n",frag->key,offset);
        //printBytes(frag->key,frag->val->strslen);
        offset += frag->val->strslen;
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
        //char* key = HashMapGet(mergedMap,p->key);
        SectionFragment *frag = p->value;
        //TODO check 可以用strcpy吧
        //strcpy(buf + frag->Offset,key);
        memcpy(buf+frag->Offset,p->key,frag->strslen);
    }
}