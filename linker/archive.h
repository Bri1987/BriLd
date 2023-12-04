#ifndef BRILINKER_ARCHIVE_H
#define BRILINKER_ARCHIVE_H

#include "util.h"
#include "filetype.h"
#include "file.h"

File** ReadArchiveMembers(File* file,int * fileCount);

#endif //BRILINKER_ARCHIVE_H
