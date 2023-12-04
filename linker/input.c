#include "input.h"

void ReadInputFiles(Context* ctx,char** remaining){
    for(int i =0;remaining[i];i++){
        char* arg = remaining[i];
        if(hasPrefix(arg,"-l")){
            //printf("a   .%s  ",arg);
            char* filename = removePrefix(arg,"-l");
            File *f = FindLibrary(ctx,filename);
            readFile(ctx, f);
            //printf("%d\n",ctx->ObjsCount);
        } else{
            //printf("%s  ",arg);
            readFile(ctx, NewFile(arg));
           // printf("%d\n",ctx->ObjsCount);
        }
    }
}

void readFile(Context *ctx,File* file){
    FileType ft = GetFileType(file->Contents);
    int fileCount = 0;
    File **aFiles = NULL;
    switch (ft) {
        case FileTypeObject:
            AddObjectFile(&ctx->Objs,&ctx->ObjsCount, CreateObjectFile(file));
            break;
        case FileTypeArchive:
            aFiles = ReadArchiveMembers(file,&fileCount);
            for(int i = 0;i<fileCount;i++){
                File *child = aFiles[i];
                assert(GetFileType(child->Contents) == FileTypeObject);
                AddObjectFile(&ctx->Objs,&ctx->ObjsCount, CreateObjectFile(child));
            }
            break;
        default:
            fatal("unknown file type\n");
    }
}

ObjectFile *CreateObjectFile(File* file){
    ObjectFile * objectFile = NewObjectFile(file);
    Parse(objectFile);
    return objectFile;
}