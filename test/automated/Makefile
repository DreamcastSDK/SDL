

CFLAGS  := -W -Wall -Wextra -g -I. `sdl-config --cflags`
LDFLAGS := `sdl-config --libs`

# If it doesn't pick up defaults
#CFLAGS  := -I. -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/local/include/SDL
#LDFLAGS := -lm -ldl -lesd -lpthread

COMMON_SRC     := SDL_at.c common/common.c
COMMON_INCLUDE := SDL_at.h

TESTS_ALL := rwops/rwops surface/surface render/render


.PHONY: all clean test


all: $(TESTS_ALL)

test: all
	@./rwops/rwops
	@./surface/surface
	@./render/render

rwops/rwops: rwops/rwops.c $(COMMON_INCLUDE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ rwops/rwops.c $(COMMON_SRC)

surface/surface: surface/surface.c $(COMMON_INCLUDE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ surface/surface.c $(COMMON_SRC)

render/render: render/render.c $(COMMON_INCLUDE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ render/render.c $(COMMON_SRC)

clean:
	$(RM) $(TESTS_ALL)
