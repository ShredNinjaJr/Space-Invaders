CC = gcc
LD = gcc
CFLAGS = -Wall -g -c 
LDFLAGS = -lSDL
EXENAME = emulator
DEBUG_EXENAME = emulator_debug
SRC = emulator.c 8080opcodes.c space_invaders.c
OBJS = emulator.o 8080opcodes.o space_invaders.o
DBG_OBJS = emulator_debug.o 8080opcodes.o space_invaders.o

all: debug build

debug: $(DBG_OBJS)
	$(LD) $(LDFLAGS) $(DBG_OBJS) -o $(DEBUG_EXENAME);

build: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(EXENAME);

emulator.o: emulator.c emulator.h
	$(CC) $(CFLAGS) emulator.c

8080opcodes.o: 8080opcodes.c emulator.h
	$(CC) $(CFLAGS) 8080opcodes.c

space_invaders.o: space_invaders.c space_invaders.h
	$(CC) $(CFLAGS) -I"/usr/include/SDL" space_invaders.c

emulator_debug.o: emulator.c
	$(CC) $(CFLAGS) -DDEBUG emulator.c -o emulator_debug.o

clean:
	rm $(OBJS) $(EXENAME) $(DEBUG_EXENAME) emulator_debug.o




	
