#include "input.h"

extern HashMap *name_map;

void ReadInputFiles(Context* ctx,char** remaining){
    name_map = HashMapInit();
    for(int i =0;remaining[i];i++){
        char* arg = remaining[i];
        if(hasPrefix(arg,"-l")){
          //  printf("a   .%s \n ",arg);
            char* filename = removePrefix(arg,"-l");
            File *f = FindLibrary(ctx,filename);
            readFile(ctx, f);
           // printf("%d\n",ctx->ObjsCount);
        } else{
           // printf("%s  \n",arg);
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
           // printf("file name o :%s\n",file->Name);
            AddObjectFile(&ctx->Objs,&ctx->ObjsCount, CreateObjectFile(ctx,file,false));
            break;
        case FileTypeArchive:
           // printf("file name a :%s\n",file->Name);
            aFiles = ReadArchiveMembers(file,&fileCount);
            for(int i = 0;i<fileCount;i++){
                File *child = aFiles[i];
                assert(GetFileType(child->Contents) == FileTypeObject);
                AddObjectFile(&ctx->Objs,&ctx->ObjsCount, CreateObjectFile(ctx,child,true));
            }
            break;
        default:
            fatal("unknown file type\n");
    }
}

ObjectFile *CreateObjectFile(Context *ctx,File* file,bool inLib){
    //TODO CheckFileCompatibility
    ObjectFile * objectFile = NewObjectFile(file,!inLib);
    Parse(ctx,objectFile);
    return objectFile;
}