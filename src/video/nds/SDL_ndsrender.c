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

static SDL_Renderer *NDS_CreateRenderer(SDL_Window * window, Uint32 flags);
static int NDS_ActivateRenderer(SDL_Renderer * renderer);
static int NDS_DisplayModeChanged(SDL_Renderer * renderer);
static int NDS_CreateTexture(SDL_Renderer * renderer, SDL_Texture * texture);
static int NDS_QueryTexturePixels(SDL_Renderer * renderer,
                                 SDL_Texture * texture, void **pixels,
                                 int *pitch);
static int NDS_SetTexturePalette(SDL_Renderer * renderer,
                                SDL_Texture * texture,
                                const SDL_Color * colors, int firstcolor,
                                int ncolors);
static int NDS_GetTexturePalette(SDL_Renderer * renderer,
                                SDL_Texture * texture, SDL_Color * colors,
                                int firstcolor, int ncolors);
static int NDS_SetTextureColorMod(SDL_Renderer * renderer,
                                 SDL_Texture * texture);
static int NDS_SetTextureAlphaMod(SDL_Renderer * renderer,
                                 SDL_Texture * texture);
static int NDS_SetTextureBlendMode(SDL_Renderer * renderer,
                                  SDL_Texture * texture);
static int NDS_SetTextureScaleMode(SDL_Renderer * renderer,
                                  SDL_Texture * texture);
static int NDS_UpdateTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                            const SDL_Rect * rect, const void *pixels,
                            int pitch);
static int NDS_LockTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                          const SDL_Rect * rect, int markDirty, void **pixels,
                          int *pitch);
static void NDS_UnlockTexture(SDL_Renderer * renderer, SDL_Texture * texture);
static void NDS_DirtyTexture(SDL_Renderer * renderer,
                            SDL_Texture * texture, int numrects,
                            const SDL_Rect * rects);
static int NDS_RenderFill(SDL_Renderer * renderer, Uint8 r, Uint8 g,
                              Uint8 b, Uint8 a, const SDL_Rect * rect);
static int NDS_RenderCopy(SDL_Renderer * renderer,
                              SDL_Texture * texture,
                              const SDL_Rect * srcrect,
                              const SDL_Rect * dstrect);
static void NDS_RenderPresent(SDL_Renderer * renderer);
static void NDS_DestroyTexture(SDL_Renderer * renderer, SDL_Texture * texture);
static void NDS_DestroyRenderer(SDL_Renderer * renderer);


SDL_RenderDriver NDS_RenderDriver = {
    NDS_CreateRenderer,
    {"nds", SDL_RENDERER_PRESENTCOPY}
/*   (SDL_RENDERER_SINGLEBUFFER | SDL_RENDERER_PRESENTCOPY |
      SDL_RENDERER_PRESENTFLIP2 | SDL_RENDERER_PRESENTFLIP3 |
      SDL_RENDERER_PRESENTDISCARD),*/
};

typedef struct
{
    int current_screen;
    u16* fb;
} NDS_RenderData;



/* this is mainly hackish testing/debugging stuff to get cleaned up soon
   anything named sdlds_blah shouldn't make it into the stable version
 */

u16
sdlds_rgb2bgr(u16 c)
{
/* hack to get the proper colors until I actually get BGR555 to work right */
    u16 Rmask = 0x7C00, Bmask = 0x001F, GAmask = 0x83E0, r, b;
    r = (c & Rmask) >> 10;
    b = (c & Bmask) << 10;
    return (c & GAmask) | r | b;
}

void
sdlds_surf2vram(SDL_Surface * s)
{
    if (s->w == 256) {
        u16 tmpbuf[0x20000];
        int i;

        dmaCopy((u8 *) (s->pixels) + 156 * sizeof(u16),
                tmpbuf, 256 * 192 * sizeof(u16));
    /* hack to fix the pixel format until I figure out why BGR doesn't work */
        for (i = 0; i < 256 * 192; ++i) {
            tmpbuf[i] = sdlds_rgb2bgr(tmpbuf[i]);
        }
        dmaCopy(tmpbuf, VRAM_A, 256 * 192 * sizeof(u16));
    }
}

void
sdlds_print_pixfmt_info(SDL_PixelFormat * f)
{
    if (!f)
        return;
    printf("bpp: %d\nRGBA: %x %x %x %x\n",
           f->BitsPerPixel, f->Rmask, f->Gmask, f->Bmask, f->Amask);
}

void
sdlds_print_surface_info(SDL_Surface * s)
{
    if (!s)
        return;
    printf("flags: %x\nsize: %dx%d, pitch: %d\nlocked: %d, refcount: %d\n",
           s->flags, s->w, s->h, s->pitch, s->locked, s->refcount);
    sdlds_print_pixfmt_info(s->format);
}



