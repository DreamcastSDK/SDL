/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

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
#include "../SDL_sysvideo.h"
#include "../SDL_yuv_sw_c.h"
#include "../SDL_renderer_sw.h"

#include "SDL_ps3video.h"
#include "spulibs/spu_common.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include <asm/ps3fb.h>

/* SDL surface based renderer implementation */

static SDL_Renderer *SDL_PS3_CreateRenderer(SDL_Window * window,
                                              Uint32 flags);
static int SDL_PS3_RenderPoint(SDL_Renderer * renderer, int x, int y);
static int SDL_PS3_RenderLine(SDL_Renderer * renderer, int x1, int y1,
                                int x2, int y2);
static int SDL_PS3_RenderFill(SDL_Renderer * renderer,
                                const SDL_Rect * rect);
static int SDL_PS3_RenderCopy(SDL_Renderer * renderer,
                                SDL_Texture * texture,
                                const SDL_Rect * srcrect,
                                const SDL_Rect * dstrect);
static void SDL_PS3_RenderPresent(SDL_Renderer * renderer);
static void SDL_PS3_DestroyRenderer(SDL_Renderer * renderer);

/* Texture */
static int PS3_CreateTexture(SDL_Renderer * renderer, SDL_Texture * texture);
static void PS3_DestroyTexture(SDL_Renderer * renderer, SDL_Texture * texture);


SDL_RenderDriver SDL_PS3_RenderDriver = {
    SDL_PS3_CreateRenderer,
    {
     "ps3",
     (SDL_RENDERER_SINGLEBUFFER | SDL_RENDERER_PRESENTVSYNC |
      SDL_RENDERER_PRESENTFLIP2)
     /* (SDL_RENDERER_SINGLEBUFFER | SDL_RENDERER_PRESENTCOPY |
      SDL_RENDERER_PRESENTFLIP2 | SDL_RENDERER_PRESENTFLIP3 |
      SDL_RENDERER_PRESENTDISCARD) */,
     }
};

typedef struct
{
    int current_screen;
    SDL_Surface *screens[3];
    SDL_VideoDisplay *display;
    uint8_t *center[2];

    /* width of input (bounded by writeable width) */
    unsigned int bounded_width;
    /* height of input (bounded by writeable height) */
    unsigned int bounded_height;
    /* offset from the left side (used for centering) */
    unsigned int offset_left;
    /* offset from the upper side (used for centering) */
    unsigned int offset_top;
    /* width of screen which is writeable */
    unsigned int wr_width;
    /* width of screen which is writeable */
    unsigned int wr_height;
    /* size of a screen line: width * bpp/8 */
    unsigned int line_length;

    /* Use two buffers in fb? res < 720p */
    unsigned int double_buffering;
} SDL_PS3_RenderData;

typedef struct
{
    void *pixels;
    int pitch;
    int bpp;
} PS3_TextureData;

