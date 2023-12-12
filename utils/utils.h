#ifndef BRILD_UTIL_H
#define BRILD_UTIL_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "hash_map.h"
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>

void fatal(const char* format, ...);
char* ReadFile(const char* filename,uint64_t *len);
void Read(void* out, const void* data, size_t size);
char** appendToRemaining(char** remaining, const char* arg,bool l);
char* removePrefix(const char* s, const char* prefix);
bool hasPrefix(const char* s, const char* prefix);
int endsWith(const char *str, const char *suffix);
uint32_t hash(const char* str);
void* convertHashToKey(uint32_t hashValue);
uint64_t AlignTo(uint64_t val, uint64_t align);
void Write(void* data, size_t dataSize, void* element);

#endif //BRILD_UTIL_H
