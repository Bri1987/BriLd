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
    uint8_t Ident[16];      //表示ELF文件的标识信息
    uint16_t  Type;         //表示 ELF 文件的类型，比如可执行文件、共享库等
    uint16_t  Machine;
    uint32_t  Version;
    uint64_t  Entry ;       //表示程序的入口地址
    uint64_t  PhOff ;       //表示程序头表（Program Header Table）的偏移量
    uint64_t  ShOff ;       //表示节表（Section Header Table）的偏移量
    uint32_t  Flags ;       //表示 ELF 文件的标志信息
    uint16_t  EhSize;       //表示 ELF 文件头部的大小  sizeof(Elf32_Ehdr);
    uint16_t  PhEntSize ;   //表示program header table中每个表项的大小，每一个表项描述一个Segment信息
    uint16_t  PhNum ;       //表示program header table中表项的数量
    uint16_t  ShEntSize;    //Section header table中每个表项的大小sizeof(Elf32_Shdr)
    uint16_t  ShNum ;       //num sections
    uint16_t  ShStrndx ;    //表示节表中字符串表的索引，第多少个表项描述的是字符串表......
}Ehdr;

// Elf32_Shdr Section header
typedef struct {
    uint32_t Name;          //section名称，是在字符串表节区的索引
    uint32_t Type;
    uint64_t Flags;         //描述杂项属性
    uint64_t Addr;          //如果该section将出现在进程的内存映像中，则该成员给出该section的第一个字节应该驻留的地址。否则，该成员的值为0
    uint64_t Offset;        //该节在elf文件中偏移量
    uint64_t Size;          //该节的大小
    uint32_t Link;          //holds a section header table index link,表示当前节依赖于对应的节
    uint32_t Info;          //该节的附加信息， 如符号表节中存储的第一个global的信息
    uint64_t AddrAlign;     //该节的对齐方式
    uint64_t EntSize;       //某些节区中包含固定大小的项目，如符号表节中每个符号的大小,没有则是0
} Shdr;

typedef struct {
    uint32_t Name;          //存储一个指向字符串表的索引来表示对应符号的名称
    uint8_t Info;
    uint8_t Other;
    uint16_t Shndx;         //每个符号都有属于的节，当前成员存储的就是对应节的索引
    uint64_t Val;           //存储对应符号的取值，具体值依赖于上下文，可能是一个指针地址，立即数等
    uint64_t Size;
} Sym;

typedef struct {
    uint64_t Offset;
    uint32_t Type;
    uint32_t Sym;
    int64_t Addend;
} Rela;

// [Section ] -> [ArHdr][                ][ArHdr][
typedef struct {
    char Name[16];
    char Date[12];
    char Uid[6];
    char Gid[6];
    char Mode[8];
    char Size[10];          //即[ArHdr][这部分的size]
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