SDL_Renderer *
SDL_PS3_CreateRenderer(SDL_Window * window, Uint32 flags)
{
    deprintf(1, "SDL_PS3_CreateRenderer()\n");
    SDL_VideoDisplay *display = SDL_GetDisplayFromWindow(window);
    SDL_DisplayMode *displayMode = &display->current_mode;
    SDL_Renderer *renderer;
    SDL_PS3_RenderData *data;
    int i, n;
    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;

    if (!SDL_PixelFormatEnumToMasks
        (displayMode->format, &bpp, &Rmask, &Gmask, &Bmask, &Amask)) {
        SDL_SetError("Unknown display format");
        return NULL;
    }

    renderer = (SDL_Renderer *) SDL_calloc(1, sizeof(*renderer));
    if (!renderer) {
        SDL_OutOfMemory();
        return NULL;
    }

    data = (SDL_PS3_RenderData *) SDL_malloc(sizeof(*data));
    if (!data) {
        SDL_PS3_DestroyRenderer(renderer);
        SDL_OutOfMemory();
        return NULL;
    }
    SDL_zerop(data);

    //renderer->CreateTexture = PS3_CreateTexture;
    //renderer->DestroyTexture = PS3_DestroyTexture;
    renderer->RenderPoint = SDL_PS3_RenderPoint;
    renderer->RenderLine = SDL_PS3_RenderLine;
    renderer->RenderFill = SDL_PS3_RenderFill;
    renderer->RenderCopy = SDL_PS3_RenderCopy;
    renderer->RenderPresent = SDL_PS3_RenderPresent;
    renderer->DestroyRenderer = SDL_PS3_DestroyRenderer;
    renderer->info.name = SDL_PS3_RenderDriver.info.name;
    renderer->info.flags = 0;
    renderer->window = window->id;
    renderer->driverdata = data;
    Setup_SoftwareRenderer(renderer);

    deprintf(1, "window->w = %u\n", window->w);
    deprintf(1, "window->h = %u\n", window->h);

    data->double_buffering = 0;

    if (flags & SDL_RENDERER_PRESENTFLIP2) {
        renderer->info.flags |= SDL_RENDERER_PRESENTFLIP2;
        n = 2;
        data->double_buffering = 1;
    } else {
        renderer->info.flags |= SDL_RENDERER_PRESENTCOPY;
        n = 1;
    }
    for (i = 0; i < n; ++i) {
        data->screens[i] =
            SDL_CreateRGBSurface(0, window->w, window->h, bpp, Rmask, Gmask,
                                 Bmask, Amask);
        if (!data->screens[i]) {
            SDL_PS3_DestroyRenderer(renderer);
            return NULL;
        }
        /* Allocate aligned memory for pixels */
        SDL_free(data->screens[i]->pixels);
        data->screens[i]->pixels = (void *)memalign(16, data->screens[i]->h * data->screens[i]->pitch);
        if (!data->screens[i]->pixels) {
            SDL_FreeSurface(data->screens[i]);
            SDL_OutOfMemory();
            return NULL;
        }
        SDL_memset(data->screens[i]->pixels, 0, data->screens[i]->h * data->screens[i]->pitch);
        SDL_SetSurfacePalette(data->screens[i], display->palette);
    }
    data->current_screen = 0;

    return renderer;
}

static int
PS3_CreateTexture(SDL_Renderer * renderer, SDL_Texture * texture) {
    deprintf(1, "PS3_CreateTexture()\n");
    PS3_TextureData *data;
    data = (PS3_TextureData *) SDL_calloc(1, sizeof(*data));
    if (!data) {
        SDL_OutOfMemory();
        return -1;
    }

    data->pitch = (texture->w * SDL_BYTESPERPIXEL(texture->format));

    data->pixels = NULL;
    data->pixels = (void *)memalign(16, texture->h * data->pitch);
    if (!data->pixels) {
        PS3_DestroyTexture(renderer, texture);
        SDL_OutOfMemory();
        return -1;
    }

    texture->driverdata = data;
    return 0;
}

static void
PS3_DestroyTexture(SDL_Renderer * renderer, SDL_Texture * texture)
{
    PS3_TextureData *data = (PS3_TextureData *) texture->driverdata;

    if (!data) {
        return;
    }

    free(data->pixels);
}

static int
SDL_PS3_RenderPoint(SDL_Renderer * renderer, int x, int y)
{
    SDL_PS3_RenderData *data =
        (SDL_PS3_RenderData *) renderer->driverdata;
    SDL_Surface *target = data->screens[data->current_screen];
    int status;

    if (renderer->blendMode == SDL_BLENDMODE_NONE ||
        renderer->blendMode == SDL_BLENDMODE_MASK) {
        Uint32 color =
            SDL_MapRGBA(target->format, renderer->r, renderer->g, renderer->b,
                        renderer->a);

        status = SDL_DrawPoint(target, x, y, color);
    } else {
        status =
            SDL_BlendPoint(target, x, y, renderer->blendMode, renderer->r,
                           renderer->g, renderer->b, renderer->a);
    }
    return status;
}

static int
SDL_PS3_RenderLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2)
{
    SDL_PS3_RenderData *data =
        (SDL_PS3_RenderData *) renderer->driverdata;
    SDL_Surface *target = data->screens[data->current_screen];
    int status;

    if (renderer->blendMode == SDL_BLENDMODE_NONE ||
        renderer->blendMode == SDL_BLENDMODE_MASK) {
        Uint32 color =
            SDL_MapRGBA(target->format, renderer->r, renderer->g, renderer->b,
                        renderer->a);

        status = SDL_DrawLine(target, x1, y1, x2, y2, color);
    } else {
        status =
            SDL_BlendLine(target, x1, y1, x2, y2, renderer->blendMode,
                          renderer->r, renderer->g, renderer->b, renderer->a);
    }
    return status;
}

