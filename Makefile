# Check if cross-compiling for Windows from WSL
ifeq ($(origin WSL), undefined)
	CC=gcc
	INCLUDES=
	LFLAGS=-lm -lraylib
	APP=laythe
else
	# Locate downloaded libraries
	RAYLIB=$(wildcard raylib*)

	CC=x86_64-w64-mingw32-gcc
	INCLUDES=-I$(RAYLIB)/include
	LFLAGS=-lm -L$(RAYLIB)/lib -lraylib -lwinmm -lgdi32
	APP=laythe.exe
endif

CFLAGS=-Wall -Wextra
SRC=$(wildcard source/*.c)

all: $(SRC)
	$(CC) -o $(APP) $(SRC) $(INCLUDES) $(LFLAGS) $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(APP)
