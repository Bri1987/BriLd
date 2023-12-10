#ifndef BRILINKER_FILETYPE_H
#define BRILINKER_FILETYPE_H

#include "utils.h"
#include "elf_std.h"

typedef uint8_t FileType;

#define FileTypeUnknown ((FileType)0)
#define FileTypeEmpty   ((FileType)1)
#define FileTypeObject  ((FileType)2)
#define FileTypeArchive ((FileType)3)

FileType GetFileType(const char* contents);

#endif //BRILINKER_FILETYPE_H
