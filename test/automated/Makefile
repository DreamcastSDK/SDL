

CFLAGS  := -I. `sdl-config --cflags`
LDFLAGS := `sdl-config --libs`

COMMON_SRC     := SDL_at.c
COMMON_INCLUDE := SDL_at.h

TESTS_ALL := rwops/rwops


.PHONY: all clean test


all: $(TESTS_ALL)

test: all
	@./rwops/rwops

rwops/rwops: rwops/rwops.c $(COMMON_INCLUDE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ rwops/rwops.c $(COMMON_SRC)

clean:
	$(RM) $(TESTS_ALL)
