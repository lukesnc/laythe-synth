CC := $(shell command -v clang 2>/dev/null || command -v gcc)
CFLAGS := -Wall -Wextra

RAYLIB := thirdparty/raylib/src
INCLUDES := -I$(RAYLIB)
LFLAGS := -lm -lpthread -L$(RAYLIB) -lraylib

APP := laythe
SRC := $(wildcard source/*.c)

ifeq ($(OS),Windows_NT)
	LFLAGS+=-lgdi32 -lwinmm
	APP := laythe.exe
endif

all: $(SRC)
	$(CC) -o $(APP) $(SRC) $(INCLUDES) $(LFLAGS) $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(APP)