static int
SDL_PS3_RenderFill(SDL_Renderer * renderer, const SDL_Rect * rect)
{
    deprintf(1, "SDL_PS3_RenderFill()\n");
    SDL_PS3_RenderData *data =
        (SDL_PS3_RenderData *) renderer->driverdata;
    SDL_Surface *target = data->screens[data->current_screen];
    SDL_Rect real_rect = *rect;
    int status;

    if (renderer->blendMode == SDL_BLENDMODE_NONE) {
        Uint32 color =
            SDL_MapRGBA(target->format, renderer->r, renderer->g, renderer->b,
                        renderer->a);

        status = SDL_FillRect(target, &real_rect, color);
    } else {
        status =
            SDL_BlendRect(target, &real_rect, renderer->blendMode,
                          renderer->r, renderer->g, renderer->b, renderer->a);
    }
    return status;
}

static int
SDL_PS3_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
                     const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
    deprintf(1, "SDL_PS3_RenderCopy()\n");
    SDL_PS3_RenderData *data =
        (SDL_PS3_RenderData *) renderer->driverdata;
    SDL_Window *window = SDL_GetWindowFromID(renderer->window);
    SDL_VideoDisplay *display = SDL_GetDisplayFromWindow(window);
    PS3_TextureData *txdata = (PS3_TextureData *) texture->driverdata;
    SDL_VideoData *devdata = display->device->driverdata;

    if (SDL_ISPIXELFORMAT_FOURCC(texture->format)) {
        deprintf(1, "SDL_ISPIXELFORMAT_FOURCC = true\n");
        SDL_Surface *target = data->screens[data->current_screen];
        void *pixels =
            (Uint8 *) target->pixels + dstrect->y * target->pitch +
            dstrect->x * target->format->BytesPerPixel;
        return SDL_SW_CopyYUVToRGB((SDL_SW_YUVTexture *) texture->driverdata,
                                   srcrect, display->current_mode.format,
                                   dstrect->w, dstrect->h, pixels,
                                   target->pitch);
    } else {
        deprintf(1, "SDL_ISPIXELFORMAT_FOURCC = false\n");
        SDL_Surface *surface = (SDL_Surface *) texture->driverdata;
        SDL_Surface *target = data->screens[data->current_screen];
        SDL_Rect real_srcrect = *srcrect;
        SDL_Rect real_dstrect = *dstrect;

        deprintf(1, "surface->w = %u\n", surface->w); // FIXME: surface->w changes to 32
        deprintf(1, "surface->h = %u\n", surface->h);

        deprintf(1, "srcrect->w = %u\n", srcrect->w);
        deprintf(1, "srcrect->h = %u\n", srcrect->h);
        deprintf(1, "dstrect->w = %u\n", dstrect->w);
        deprintf(1, "dstrect->h = %u\n", dstrect->h);

        deprintf(1, "txdata->bpp = %u\n", txdata->bpp);
        deprintf(1, "texture->format (bpp) = %u\n", SDL_BYTESPERPIXEL(texture->format));

        /* For testing, align pixels */
        void *pixels = (void *)memalign(16, dstrect->h * data->screens[0]->pitch);
        SDL_memcpy(pixels, surface->pixels, dstrect->h * data->screens[0]->pitch);

        /* Get screeninfo */
        struct fb_fix_screeninfo fb_finfo;
        if (ioctl(devdata->fbdev, FBIOGET_FSCREENINFO, &fb_finfo)) {
            SDL_SetError("[PS3] Can't get fixed screeninfo");
            return -1;
        }
        struct fb_var_screeninfo fb_vinfo;
        if (ioctl(devdata->fbdev, FBIOGET_VSCREENINFO, &fb_vinfo)) {
            SDL_SetError("[PS3] Can't get VSCREENINFO");
            return -1;
        }
        /* 16 and 15 bpp is reported as 16 bpp */
        txdata->bpp = fb_vinfo.bits_per_pixel;
        if (txdata->bpp == 16)
            txdata->bpp = fb_vinfo.red.length + fb_vinfo.green.length + fb_vinfo.blue.length;

        /* Adjust centering */
        data->bounded_width = window->w < fb_vinfo.xres ? window->w : fb_vinfo.xres;
        data->bounded_height = window->h < fb_vinfo.yres ? window->h : fb_vinfo.yres;
        data->offset_left = (fb_vinfo.xres - data->bounded_width) >> 1;
        data->offset_top = (fb_vinfo.yres - data->bounded_height) >> 1;
        data->center[0] = devdata->frame_buffer + data->offset_left * /*txdata->bpp/8*/ 4 +
                    data->offset_top * fb_finfo.line_length;
        data->center[1] = data->center[0] + fb_vinfo.yres * fb_finfo.line_length;

        deprintf(1, "offset_left = %u\n", data->offset_left);
        deprintf(1, "offset_top = %u\n", data->offset_top);

        /* Set SPU parms for copying the surface to framebuffer */
        devdata->fb_parms->data = (unsigned char *)pixels;
        devdata->fb_parms->center = data->center[data->current_screen];
        devdata->fb_parms->out_line_stride = fb_finfo.line_length;
        devdata->fb_parms->in_line_stride = dstrect->w * /*txdata->bpp / 8*/4;
        devdata->fb_parms->bounded_input_height = data->bounded_height;
        devdata->fb_parms->bounded_input_width = data->bounded_width;
        devdata->fb_parms->fb_pixel_size = txdata->bpp / 8;

        deprintf(3, "[PS3->SPU] fb_thread_data->argp = 0x%x\n", devdata->fb_thread_data->argp);
        
        /* Copying.. */
        SPE_SendMsg(devdata->fb_thread_data, SPU_START);
        SPE_SendMsg(devdata->fb_thread_data, (unsigned int)devdata->fb_thread_data->argp);
        
        SPE_WaitForMsg(devdata->fb_thread_data, SPU_FIN);
        free(pixels);

        return 0;
        //return SDL_LowerBlit(surface, &real_srcrect, target, &real_dstrect);
    }
}

