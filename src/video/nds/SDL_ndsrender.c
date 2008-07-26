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
    {   "nds", /* char* name */
        (SDL_RENDERER_SINGLEBUFFER|SDL_RENDERER_ACCELERATED), /* u32 flags */
        (SDL_TEXTUREMODULATE_NONE), /* u32 mod_modes */
        (SDL_TEXTUREBLENDMODE_NONE), /* u32 blend_modes */
        (SDL_TEXTURESCALEMODE_NONE), /* u32 scale_modes */
        3, /* u32 num_texture_formats */
        {
            SDL_PIXELFORMAT_INDEX8,
            SDL_PIXELFORMAT_ABGR1555,
            SDL_PIXELFORMAT_BGR555,
        }, /* u32 texture_formats[20] */
        (256), /* int max_texture_width */
        (256), /* int max_texture_height */
    }
};

typedef struct
{
    bg_attribute *bg;
    u8 bg_taken[4];
    int sub;
} NDS_RenderData;

typedef struct
{
    enum { NDSTX_BG, NDSTX_SPR } type;
    int hw_index;
    struct { int w, h, pitch, bpp; } dim;
    u16 *vram;
} NDS_TextureData;


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

/* again the above shouldn't make it into the stable version */

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
printf("+NDS_CreateRenderer\n");

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
    renderer->CreateTexture = NDS_CreateTexture;
    renderer->QueryTexturePixels = NDS_QueryTexturePixels;
    renderer->SetTexturePalette = NDS_SetTexturePalette;
    renderer->GetTexturePalette = NDS_GetTexturePalette;
    renderer->SetTextureColorMod = NDS_SetTextureColorMod;
    renderer->SetTextureAlphaMod = NDS_SetTextureAlphaMod;
    renderer->SetTextureBlendMode = NDS_SetTextureBlendMode;
    renderer->SetTextureScaleMode = NDS_SetTextureScaleMode;
    renderer->UpdateTexture = NDS_UpdateTexture;
    renderer->LockTexture = NDS_LockTexture;
    renderer->UnlockTexture = NDS_UnlockTexture;
    renderer->DirtyTexture = NDS_DirtyTexture;
    renderer->DestroyTexture = NDS_DestroyTexture;

    renderer->info.mod_modes = NDS_RenderDriver.info.mod_modes;
    renderer->info.blend_modes = NDS_RenderDriver.info.blend_modes;
    renderer->info.scale_modes = NDS_RenderDriver.info.scale_modes;
    renderer->info.num_texture_formats =
        NDS_RenderDriver.info.num_texture_formats;
    SDL_memcpy(renderer->info.texture_formats,
               NDS_RenderDriver.info.texture_formats,
               sizeof(renderer->info.texture_formats));;
    renderer->info.max_texture_width = NDS_RenderDriver.info.max_texture_width;
    renderer->info.max_texture_height =
        NDS_RenderDriver.info.max_texture_height;

    /*data->fb = (u16*)0x06020000;*/
    data->bg = &BACKGROUND;
    data->bg_taken[2] = data->bg_taken[3] = 0;
    data->sub = 0;

printf("-NDS_CreateRenderer\n");
    return renderer;
}

static int
NDS_ActivateRenderer(SDL_Renderer * renderer)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    /* stub. TODO: figure out what needs to be done, if anything. */
printf("!NDS_ActivateRenderer\n");
    return 0;
}

static int
NDS_DisplayModeChanged(SDL_Renderer * renderer)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    /* stub. TODO: figure out what needs to be done */
printf("!NDS_DisplayModeChanged\n");
    return 0;
}

static int
NDS_CreateTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    NDS_TextureData *txdat = NULL;
    int i;
