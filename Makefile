CC := gcc

CFLAGS := -g -Wall -DNETWORK_SUPPORT -DVMRP
OBJS := 
CAPSTONE := 

ifeq ($(DEBUG),1)
	CFLAGS += -DDEBUG
	OBJS := debug.o
	CAPSTONE := ./windows/capstone-4.0.1-win32/capstone.dll
endif

OBJS += network.o fileLib.o vmrp.o utils.o rbtree.o bridge.o memory.o

# UNICORN = ./windows/unicorn-1.0.2-win32/unicorn.lib
UNICORN = /usr/local/lib/libunicorn.a

SDL2 = ./windows/SDL2-2.0.10/i686-w64-mingw32
M = 32

# -Wl,-subsystem,windows gets rid of the console window
# gcc  -o main.exe main.c -lmingw32 -Wl,-subsystem,windows -L./lib -lSDL2main -lSDL2
main: $(OBJS) main.o
	$(CC) $(CFLAGS) -m$(M) -o ./bin/$@ $^ $(UNICORN) $(CAPSTONE) -lpthread -lm \
	 -lSDL2main -lSDL2

dll: $(OBJS)
	$(CC) $(CFLAGS) -m$(M) -shared -o ./bin/vmrp.dll $^ $(UNICORN) $(CAPSTONE) -lpthread -lm -lws2_32 -lmingw32

%.o:%.c
	$(CC) $(CFLAGS) -m$(M) -c $^

.PHONY: clean
clean:
	-rm *.o



