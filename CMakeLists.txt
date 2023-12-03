cmake_minimum_required(VERSION 3.22)
project(BriLinker C)

set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE DIR_ELF ./elf/*.c)
file(GLOB_RECURSE DIR_LD ./ld/*.c)
file(GLOB_RECURSE DIR_UTILS ./utils/*.c)

#include_directories(.)
include_directories(elf)
include_directories(ld)
include_directories(utils)

add_executable(ld main.c ${DIR_ELF} ${DIR_LD} ${DIR_UTILS})
