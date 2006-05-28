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

/* This is the implementation of the YUV video surface support */

#include "SDL_video.h"
#include "SDL_sysvideo.h"
#include "SDL_yuvfuncs.h"
#include "SDL_yuv_sw_c.h"


SDL_Overlay *
SDL_CreateYUVOverlay (int w, int h, Uint32 format, SDL_Surface * display)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();
    SDL_Window *window;
    const char *yuv_hwaccel;
    SDL_Overlay *overlay;

    window = SDL_GetWindowFromSurface (display);
    if (window && (window->flags & SDL_WINDOW_OPENGL)) {
        SDL_SetError ("YUV overlays are not supported in OpenGL mode");
        return NULL;
    }

    /* Display directly on video surface, if possible */
    if (SDL_getenv ("SDL_VIDEO_YUV_DIRECT")) {
        if (window &&
            ((window->surface->format->BytesPerPixel == 2) ||
             (window->surface->format->BytesPerPixel == 4))) {
            display = window->surface;
        }
    }
    overlay = NULL;
    yuv_hwaccel = SDL_getenv ("SDL_VIDEO_YUV_HWACCEL");
    if (((display->flags & SDL_SCREEN_SURFACE) && _this->CreateYUVOverlay) &&
        (!yuv_hwaccel || (SDL_atoi (yuv_hwaccel) > 0))) {
        overlay = _this->CreateYUVOverlay (_this, w, h, format, display);
    }
    /* If hardware YUV overlay failed ... */
    if (overlay == NULL) {
        overlay = SDL_CreateYUV_SW (_this, w, h, format, display);
    }
    return overlay;
}

int
SDL_LockYUVOverlay (SDL_Overlay * overlay)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();
    return overlay->hwfuncs->Lock (_this, overlay);
}

void
SDL_UnlockYUVOverlay (SDL_Overlay * overlay)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();
    overlay->hwfuncs->Unlock (_this, overlay);
}

int
SDL_DisplayYUVOverlay (SDL_Overlay * overlay, SDL_Rect * dstrect)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();
    SDL_Rect src, dst;
    int srcx, srcy, srcw, srch;
    int dstx, dsty, dstw, dsth;

    /* Clip the rectangle to the screen area */
    srcx = 0;
    srcy = 0;
    srcw = overlay->w;
    srch = overlay->h;
    dstx = dstrect->x;
    dsty = dstrect->y;
    dstw = dstrect->w;
    dsth = dstrect->h;
    if (dstx < 0) {
        srcw += (dstx * overlay->w) / dstrect->w;
        dstw += dstx;
        srcx -= (dstx * overlay->w) / dstrect->w;
        dstx = 0;
    }
    if ((dstx + dstw) > SDL_VideoSurface->w) {
        int extra = (dstx + dstw - SDL_VideoSurface->w);
        srcw -= (extra * overlay->w) / dstrect->w;
        dstw -= extra;
    }
    if (dsty < 0) {
        srch += (dsty * overlay->h) / dstrect->h;
        dsth += dsty;
        srcy -= (dsty * overlay->h) / dstrect->h;
        dsty = 0;
    }
    if ((dsty + dsth) > SDL_VideoSurface->h) {
        int extra = (dsty + dsth - SDL_VideoSurface->h);
        srch -= (extra * overlay->h) / dstrect->h;
        dsth -= extra;
    }
    if (srcw <= 0 || srch <= 0 || srch <= 0 || dsth <= 0) {
        return 0;
    }
    /* Ugh, I can't wait for SDL_Rect to be int values */
    src.x = srcx;
    src.y = srcy;
    src.w = srcw;
    src.h = srch;
    dst.x = dstx;
    dst.y = dsty;
    dst.w = dstw;
    dst.h = dsth;
    return overlay->hwfuncs->Display (_this, overlay, &src, &dst);
}

void
SDL_FreeYUVOverlay (SDL_Overlay * overlay)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();
    if (overlay) {
        if (overlay->hwfuncs) {
            overlay->hwfuncs->FreeHW (_this, overlay);
        }
        SDL_free (overlay);
    }
}

/* vi: set ts=4 sw=4 expandtab: */
