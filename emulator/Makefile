CFILES := $(wildcard *.c)
OFILES := $(CFILES:.c=.o)
CFLAGS := -I ./include -lSDL2

all: $(OFILES)
	nasm -f elf64 -o bin/assembly.o assembly.asm
	gcc ./bin/*.o -no-pie -lSDL2 -o ../emulator.out

%.o: %.c
	gcc -c $< -o ./bin/$@ $(CFLAGS)
