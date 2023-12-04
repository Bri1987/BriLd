#include <stdio.h>
#include "file.h"
#include "elf_std.h"
#include "objectfile.h"
#include "context.h"

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
            return 1; // 找到选项
        }
    }

    return 0; // 未找到选项
}

// 实现 readArg 函数
int readArg(const char* name, char*** args, char** arg) {
    char** opts = dashes(name);

    for(int i = 0; opts[i]; i++){
        char* t1 = *(args[0]);
        char* t2 = opts[i];
        if(strcmp(*(args[0]),opts[i])==0){
            if(strlen(*(args[0])) == 1)
                fatal("arg missing\n");
            *arg = (*args)[1];
            (*args) += 2;
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
            return 1;
        }
    }

    return 0; // 未找到选项
}

char** parseArgs(int argc, char* argv[],Context* ctx){
    char** remaining;
    //忽略第一个
    argv += 1;
    while (argv[0] != NULL){
        char *arg;
        if(readArg("output",&argv,&arg) || readArg("o",&argv,&arg)){
            ctx->Args.Output = malloc(strlen(arg));
            memcpy(ctx->Args.Output,arg,strlen(arg));
            printf("%s\n",ctx->Args.Output);
        } else if(readArg("m",&argv,&arg)){
            if(strcmp(arg,"elf64lriscv")==0){
                ctx->Args.Emulation = MachineTypeRISCV64;
                printf("%s\n",MachineType_String(ctx->Args.Emulation));
            } else
                fatal("unknown -m arg\n");
        }else if (readArg("L",&argv,&arg)) {
            appendLibraryPath(ctx,arg);
        } else if (readArg("l",&argv,&arg)) {
            remaining = appendToRemaining(remaining,arg);
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

            remaining = appendToRemaining(remaining,argv[0]);
            argv += 1;
        }
    }
    return remaining;
}

int main(int argc, char* argv[]) {
    if(argc < 2)
        fatal("less args\n");
//    for(int i = 0;i<argc;i++){
//        printf("%s\n",argv[i]);
//    }

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
        fatal("unknown emulation type");
    }

    for (size_t i = 0; remaining[i] != NULL; ++i) {
        printf("%s\n", remaining[i]);
    }

    File *file = NewFile(argv[1]);

    ObjectFile *objectFile = NewObjectFile(file);
    Parse(objectFile);
    for(int i=0;i<objectFile->inputFile->symNum;i++){
        printf("%s\n", ElfGetName(objectFile->inputFile->SymbolStrtab,objectFile->inputFile->ElfSyms[i].Name));
    }
    return 0;
}
