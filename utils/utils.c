#include "utils.h"

void fatal(const char* format, ...) {
    va_list args;
    va_start(args, format);

    printf("fatal: ");
    vprintf(format, args);
    printf("\n");

    va_end(args);

    exit(1);
}

// Function to read the entire content of a file into a dynamically allocated buffer
char* ReadFile(const char* filename,uint64_t *len) {
    FILE* file;
    char* buffer;
    size_t file_size;

    // Open the file
    file = fopen(filename, "rb");
    if (file == NULL) {
        //perror("Error opening file");
        return NULL;
    }

    // Find the size of the file
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    // Allocate memory for the entire content
    buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    if (fread(buffer, 1, file_size, file) != file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    // Null-terminate the buffer
    buffer[file_size] = '\0';
    *len = file_size;

    // Close the file
    fclose(file);
    return buffer;
}

void Read(void* out, const void* data, size_t size) {
    // Assuming little-endian format
    memcpy(out, data, size);
}

char** appendToRemaining(char** remaining, const char* arg,bool l) {
    size_t length = 0;

    // 计算 remaining 的长度
    while (remaining && remaining[length] != NULL) {
        length++;
    }

    // 分配新的内存来存储扩展后的 remaining
    char** newRemaining = (char**)realloc(remaining, sizeof(char*) * (length + 2));

    char* newEntry = NULL;
    if(l){
        // 在新的 remaining 中添加 "-l"+arg
        newEntry = (char*)malloc(strlen(arg) + 3); // 预留 3 字节用于 "-l" 前缀
        strcpy(newEntry, "-l");
        strcat(newEntry, arg);
    } else {
        newEntry = (char*)malloc(strlen(arg));
        strcpy(newEntry,arg);
    }
    newRemaining[length] = newEntry;
    newRemaining[length + 1] = NULL; // remaining 最后一个元素置为 NULL

    return newRemaining;
}

// 移除字符串前缀
char* removePrefix(const char* s, const char* prefix) {
    size_t prefixLen = strlen(prefix);
    size_t sLen = strlen(s);

    // 检查 s 是否以 prefix 开头
    if (strncmp(s, prefix, prefixLen) == 0) {
        // 动态分配内存以存储移除前缀后的字符串
        char* result = (char*)malloc(sLen - prefixLen + 1);
        strcpy(result, s + prefixLen);
        return result;
    }

    // 如果不是以 prefix 开头，动态分配内存以存储原始字符串
    char* result = (char*)malloc(sLen + 1);
    strcpy(result, s);
    return result;
}

// 检查字符串是否以指定前缀开头
bool hasPrefix(const char* s, const char* prefix) {
    size_t prefixLen = strlen(prefix);

    // 检查 s 是否以 prefix 开头
    if (strncmp(s, prefix, prefixLen) == 0) {
        return true;
    }

    return false;
}

int endsWith(const char *str, const char *suffix) {
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);

    if (str_len >= suffix_len && strcmp(str + str_len - suffix_len, suffix) == 0) {
        return 1; // 字符串以指定后缀结尾
    } else {
        return 0; // 字符串不以指定后缀结尾
    }
}

//uint32_t hash(const char* str, uint32_t offset) {
//    const uint32_t FNV_PRIME = 16777619;
//    const uint32_t FNV_OFFSET_BASIS = 2166136261;
//
//    uint32_t hashValue = FNV_OFFSET_BASIS;
//
//    // 计算字符串的哈希值
//    for (const char* c = str; *c != '\0'; ++c) {
//        hashValue ^= (uint32_t)*c;
//        hashValue *= FNV_PRIME;
//    }
//
//    // 综合加上 offset 的值
//    hashValue ^= offset;
//    hashValue *= FNV_PRIME;
//
//    return hashValue;
//}

uint32_t hash(const char* str) {
    const uint32_t FNV_PRIME = 16777619;
    const uint32_t FNV_OFFSET_BASIS = 2166136261;

    uint32_t hashValue = FNV_OFFSET_BASIS;

    // 计算字符串的哈希值
    for (const char* c = str; *c != '\0'; ++c) {
        hashValue ^= (uint32_t)*c;
        hashValue *= FNV_PRIME;
    }

    return hashValue;
}

void* convertHashToKey(uint32_t hashValue) {
    // 将哈希值转换为指针类型
    return (void*)(uintptr_t)hashValue;
}

uint64_t AlignTo(uint64_t val, uint64_t align) {
    if (align == 0) {
        return val;
    }

    return (val + align - 1) & /*clear bit*/(~(align - 1));
}