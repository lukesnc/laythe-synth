RAYLIB = thirdparty/raylib/src
CFLAGS = -Wall -Wextra -I$(RAYLIB) -lm -lpthread -L$(RAYLIB) -lraylib 

ifeq ($(OS),Windows_NT)
	CFLAGS+=-lgdi32 -lwinmm
endif

all: laythe

laythe: $(wildcard source/*.c)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: all
