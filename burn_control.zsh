#!/bin/zsh

set -euo pipefail

ROM="build/custom-cpu_control.bin"
DEVICE="SST39SF010"

echo 'Place CONTROL0 ROM in burner..'; read -k1 -s

set -x

minipro --device "$DEVICE" --write "$ROM"
minipro --device "$DEVICE" --verify "$ROM"
minipro --device "$DEVICE" --verify "$ROM"
minipro --device "$DEVICE" --verify "$ROM"

set +x

echo 'Place CONTROL1 ROM in burner..'; read -k1 -s

set -x

minipro --device "$DEVICE" --write "$ROM"
minipro --device "$DEVICE" --verify "$ROM"
minipro --device "$DEVICE" --verify "$ROM"
minipro --device "$DEVICE" --verify "$ROM"

cp "$ROM" "$ROM.burned"
