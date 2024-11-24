CC=gcc
CFLAGS=-Wall -Wextra
LFLAGS=-lm -lraylib

APP=laythe
SRC=source/main.c source/synth.c

all: $(SRC)
	$(CC) -o $(APP) $(SRC) $(LFLAGS) $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(APP)
