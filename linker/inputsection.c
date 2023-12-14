#include "union.h"
#include "chunk.h"

// Shdr 返回一个section对应的section header的信息
Shdr *shdr_(InputSection* i){
    assert(i->shndx < i->objectFile->inputFile->sectionNum);
    return &i->objectFile->inputFile->ElfSections[i->shndx];
}

//1 2 4 8 16
//1 10 100 1000 10000
uint8_t toP2Align(uint64_t align) {
    if (align == 0) {
        return 0;
    }
    return __builtin_ctzll(align);
}

InputSection *NewInputSection(Context *ctx,char* name,ObjectFile* file,uint32_t shndx){
    InputSection *inputSection = (InputSection*) malloc(sizeof (InputSection));
    inputSection->objectFile = file;
    inputSection->shndx = shndx;
    inputSection->isAlive = true;
    inputSection->offset = UINT32_MAX;
    inputSection->RelsecIdx = UINT32_MAX;
    inputSection->shsize = UINT32_MAX;
    inputSection->relNum = 0;
    inputSection->rels = NULL;

    Shdr *shdr = shdr_(inputSection);
    inputSection->contents = malloc(shdr->Size+1);
    memcpy(inputSection->contents, file->inputFile->file->Contents + shdr->Offset, shdr->Size);
    inputSection->contents[shdr->Size] = '\0';

    assert((shdr->Flags & (uint64_t )2048) == 0);  //SHF_COMPRESSED
    inputSection->shsize = shdr->Size;
    inputSection->isAlive = true;
    inputSection->P2Align = toP2Align(shdr->AddrAlign);

    inputSection->outputSection = GetOutputSection(ctx,name,shdr->Type,shdr->Flags);

    return inputSection;
}

// Name 拿到这个inputSection的名字
char* Name(InputSection* inputSection){
    return ElfGetName(inputSection->objectFile->inputFile->ShStrtab,shdr_(inputSection)->Name);
}

void  CopyContents(InputSection* i,char* buf){
    memcpy(buf,i->contents,i->shsize);
}

void WriteTo(InputSection *i,char* buf,Context* ctx){
    if(shdr_(i)->Type == SHT_NOBITS || i->shsize == 0)
        return;

    CopyContents(i,buf);

    if((shdr_(i)->Flags & SHF_ALLOC) != 0){
        ApplyRelocAlloc(i,ctx,buf);
    }
}


void ApplyRelocAlloc(InputSection* i,Context* ctx,char* base){
    Rela *rels = GetRels(i);
 //   printf("new\n before: ");
//    uint32_t value1;
//    memcpy(&value1, base, sizeof(uint32_t));
//    printf("test!__  %u\n",value1);
    for(int a = 0; a < i->relNum;a++){
        Rela rel = rels[a];
        if(rel.Type == 0/*R_RISCV_NONE*/ ||
            rel.Type == 51 /*R_RISCV_RELAX*/){
            continue;
        }

        Symbol *sym = i->objectFile->inputFile->Symbols[rel.Sym];
        char* loc = base + rel.Offset;

        if(sym->file == NULL)
            continue;

        uint64_t S = Symbol_GetAddr(sym);
        uint64_t A = rel.Addend;
        uint64_t P = InputSec_GetAddr(i) + rel.Offset;
        //printf("P %lu\n",P);

        uint32_t tmp = 0;
        uint64_t tmp_64 =0;
        uint64_t val = 0;
        uint64_t value1;
        switch (rel.Type) {
            case 1/*R_RISCV_32*/:
                tmp = S+A;
                Write(loc,sizeof (uint32_t),&tmp);
                break;
            case 2/*R_RISCV_64*/:
                tmp_64 = S+A;
                Write(loc,sizeof (uint64_t),&tmp_64);
                break;
            case 16/*R_RISCV_BRANCH*/:
                tmp = S+A-P;
                writeBtype(loc,tmp);
                break;
            case 17/*R_RISCV_JAL*/:
                tmp = S+A-P;
                writeJtype(loc,tmp);
                break;
            case 18/*R_RISCV_CALL*/:
            case 19/*R_RISCV_CALL_PLT*/:
                tmp = S+A-P;
                writeUtype(loc,tmp);
                writeItype((loc + 4),tmp);
                break;
            case 21/*R_RISCV_TLS_GOT_HI20*/:
                tmp = GetGotTpAddr(ctx,sym) + A -P;
                Write(loc,sizeof (uint32_t),&tmp);
                break;
            case 23/*R_RISCV_PCREL_HI20*/:
                tmp = S+A-P;
                Write(loc,sizeof (uint32_t),&tmp);
                break;
            case 26/*R_RISCV_HI20*/:
                tmp = S+A;
                writeUtype(loc,tmp);
                break;
            case 27/*R_RISCV_LO12_I*/:
            case 28/*R_RISCV_LO12_S*/:
                val = S+A;
                if(rel.Type == 27)
                    writeItype(loc,(uint64_t)val);
                else
                    writeStype(loc,(uint64_t)val);

                if(SignExtend(val,11) == val)
                    setRs1(loc,0);
                break;
            case 30/*R_RISCV_TPREL_LO12_I*/:
            case 31/*R_RISCV_TPREL_LO12_S*/:
                val = S+A-ctx->TpAddr;
                if(rel.Type == 30)
                    writeItype(loc,(uint32_t)val);
                else
                    writeStype(loc,(uint32_t)val);

                if(SignExtend(val,11) == val)
                    setRs1(loc,4);
                break;
            default:
                //printf("other !\n");
                break;
        }
    }

    for(int a = 0; a < i->relNum;a++) {
        Rela rel = rels[a];
        Symbol *sym = i->objectFile->inputFile->Symbols[rel.Sym];
        char* loc = base + rel.Offset;
        uint32_t val = 0;
        switch (rel.Type) {
            case 24/*R_RISCV_PCREL_LO12_I*/:
            case 25/*R_RISCV_PCREL_LO12_S*/:
                assert(sym->inputSection == i);
                Read(&val,base + sym->value,sizeof(uint32_t));

                if(rel.Type == 24)
                    writeItype(loc,val);
                else
                    writeStype(loc,val);
                break;
        }
    }

    for(int a = 0; a < i->relNum;a++) {
        Rela rel = rels[a];
        char *loc = base + rel.Offset;
        uint32_t val = 0;
        uint32_t tmp = 0;
        switch (rel.Type) {
            case 23/*R_RISCV_PCREL_HI20*/:
            case 21/*R_RISCV_TLS_GOT_HI20*/:
                Read(&val,loc,sizeof(uint32_t));
                Read(&tmp,i->contents + rel.Offset,sizeof (uint32_t));
                Write(loc,sizeof (uint32_t),&tmp);
                writeUtype(loc,val);
                break;
        }
    }
}

