#include "file.h"
#include "util.h"

File* NewFile(const char* filename) {
    char* contents = ReadFile(filename);
    if (contents == NULL) {
        return NULL;
    }

    File* file = (File*)malloc(sizeof(File));
    if (file == NULL) {
        free(contents);
        return NULL;
    }

    file->Name = strdup(filename);
    file->Contents = contents;

    return file;
}