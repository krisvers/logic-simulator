CC = gcc
CCFLAGS = -Wall -Werror -Wextra -Wpedantic -std=c99 -Iinclude
CFILES = $(shell find . -type f -name "*.c")
OUTFILE = ./bin/logic

all: clean build

build:
	$(CC) $(CFILES) -o $(OUTFILE) $(CCFLAGS)

clean:
	rm -rf bin
	mkdir bin