printf("+NDS_CreateTexture\n");
    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        SDL_SetError("Unsupported texture format");
        return -1;
    } else {
        int bpp;
        Uint32 Rmask, Gmask, Bmask, Amask;

        if (!SDL_PixelFormatEnumToMasks
            (texture->format, &bpp, &Rmask, &Gmask, &Bmask, &Amask)) {
            SDL_SetError("Unknown texture format");
            return -1;
        }
        /* conditional statements on w/h to place it as bg/sprite */
        /*if(texture->w <= 64 && texture->h <= 64) {
            sprites not implemented yet.  elegant, I know.
        } else*/ if(texture->w <= 256 && texture->h <= 256) {
            int whichbg = -1;
            if(!data->bg_taken[2]) {
                whichbg = 2;
                data->bg->bg2_rotation.xdx = 0x100;
                data->bg->bg2_rotation.xdy = 0;
                data->bg->bg2_rotation.ydx = 0;
                data->bg->bg2_rotation.ydy = 0x100;
                data->bg->bg2_rotation.centerX = 0;
                data->bg->bg2_rotation.centerY = 0;
            } else if(!data->bg_taken[3]) {
                whichbg = 3;
                data->bg->bg3_rotation.xdx = 0x100;
                data->bg->bg3_rotation.xdy = 0;
                data->bg->bg3_rotation.ydx = 0;
                data->bg->bg3_rotation.ydy = 0x100;
                data->bg->bg3_rotation.centerX = 0;
                data->bg->bg3_rotation.centerY = 0;
            }
            if(whichbg >= 0) {
                data->bg->control[whichbg] = (bpp == 8) ?
                    BG_BMP8_256x256 : BG_BMP16_256x256;
                data->bg->scroll[whichbg].x = 0;
                data->bg->scroll[whichbg].y = 0;
                texture->driverdata = SDL_calloc(1, sizeof(NDS_TextureData));
                txdat = (NDS_TextureData*)texture->driverdata;
                txdat->type = NDSTX_BG;
                txdat->hw_index = whichbg;
                txdat->dim.w = texture->w;
                txdat->dim.h = texture->h;
                txdat->dim.pitch = 256 * (bpp/8);
                txdat->dim.bpp = bpp;
                txdat->vram = (u16*)(data->sub ?
                    BG_BMP_RAM_SUB(whichbg) : BG_BMP_RAM(whichbg));
                for(i = 0; i < 256*256; ++i) {
                    txdat->vram[i] = 0x8000|RGB15(0,31,31);
                }
                for(i = 0; i < 60; ++i) swiWaitForVBlank();
            } else {
                SDL_SetError("Out of NDS backgrounds.");
            }
        } else {
            SDL_SetError("Texture too big for NDS hardware.");
        }
    }

printf("-NDS_CreateTexture\n");
    if (!texture->driverdata) {
        return -1;
    }
    return 0;
}

static int
NDS_QueryTexturePixels(SDL_Renderer * renderer, SDL_Texture * texture,
                      void **pixels, int *pitch)
{
printf("+NDS_QueryTexturePixels\n");
    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        SDL_SetError("Unsupported texture format");
        return -1;
    } else {
        NDS_TextureData *txdat = (NDS_TextureData *) texture->driverdata;

        *pixels = txdat->vram;
        *pitch = txdat->dim.pitch;
printf("-NDS_QueryTexturePixels\n");
        return 0;
    }
}

static int
NDS_SetTexturePalette(SDL_Renderer * renderer, SDL_Texture * texture,
                     const SDL_Color * colors, int firstcolor, int ncolors)
{
printf("+NDS_SetTexturePalette\n");
    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        SDL_SetError("YUV textures don't have a palette");
        return -1;
    } else {
        NDS_TextureData *txdat = (NDS_TextureData *) texture->driverdata;
        /* TODO: mess with 8-bit modes and/or 16-color palette modes */
printf("-NDS_SetTexturePalette\n");
        return 0;
    }
}

