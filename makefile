CC=cc
NASM=nasm

OBJECTS=main.o Bresenham.o
OUTPUT=main

CFLAGS=-O0 -I/usr/local/Mesa-3.4/include
LDLIBS=-lX11 -lglut -lGLU -lGL -lm -lXext -lXmu
LDFLAGS=-L/usr/local/Mesa-3.4/lib -L/usr/X11R6/lib

.PHONY: all run clean

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT) $(OBJECTS) $(LDLIBS)

main.o: main.c main.h
	$(CC) $(CFLAGS) -c main.c -o main.o

Bresenham.o: Bresenham.s
	$(NASM) -f elf64 Bresenham.s -o Bresenham.o

run: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -f $(OBJECTS) $(OUTPUT)