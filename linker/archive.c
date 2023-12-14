#include "archive.h"
/*
参考资料：https://en.wikipedia.org/wiki/Ar_(Unix)
*/

File** appendFile(File** files, File* newFile, int* count) {
    *count += 1;
    files = (File**)realloc(files, (*count + 1) * sizeof(File*));
    files[*count - 1] = newFile;
    files[*count] = NULL;
    return files;
}

// ReadArchiveMembers 静态库可以被看作是一组目标文件（.o 或 .obj 文件）的组合
// 读取并返回归档的静态文件中的所有目标文件
// [!<arch>\n][Section ][Section ]......
// [Section ] -> [ArHdr][                ][ArHdr][                ]
File** ReadArchiveMembers(File* file,int * fileCount) {
    if (GetFileType(file->Contents) != FileTypeArchive) {
        return NULL;
    }

    //读取位置跳过文件头
    int pos = 8;
    uint8_t* strTab = NULL;
    File** files = NULL;

    //考虑align 2，为1即可停下
    while (file->contents_len - pos > 1){
        if (pos % 2 == 1) {
            pos++;
        }

        //拿到ArHdr header
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

        //如果section既不是arch独有的symtab也不是strtab , 就是原来obj文件本身的内容
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