CC := gcc
AR := ar
# CFLAGS := -Wall -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
CFLAGS := -g -Wall -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -DDEBUG

OBJS = fileLib.o font16_st.o gb2unicode.o vmrp.o tsf_font.o utils.o debug.o \
	rbtree.o bridge.o memory.o baseLib_cfunction.ext.o main.o

UNICORN = -lunicorn
CAPSTONE = -lcapstone
ifeq ($(OS),Windows_NT)
	UNICORN = ./windows/unicorn-1.0.1-win32/unicorn.a
	CAPSTONE = ./windows/capstone-4.0.1-win32/capstone.dll
endif

SDL2 = ./windows/SDL2-2.0.10/i686-w64-mingw32/bin/SDL2.dll

# -Wl,-subsystem,windows gets rid of the console window
# gcc  -o main.exe main.c -lmingw32 -Wl,-subsystem,windows -L./lib -lSDL2main -lSDL2
main: $(OBJS)
	$(CC) $(CFLAGS) -m32  -o ./mythroad/$@ $^ $(UNICORN) $(CAPSTONE) -lpthread -lm -lz \
		-lmingw32  -L./windows/SDL2-2.0.10/i686-w64-mingw32/lib/ -lSDL2main -lSDL2
	cp $(SDL2) ./mythroad/
	cp $(CAPSTONE) ./mythroad/


lib: $(OBJS)
	$(AR) crv ./libvmrp.a $^

%.o:%.c
	$(CC) $(CFLAGS) -m32 -c $^

.PHONY: clean
clean:
	-rm *.o



