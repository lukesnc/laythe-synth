CC := $(shell command -v clang 2>/dev/null || command -v gcc)
CFLAGS := -Wall -Wextra

RAYLIB := thirdparty/raylib/src
INCLUDES := -I$(RAYLIB)
LFLAGS := -lm -lpthread -L$(RAYLIB) -lraylib

APP := laythe
SRC := $(wildcard source/*.c)

all: $(SRC)
	$(CC) -o $(APP) $(SRC) $(INCLUDES) $(LFLAGS) $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(APP)
