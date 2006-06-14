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
#include "SDL_config.h"

#include "SDL_video.h"
#include "SDL_sysvideo.h"


/* SDL surface based renderer implementation */

static SDL_Renderer *SDL_SW_CreateRenderer(SDL_Window * window, Uint32 flags);
static int SDL_SW_CreateTexture(SDL_Renderer * renderer,
                                SDL_Texture * texture);
static int SDL_SW_QueryTexturePixels(SDL_Renderer * renderer,
                                     SDL_Texture * texture, void **pixels,
                                     int *pitch);
static int SDL_SW_SetTexturePalette(SDL_Renderer * renderer,
                                    SDL_Texture * texture, SDL_Color * colors,
                                    int firstcolor, int ncolors);
static int SDL_SW_UpdateTexture(SDL_Renderer * renderer,
                                SDL_Texture * texture, SDL_Rect * rect,
                                const void *pixels, int pitch);
static int SDL_SW_LockTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                              SDL_Rect * rect, int markDirty, void **pixels,
                              int *pitch);
static void SDL_SW_UnlockTexture(SDL_Renderer * renderer,
                                 SDL_Texture * texture);
static void SDL_SW_DirtyTexture(SDL_Renderer * renderer,
                                SDL_Texture * texture, int numrects,
                                SDL_Rect * rects);
static void SDL_SW_SelectRenderTexture(SDL_Renderer * renderer,
                                       SDL_Texture * texture);
static void SDL_SW_RenderFill(SDL_Renderer * renderer, SDL_Rect * rect,
                              Uint32 color);
static int SDL_SW_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
                             SDL_Rect * srcrect, SDL_Rect * dstrect,
                             int blendMode, int scaleMode);
static int SDL_SW_RenderReadPixels(SDL_Renderer * renderer, SDL_Rect * rect,
                                   void *pixels, int pitch);
static int SDL_SW_RenderWritePixels(SDL_Renderer * renderer, SDL_Rect * rect,
                                    const void *pixels, int pitch);
static void SDL_SW_RenderPresent(SDL_Renderer * renderer);
static void SDL_SW_DestroyTexture(SDL_Renderer * renderer,
                                  SDL_Texture * texture);
static void SDL_SW_DestroyRenderer(SDL_Renderer * renderer);


SDL_RenderDriver SDL_SW_RenderDriver = {
    SDL_SW_CreateRenderer,
    {
     "software",
     (SDL_Renderer_PresentDiscard |
      SDL_Renderer_PresentCopy |
      SDL_Renderer_PresentFlip2 |
      SDL_Renderer_PresentFlip3 | SDL_Renderer_RenderTarget),
     (SDL_TextureBlendMode_None |
      SDL_TextureBlendMode_Mask | SDL_TextureBlendMode_Blend),
     (SDL_TextureScaleMode_None | SDL_TextureScaleMode_Fast),
     11,
     {
      SDL_PixelFormat_Index8,
      SDL_PixelFormat_RGB555,
      SDL_PixelFormat_RGB565,
      SDL_PixelFormat_RGB888,
      SDL_PixelFormat_BGR888,
      SDL_PixelFormat_ARGB8888,
      SDL_PixelFormat_RGBA8888,
      SDL_PixelFormat_ABGR8888,
      SDL_PixelFormat_BGRA8888,
      SDL_PixelFormat_YUY2,
      SDL_PixelFormat_UYVY},
     32768,
     32768}
};

typedef struct
{
    int current_screen;
    SDL_Surface *screens[3];
    SDL_Surface *target;
    SDL_Renderer *renderer;
} SDL_SW_RenderData;

