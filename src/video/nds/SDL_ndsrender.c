/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#include <stdio.h>
#include <stdlib.h>
#include <nds.h>

#include "SDL_config.h"

#include "SDL_video.h"
#include "../SDL_sysvideo.h"
#include "../SDL_yuv_sw_c.h"
#include "../SDL_renderer_sw.h"


/* SDL surface based renderer implementation */

static SDL_Renderer *SDL_NDS_CreateRenderer(SDL_Window * window,
                                              Uint32 flags);
static int SDL_NDS_RenderFill(SDL_Renderer * renderer, Uint8 r, Uint8 g,
                                Uint8 b, Uint8 a, const SDL_Rect * rect);
static int SDL_NDS_RenderCopy(SDL_Renderer * renderer,
                                SDL_Texture * texture,
                                const SDL_Rect * srcrect,
                                const SDL_Rect * dstrect);
static void SDL_NDS_RenderPresent(SDL_Renderer * renderer);
static void SDL_NDS_DestroyRenderer(SDL_Renderer * renderer);


SDL_RenderDriver SDL_NDS_RenderDriver = {
    SDL_NDS_CreateRenderer,
    { "nds",  SDL_RENDERER_PRESENTCOPY }
/*   (SDL_RENDERER_SINGLEBUFFER | SDL_RENDERER_PRESENTCOPY |
      SDL_RENDERER_PRESENTFLIP2 | SDL_RENDERER_PRESENTFLIP3 |
      SDL_RENDERER_PRESENTDISCARD),*/
};

typedef struct
{
    int current_screen;
    SDL_Surface *screens[3];
    int ultimate_answer;
} SDL_NDS_RenderData;

SDL_Renderer *
SDL_NDS_CreateRenderer(SDL_Window * window, Uint32 flags)
{
    SDL_VideoDisplay *display = SDL_GetDisplayFromWindow(window);
    SDL_DisplayMode *displayMode = &display->current_mode;
    SDL_Renderer *renderer;
    SDL_NDS_RenderData *data;
    int i, n;
    int bpp = 16;
    Uint32 Rmask = 0x7C00, Gmask = 0x03E0, Bmask = 0x001F, Amask = 0x8000;

    printf("SDL_NDS_CreateRenderer(window, 0x%x)\n", flags);
    printf(" window: (%d,%d), %dx%d\n", window->x, window->y, window->w, window->h);

    /* hard coded this to ARGB1555 for now
    if (!SDL_PixelFormatEnumToMasks
        (displayMode->format, &bpp, &Rmask, &Gmask, &Bmask, &Amask)) {
        SDL_SetError("Unknown display format");
        return NULL;
    }*/

    renderer = (SDL_Renderer *) SDL_calloc(1, sizeof(*renderer));
    if (!renderer) {
        SDL_OutOfMemory();
        return NULL;
    }

    data = (SDL_NDS_RenderData *) SDL_malloc(sizeof(*data));
    if (!data) {
        SDL_NDS_DestroyRenderer(renderer);
        SDL_OutOfMemory();
        return NULL;
    }
    SDL_zerop(data);

    renderer->RenderFill = SDL_NDS_RenderFill;
    renderer->RenderCopy = SDL_NDS_RenderCopy;
    renderer->RenderPresent = SDL_NDS_RenderPresent;
    renderer->DestroyRenderer = SDL_NDS_DestroyRenderer;
    renderer->info.name = SDL_NDS_RenderDriver.info.name;
    renderer->info.flags = 0;
    renderer->window = window->id;
    renderer->driverdata = data;
    Setup_SoftwareRenderer(renderer);

    if (flags & SDL_RENDERER_PRESENTFLIP2) {
        renderer->info.flags |= SDL_RENDERER_PRESENTFLIP2;
        n = 2;
    } else if (flags & SDL_RENDERER_PRESENTFLIP3) {
        renderer->info.flags |= SDL_RENDERER_PRESENTFLIP3;
        n = 3;
    } else {
        renderer->info.flags |= SDL_RENDERER_PRESENTCOPY;
        n = 1;
    }
    for (i = 0; i < n; ++i) {
        data->screens[i] =
            SDL_CreateRGBSurface(0, 256, 192, bpp, Rmask, Gmask, Bmask, Amask);
        if (!data->screens[i]) {
            SDL_NDS_DestroyRenderer(renderer);
            return NULL;
        }
        SDL_SetSurfacePalette(data->screens[i], display->palette);
    }

    data->current_screen = 0;
    data->ultimate_answer = 42;
#if 0
#define blarg (data->screens[0])
    printf("hello?\n");
    if(!data || !(data->screens) || !blarg) {
        printf("they're null.\n");
    } else {
        printf("not null.\n");
        printf("%d\n%d\n%d\n%d\n%x\n%x\n%x\n%x\n",
        blarg->w, blarg->h, blarg->pitch,
        blarg->format->BitsPerPixel,
        blarg->format->Rmask,
        blarg->format->Gmask,
        blarg->format->Bmask,
        (u32)(blarg->pixels)); /* ARGH WHY DOESN'T THIS PRINT AT ALL? */
        printf("hurr\n");
    }
#undef blarg
#endif
    return renderer;
}

static int
SDL_NDS_RenderFill(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b,
                     Uint8 a, const SDL_Rect * rect)
{
    SDL_NDS_RenderData *data = (SDL_NDS_RenderData *) renderer->driverdata;
    SDL_Surface *target = data->screens[data->current_screen];
    Uint32 color;
    SDL_Rect real_rect = *rect;

    color = SDL_MapRGBA(target->format, r, g, b, a);

    return SDL_FillRect(target, &real_rect, color);
}

