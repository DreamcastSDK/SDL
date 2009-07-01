/* A simple program to test the Input Method support in the SDL library (1.3+) */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include <SDL/SDL_ttf.h>
 
#define DEFAULT_PTSIZE  30
#define DEFAULT_FONT    "DroidSansFallback.ttf"
#define MAX_TEXT_LENGTH 256

static void render_text(SDL_Surface *sur,
                        TTF_Font *font,
                        const char *text,
                        int x, int y,
                        SDL_Color color)
{
    SDL_Surface *textSur = TTF_RenderUTF8_Blended(font, text, color);
    SDL_Rect dest = { x, y, textSur->w, textSur->h };

    SDL_BlitSurface(textSur, NULL, sur, &dest);
    SDL_FreeSurface(textSur);
}

int main(int argc, char *argv[])
{
    int width, height;
    SDL_Surface *screen;
    TTF_Font *font;

    width = 500, height = 250;

    SDL_putenv("SDL_VIDEO_WINDOW_POS=center");

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return -1;
    }

    /* Initialize fonts */
    TTF_Init();

    font = TTF_OpenFont(DEFAULT_FONT, DEFAULT_PTSIZE);
    if (! font)
    {
        fprintf(stderr, "Failed to find font: %s\n", SDL_GetError());
        exit(-1);
    }

    atexit(SDL_Quit);

    /* Create window */
    screen = SDL_SetVideoMode(width, height, 32,
                              SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        fprintf(stderr, "Unable to set %dx%d video: %s\n",
                width, height, SDL_GetError());
        return -1;
    }

    /* Prepare a rect for text input */
    SDL_Rect textRect = { 100, 80, 300, 50 }, markedRect, underlineRect, cursorRect;
    Uint32 backColor = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
    Uint32 lineColor = SDL_MapRGB(screen->format, 0x0, 0x0, 0x0);
    SDL_Color textColor = { 0, 0, 0 };
    SDL_FillRect(screen, &textRect, backColor);

    markedRect = textRect;
    SDL_StartTextInput(&markedRect);

    SDL_Flip(screen);

    SDL_Event event;
    int done = 0, inputed = 0;
    int w, h;
    char text[MAX_TEXT_LENGTH];

    while (! done && SDL_WaitEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            fprintf(stderr,
                    "Keyboard %d: scancode 0x%08X = %s, keycode 0x%08X = %s\n",
                    event.key.which, event.key.keysym.scancode,
                    SDL_GetScancodeName(event.key.keysym.scancode),
                    event.key.keysym.sym, SDL_GetKeyName(event.key.keysym.sym));
            break;

        case SDL_TEXTINPUT:
            fprintf(stderr, "Keyboard %d: text input \"%s\"\n",
                    event.text.which, event.text.text);

            if (inputed < sizeof(text))
            {
                strcpy(text + inputed, event.text.text);
                inputed += strlen(event.text.text);
            }

            fprintf(stderr, "text inputed: %s\n", text);
            SDL_FillRect(screen, &textRect, backColor);

            render_text(screen, font, text, textRect.x, textRect.y, textColor);
            TTF_SizeUTF8(font, text, &w, &h);
            markedRect.x = textRect.x + w;

            cursorRect = markedRect;
            cursorRect.w = 2;
            cursorRect.h = h;
            SDL_FillRect(screen, &cursorRect, lineColor);
            SDL_Flip(screen);

            SDL_StartTextInput(&markedRect);
            break;

        case SDL_TEXTEDITING:
            fprintf(stderr, "text editing \"%s\", selected range (%d, %d)\n",
                    event.edit.text, event.edit.start, event.edit.length);

            SDL_FillRect(screen, &markedRect, backColor);
            render_text(screen, font, event.edit.text, markedRect.x, markedRect.y, textColor);
            TTF_SizeUTF8(font, event.edit.text, &w, &h);
            underlineRect = markedRect;
            underlineRect.y += (h - 2);
            underlineRect.h = 2;
            underlineRect.w = w;
            SDL_FillRect(screen, &underlineRect, lineColor);

            SDL_Flip(screen);
            break;

        case SDL_QUIT:
            done = 1;
            break;

        default:
            break;
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();

    return 0;
}

