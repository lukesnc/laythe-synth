CC=gcc
CFLAGS=-Wall -Wextra
INCLUDES=
LFLAGS=-lm -lraylib

APP=laythe
SRC=source/main.c

all: $(SRC)
	mkdir -p target
	$(CC) -o target/$(APP) $(SRC) $(INCLUDES) $(LFLAGS) $(CFLAGS)

.PHONY: clean run
clean:
	rm -rf target/

run: all
	./target/$(APP) $(ARGS)
