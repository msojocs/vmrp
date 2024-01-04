#!/bin/bash
set -e

mkdir -p build
cd build
rm -rf ./*
cmake ..
make -j16
sudo make install

# 查看构建结果是否32位
# elf32-i386 就是32位，是目标
objdump -a /usr/local/lib/libunicorn.a