SDL_Renderer *
SDL_SW_CreateRenderer(SDL_Window * window, Uint32 flags)
{
    SDL_DisplayMode *displayMode = &window->display->current_mode;
    SDL_Renderer *renderer;
    SDL_SW_RenderData *data;
    int i, n;
    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;

    if (!SDL_PixelFormatEnumToMasks
        (displayMode->format, &bpp, &Rmask, &Gmask, &Bmask, &Amask)) {
        SDL_SetError("Unknown display format");
        return NULL;
    }

    renderer = (SDL_Renderer *) SDL_malloc(sizeof(*renderer));
    if (!renderer) {
        SDL_OutOfMemory();
        return NULL;
    }
    SDL_zerop(renderer);

    data = (SDL_SW_RenderData *) SDL_malloc(sizeof(*data));
    if (!data) {
        SDL_SW_DestroyRenderer(renderer);
        SDL_OutOfMemory();
        return NULL;
    }
    SDL_zerop(data);

    renderer->CreateTexture = SDL_SW_CreateTexture;
    renderer->QueryTexturePixels = SDL_SW_QueryTexturePixels;
    renderer->SetTexturePalette = SDL_SW_SetTexturePalette;
    renderer->UpdateTexture = SDL_SW_UpdateTexture;
    renderer->LockTexture = SDL_SW_LockTexture;
    renderer->UnlockTexture = SDL_SW_UnlockTexture;
    renderer->DirtyTexture = SDL_SW_DirtyTexture;
    renderer->SelectRenderTexture = SDL_SW_SelectRenderTexture;
    renderer->RenderFill = SDL_SW_RenderFill;
    renderer->RenderCopy = SDL_SW_RenderCopy;
    renderer->RenderReadPixels = SDL_SW_RenderReadPixels;
    renderer->RenderWritePixels = SDL_SW_RenderWritePixels;
    renderer->RenderPresent = SDL_SW_RenderPresent;
    renderer->DestroyTexture = SDL_SW_DestroyTexture;
    renderer->DestroyRenderer = SDL_SW_DestroyRenderer;
    renderer->info = SDL_SW_RenderDriver.info;
    renderer->window = window;
    renderer->driverdata = data;

    renderer->info.flags = SDL_Renderer_RenderTarget;

    if (flags & SDL_Renderer_PresentFlip2) {
        renderer->info.flags |= SDL_Renderer_PresentFlip2;
        n = 2;
    } else if (flags & SDL_Renderer_PresentFlip3) {
        renderer->info.flags |= SDL_Renderer_PresentFlip3;
        n = 3;
    } else {
        renderer->info.flags |= SDL_Renderer_PresentCopy;
        n = 1;
    }
    for (i = 0; i < n; ++i) {
        data->screens[i] =
            SDL_CreateRGBSurface(0, window->w, window->h, bpp, Rmask, Gmask,
                                 Bmask, Amask);
        if (!data->screens[i]) {
            SDL_SW_DestroyRenderer(renderer);
            return NULL;
        }
    }
    data->current_screen = 0;
    data->target = data->screens[0];

    /* Find a render driver that we can use to display data */
    for (i = 0; i < window->display->num_render_drivers; ++i) {
        SDL_RenderDriver *driver = &window->display->render_drivers[i];
        if (driver->info.name != SDL_SW_RenderDriver.info.name) {
            data->renderer =
                driver->CreateRenderer(window, SDL_Renderer_PresentDiscard);
            if (data->renderer) {
                break;
            }
        }
    }
    if (i == window->display->num_render_drivers) {
        SDL_SW_DestroyRenderer(renderer);
        SDL_SetError("Couldn't find display render driver");
        return NULL;
    }
    return renderer;
}

static int
SDL_SW_CreateTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
    SDL_Surface *surface;
    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;

    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        /* FIXME: implement this */
        return -1;
    }

    if (!SDL_PixelFormatEnumToMasks
        (texture->format, &bpp, &Rmask, &Gmask, &Bmask, &Amask)) {
        SDL_SetError("Unknown texture format");
        return -1;
    }

    surface =
        SDL_CreateRGBSurface(0, texture->w, texture->h, bpp, Rmask, Gmask,
                             Bmask, Amask);
    if (!surface) {
        return -1;
    }

    texture->driverdata = surface;
    return 0;
}

static int
SDL_SW_QueryTexturePixels(SDL_Renderer * renderer, SDL_Texture * texture,
                          void **pixels, int *pitch)
{
    SDL_Surface *surface = (SDL_Surface *) texture->driverdata;

    *pixels = surface->pixels;
    *pitch = surface->pitch;
    return 0;
}

static int
SDL_SW_SetTexturePalette(SDL_Renderer * renderer, SDL_Texture * texture,
                         SDL_Color * colors, int firstcolor, int ncolors)
{
    SDL_Surface *surface = (SDL_Surface *) texture->driverdata;

    SDL_SetColors(surface, colors, firstcolor, ncolors);
    return 0;
}

static int
SDL_SW_UpdateTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                     SDL_Rect * rect, const void *pixels, int pitch)
{
    SDL_Surface *surface = (SDL_Surface *) texture->driverdata;
    Uint8 *src, *dst;
    int row;
    size_t length;

    src = (Uint8 *) pixels;
    dst =
        (Uint8 *) surface->pixels + rect->y * surface->pitch +
        rect->x * surface->format->BytesPerPixel;
    length = rect->w * surface->format->BytesPerPixel;
    for (row = 0; row < rect->h; ++row) {
        SDL_memcpy(dst, src, length);
        src += pitch;
        dst += surface->pitch;
    }
    return 0;
}

static int
SDL_SW_LockTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                   SDL_Rect * rect, int markDirty, void **pixels, int *pitch)
{
    SDL_Surface *surface = (SDL_Surface *) texture->driverdata;

    *pixels =
        (void *) ((Uint8 *) surface->pixels + rect->y * surface->pitch +
                  rect->x * surface->format->BytesPerPixel);
    *pitch = surface->pitch;
    return 0;
}

static void
SDL_SW_UnlockTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
}

