#!/bin/zsh

set -euo pipefail

ROM="build/custom-cpu_alu.bin"
DEVICE="SST39SF040"

echo 'Place ALU ROM in burner..'; read -k1 -s

set -x

minipro --device "$DEVICE" --write "$ROM"
minipro --device "$DEVICE" --verify "$ROM"
minipro --device "$DEVICE" --verify "$ROM"
minipro --device "$DEVICE" --verify "$ROM"

cp "$ROM" "$ROM.burned"