static int
NDS_GetTexturePalette(SDL_Renderer * renderer, SDL_Texture * texture,
                     SDL_Color * colors, int firstcolor, int ncolors)
{
printf("+NDS_GetTexturePalette\n");
    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        SDL_SetError("YUV textures don't have a palette");
        return -1;
    } else {
        NDS_TextureData *txdat = (NDS_TextureData *) texture->driverdata;
printf("-NDS_GetTexturePalette\n");
        /* TODO: mess with 8-bit modes and/or 16-color palette modes */
        return 0;
    }
}

static int
NDS_SetTextureColorMod(SDL_Renderer * renderer, SDL_Texture * texture)
{
printf("!NDS_SetTextureColorMod\n");
    /* stub. TODO: figure out what needs to be done, if anything */
    return 0;
}

static int
NDS_SetTextureAlphaMod(SDL_Renderer * renderer, SDL_Texture * texture)
{
printf("!NDS_SetTextureAlphaMod\n");
    /* stub. TODO: figure out what needs to be done, if anything */
    return 0;
}

static int
NDS_SetTextureBlendMode(SDL_Renderer * renderer, SDL_Texture * texture)
{
printf("!NDS_SetTextureBlendMode\n");
    /* stub. TODO: figure out what needs to be done, if anything */
    return 0;
}

static int
NDS_SetTextureScaleMode(SDL_Renderer * renderer, SDL_Texture * texture)
{
printf("!NDS_SetTextureScaleMode\n");
    /* stub. TODO: figure out what needs to be done.
       (NDS hardware scaling is nearest neighbor.) */
    return 0;
}

static int
NDS_UpdateTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                 const SDL_Rect * rect, const void *pixels, int pitch)
{
printf("+NDS_UpdateTexture\n");
    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        SDL_SetError("Unsupported texture format");
        return -1;
    } else {
        NDS_TextureData *txdat = (NDS_TextureData *) texture->driverdata;
        Uint8 *src, *dst;
        int row;
        size_t length;
        /* IMPORTANT! copy the new pixels into the sprite or bg. */
        src = (Uint8 *) pixels;
        dst =
            (Uint8 *) txdat->vram + rect->y * txdat->dim.pitch +
            rect->x * (txdat->dim.bpp/8);
        length = rect->w * (txdat->dim.bpp/8);
        for (row = 0; row < rect->h; ++row) {
            SDL_memcpy(dst, src, length);
            src += pitch;
            dst += txdat->dim.pitch;
        }
printf("-NDS_UpdateTexture\n");
        return 0;
    }
}

static int
NDS_LockTexture(SDL_Renderer * renderer, SDL_Texture * texture,
               const SDL_Rect * rect, int markDirty, void **pixels,
               int *pitch)
{
printf("+NDS_LockTexture\n");
    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        SDL_SetError("Unsupported texture format");
        return -1;
    } else {
        NDS_TextureData *txdat = (NDS_TextureData *) texture->driverdata;

        if (markDirty) {
            /*SDL_AddDirtyRect(&txdat->dirty, rect);*/
        }

        *pixels = (void *) ((u8 *)txdat->vram + rect->y * txdat->dim.pitch
                            + rect->x * (txdat->dim.bpp/8));
        *pitch = txdat->dim.pitch;
printf("-NDS_LockTexture\n");
        return 0;
    }
}

static void
NDS_UnlockTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        SDL_SetError("Unsupported texture format");
    }
printf("!NDS_UnlockTexture\n");
}

static void
NDS_DirtyTexture(SDL_Renderer * renderer, SDL_Texture * texture,
                int numrects, const SDL_Rect * rects)
{ /* stub */
printf("!NDS_DirtyTexture\n");
}

static int
NDS_RenderFill(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b,
                   Uint8 a, const SDL_Rect * rect)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    SDL_Rect real_rect = *rect;
    u16 color;
    int i, j;

