CC = gcc
WINCC = i686-w64-mingw32-gcc
CCFLAGS = -Iinclude -Wall -Wpedantic -Wextra -std=c99 -fanalyzer -O2
WINCCFLAGS = -mwindows
CCLIBS = -lglfw -lGL
WINCCLIBS = -Llib -lglfw3 -lopengl32 -lgdi32 -lkernel32 -lmingw32 -DGLFW_DLL
DBGFLAGS = -fsanitize=address
CFILES = $(shell find . -type f -name "*.c")
OUTDIR = ./bin
OUTFILE = window

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
	@cp ./lib/glfw3.dll $(OUTDIR)/win32/glfw3.dll
	@cp -R ./resources $(OUTDIR)/win32/
	@cp -R ./resources $(OUTDIR)/default/
