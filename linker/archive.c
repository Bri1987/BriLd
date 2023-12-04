#include "archive.h"

File** appendFile(File** files, File* newFile, int* count) {
    *count += 1;
    files = (File**)realloc(files, (*count + 1) * sizeof(File*));
    files[*count - 1] = newFile;
    files[*count] = NULL;
    return files;
}

File** ReadArchiveMembers(File* file,int * fileCount) {
    if (GetFileType(file->Contents) != FileTypeArchive) {
        return NULL;
    }

    int pos = 8;
    uint8_t* strTab = NULL;
    File** files = NULL;

    while (file->contents_len - pos > 1){
        if (pos % 2 == 1) {
            pos++;
        }

        ArHdr *hdr = (ArHdr*) malloc(sizeof (ArHdr));
        Read(hdr,file->Contents+pos,sizeof(ArHdr));

        int dataStart = pos + sizeof(ArHdr);
        pos = dataStart + GetSize(hdr);
        int dataEnd = pos;

        uint8_t* contents = (uint8_t*)malloc(dataEnd - dataStart);
        memcpy(contents, file->Contents + dataStart, dataEnd - dataStart);

        if (IsSymtab(hdr)) {
            continue;
        } else if (IsStrtab(hdr)) {
            strTab = malloc(dataEnd-dataStart);
            memcpy(strTab,contents,dataEnd-dataStart);
            continue;
        }

        File* newFile = (File*)malloc(sizeof(File) + 1);
        char* name = ReadName(hdr,(char *)strTab);
       // printf("%s %lu\n",name,strlen(name));
        newFile->Name = name;
        newFile->Contents = (char*) malloc(dataEnd-dataStart);
        memcpy(newFile->Contents,contents,dataEnd-dataStart);
        newFile->contents_len = dataEnd - dataStart;
        newFile->Parent = file;
        //printf("%s\n",name);

        files = appendFile(files, newFile, fileCount);
    }
    return files;
}