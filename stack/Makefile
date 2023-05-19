CC = gcc
CCFLAGS = -I../include -I./include -std=c99 -Wall -Werror -Wextra -Wpedantic
CFILES = $(shell find . -type f -name "*.c") ../type.c
OUT = ../bin/stack

all:
	@echo "  - Building stack demo ($(CC) $(CFILES) -o $(OUT) $(CCFLAGS))"
	@$(CC) $(CFILES) -o $(OUT) $(CCFLAGS)