SDL_Renderer *
NDS_CreateRenderer(SDL_Window * window, Uint32 flags)
{
    SDL_VideoDisplay *display = SDL_GetDisplayFromWindow(window);
    SDL_DisplayMode *displayMode = &display->current_mode;
    SDL_Renderer *renderer;
    NDS_RenderData *data;
    int i, n;
    int bpp = 15;
    Uint32 Rmask, Gmask, Bmask, Amask;
/*  Uint32 Rmask = 0x7C00, Gmask = 0x03E0, Bmask = 0x001F, Amask = 0x8000;
    Uint32 Rmask = 0x001F, Gmask = 0x03E0, Bmask = 0x7C00, Amask = 0x8000;
*/

    /* hard coded this to BGR555 for now */
    if (!SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_BGR555, &bpp,
                                    &Rmask, &Gmask, &Bmask, &Amask)) {
        SDL_SetError("Unknown display format");
        return NULL;
    }

    renderer = (SDL_Renderer *) SDL_calloc(1, sizeof(*renderer));
    if (!renderer) {
        SDL_OutOfMemory();
        return NULL;
    }

    data = (NDS_RenderData *) SDL_malloc(sizeof(*data));
    if (!data) {
        NDS_DestroyRenderer(renderer);
        SDL_OutOfMemory();
        return NULL;
    }
    SDL_zerop(data);

    renderer->RenderFill = NDS_RenderFill;
    renderer->RenderCopy = NDS_RenderCopy;
    renderer->RenderPresent = NDS_RenderPresent;
    renderer->DestroyRenderer = NDS_DestroyRenderer;
    renderer->info.name = NDS_RenderDriver.info.name;
    renderer->info.flags = 0;
    renderer->window = window->id;
    renderer->driverdata = data;
    Setup_SoftwareRenderer(renderer); /* TODO: well, "TODON'T" is more like it */

    if (flags & SDL_RENDERER_PRESENTFLIP2) {
        renderer->info.flags |= SDL_RENDERER_PRESENTFLIP2;
        n = 2;
    } else if (flags & SDL_RENDERER_PRESENTFLIP3) {
        renderer->info.flags |= SDL_RENDERER_PRESENTFLIP3;
        n = 3;
    } else {
        renderer->info.flags |= SDL_RENDERER_PRESENTCOPY;
        n = 1;
    }/*
    for (i = 0; i < n; ++i) {
        data->screens[i] =
            SDL_CreateRGBSurface(0, 256, 192, bpp, Rmask, Gmask, Bmask,
                                 Amask);
        if (!data->screens[i]) {
            NDS_DestroyRenderer(renderer);
            return NULL;
        }
        SDL_SetSurfacePalette(data->screens[i], display->palette);
        sdlds_print_surface_info(data->screens[i]);
    }*/

    data->fb = (u16*)0x06020000;

    return renderer;
}

static int
NDS_RenderFill(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b,
                   Uint8 a, const SDL_Rect * rect)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    SDL_Rect real_rect = *rect;
    u16 color;
    int i, j;

    color = RGB15(r>>3,g>>3,b>>3);
    for (i = real_rect.x; i < real_rect.x+real_rect.w; ++i) {
        for (j = real_rect.y; j < real_rect.y+real_rect.h; ++j) {
            data->fb[(j + real_rect.y) * 256 + i + real_rect.x] = 
                0x8000 | color;
        }
    }
    return 0;
}
static int
NDS_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
                   const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    SDL_Window *window = SDL_GetWindowFromID(renderer->window);
    SDL_VideoDisplay *display = SDL_GetDisplayFromWindow(window);

#if 0
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
        /*sdlds_print_surface_info(surface);
           sdlds_print_surface_info(target); */
        sdlds_surf2vram(surface);
        return SDL_LowerBlit(surface, &real_srcrect, target, &real_dstrect);
    }
#endif
    /* copy it directly to vram */
    SDL_Surface *surface = (SDL_Surface *) texture->driverdata;
    sdlds_surf2vram(surface);
    /*
    int sx = srcrect->x, sy = srcrect->y, sw = srcrect->w, sh = srcrect->h;
    int dx = dstrect->x, dy = dstrect->y, dw = dstrect->w, dh = dstrect->h;
    int si, sj, di, dj;
    for (sj = 0, dj = 0; sj < sh && dj < dh; ++sj, ++dj) {
        for (si = 0, di = 0; si < sw && di < dw; ++si, ++di) {
            data->fb[(dj + dy) * 256 + di + dx] = 0x8000 |
                ((u16 *) surface->pixels)[(sj + sy) * (surface->w) + si +
                                             sx];
        }
    }*/
    return 0;
}


static void
NDS_RenderPresent(SDL_Renderer * renderer)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    /* Send the data to the display TODO */

    /* Update the flipping chain, if any */
    if (renderer->info.flags & SDL_RENDERER_PRESENTFLIP2) {
        data->current_screen = (data->current_screen + 1) % 2;
    } else if (renderer->info.flags & SDL_RENDERER_PRESENTFLIP3) {
        data->current_screen = (data->current_screen + 1) % 3;
    }
}

static void
NDS_DestroyRenderer(SDL_Renderer * renderer)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    int i;

    printf("NDS_DestroyRenderer(renderer)\n");
    printf(" renderer: %s\n", renderer->info.name);
    if (data) {
        /*for (i = 0; i < SDL_arraysize(data->screens); ++i) {
            if (data->screens[i]) {
                SDL_FreeSurface(data->screens[i]);
            }
        }*/
        SDL_free(data);
    }
    SDL_free(renderer);
}

/* vi: set ts=4 sw=4 expandtab: */