Rela *GetRels(InputSection* i){
    if(i->RelsecIdx == UINT32_MAX || i->rels != NULL){
        return i->rels;
    }

    char* bs = GetBytesFromShdr(i->objectFile->inputFile,&i->objectFile->inputFile->ElfSections[i->RelsecIdx]);
    uint64_t numbs = (i->objectFile->inputFile->ElfSections[i->RelsecIdx].Size) / sizeof (Rela);
    i->rels = (Rela*) malloc(sizeof (Rela) * numbs);
    while (numbs > 0){
        Read(&i->rels[i->relNum],bs,sizeof(Rela));
        bs += sizeof(Rela);
        numbs--;
        i->relNum++;
    }
    return i->rels;
}

uint64_t InputSec_GetAddr(InputSection* i){
    return i->outputSection->chunk->shdr.Addr + i->offset;
}

void ScanRelocations__(InputSection* isec){
    GetRels(isec);
    for(int i=0; i<isec->relNum;i++){
        Rela rel = isec->rels[i];
        Symbol *sym = isec->objectFile->inputFile->Symbols[rel.Sym];
        if(sym->file == NULL)
            continue;

        if(rel.Type == 21/*R_RISCV_TLS_GOT_HI20*/){
            sym->flags |= 1;
        }
    }
    //printf("relNUm %d\n",isec->relNum);
}

uint32_t itype(uint32_t val){
    return val << 20;
}

uint32_t stype(uint32_t val){
    return Bits_32(val,11,5) << 25 | Bits_32(val,4,0) << 7;
}

uint32_t btype(uint32_t val){
    return Bit_32(val,12) << 31 | Bits_32(val,10,5) << 25 |
            Bits_32(val,4,1) << 8 | Bit_32(val,11) << 7;
}

uint32_t utype(uint32_t val){
    return (val + 0x800) & 0xfffff000;
}

uint32_t jtype(uint32_t val){
    return Bit_32(val,20) << 31 | Bits_32(val,10,1) << 21 |
            Bit_32(val,11) << 20 | Bits_32(val,19,12) << 12;
}

void writeItype(void* loc, uint32_t val) {
    uint32_t mask = 0b00000000000011111111111111111111;
    uint32_t v ;
    Read(&v,loc,sizeof (uint32_t));
    v = (v & mask) | itype(val);
    Write(loc,sizeof (uint32_t),&v);
}

void writeStype(void* loc, uint32_t val) {
    uint32_t mask = 0b0000001111111111111000001111111;
    uint32_t v ;
    Read(&v,loc,sizeof (uint32_t));
    v = (v & mask) | stype(val);
    Write(loc,sizeof (uint32_t),&v);
}

void writeBtype(void* loc, uint32_t val) {
    uint32_t mask = 0b0000001111111111111000001111111;
    uint32_t v ;
    Read(&v,loc,sizeof (uint32_t));
    v = (v & mask) | btype(val);
    Write(loc,sizeof (uint32_t),&v);
}

void writeUtype(void* loc, uint32_t val) {
    uint32_t mask = 0b0000000000000000000111111111111;
    uint32_t v ;
    Read(&v,loc,sizeof (uint32_t));
    v = (v & mask) | utype(val);
    Write(loc,sizeof (uint32_t),&v);
}

void writeJtype(void* loc, uint32_t val) {
    uint32_t mask = 0b0000000000000000000111111111111;
    uint32_t v ;
    Read(&v,loc,sizeof (uint32_t));
    v = (v & mask) | jtype(val);
    Write(loc,sizeof (uint32_t),&v);
}

void setRs1(void* loc,uint32_t rs1){
    uint32_t mask = 0b1111111111100000111111111111111;
    uint32_t v ;
    Read(&v,loc,sizeof (uint32_t));
    v = v & mask;
    Write(loc,sizeof (uint32_t),&v);

    Read(&v,loc,sizeof (uint32_t));
    v = v | rs1 << 15;
    Write(loc,sizeof (uint32_t),&v);
}