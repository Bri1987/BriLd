cmake_minimum_required(VERSION 3.18)
project(BriLinker C)

set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE DIR_ELF ./elf/*.c)
file(GLOB_RECURSE DIR_LD linker/*.c)
file(GLOB_RECURSE DIR_UTILS ./utils/*.c)
file(GLOB_RECURSE DIR_LIB ./lib/*.c)

#include_directories(.)
include_directories(elf)
include_directories(linker)
include_directories(utils)
include_directories(lib)

add_executable(ld main.c ${DIR_ELF} ${DIR_LD} ${DIR_UTILS} ${DIR_LIB})
