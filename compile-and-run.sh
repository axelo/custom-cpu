#!/bin/sh

# minipro --device SST39SF040 --write rom_alu.bin
# minipro --device SST39SF010 --write rom_signals0.bin
# minipro --device SST39SF010 --write rom_signals1.bin

set -ex

cc -Werror -Wall -Wpedantic -Wconversion -Wno-gnu-binary-literal -Wno-unused-function -fsanitize=undefined,integer,nullability -std=c17 -O3 --debug a-cpu-1.c -o a-cpu-1 && \
./a-cpu-1