static void
SDL_PS3_RenderPresent(SDL_Renderer * renderer)
{
    deprintf(1, "SDL_PS3_RenderPresent()\n");
    static int frame_number;
    SDL_PS3_RenderData *data =
        (SDL_PS3_RenderData *) renderer->driverdata;
    SDL_Window *window = SDL_GetWindowFromID(renderer->window);
    SDL_VideoDisplay *display = SDL_GetDisplayFromWindow(window);
    SDL_VideoData *devdata = display->device->driverdata;

    /* Send the data to the display */
    if (SDL_getenv("SDL_VIDEO_PS3_SAVE_FRAMES")) {
        char file[128];
        SDL_snprintf(file, sizeof(file), "SDL_window%d-%8.8d.bmp",
                     renderer->window, ++frame_number);
        SDL_SaveBMP(data->screens[data->current_screen], file);
    }

    /* Wait for vsync */
    if (renderer->info.flags & SDL_RENDERER_PRESENTVSYNC) {
        unsigned long crt = 0;
        deprintf(1, "[PS3] Wait for vsync\n");
        ioctl(devdata->fbdev, FBIO_WAITFORVSYNC, &crt);
    }

    /* Page flip */
    deprintf(1, "[PS3] Page flip to buffer #%u 0x%x\n", data->current_screen, data->center[data->current_screen]);
    ioctl(devdata->fbdev, PS3FB_IOCTL_FSEL, (unsigned long)&data->current_screen);

    /* Update the flipping chain, if any */
    if (renderer->info.flags & SDL_RENDERER_PRESENTFLIP2 && data->double_buffering) {
        data->current_screen = (data->current_screen + 1) % 2;
    }
}

static void
SDL_PS3_DestroyRenderer(SDL_Renderer * renderer)
{
    deprintf(1, "SDL_PS3_DestroyRenderer()\n");
    SDL_PS3_RenderData *data =
        (SDL_PS3_RenderData *) renderer->driverdata;
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
