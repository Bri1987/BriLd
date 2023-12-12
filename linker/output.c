#include "output.h"

char* GetOutputName(char* name, uint64_t flags) {
    const char* prefixes_[] = {
            ".text.", ".data.rel.ro.", ".data.", ".rodata.", ".bss.rel.ro.", ".bss.",
            ".init_array.", ".fini_array.", ".tbss.", ".tdata.", ".gcc_except_table.",
            ".ctors.", ".dtors."
    };

    if ((strcmp(name, ".rodata") == 0 || strncmp(name, ".rodata.", 8) == 0) && (flags & SHF_MERGE) != 0) {
        if ((flags & SHF_STRINGS) != 0) {
            return ".rodata.str";
        } else {
            return ".rodata.cst";
        }
    }

    size_t prefixesCount = sizeof(prefixes_) / sizeof(prefixes_[0]);
    for (size_t i = 0; i < prefixesCount; i++) {
        const char* prefix = prefixes_[i];
        size_t prefixLength = strlen(prefix);
        const char* stem = prefix;

        if (prefixLength > 0) {
            prefixLength--;
            if (strcmp(name, stem) == 0 || strncmp(name, prefix, prefixLength) == 0) {
                return stem;
            }
        } else {
            //prefixLength = 0
            printf("prefixLength : %zu\n",prefixLength);
            if (strcmp(name, stem) == 0) {
                return stem;
            }
        }
    }

    return name;
}