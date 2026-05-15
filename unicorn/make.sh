#!/bin/bash
set -e

cd /home/msojocs/github/unicorn

# export PKG_CONFIG_PATH="/usr/lib/i386-linux-gnu/pkgconfig" 
# export CFLAGS=-m32
# export LDFLAGS=-m32
# export LDFLAGS_STATIC=-m32
# export LIBRARY_PATH="/usr/lib/i386-linux-gnu"
# export UNICORN_QEMU_FLAGS="--cpu=i386 ${UNICORN_QEMU_FLAGS}"
mkdir -p build
cd build
rm -rf ./*
cmake ..
make -j16
sudo make install

# 查看构建结果是否32位
# elf32-i386 就是32位，是目标
objdump -a /usr/local/lib/libunicorn.a
