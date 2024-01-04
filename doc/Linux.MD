
sudo dpkg --add-architecture i386 
sudo apt-get install libsdl2-dev:i386


ln -s /usr/include/asm-generic /usr/include/asm

## 构建32位的unicorn

`set(EXTRA_CFLAGS "--extra-cflags=")` -> `set(EXTRA_CFLAGS "--extra-cflags=-m32 ")`

```
else()
    set(UNICORN_CFLAGS -m64 -mcx16)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m64")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m64")
endif()

---->

else()
    set(UNICORN_CFLAGS -m32 -mcx16)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
endif()

```

```shell
#!/bin/bash
# make.sh
set -e

export PKG_CONFIG_PATH="/usr/lib/i386-linux-gnu/pkgconfig" 
export CFLAGS=-m32
export LDFLAGS=-m32
export LDFLAGS_STATIC=-m32
export LIBRARY_PATH="/usr/lib/i386-linux-gnu"
export UNICORN_QEMU_FLAGS="--cpu=i386 ${UNICORN_QEMU_FLAGS}"
mkdir -p build
cd build
rm -rf ./*
cmake ..
make -j16
sudo make install
objdump -a /usr/local/lib/libunicorn.a

```

