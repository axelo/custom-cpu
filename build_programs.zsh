#!/bin/zsh

set -euo pipefail

mkdir -p build/programs

set -x

customasm -q -o build/programs/hello_world.bin programs/hello_world.asm
./build/prepend_size build/programs/hello_world.bin
