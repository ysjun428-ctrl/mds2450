#!/usr/bin/env bash
set -euo pipefail
sudo apt update
sudo apt install -y \
  gcc-arm-none-eabi binutils-arm-none-eabi \
  make gdb-multiarch picocom python3 python3-serial

echo
echo "Toolchain:"
arm-none-eabi-gcc --version | head -1
arm-none-eabi-objcopy --version | head -1
