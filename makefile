CC=cc
NASM=nasm

OBJECTS=src/main.o src/Bresenham.o
OUTPUT=main

CFLAGS=-O0 -Isrc -I/usr/local/Mesa-3.4/include
LDFLAGS=-L/usr/local/Mesa-3.4/lib -L/usr/X11R6/lib
LDLIBS=-lX11 -lglut -lGLU -lGL -lm -lXext -lXmu

.PHONY: all run clean

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

src/main.o: src/main.c src/main.h
	$(CC) $(CFLAGS) -c $< -o $@

src/Bresenham.o: src/Bresenham.s
	$(NASM) -f elf64 $< -o $@

run: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -f $(OBJECTS) $(OUTPUT)