#!/bin/zsh

set -euo pipefail

mkdir -p build/rom

flags=(
    -fsanitize=undefined,integer,nullability
    -ferror-limit=4
    -Werror
    -Wall
    -Wpedantic
    -Wconversion
    -Wsign-compare
    -Wswitch-enum
    -Wno-gnu-binary-literal
    -Wunused-variable
    -Wunused-function
    -Wunused-but-set-variable
    -Wunused-parameter
    -std=c17
    --debug)

set -x

clang "${flags[@]}" -o ./build/prepend_size prepend_size.c

clang "${flags[@]}" -o ./build/control_roms control_roms.c

pushd ./build && UBSAN_OPTIONS="halt_on_error=1 report_error_type=1 print_stacktrace=1" ./control_roms ; popd

set +x
customasm -q -dPROGRAM_START_ADDRESS=0x0000 rom/boot.asm -f symbols -p | grep "^uart_" | grep -v "\." > build/rom/symbols.inc
