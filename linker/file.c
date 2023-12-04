#include "file.h"
#include "util.h"

File* NewFile(const char* filename) {
    File* file = (File*)malloc(sizeof(File));
    char* contents = ReadFile(filename,&file->contents_len);
    if (contents == NULL) {
        return NULL;
    }

    file->Name = strdup(filename);
    file->Contents = contents;

    return file;
}

File* OpenLibrary(const char* filepath) {

    File* library = (File*)malloc(sizeof(File));
    char* contents = ReadFile(filepath, &library->contents_len);
    if (contents == NULL) {
        return NULL;
    }
    if (library == NULL) {
      //  free(contents);
        return NULL;
    }

    library->Name = strdup(filepath);
    library->Contents = contents;

    return library;
}

File* FindLibrary(Context* ctx, const char* name) {
    for (size_t i = 0; i < ctx->Args.LibraryPathsCount; i++) {
        const char* dir = ctx->Args.LibraryPaths[i];
        size_t stemSize = strlen(dir) + strlen("/lib") + strlen(name) + strlen(".a") + 1;
        char* stem = (char*)malloc(stemSize);
        if (stem == NULL) {
            fatal("Failed to allocate memory");
            return NULL;
        }

        snprintf(stem, stemSize, "%s/lib%s.a", dir, name);

        File* library = OpenLibrary(stem);

        if (library != NULL) {
           // printf("lib : %s\n",stem);
           // free(stem);
            return library;
        }
       // free(stem);
    }

    fatal("Library not found");
    return NULL;
}