CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
LDLIBS  = -lX11 -lglut -lGLU -lGL -lm -lXext -lXmu
TARGET  = mapa

SRCS    = $(wildcard src/*.c)
OBJS    = $(patsubst src/%.c,build/%.o,$(SRCS)) build/Bresenham.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

build/%.o: src/%.c $(wildcard src/*.h) | build
	$(CC) $(CFLAGS) -c $< -o $@

build/Bresenham.o: src/Bresenham.s | build
	nasm -f elf64 $< -o $@

build:
	mkdir -p build

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build $(TARGET)

.PHONY: all run clean
