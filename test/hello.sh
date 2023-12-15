#!/bin/bash

test_name=$(basename "$0" .sh)
t=out/tests/$test_name

mkdir -p "$t"

cat <<EOF | riscv64-linux-gnu-gcc -o "$t"/a.o -c -xc -
#include <stdio.h>

int main(void) {
    printf("hello chaochao !\n");
    return 0;
}
EOF

riscv64-linux-gnu-gcc -B./cmake-build-debug/ -static "$t"/a.o -o "$t"/out -O0
qemu-riscv64 "$t"/out