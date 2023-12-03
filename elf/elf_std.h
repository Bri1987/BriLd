#ifndef BRILINKER_ELF_STD_H
#define BRILINKER_ELF_STD_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

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



bool CheckMagic(const char* contents);
void WriteMagic(char* contents);

#endif //BRILINKER_ELF_STD_H