/* this is mainly for testing stuff to put a surface where I can see it */
void sdlds_surf2vram(SDL_Surface *s) {
    int i;
    for(i = 0; i < 256*192; ++i) {
        ((u16*)VRAM_A)[i] = ((u16*)(s->pixels))[i];
    }
}

static int
SDL_NDS_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
                     const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
    SDL_NDS_RenderData *data =
        (SDL_NDS_RenderData *) renderer->driverdata;
    SDL_Window *window = SDL_GetWindowFromID(renderer->window);
    SDL_VideoDisplay *display = SDL_GetDisplayFromWindow(window);
    printf("SDL_NDS_RenderCopy(renderer, texture, srcrect, dstrect)\n");
    printf(" renderer: %s\n", renderer->info.name);
    printf(" texture: %dx%d\n", texture->w, texture->h);
    printf(" srcrect: (%d,%d), %dx%d\n", srcrect->x, srcrect->y, srcrect->w, srcrect->h);
    printf(" dstrect: (%d,%d), %dx%d\n", dstrect->x, dstrect->y, dstrect->w, dstrect->h);

    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        SDL_Surface *target = data->screens[data->current_screen];
        void *pixels =
            (Uint8 *) target->pixels + dstrect->y * target->pitch +
            dstrect->x * target->format->BytesPerPixel;
        return SDL_SW_CopyYUVToRGB((SDL_SW_YUVTexture *) texture->driverdata,
                                   srcrect, display->current_mode.format,
                                   dstrect->w, dstrect->h, pixels,
                                   target->pitch);
    } else {
        SDL_Surface *surface = (SDL_Surface *) texture->driverdata;
        SDL_Surface *target = data->screens[data->current_screen];
        SDL_Rect real_srcrect = *srcrect;
        SDL_Rect real_dstrect = *dstrect;
        printf("Rmask %x Gmask %x Bmask %x Amask %x\n"
               "width %d, height %d, pitch %d\nbpp %d, pixels %x\n",
            surface->format->Rmask, surface->format->Gmask,
            surface->format->Bmask, surface->format->Amask,
            surface->w, surface->h, surface->pitch,
            surface->format->BitsPerPixel, (u32)(surface->pixels));
        sdlds_surf2vram(surface);
        return SDL_LowerBlit(surface, &real_srcrect, target, &real_dstrect);
    }
#if 0
/* previous attempt to copy it directly to vram */
        SDL_Surface *surface = (SDL_Surface *) texture->driverdata;
        int sx = srcrect->x, sy = srcrect->y, sw = srcrect->w, sh = srcrect->h;
        int dx = dstrect->x, dy = dstrect->y, dw = dstrect->w, dh = dstrect->h;
        int si,sj, di,dj;
        /*printf("DEBUG: still alive!\n");*/
        for(sj=0, dj=0; sj<sh && dj<dh; ++sj, ++dj) {
            for(si=0, di=0; si<sw && di<dw; ++si, ++di) {
                ((uint16*)VRAM_A)[(dj+dy)*256 + di+dx]
                 = ((Uint16*)surface->pixels)[(sj+sy)*(surface->w) + si+sx];
            }
        }
        /*printf("DEBUG: still alive!\n");*/
    }
    return 0;
#endif
}

static void
SDL_NDS_RenderPresent(SDL_Renderer * renderer)
{
    SDL_NDS_RenderData *data =
        (SDL_NDS_RenderData *) renderer->driverdata;

    printf("SDL_NDS_RenderPresent(renderer)\n");
    printf(" renderer: %s\n", renderer->info.name);
    /* Send the data to the display */

#if 0
/*testing to see if rectangles drawn get copied right*/
    {
    SDL_Rect ra;
  	ra.x=0; ra.y=0; ra.w=256; ra.h=192;
	SDL_FillRect(data->screens[data->current_screen], &ra, 0x250);
	ra.x=32; ra.y=32; ra.w=192; ra.h=128;
	SDL_FillRect(data->screens[data->current_screen], &ra,
	    SDL_MapRGBA(data->screens[data->current_screen]->format,
	    255,255,255,255));
	}
/*okay so this works but why not when I do it in the main()?
  for some reason the screen I get from screen=SDL_SetVideoMode(...)
  doesn't get copied to renderer->driverdata? */
    for(i = 0; i < 30; ++i) swiWaitForVBlank(); /* delay for debug purpose */
#endif
    sdlds_surf2vram(data->screens[data->current_screen]);

    /* Update the flipping chain, if any */
    if (renderer->info.flags & SDL_RENDERER_PRESENTFLIP2) {
        data->current_screen = (data->current_screen + 1) % 2;
    } else if (renderer->info.flags & SDL_RENDERER_PRESENTFLIP3) {
        data->current_screen = (data->current_screen + 1) % 3;
    }
}

static void
SDL_NDS_DestroyRenderer(SDL_Renderer * renderer)
{
    SDL_NDS_RenderData *data = (SDL_NDS_RenderData *) renderer->driverdata;
    int i;

    printf("SDL_NDS_DestroyRenderer(renderer)\n");
    printf(" renderer: %s\n", renderer->info.name);
    if (data) {
        for (i = 0; i < SDL_arraysize(data->screens); ++i) {
            if (data->screens[i]) {
                SDL_FreeSurface(data->screens[i]);
            }
        }
        SDL_free(data);
    }
    SDL_free(renderer);
}

/* vi: set ts=4 sw=4 expandtab: */
