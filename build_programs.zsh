#!/bin/zsh

set -euo pipefail

mkdir -p build/programs

set -x

customasm -q -o build/programs/hello_world.bin programs/hello_world.asm
./build/prepend_size build/programs/hello_world.bin

customasm -q -o build/programs/echo.bin programs/echo.asm
./build/prepend_size build/programs/echo.bin

customasm -q -o build/programs/gpo_sweep.bin programs/gpo_sweep.asm
./build/prepend_size build/programs/gpo_sweep.bin

customasm -q -o build/programs/blink.bin programs/blink.asm
./build/prepend_size build/programs/blink.bin
