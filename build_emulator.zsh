#!/bin/zsh

set -euo pipefail

flags=(
    -O3
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

clang "${flags[@]}" -o ./build/emulator emulator.c

cd ./build && UBSAN_OPTIONS="halt_on_error=1 report_error_type=1 print_stacktrace=1" ./emulator