static void
SDL_SW_DirtyTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                    int numrects, SDL_Rect * rects)
{
}

static void
SDL_SW_SelectRenderTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
    SDL_SW_RenderData *data = (SDL_SW_RenderData *) renderer->driverdata;
    data->target = (SDL_Surface *) texture->driverdata;
}

static void
SDL_SW_RenderFill(SDL_Renderer * renderer, SDL_Rect * rect, Uint32 color)
{
    SDL_SW_RenderData *data = (SDL_SW_RenderData *) renderer->driverdata;
    Uint8 r, g, b, a;

    a = (Uint8) ((color >> 24) & 0xFF);
    r = (Uint8) ((color >> 16) & 0xFF);
    g = (Uint8) ((color >> 8) & 0xFF);
    b = (Uint8) (color & 0xFF);
    color = SDL_MapRGBA(data->target->format, r, g, b, a);

    SDL_FillRect(data->target, rect, color);
}

static int
SDL_SW_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
                  SDL_Rect * srcrect, SDL_Rect * dstrect, int blendMode,
                  int scaleMode)
{
    SDL_SW_RenderData *data = (SDL_SW_RenderData *) renderer->driverdata;
    SDL_Surface *surface = (SDL_Surface *) texture->driverdata;

    if (blendMode & (SDL_TextureBlendMode_Mask | SDL_TextureBlendMode_Blend)) {
        SDL_SetAlpha(surface, SDL_SRCALPHA, 0);
    } else {
        SDL_SetAlpha(surface, 0, 0);
    }
    if (scaleMode != SDL_TextureScaleMode_None &&
        (srcrect->w != dstrect->w || srcrect->h != dstrect->h)) {
        return SDL_SoftStretch(surface, srcrect, data->target, dstrect);
    } else {
        return SDL_LowerBlit(surface, srcrect, data->target, dstrect);
    }
}

static int
SDL_SW_RenderReadPixels(SDL_Renderer * renderer, SDL_Rect * rect,
                        void *pixels, int pitch)
{
    SDL_SW_RenderData *data = (SDL_SW_RenderData *) renderer->driverdata;
    SDL_Surface *surface = data->target;
    Uint8 *src, *dst;
    int row;
    size_t length;

    src =
        (Uint8 *) surface->pixels + rect->y * surface->pitch +
        rect->x * surface->format->BytesPerPixel;
    dst = (Uint8 *) pixels;
    length = rect->w * surface->format->BytesPerPixel;
    for (row = 0; row < rect->h; ++row) {
        SDL_memcpy(dst, src, length);
        src += surface->pitch;
        dst += pitch;
    }
    return 0;
}

static int
SDL_SW_RenderWritePixels(SDL_Renderer * renderer, SDL_Rect * rect,
                         const void *pixels, int pitch)
{
    SDL_SW_RenderData *data = (SDL_SW_RenderData *) renderer->driverdata;
    SDL_Surface *surface = data->target;
    Uint8 *src, *dst;
    int row;
    size_t length;

    src = (Uint8 *) pixels;
    dst =
        (Uint8 *) surface->pixels + rect->y * surface->pitch +
        rect->x * surface->format->BytesPerPixel;
    length = rect->w * surface->format->BytesPerPixel;
    for (row = 0; row < rect->h; ++row) {
        SDL_memcpy(dst, src, length);
        src += pitch;
        dst += surface->pitch;
    }
    return 0;
}

static void
SDL_SW_RenderPresent(SDL_Renderer * renderer)
{
    SDL_SW_RenderData *data = (SDL_SW_RenderData *) renderer->driverdata;
    SDL_Surface *surface = data->screens[data->current_screen];
    SDL_Rect rect;
    int new_screen;

    /* Send the data to the display */
    /* FIXME: implement dirty rect updates */
    rect.x = 0;
    rect.y = 0;
    rect.w = surface->w;
    rect.h = surface->h;
    data->renderer->RenderWritePixels(data->renderer, &rect, surface->pixels,
                                      surface->pitch);
    data->renderer->RenderPresent(data->renderer);

    /* Update the flipping chain, if any */
    if (renderer->info.flags & SDL_Renderer_PresentFlip2) {
        new_screen = (data->current_screen + 1) % 2;
    } else if (renderer->info.flags & SDL_Renderer_PresentFlip3) {
        new_screen = (data->current_screen + 1) % 3;
    } else {
        new_screen = 0;
    }
    if (data->target == data->screens[data->current_screen]) {
        data->target = data->screens[new_screen];
    }
    data->current_screen = new_screen;
}

static void
SDL_SW_DestroyTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
    SDL_Surface *surface = (SDL_Surface *) texture->driverdata;

    SDL_FreeSurface(surface);
}

static void
SDL_SW_DestroyRenderer(SDL_Renderer * renderer)
{
    SDL_SW_RenderData *data = (SDL_SW_RenderData *) renderer->driverdata;
    int i;

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
