#ifndef BRILD_UTIL_H
#define BRILD_UTIL_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void fatal(const char* format, ...);
char* ReadFile(const char* filename);
void Read(void* out, const void* data, size_t size);
char** appendToRemaining(char** remaining, const char* arg);

#endif //BRILD_UTIL_H
