
#include <SDL/SDL.h>
#if defined(NDS) || defined(__NDS__) || defined (__NDS)
#include <nds.h>
#else
#define swiWaitForVBlank() 
#define consoleDemoInit() 
#define RGB15(r,g,b) SDL_MapRGB(screen->format,((r)<<3),((g)<<3),((b)<<3))
#endif

void delay(int s) {
	while(s-- > 0) {
		int i = 60;
		while(--i) swiWaitForVBlank();
	}
}

int main(void) {
	SDL_Surface *screen;
	SDL_Joystick *stick;
	SDL_Event event;
	SDL_Rect rect = {8,8,240,176};
	int i;

	consoleDemoInit();
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0) {
		puts("# error initializing SDL");
		puts(SDL_GetError());
		return 1;
	}
	puts("* initialized SDL");
	screen = SDL_SetVideoMode(256, 192, 15, SDL_SWSURFACE);
	if(!screen) {
		puts("# error setting video mode");
		puts(SDL_GetError());
		return 2;
	}
	screen->flags &= ~SDL_PREALLOC;
	puts("* set video mode");
	stick = SDL_JoystickOpen(0);
	if(stick == NULL) {
		puts("# error opening joystick");
		puts(SDL_GetError());
//		return 3;
	}
	puts("* opened joystick");
	SDL_FillRect(screen, &rect, RGB15(0,0,31)|0x8000);
	SDL_Flip(screen);

	while(1)
	while(SDL_PollEvent(&event))
	switch(event.type) {
		case SDL_JOYBUTTONDOWN:
		switch(event.jbutton.which) {
			case 0:
			SDL_FillRect(screen, &rect, RGB15(31,0,0)|0x8000);
			break;
			case 1:
			SDL_FillRect(screen, &rect, RGB15(0,31,0)|0x8000);
			break;
			case 2:
			SDL_FillRect(screen, &rect, RGB15(0,0,31)|0x8000);
			break;
			case 3:
			SDL_FillRect(screen, &rect, RGB15(0,0,0)|0x8000);
			break;
			default: break;
		}
		printf("joy_%d\n", event.jbutton.which);
		SDL_Flip(screen);
		break;
		case SDL_QUIT: SDL_Quit(); return 0;
		default: break;
	}

	return 0;
}
