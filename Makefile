CC = gcc
CCFLAGS = -std=c99 -Wall -Werror -Wextra -Wpedantic -Iinclude
CFILES = $(shell find . -type f -name "*.c")
OUTFILE = ./logic

all: clean build

build:
	$(CC) $(CFILES) -o $(OUTFILE) $(CCFLAGS)

clean:
	@rm -f ./logic
