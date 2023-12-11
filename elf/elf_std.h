#ifndef BRILINKER_ELF_STD_H
#define BRILINKER_ELF_STD_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

// Elf32_Ehdr Executable header type. One per ELF file.
typedef struct Ehdr_ {
    uint8_t Ident[16];
    uint16_t  Type;
    uint16_t  Machine;
    uint32_t  Version;
    uint64_t  Entry ;
    uint64_t  PhOff ;
    uint64_t  ShOff ;
    uint32_t  Flags ;
    uint16_t  EhSize;
    uint16_t  PhEntSize ;
    uint16_t  PhNum ;
    uint16_t  ShEntSize;
    uint16_t  ShNum ;
    uint16_t  ShStrndx ;
}Ehdr;

// Elf32_Shdr Section header
typedef struct {
    uint32_t Name;
    uint32_t Type;
    uint64_t Flags;
    uint64_t Addr;
    uint64_t Offset;
    uint64_t Size;
    uint32_t Link;
    uint32_t Info;
    uint64_t AddrAlign;
    uint64_t EntSize;
} Shdr;

typedef struct {
    uint32_t Name;
    uint8_t Info;
    uint8_t Other;
    uint16_t Shndx;
    uint64_t Val;
    uint64_t Size;
} Sym;

typedef struct {
    uint64_t Offset;
    uint32_t Type;
    uint32_t Sym;
    int64_t Addend;
} Rela;

typedef struct {
    char Name[16];
    char Date[12];
    char Uid[6];
    char Gid[6];
    char Mode[8];
    char Size[10];
    char Fmag[2];
} ArHdr;

typedef struct {
    uint32_t Type;
    uint32_t Flags;
    uint64_t Offset;
    uint64_t VAddr;
    uint64_t PAddr;
    uint64_t FileSize;
    uint64_t MemSize;
    uint64_t Align;
} Phdr;

bool CheckMagic(const char* contents);
void WriteMagic(uint8_t * contents);
char* ElfGetName(char* strTab, uint32_t offset);
int GetSize(const ArHdr* a);
bool IsAbs(const Sym* s);
bool IsUndef(const Sym* s);
bool IsCommon(const Sym* s);
bool HasPrefix(const ArHdr* a, const char* s);
bool IsStrtab(const ArHdr* a);
bool IsSymtab(const ArHdr* a);
char* ReadName(const ArHdr* a, char* strTab);

#endif //BRILINKER_ELF_STD_H
