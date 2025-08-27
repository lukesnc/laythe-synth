RAYLIB = thirdparty/raylib/src
CFLAGS = -Wall -Wextra -I$(RAYLIB) -lm -lpthread -L$(RAYLIB) -lraylib 

ifeq ($(OS),Windows_NT)
	CFLAGS+=-lgdi32 -lwinmm
endif

.PHONY: all
all: laythe

LIBRAYLIB = $(RAYLIB)/libraylib.a

$(LIBRAYLIB):
	$(MAKE) PLATFORM=PLATFORM_DESKTOP -C $(RAYLIB)

SRC = $(wildcard source/*.c)

laythe: $(LIBRAYLIB) $(SRC)
	$(CC) -o $@ $(SRC) $(CFLAGS)
