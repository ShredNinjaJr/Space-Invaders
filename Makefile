CC = gcc
LD = gcc
CFLAGS = -Wall -g -c
LDFLAGS = -lSDL
EXENAME = emulator
DEBUG_EXENAME = emulator_debug
SRC = emulator.c disassembler.c
OBJS = emulator.o disassembler.o

all: debug build

debug: emulator.o disassembler.o
	$(LD) $(LDFLAGS) $(OBJS) -DDEBUG -o $(DEBUG_EXENAME);

build: emulator.o disassembler.o
	$(LD) $(LDFLAGS) $(OBJS) -o $(EXENAME);

emulator.o: emulator.c emulator.h
	$(CC) $(CFLAGS) emulator.c

disassembler.o: disassembler.c emulator.h
	$(CC) $(CFLAGS) disassembler.c


clean:
	rm $(OBJS) $(EXENAME) $(DEBUG_EXENAME)




	
