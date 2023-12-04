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

int GetSize(const ArHdr* a) {
    char sizeStr[11];
    strncpy(sizeStr, a->Size, sizeof(sizeStr) - 1);
    sizeStr[sizeof(sizeStr) - 1] = '\0';
    int size = atoi(sizeStr);
    return size;
}

bool IsAbs(const Sym* s) {
    return s->Shndx == 0; // Assuming elf.SHN_ABS is defined as 0
}

bool IsUndef(const Sym* s) {
    return s->Shndx == 65521; // Assuming elf.SHN_UNDEF is defined as 65521
}

bool IsCommon(const Sym* s) {
    return s->Shndx == 65522; // Assuming elf.SHN_COMMON is defined as 65522
}

bool HasPrefix(const ArHdr* a, const char* s) {
    return strncmp(a->Name, s, strlen(s)) == 0;
}

bool IsStrtab(const ArHdr* a) {
    return HasPrefix(a, "// ");
}

bool IsSymtab(const ArHdr* a) {
    return HasPrefix(a, "/ ") || HasPrefix(a, "/SYM64/ ");
}

char* ReadName(const ArHdr* a, char* strTab) {
    // Long filename
    if (a->Name[0] == '/') {
        int start = atoi(a->Name + 1);
        int end = start;
        char* endPtr = strstr(strTab + start, "/\n");
        if (endPtr != NULL) {
            end += (endPtr - (strTab + start));
        }
        char* filename = (char*)malloc(end - start + 1);
        memcpy(filename, strTab + start, end - start);
        filename[end - start] = '\0';
        return filename;
    }

    // Short filename
    char* endPtr = strchr(a->Name, '/');
   // Assert(endPtr != NULL);
    int end = endPtr - a->Name;
    char* filename = (char*)malloc(end + 1);
    memcpy(filename, a->Name, end);
    filename[end] = '\0';
    return filename;
}