printf("+NDS_RenderFill\n");
    /* TODO: make a single-color sprite and stretch it.
    color = RGB15(r>>3,g>>3,b>>3);
    for (i = real_rect.x; i < real_rect.x+real_rect.w; ++i) {
        for (j = real_rect.y; j < real_rect.y+real_rect.h; ++j) {
            data->fb[(j + real_rect.y) * 256 + i + real_rect.x] = 
                0x8000 | color;
        }
    }*/
printf("-NDS_RenderFill\n");
    return 0;
}

static int
NDS_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
                   const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    NDS_TextureData *txdat = (NDS_TextureData *) texture->driverdata;
//    SDL_Window *window = SDL_GetWindowFromID(renderer->window);
//    SDL_VideoDisplay *display = SDL_GetDisplayFromWindow(window);
    int i;
printf("+NDS_RenderCopy\n");
    for(i = 0; i <= 0xFFFF; ++i) {
        txdat->vram[i] = 0x8000|i;
    }
    printf("/txdat->hw_index = %d\n", txdat->hw_index);
#if 0
    if (txdat->dirty.list) {
        SDL_DirtyRect *dirty;
        void *pixels;
        int bpp = SDL_BYTESPERPIXEL(texture->format);
        int pitch = txdat->pitch;
/* below was borrowed from SDL_renderer_gl.c.  doesn't work, obv.
   figure out how to get the graphics data into VRAM. */

        for (dirty = txdat->dirty.list; dirty; dirty = dirty->next) {
            SDL_Rect *rect = &dirty->rect;
            /*pixels =
                (void *) ((Uint8 *) txdat->vram + rect->y * pitch +
                          rect->x * bpp);
            data->glTexSubImage2D(texturedata->type, 0, rect->x, rect->y,
                                  rect->w, rect->h, texturedata->format,
                                  texturedata->formattype, pixels);*/
        }
        SDL_ClearDirtyRects(&txdat->dirty);
    }
#endif
printf("-NDS_RenderCopy\n");
    return 0;
}


static void
NDS_RenderPresent(SDL_Renderer * renderer)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    /* Send the data to the display TODO */

printf("+NDS_RenderPresent\n");
    /* Update the flipping chain, if any */
    if (renderer->info.flags & SDL_RENDERER_PRESENTVSYNC) {
        swiWaitForVBlank();
    }
printf("-NDS_RenderPresent\n");
}

static void
NDS_DestroyTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
printf("+NDS_DestroyTexture\n");
    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        SDL_SetError("Unsupported texture format");
    } else {
        /* free anything else allocated for texture */
        NDS_TextureData *txdat = texture->driverdata;
        /*SDL_FreeDirtyRects(&txdat->dirty);*/
        SDL_free(txdat);
    }
printf("-NDS_DestroyTexture\n");
}

static void
NDS_DestroyRenderer(SDL_Renderer * renderer)
{
    NDS_RenderData *data = (NDS_RenderData *) renderer->driverdata;
    /*SDL_Window *window = SDL_GetWindowFromID(renderer->window);
    SDL_VideoDisplay *display = SDL_GetDisplayFromWindow(window);*/
    int i;

printf("+NDS_DestroyRenderer\n");
    if (data) {
        /* TODO: free anything relevant. */
        /*for (i = 0; i < SDL_arraysize(data->texture); ++i) {
            if (data->texture[i]) {
                DestroyTexture(data->renderer, data->texture[i]);
            }
        }
        if (data->surface.format) {
            SDL_SetSurfacePalette(&data->surface, NULL);
            SDL_FreeFormat(data->surface.format);
        }
        if (display->palette) {
            SDL_DelPaletteWatch(display->palette, DisplayPaletteChanged,
                                data);
        }
        SDL_FreeDirtyRects(&data->dirty);*/
        SDL_free(data);
    }
    SDL_free(renderer);
printf("-NDS_DestroyRenderer\n");
}

/* vi: set ts=4 sw=4 expandtab: */
