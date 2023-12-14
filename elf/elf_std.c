#include "elf_std.h"

extern HashMap *name_map;

bool CheckMagic(const char* contents) {
    const unsigned char magic[] = {0x7F, 'E', 'L', 'F'};
    size_t magicSize = sizeof(magic) / sizeof(magic[0]);

    return memcmp(contents, magic, magicSize) == 0;
}

void WriteMagic(uint8_t * contents) {
    const char magic[] = {0x7F, 'E', 'L', 'F'};
    size_t magicSize = sizeof(magic) / sizeof(magic[0]);

    memcpy(contents, magic, magicSize);
}

//获取一个section对应的名字 , 延展在section strtab和symbol strtab中根据name偏移拿到对应名字
char* ElfGetName(char* strTab, uint32_t offset) {
    uint32_t length = 0;
    while (strTab[offset + length] != '\0') {
        length++;
    }
    char* name = (char*)malloc(length + 1);
    memcpy(name, strTab + offset, length);
    name[length] = '\0';

    uint32_t hashName = hash(name);
    char* key = convertHashToKey(hashName);
    if(HashMapContain(name_map,key))
        return HashMapGet(name_map,key);

    HashMapPut(name_map,key,name);
    return name;
}

int GetSize(const ArHdr* a) {
    char sizeStr[11];
    strncpy(sizeStr, a->Size, sizeof(sizeStr) - 1);
    sizeStr[sizeof(sizeStr) - 1] = '\0';
    int size = atoi(sizeStr);
    return size;
}

//返回判断这一个符号是不是绝对符号,即值确定，不需要重定向, 不指向任何section
bool IsAbs(const Sym* s) {
    return s->Shndx == 65521; // Assuming elf.SHN_ABS is defined as 0
}

//未定义符号，需要链接器来找
bool IsUndef(const Sym* s) {
    return s->Shndx == 0; // Assuming elf.SHN_UNDEF is defined as 65521
}

//比如a.o和b.o中都声明或定义了一个全局变量，在a.o中进行定义，在b.o中进行声明，那么这个符号在b.o中就是common symbol
// 链接器最终处理时，只会加入a.o中的common symbol
bool IsCommon(const Sym* s) {
    return s->Shndx == 65522; // Assuming elf.SHN_COMMON is defined as 65522
}

bool HasPrefix(const ArHdr* a, const char* s) {
    return strncmp(a->Name, s, strlen(s)) == 0;
}

//一个archive通常只有一个strtab , strtab归档文件里面obj的名字
bool IsStrtab(const ArHdr* a) {
    return HasPrefix(a, "// ");
}

bool IsSymtab(const ArHdr* a) {
    return HasPrefix(a, "/ ") || HasPrefix(a, "/SYM64/ ");
}

//得到obj的文件名，如果名字不长，直接放在arhdr的name里，如果文件名字长，name放不下，才去strtab里找
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