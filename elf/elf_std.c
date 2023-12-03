#include "elf_std.h"

bool CheckMagic(const char* contents) {
    const unsigned char magic[] = {0x7F, 'E', 'L', 'F'};
    size_t magicSize = sizeof(magic) / sizeof(magic[0]);

    return memcmp(contents, magic, magicSize) == 0;
}

void WriteMagic(char* contents) {
    const char magic[] = {0x7F, 'E', 'L', 'F'};
    size_t magicSize = sizeof(magic) / sizeof(magic[0]);

    memcpy(contents, magic, magicSize);
}

char* ElfGetName(const char* strTab, uint32_t offset) {
    uint32_t length = 0;
    while (strTab[offset + length] != '\0') {
        length++;
    }
    char* name = (char*)malloc(length + 1);
    memcpy(name, strTab + offset, length);
    name[length] = '\0';
    return name;
}