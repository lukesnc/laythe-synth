# Check if cross-compiling for Windows from WSL
ifeq ($(origin WSL), undefined)
	CC=gcc
	INCLUDES=
	LFLAGS=-lm -lraylib
	APP=laythe
else
	CC=x86_64-w64-mingw32-gcc
	INCLUDES=-Iraylib-5.5_win64_mingw-w64/include
	LFLAGS=-lm -Lraylib-5.5_win64_mingw-w64/lib -lraylib 
	APP=laythe.exe
endif

CFLAGS=-Wall -Wextra
SRC=$(wildcard source/*.c)

all: $(SRC)
	$(CC) -o $(APP) $(SRC) $(INCLUDES) $(LFLAGS) $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(APP)
