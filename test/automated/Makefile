

CFLAGS  := `sdl-config --cflags`
LDFLAGS := `sdl-config --libs`

COMMON_SRC     := SDL_at.c
COMMON_INCLUDE := SDL_at.h


.PHONY: all


all: rwops

rwops: rwops.c $(COMMON_INCLUDE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o rwops rwops.c $(COMMON_SRC)
