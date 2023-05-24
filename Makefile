CC = gcc
WINCC = i686-w64-mingw32-gcc
CCFLAGS = -Iinclude -Wall -Wpedantic -Wextra -std=c99 -O2
WINCCFLAGS =
CCLIBS =
WINCCLIBS = -Llib -lgdi32 -lkernel32 -lmingw32 -DGLFW_DLL
DBGFLAGS = -fsanitize=address
CFILES = $(shell find . -type f -name "*.c")
OUTDIR = ./bin
OUTFILE = simulator

default: clean default-build
all: clean default-build win32

default-build:
	$(CC) $(CFILES) -o $(OUTDIR)/default/$(OUTFILE) $(CCFLAGS) $(CCLIBS)

win32:
	$(WINCC) $(CFILES) -o $(OUTDIR)/win32/$(OUTFILE).exe $(CCFLAGS) $(WINCCFLAGS) $(WINCCLIBS)

clean:
	@rm -rf $(shell find . -type f -name "$(OUTFILE)*")
	@rm -rf $(shell find $(OUTDIR)/*)
	@mkdir -p $(OUTDIR)/win32 $(OUTDIR)/default
