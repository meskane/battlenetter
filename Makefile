CC := x86_64-w64-mingw32-gcc

TARGET := build/battlenetter.exe
SRC := src/main.c src/config.c

CFLAGS := -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow

.PHONY: all debug release clean

all: debug

debug:
	mkdir -p build
	$(CC) $(CFLAGS) -O0 -g $(SRC) -o $(TARGET)

release:
	mkdir -p build
	$(CC) $(CFLAGS) -O2 -s -static-libgcc $(SRC) -o $(TARGET)

clean:
	rm -rf build
