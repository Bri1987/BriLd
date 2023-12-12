#include <stdio.h>
#include "file.h"
#include "elf_std.h"
#include "input.h"
#include "passes.h"
#include "chunk.h"

HashMap *name_map;

char** dashes(const char* name) {
    // 分配足够的内存来存储带有单破折号和双破折号的参数
    char** result = (char**)malloc(3 * sizeof(char*));
    result[0] = (char*)malloc((strlen(name) + 2) * sizeof(char));
    result[1] = (char*)malloc((strlen(name) + 3) * sizeof(char));
    result[2] = NULL;

    // 填充参数
    sprintf(result[0], "-%s", name);
    sprintf(result[1], "--%s", name);

    return result;
}
int readFlag(const char* name, char*** args) {
    char** opts = dashes(name);

    for (int i = 0; opts[i]; i++) {
        if (*args[0] && strcmp(*args[0], opts[i]) == 0) {
            (*args)++;

          //  printf("flag : %s\n",name);
            return 1; // 找到选项
        }
    }

    return 0; // 未找到选项
}

// 实现 readArg 函数
int readArg(const char* name, char*** args, char** arg) {
    char** opts = dashes(name);


    for(int i = 0; opts[i]; i++){
//        char* t1 = *(args[0]);
//        char* t2 = opts[i];
        if(strcmp(*(args[0]),opts[i])==0){
            if(strlen(*(args[0])) == 1)
                fatal("arg missing\n");
            *arg = (*args)[1];
            (*args) += 2;

//            printf("\nname : %s\n",name);
//            printf("args : %s\n",*arg);
            return 1;
        }

        char* prefix =opts[i];
        if (name[1] != '\0') {
            // 对于长选项，在前缀后添加 '=' 符号
            size_t len = strlen(opts[i]) + 2; // 包括字符串结束符 '\0' 和 '=' 符号
            char* tempPrefix = (char*)malloc(len);
            snprintf(tempPrefix, len, "%s=", opts[i]);
            prefix = tempPrefix;
        }

        if (strncmp((*args)[0], prefix, strlen(prefix)) == 0) {
            *arg = (*args)[0] + strlen(prefix);
            (*args) += 1;

//            printf("\nname : %s\n",name);
//            printf("args : %s\n",*arg);
            return 1;
        }
    }

    return 0; // 未找到选项
}

char** parseArgs(int argc, char* argv[],Context* ctx){
    char** remaining = NULL;
    //忽略第一个
    argv += 1;

    while (argv[0] != NULL){
        char *arg;
        if(readArg("output",&argv,&arg) || readArg("o",&argv,&arg)){
            ctx->Args.Output = malloc(strlen(arg));
            memcpy(ctx->Args.Output,arg,strlen(arg));
        } else if(readArg("m",&argv,&arg)){
            if(strcmp(arg,"elf64lriscv")==0){
                ctx->Args.Emulation = MachineTypeRISCV64;
                printf("%s\n",MachineType_String(ctx->Args.Emulation));
            } else
                fatal("unknown -m arg\n");
        }else if (readArg("L",&argv,&arg)) {
//            printf("%s\n",argv[0]);
//            printf("arg : %s\n",arg);
            appendLibraryPath(ctx,arg);
        } else if (readArg("l",&argv,&arg)) {
            remaining = appendToRemaining(remaining,arg,true);
        }
        else if (readArg("sysroot",&argv,&arg) ||
                 readFlag("static",&argv) ||
                readArg("z",&argv,&arg) ||
                readArg("plugin",&argv,&arg) ||
                 readArg("plugin-opt",&argv,&arg) ||
                 readFlag("as-needed",&argv) ||
                 readFlag("start-group",&argv) ||
                 readFlag("end-group",&argv) ||
                 readArg("hash-style",&argv,&arg) ||
                 readArg("build-id",&argv,&arg) ||
                 readFlag("s",&argv) ||
                 readFlag("no-relax",&argv) ||
                 readFlag("v",&argv) || readFlag("version",&argv)){
            // Ignored
        } else{
            if(argv[0][0] == '-'){
                printf("%s\n",argv[0]);
                fatal("wrong arg!!!!\n");
            }

//            printf("file :%s\n",argv[0]);
//            printf("%s\n",MachineType_String(ctx->Args.Emulation));
//            printf("count :%d\n",ctx->Args.LibraryPathsCount);
            //.so不处理了
            if(!endsWith(argv[0],"so")){
                remaining = appendToRemaining(remaining,argv[0],false);
            }
            argv += 1;
        }
    }

    //TODO filepath clean
//    for(int j = 0;j<ctx->Args.LibraryPathsCount;j++){
//
//    }
    return remaining;
}

int main(int argc, char* argv[]) {
    if(argc < 2)
        fatal("less args\n");
//    for(int i = 0;i<argc;i++){
//        printf("%s\n",argv[i]);
//    }

    //name_map = HashMapInit();
    Context *ctx = NewContext();
    char **remaining = parseArgs(argc,argv,ctx);

    if (ctx->Args.Emulation == MachineTypeNone) {
        for (int i = 0; remaining[i]!=NULL; i++) {
            const char* filename = remaining[i];
            if (filename[0] == '-' && filename[1] != '\0') {
                continue;
            }

            File* file= NewFile(filename);
            ctx->Args.Emulation = GetMachineTypeFromContents(file->Contents);
            if (ctx->Args.Emulation != MachineTypeNone) {
                break;
            }
        }
    }

    if (ctx->Args.Emulation != MachineTypeRISCV64) {
        printf("%d\n",ctx->Args.Emulation);
        fatal("unknown emulation type");
    }

//    for (size_t i = 0; remaining[i] != NULL; ++i) {
//        printf("%s\n", remaining[i]);
//    }

    ReadInputFiles(ctx,remaining);
    printf("%d\n",ctx->ObjsCount);
//    printf("symbols :%d\n", HashMapSize(ctx->SymbolMap));
//    HashMapFirst(ctx->SymbolMap);
//    for(Pair* p = HashMapNext(ctx->SymbolMap); p!=NULL; p = HashMapNext(ctx->SymbolMap)){
//        printf("%s\n",p->key);
//    }
//    for(int i=0;i<ctx->ObjsCount;i++){
//        printf("%d : %s\n",i,ctx->Objs[i]->inputFile->file->Name);
//    }
    ResolveSymbols_pass(ctx);
    printf("final %d\n",ctx->ObjsCount);
//    for(int i=0;i<ctx->ObjsCount;i++){
//        printf("%s\n",ctx->Objs[i]->inputFile->file->Name);
//    }
    RegisterSectionPieces(ctx);
    CreateSyntheticSections(ctx);
    BinSections(ctx);
    CollectOutputSections(ctx);
    ComputeSectionSizes(ctx);

    for(int i=0; i<ctx->chunkNum;i++){
        Chunk *c = ctx->chunk[i];
        Update(c,ctx);
    }

    uint64_t fileoff = SetOutputSectionOffsets(ctx);
    printf("fileoff %lu\n",fileoff);
    ctx->buf = malloc(fileoff);
    //printf("%s\n",ctx->Args.Output);
    int file = open(ctx->Args.Output, O_RDWR | O_CREAT, 0777);
    assert(file != -1);

    for(int i=0; i< ctx->chunkNum;i++){
        Chunk *c = ctx->chunk[i];
        CopyBuf(c,ctx);
    }
    write(file, ctx->buf, fileoff);
    close(file);

    return 0;
}
