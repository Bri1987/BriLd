#include "output.h"

char* GetOutputName(char* name, uint64_t flags) {
    const char* prefixes_[] = {
            ".text.", ".data.rel.ro.", ".data.", ".rodata.", ".bss.rel.ro.", ".bss.",
            ".init_array.", ".fini_array.", ".tbss.", ".tdata.", ".gcc_except_table.",
            ".ctors.", ".dtors."
    };

    //merge-able section一定是readonly的
    if ((strcmp(name, ".rodata") == 0 || strncmp(name, ".rodata.", 8) == 0) && (flags & SHF_MERGE) != 0) {
        if ((flags & SHF_STRINGS) != 0) {
            return ".rodata.str";
        } else {
            return ".rodata.cst";
        }
    }

    //.text或者.text.1 .text.* , 返回.text
    //即都将这些section映射到.text
    //多对一的映射，即多个inputsection映射到一个outputsection中
    size_t prefixesCount = sizeof(prefixes_) / sizeof(prefixes_[0]);
    for (size_t i = 0; i < prefixesCount; i++) {
        const char* prefix = prefixes_[i];
        size_t prefixLength = strlen(prefix);
        char* stem = malloc(strlen(prefix));
        strncpy(stem,prefix, strlen(prefix)-1);
        stem[strlen(prefix) - 1] = '\0';

        if (prefixLength > 0) {
            prefixLength--;
            if (strcmp(name, stem) == 0 || strncmp(name, prefix, prefixLength) == 0) {
               // printf("_stem %s\n",stem);
                return stem;
            }
        } else {
            //prefixLength = 0
            //printf("prefixLength : %zu\n",prefixLength);
            if (strcmp(name, stem) == 0) {
               // printf("_stem %s\n",stem);
                return stem;
            }
        }
    }

   // printf("_name %s\n",name);
    return name;
}