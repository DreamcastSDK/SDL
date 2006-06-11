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

/* This file contains functions for backwards compatibility with SDL 1.2 */

#include "SDL.h"

#include "video/SDL_sysvideo.h"


static SDL_WindowID window;
static char *wm_title;

char *
SDL_AudioDriverName(char *namebuf, int maxlen)
{
    const char *name = SDL_GetCurrentAudioDriver();
    if (name) {
        SDL_strlcpy(namebuf, name, maxlen);
        return namebuf;
    }
    return NULL;
}

char *
SDL_VideoDriverName(char *namebuf, int maxlen)
{
    const char *name = SDL_GetCurrentVideoDriver();
    if (name) {
        SDL_strlcpy(namebuf, name, maxlen);
        return namebuf;
    }
    return NULL;
}

const SDL_VideoInfo *
SDL_GetVideoInfo(void)
{
    static SDL_VideoInfo info;

    /* Memory leak, compatibility code, who cares? */
    if (!info.vfmt && SDL_GetDesktopDisplayMode()) {
        int bpp;
        Uint32 Rmask, Gmask, Bmask, Amask;

        SDL_PixelFormatEnumToMasks(SDL_GetDesktopDisplayMode()->format, &bpp,
                                   &Rmask, &Gmask, &Bmask, &Amask);
        info.vfmt = SDL_AllocFormat(bpp, Rmask, Gmask, Bmask, Amask);
    }
    return &info;
}

int
SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags)
{
    int i, actual_bpp = 0;

    if (!SDL_GetVideoDevice()) {
        return 0;
    }

    if (!(flags & SDL_FULLSCREEN)) {
        return SDL_BITSPERPIXEL(SDL_GetDesktopDisplayMode()->format);
    }

    for (i = 0; i < SDL_GetNumDisplayModes(); ++i) {
        const SDL_DisplayMode *mode = SDL_GetDisplayMode(i);
        if (!mode->w || !mode->h || (width == mode->w && height == mode->h)) {
            if (!mode->format) {
                return bpp;
            }
            if (SDL_BITSPERPIXEL(mode->format) >= bpp) {
                actual_bpp = SDL_BITSPERPIXEL(mode->format);
            }
        }
    }
    return actual_bpp;
}

SDL_Rect **
SDL_ListModes(SDL_PixelFormat * format, Uint32 flags)
{
    int i, nmodes;
    SDL_Rect **modes;

    if (!SDL_GetVideoDevice()) {
        return NULL;
    }

    if (!(flags & SDL_FULLSCREEN)) {
        return (SDL_Rect **) (-1);
    }

    /* Memory leak, but this is a compatibility function, who cares? */
    nmodes = 0;
    for (i = 0; i < SDL_GetNumDisplayModes(); ++i) {
        const SDL_DisplayMode *mode = SDL_GetDisplayMode(i);
        if (!mode->w || !mode->h) {
            return (SDL_Rect **) (-1);
        }
        if (SDL_BITSPERPIXEL(mode->format) != format->BitsPerPixel) {
            continue;
        }
        if (nmodes > 0 && modes[nmodes - 1]->w == mode->w
            && modes[nmodes - 1]->h == mode->h) {
            continue;
        }

        modes = SDL_realloc(modes, (nmodes + 2) * sizeof(*modes));
        if (!modes) {
            return NULL;
        }
        modes[nmodes] = (SDL_Rect *) SDL_malloc(sizeof(SDL_Rect));
        if (!modes[nmodes]) {
            return NULL;
        }
        modes[nmodes]->x = 0;
        modes[nmodes]->y = 0;
        modes[nmodes]->w = mode->w;
        modes[nmodes]->h = mode->h;
        ++nmodes;
    }
    if (modes) {
        modes[nmodes] = NULL;
    }
    return modes;
}

static int (*orig_eventfilter) (const SDL_Event * event);

static int
SDL_CompatEventFilter(const SDL_Event * event)
{
    SDL_Event fake;

    switch (event->type) {
    case SDL_WINDOWEVENT:
        switch (event->window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            fake.type = SDL_VIDEORESIZE;
            fake.resize.w = event->window.data1;
            fake.resize.h = event->window.data2;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 0;
            fake.active.state = SDL_APPACTIVE;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 1;
            fake.active.state = SDL_APPACTIVE;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_ENTER:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 1;
            fake.active.state = SDL_APPMOUSEFOCUS;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_LEAVE:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 0;
            fake.active.state = SDL_APPMOUSEFOCUS;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 1;
            fake.active.state = SDL_APPINPUTFOCUS;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 1;
            fake.active.state = SDL_APPINPUTFOCUS;
            SDL_PushEvent(&fake);
            break;
        }
    }
    return orig_eventfilter(event);
}

SDL_Surface *
SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
    int (*filter) (const SDL_Event * event);
    const SDL_DisplayMode *desktop_mode;
    SDL_DisplayMode mode;
    int i;
    Uint32 window_flags;
    Uint32 desktop_format;
    Uint32 desired_format;

    if (!SDL_GetVideoDevice()) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
            return NULL;
        }
    }

    /* Destroy existing window */
    SDL_DestroyWindow(window);

    /* Set up the event filter */
    filter = SDL_GetEventFilter();
    if (filter != SDL_CompatEventFilter) {
        orig_eventfilter = filter;
    }
    SDL_SetEventFilter(SDL_CompatEventFilter);

    /* Create a new window */
    window_flags = SDL_WINDOW_SHOWN;
    if (flags & SDL_FULLSCREEN) {
        window_flags |= SDL_WINDOW_FULLSCREEN;
    }
    if (flags & SDL_OPENGL) {
        window_flags |= SDL_WINDOW_OPENGL;
    }
    if (flags & SDL_RESIZABLE) {
        window_flags |= SDL_WINDOW_RESIZABLE;
    }
    if (flags & SDL_NOFRAME) {
        window_flags |= SDL_WINDOW_BORDERLESS;
    }
    window = SDL_CreateWindow(wm_title, 0, 0, width, height, window_flags);
    if (!window) {
        return NULL;
    }

    /* Set up the desired display mode */
    desktop_mode = SDL_GetDesktopDisplayMode();
    desktop_format = desktop_mode->format;
    if (desktop_format && ((flags & SDL_ANYFORMAT)
                           || (bpp == SDL_BITSPERPIXEL(desktop_format)))) {
        desired_format = desktop_format;
    } else {
        switch (bpp) {
        case 0:
            if (desktop_format) {
                desired_format = desktop_format;
            } else {
                desired_format = SDL_PixelFormat_RGB888;
            }
            break;
        case 8:
            desired_format = SDL_PixelFormat_Index8;
            break;
        case 15:
            desired_format = SDL_PixelFormat_RGB555;
            break;
        case 16:
            desired_format = SDL_PixelFormat_RGB565;
            break;
        case 24:
            desired_format = SDL_PixelFormat_RGB24;
            break;
        case 32:
            desired_format = SDL_PixelFormat_RGB888;
            break;
        default:
            SDL_SetError("Unsupported bpp in SDL_SetVideoMode()");
            return NULL;
        }
    }
    mode.format = desired_format;
    mode.w = width;
    mode.h = height;
    mode.refresh_rate = 0;

    /* Set the desired display mode */
    if (flags & SDL_FULLSCREEN) {
        if (!SDL_GetClosestDisplayMode(&mode, &mode)) {
            return NULL;
        }
    } else {
        if (desktop_format) {
            mode.format = desktop_format;
        }
        if (desktop_mode->w && desktop_mode->h) {
            mode.w = desktop_mode->w;
            mode.h = desktop_mode->h;
        }
        mode.refresh_rate = desktop_mode->refresh_rate;
    }
    if (SDL_SetDisplayMode(&mode) < 0) {
        return NULL;
    }

    /* Create the display surface */
    return SDL_CreateWindowSurface(window, desired_format, flags);
}

SDL_Surface *
SDL_CreateWindowSurface(SDL_WindowID windowID, Uint32 format, Uint32 flags)
{
    SDL_Window *window = SDL_GetWindowFromID(windowID);
    Uint32 black;
    SDL_Surface *surface;

    if (!window) {
        return NULL;
    }

    if (!_this->CreateWindowSurface) {
        return NULL;
    }

    if (!window->surface) {
        _this->CreateWindowSurface(_this, window, flags);
        if (!window->surface) {
            return NULL;
        }
        window->surface->flags |= SDL_SCREEN_SURFACE;

        /* If we have a palettized surface, create a default palette */
        if (window->surface->format->palette) {
            SDL_Color colors[256];
            SDL_PixelFormat *vf = window->surface->format;
            SDL_DitherColors(colors, vf->BitsPerPixel);
            SDL_SetColors(window->surface, colors, 0, vf->palette->ncolors);
        }
    }
    surface = window->surface;

    if (window->shadow) {
        SDL_FreeSurface(window->shadow);
        window->shadow = NULL;
    }

    /* Create a shadow surface if necessary */
    if ((!(flags & SDL_ANYFORMAT)
         && (format != SDL_GetCurrentDisplayMode()->format))
        || ((flags & SDL_HWPALETTE)
            && !(window->surface->flags & SDL_HWPALETTE))) {
        int bpp;
        Uint32 Rmask, Gmask, Bmask, Amask;

        SDL_PixelFormatEnumToMasks(format, &bpp, &Amask, &Gmask, &Bmask,
                                   &Amask);
        window->shadow =
            SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, bpp,
                                 Rmask, Gmask, Bmask, Amask);
        if (window->shadow == NULL) {
            return NULL;
        }
        window->shadow->flags |= SDL_SHADOW_SURFACE;

        surface = window->shadow;

        /* 8-bit shadow surfaces report that they have exclusive palette */
        if (surface->format->palette) {
            surface->flags |= SDL_HWPALETTE;
            if (format == SDL_GetCurrentDisplayMode()->format) {
                SDL_memcpy(surface->format->palette->colors,
                           window->surface->format->palette->colors,
                           window->surface->format->palette->ncolors *
                           sizeof(SDL_Color));
            } else {
                SDL_DitherColors(surface->format->palette->colors, bpp);
            }
        }
    }

    /* Clear the surface for display */
    {
        Uint32 black = SDL_MapRGB(surface->format, 0, 0, 0);
        SDL_FillRect(surface, NULL, black);
        if (surface->flags & SDL_DOUBLEBUF) {
            SDL_Flip(surface);
            SDL_FillRect(surface, NULL, black);
        }
        SDL_Flip(surface);
    }

    return surface;
}

SDL_Surface *
SDL_GetVideoSurface(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    return SDL_VideoSurface;
}

SDL_Surface *
SDL_DisplayFormat(SDL_Surface * surface)
{
    Uint32 flags;

    if (!SDL_PublicSurface) {
        SDL_SetError("No video mode has been set");
        return (NULL);
    }
    /* Set the flags appropriate for copying to display surface */
    if (((SDL_PublicSurface->flags & SDL_HWSURFACE) == SDL_HWSURFACE)
        && _this->info.blit_hw)
        flags = SDL_HWSURFACE;
    else
        flags = SDL_SWSURFACE;
#ifdef AUTORLE_DISPLAYFORMAT
    flags |= (surface->flags & (SDL_SRCCOLORKEY | SDL_SRCALPHA));
    flags |= SDL_RLEACCELOK;
#else
    flags |=
        surface->flags & (SDL_SRCCOLORKEY | SDL_SRCALPHA | SDL_RLEACCELOK);
#endif
    return (SDL_ConvertSurface(surface, SDL_PublicSurface->format, flags));
}

SDL_Surface *
SDL_DisplayFormatAlpha(SDL_Surface * surface)
{
    SDL_PixelFormat *vf;
    SDL_PixelFormat *format;
    SDL_Surface *converted;
    Uint32 flags;
    /* default to ARGB8888 */
    Uint32 amask = 0xff000000;
    Uint32 rmask = 0x00ff0000;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x000000ff;

    if (!SDL_PublicSurface) {
        SDL_SetError("No video mode has been set");
        return (NULL);
    }
    vf = SDL_PublicSurface->format;

    switch (vf->BytesPerPixel) {
    case 2:
        /* For XGY5[56]5, use, AXGY8888, where {X, Y} = {R, B}.
           For anything else (like ARGB4444) it doesn't matter
           since we have no special code for it anyway */
        if ((vf->Rmask == 0x1f) &&
            (vf->Bmask == 0xf800 || vf->Bmask == 0x7c00)) {
            rmask = 0xff;
            bmask = 0xff0000;
        }
        break;

    case 3:
    case 4:
        /* Keep the video format, as long as the high 8 bits are
           unused or alpha */
        if ((vf->Rmask == 0xff) && (vf->Bmask == 0xff0000)) {
            rmask = 0xff;
            bmask = 0xff0000;
        }
        break;

    default:
        /* We have no other optimised formats right now. When/if a new
           optimised alpha format is written, add the converter here */
        break;
    }
    format = SDL_AllocFormat(32, rmask, gmask, bmask, amask);
    flags = SDL_PublicSurface->flags & SDL_HWSURFACE;
    flags |= surface->flags & (SDL_SRCALPHA | SDL_RLEACCELOK);
    converted = SDL_ConvertSurface(surface, format, flags);
    SDL_FreeFormat(format);
    return (converted);
}

void
SDL_UpdateRect(SDL_Surface * screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
    if (screen) {
        SDL_Rect rect;

        /* Perform some checking */
        if (w == 0)
            w = screen->w;
        if (h == 0)
            h = screen->h;
        if ((int) (x + w) > screen->w)
            return;
        if ((int) (y + h) > screen->h)
            return;

        /* Fill the rectangle */
        rect.x = (Sint16) x;
        rect.y = (Sint16) y;
        rect.w = (Uint16) w;
        rect.h = (Uint16) h;
        SDL_UpdateRects(screen, 1, &rect);
    }
}
void
SDL_UpdateRects(SDL_Surface * screen, int numrects, SDL_Rect * rects)
{
    int i;
    SDL_Window *window;

    /* Find the window corresponding to this surface */
    window = SDL_GetWindowFromSurface(screen);
    if (!window) {
        SDL_SetError("Couldn't find window associated with surface");
        return;
    }

    if (screen->flags & SDL_SHADOW_SURFACE) {
        if (SHOULD_DRAWCURSOR(SDL_cursorstate)) {
            SDL_LockCursor();
            SDL_DrawCursor(screen);
            for (i = 0; i < numrects; ++i) {
                SDL_LowerBlit(screen, &rects[i], window->surface, &rects[i]);
            }
            SDL_EraseCursor(screen);
            SDL_UnlockCursor();
        } else {
            for (i = 0; i < numrects; ++i) {
                SDL_LowerBlit(screen, &rects[i], window->surface, &rects[i]);
            }
        }

        /* Fall through to video surface update */
        screen = window->surface;
    }
    if ((screen->flags & SDL_SCREEN_SURFACE) && _this->UpdateWindowSurface) {
        /* Update the video surface */
        if (screen->offset) {
            int offset_y = screen->offset / screen->pitch;
            int offset_x = screen->offset % screen->pitch;
            for (i = 0; i < numrects; ++i) {
                rects[i].x += offset_x;
                rects[i].y += offset_y;
            }
            _this->UpdateWindowSurface(_this, window, numrects, rects);
            for (i = 0; i < numrects; ++i) {
                rects[i].x -= offset_x;
                rects[i].y -= offset_y;
            }
        } else {
            _this->UpdateWindowSurface(_this, window, numrects, rects);
        }
    }
}

int
SDL_Flip(SDL_Surface * screen)
{
    SDL_Window *window;

    /* Find the window corresponding to this surface */
    window = SDL_GetWindowFromSurface(screen);
    if (!window) {
        SDL_SetError("Couldn't find window associated with surface");
        return;
    }

    /* Copy the shadow surface to the video surface */
    if (screen->flags & SDL_SHADOW_SURFACE) {
        SDL_Rect rect;

        rect.x = 0;
        rect.y = 0;
        rect.w = screen->w;
        rect.h = screen->h;
        if (SHOULD_DRAWCURSOR(SDL_cursorstate)) {
            SDL_LockCursor();
            SDL_DrawCursor(screen);
            SDL_LowerBlit(screen, &rect, window->surface, &rect);
            SDL_EraseCursor(screen);
            SDL_UnlockCursor();
        } else {
            SDL_LowerBlit(screen, &rect, window->surface, &rect);
        }

        /* Fall through to video surface update */
        screen = window->surface;
    }
    if (screen->flags & SDL_DOUBLEBUF) {
        _this->FlipWindowSurface(_this, window);
    } else {
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
    return (0);
}

void
SDL_WM_SetCaption(const char *title, const char *icon)
{
    if (wm_title) {
        SDL_free(wm_title);
    } else {
        wm_title = SDL_strdup(title);
    }
    SDL_SetWindowTitle(window, wm_title);
}

void
SDL_WM_GetCaption(char **title, char **icon)
{
    if (title) {
        *title = wm_title;
    }
    if (icon) {
        *icon = "";
    }
}

void
SDL_WM_SetIcon(SDL_Surface * icon, Uint8 * mask)
{
    /* FIXME */
}

int
SDL_WM_IconifyWindow(void)
{
    SDL_MinimizeWindow(window);
}

int
SDL_WM_ToggleFullScreen(SDL_Surface * surface)
{
    return 0;
}

SDL_GrabMode
SDL_WM_GrabInput(SDL_GrabMode mode)
{
    if (mode != SDL_GRAB_QUERY) {
        SDL_SetWindowGrab(window, mode);
    }
    return (SDL_GrabMode) SDL_GetWindowGrab(window);
}

Uint8
SDL_GetAppState(void)
{
    Uint8 state = 0;
    Uint32 flags = 0;

    flags = SDL_GetWindowFlags(window);
    if ((flags & SDL_WINDOW_SHOWN) && !(flags & SDL_WINDOW_MINIMIZED)) {
        state |= SDL_APPACTIVE;
    }
    if (flags & SDL_WINDOW_KEYBOARD_FOCUS) {
        state |= SDL_APPINPUTFOCUS;
    }
    if (flags & SDL_WINDOW_MOUSE_FOCUS) {
        state |= SDL_APPMOUSEFOCUS;
    }
    return state;
}

const SDL_version *
SDL_Linked_Version(void)
{
    static SDL_version version;
    SDL_VERSION(&version);
    return &version;
}

int
SDL_SetPalette(SDL_Surface * surface, int flags, SDL_Color * colors,
               int firstcolor, int ncolors)
{
    SDL_SetColors(surface, colors, firstcolor, ncolors);
}

int
SDL_SetScreenColors(SDL_Surface * screen, SDL_Color * colors, int firstcolor,
                    int ncolors)
{
    SDL_Window *window = NULL;
    SDL_Palette *pal;
    int gotall;
    int palsize;

    /* Verify the parameters */
    pal = screen->format->palette;
    if (!pal) {
        return 0;               /* not a palettized surface */
    }
    gotall = 1;
    palsize = 1 << screen->format->BitsPerPixel;
    if (ncolors > (palsize - firstcolor)) {
        ncolors = (palsize - firstcolor);
        gotall = 0;
    }

    if (screen->flags & (SDL_SHADOW_SURFACE | SDL_SCREEN_SURFACE)) {
        window = SDL_GetWindowFromSurface(screen);
        if (!window) {
            return 0;
        }
    }

    if (screen->flags & SDL_SHADOW_SURFACE) {
        SDL_Palette *vidpal;

        vidpal = window->surface->format->palette;
        if (vidpal && vidpal->ncolors == pal->ncolors) {
            /* This is a shadow surface, and the physical
             * framebuffer is also indexed. Propagate the
             * changes to its logical palette so that
             * updates are always identity blits
             */
            SDL_memcpy(vidpal->colors + firstcolor, colors,
                       ncolors * sizeof(*colors));
        }
        if (window->surface->flags & SDL_HWPALETTE) {
            /* Set the physical palette */
            screen = window->surface;
        } else {
            SDL_UpdateRect(screen, 0, 0, 0, 0);
        }
    }

    if (screen->flags & SDL_SCREEN_SURFACE) {
        if (_this->SetWindowColors) {
            gotall =
                _this->SetWindowColors(_this, window, firstcolor, ncolors,
                                       colors);
            if (!gotall) {
                /* The video flags shouldn't have SDL_HWPALETTE, and
                   the video driver is responsible for copying back the
                   correct colors into the video surface palette.
                 */
                ;
            }
        }
        SDL_CursorPaletteChanged();
    }

    return gotall;
}

int
SDL_GetWMInfo(SDL_SysWMinfo * info)
{
    return SDL_GetWindowWMInfo(window, info);
}

void
SDL_MoveCursor(int x, int y)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    /* Erase and update the current mouse position */
    if (SHOULD_DRAWCURSOR(SDL_cursorstate)) {
        /* Erase and redraw mouse cursor in new position */
        SDL_LockCursor();
        SDL_EraseCursor(SDL_VideoSurface);
        SDL_cursor->area.x = (x - SDL_cursor->hot_x);
        SDL_cursor->area.y = (y - SDL_cursor->hot_y);
        SDL_DrawCursor(SDL_VideoSurface);
        SDL_UnlockCursor();
    } else if (_this->MoveWMCursor) {
        _this->MoveWMCursor(_this, x, y);
    }
}

/* Keep track of the current cursor colors */
static int palette_changed = 1;
static Uint8 pixels8[2];

void
SDL_CursorPaletteChanged(void)
{
    palette_changed = 1;
}

void
SDL_MouseRect(SDL_Rect * area)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    int clip_diff;

    *area = SDL_cursor->area;
    if (area->x < 0) {
        area->w += area->x;
        area->x = 0;
    }
    if (area->y < 0) {
        area->h += area->y;
        area->y = 0;
    }
    clip_diff = (area->x + area->w) - SDL_VideoSurface->w;
    if (clip_diff > 0) {
        area->w = area->w < clip_diff ? 0 : area->w - clip_diff;
    }
    clip_diff = (area->y + area->h) - SDL_VideoSurface->h;
    if (clip_diff > 0) {
        area->h = area->h < clip_diff ? 0 : area->h - clip_diff;
    }
}

static void
SDL_DrawCursorFast(SDL_Surface * screen, SDL_Rect * area)
{
    const Uint32 pixels[2] = { 0xFFFFFFFF, 0x00000000 };
    int i, w, h;
    Uint8 *data, datab;
    Uint8 *mask, maskb;

    data = SDL_cursor->data + area->y * SDL_cursor->area.w / 8;
    mask = SDL_cursor->mask + area->y * SDL_cursor->area.w / 8;
    switch (screen->format->BytesPerPixel) {

    case 1:
        {
            Uint8 *dst;
            int dstskip;

            if (palette_changed) {
                pixels8[0] =
                    (Uint8) SDL_MapRGB(screen->format, 255, 255, 255);
                pixels8[1] = (Uint8) SDL_MapRGB(screen->format, 0, 0, 0);
                palette_changed = 0;
            }
            dst = (Uint8 *) screen->pixels +
                (SDL_cursor->area.y + area->y) * screen->pitch +
                SDL_cursor->area.x;
            dstskip = screen->pitch - area->w;

            for (h = area->h; h; h--) {
                for (w = area->w / 8; w; w--) {
                    maskb = *mask++;
                    datab = *data++;
                    for (i = 0; i < 8; ++i) {
                        if (maskb & 0x80) {
                            *dst = pixels8[datab >> 7];
                        }
                        maskb <<= 1;
                        datab <<= 1;
                        dst++;
                    }
                }
                dst += dstskip;
            }
        }
        break;

    case 2:
        {
            Uint16 *dst;
            int dstskip;

            dst = (Uint16 *) screen->pixels +
                (SDL_cursor->area.y + area->y) * screen->pitch / 2 +
                SDL_cursor->area.x;
            dstskip = (screen->pitch / 2) - area->w;

            for (h = area->h; h; h--) {
                for (w = area->w / 8; w; w--) {
                    maskb = *mask++;
                    datab = *data++;
                    for (i = 0; i < 8; ++i) {
                        if (maskb & 0x80) {
                            *dst = (Uint16) pixels[datab >> 7];
                        }
                        maskb <<= 1;
                        datab <<= 1;
                        dst++;
                    }
                }
                dst += dstskip;
            }
        }
        break;

    case 3:
        {
            Uint8 *dst;
            int dstskip;

            dst = (Uint8 *) screen->pixels +
                (SDL_cursor->area.y + area->y) * screen->pitch +
                SDL_cursor->area.x * 3;
            dstskip = screen->pitch - area->w * 3;

            for (h = area->h; h; h--) {
                for (w = area->w / 8; w; w--) {
                    maskb = *mask++;
                    datab = *data++;
                    for (i = 0; i < 8; ++i) {
                        if (maskb & 0x80) {
                            SDL_memset(dst, pixels[datab >> 7], 3);
                        }
                        maskb <<= 1;
                        datab <<= 1;
                        dst += 3;
                    }
                }
                dst += dstskip;
            }
        }
        break;

    case 4:
        {
            Uint32 *dst;
            int dstskip;

            dst = (Uint32 *) screen->pixels +
                (SDL_cursor->area.y + area->y) * screen->pitch / 4 +
                SDL_cursor->area.x;
            dstskip = (screen->pitch / 4) - area->w;

            for (h = area->h; h; h--) {
                for (w = area->w / 8; w; w--) {
                    maskb = *mask++;
                    datab = *data++;
                    for (i = 0; i < 8; ++i) {
                        if (maskb & 0x80) {
                            *dst = pixels[datab >> 7];
                        }
                        maskb <<= 1;
                        datab <<= 1;
                        dst++;
                    }
                }
                dst += dstskip;
            }
        }
        break;
    }
}

static void
SDL_DrawCursorSlow(SDL_Surface * screen, SDL_Rect * area)
{
    const Uint32 pixels[2] = { 0xFFFFFF, 0x000000 };
    int h;
    int x, minx, maxx;
    Uint8 *data, datab = 0;
    Uint8 *mask, maskb = 0;
    Uint8 *dst;
    int dstbpp, dstskip;

    data = SDL_cursor->data + area->y * SDL_cursor->area.w / 8;
    mask = SDL_cursor->mask + area->y * SDL_cursor->area.w / 8;
    dstbpp = screen->format->BytesPerPixel;
    dst = (Uint8 *) screen->pixels +
        (SDL_cursor->area.y + area->y) * screen->pitch +
        SDL_cursor->area.x * dstbpp;
    dstskip = screen->pitch - SDL_cursor->area.w * dstbpp;

    minx = area->x;
    maxx = area->x + area->w;
    if (screen->format->BytesPerPixel == 1) {
        if (palette_changed) {
            pixels8[0] = (Uint8) SDL_MapRGB(screen->format, 255, 255, 255);
            pixels8[1] = (Uint8) SDL_MapRGB(screen->format, 0, 0, 0);
            palette_changed = 0;
        }
        for (h = area->h; h; h--) {
            for (x = 0; x < SDL_cursor->area.w; ++x) {
                if ((x % 8) == 0) {
                    maskb = *mask++;
                    datab = *data++;
                }
                if ((x >= minx) && (x < maxx)) {
                    if (maskb & 0x80) {
                        SDL_memset(dst, pixels8[datab >> 7], dstbpp);
                    }
                }
                maskb <<= 1;
                datab <<= 1;
                dst += dstbpp;
            }
            dst += dstskip;
        }
    } else {
        for (h = area->h; h; h--) {
            for (x = 0; x < SDL_cursor->area.w; ++x) {
                if ((x % 8) == 0) {
                    maskb = *mask++;
                    datab = *data++;
                }
                if ((x >= minx) && (x < maxx)) {
                    if (maskb & 0x80) {
                        SDL_memset(dst, pixels[datab >> 7], dstbpp);
                    }
                }
                maskb <<= 1;
                datab <<= 1;
                dst += dstbpp;
            }
            dst += dstskip;
        }
    }
}

/* This handles the ugly work of converting the saved cursor background from
   the pixel format of the shadow surface to that of the video surface.
   This is only necessary when blitting from a shadow surface of a different
   pixel format than the video surface, and using a software rendered cursor.
*/
static void
SDL_ConvertCursorSave(SDL_Surface * screen, int w, int h)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_BlitInfo info;
    SDL_loblit RunBlit;

    /* Make sure we can steal the blit mapping */
    if (screen->map->dst != SDL_VideoSurface) {
        return;
    }

    /* Set up the blit information */
    info.s_pixels = SDL_cursor->save[1];
    info.s_width = w;
    info.s_height = h;
    info.s_skip = 0;
    info.d_pixels = SDL_cursor->save[0];
    info.d_width = w;
    info.d_height = h;
    info.d_skip = 0;
    info.aux_data = screen->map->sw_data->aux_data;
    info.src = screen->format;
    info.table = screen->map->table;
    info.dst = SDL_VideoSurface->format;
    RunBlit = screen->map->sw_data->blit;

    /* Run the actual software blit */
    RunBlit(&info);
}

void
SDL_DrawCursorNoLock(SDL_Surface * screen)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Rect area;

    /* Get the mouse rectangle, clipped to the screen */
    SDL_MouseRect(&area);
    if ((area.w == 0) || (area.h == 0)) {
        return;
    }

    /* Copy mouse background */
    {
        int w, h, screenbpp;
        Uint8 *src, *dst;

        /* Set up the copy pointers */
        screenbpp = screen->format->BytesPerPixel;
        if ((screen == SDL_VideoSurface) ||
            FORMAT_EQUAL(screen->format, SDL_VideoSurface->format)) {
            dst = SDL_cursor->save[0];
        } else {
            dst = SDL_cursor->save[1];
        }
        src = (Uint8 *) screen->pixels + area.y * screen->pitch +
            area.x * screenbpp;

        /* Perform the copy */
        w = area.w * screenbpp;
        h = area.h;
        while (h--) {
            SDL_memcpy(dst, src, w);
            dst += w;
            src += screen->pitch;
        }
    }

    /* Draw the mouse cursor */
    area.x -= SDL_cursor->area.x;
    area.y -= SDL_cursor->area.y;
    if ((area.x == 0) && (area.w == SDL_cursor->area.w)) {
        SDL_DrawCursorFast(screen, &area);
    } else {
        SDL_DrawCursorSlow(screen, &area);
    }
}

void
SDL_DrawCursor(SDL_Surface * screen)
{
    /* Lock the screen if necessary */
    if (screen == NULL) {
        return;
    }
    if (SDL_MUSTLOCK(screen)) {
        if (SDL_LockSurface(screen) < 0) {
            return;
        }
    }

    SDL_DrawCursorNoLock(screen);

    /* Unlock the screen and update if necessary */
    if (SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }
    if ((screen->flags & SDL_SCREEN_SURFACE) &&
        !(screen->flags & SDL_HWSURFACE)) {
        SDL_VideoDevice *_this = SDL_GetVideoDevice();
        SDL_Window *window;
        SDL_Rect area;

        window = SDL_GetWindowFromSurface(screen);
        if (!window) {
            return;
        }

        SDL_MouseRect(&area);

        if (_this->UpdateWindowSurface) {
            _this->UpdateWindowSurface(_this, window, 1, &area);
        }
    }
}

void
SDL_EraseCursorNoLock(SDL_Surface * screen)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Window *window;
    SDL_Rect area;

    /* Get the window associated with the surface */
    window = SDL_GetWindowFromSurface(screen);
    if (!window || !window->surface) {
        return;
    }

    /* Get the mouse rectangle, clipped to the screen */
    SDL_MouseRect(&area);
    if ((area.w == 0) || (area.h == 0)) {
        return;
    }

    /* Copy mouse background */
    {
        int w, h, screenbpp;
        Uint8 *src, *dst;

        /* Set up the copy pointers */
        screenbpp = screen->format->BytesPerPixel;
        if ((screen->flags & SDL_SCREEN_SURFACE) ||
            FORMAT_EQUAL(screen->format, window->surface->format)) {
            src = SDL_cursor->save[0];
        } else {
            src = SDL_cursor->save[1];
        }
        dst = (Uint8 *) screen->pixels + area.y * screen->pitch +
            area.x * screenbpp;

        /* Perform the copy */
        w = area.w * screenbpp;
        h = area.h;
        while (h--) {
            SDL_memcpy(dst, src, w);
            src += w;
            dst += screen->pitch;
        }

        /* Perform pixel conversion on cursor background */
        if (src > SDL_cursor->save[1]) {
            SDL_ConvertCursorSave(screen, area.w, area.h);
        }
    }
}

void
SDL_EraseCursor(SDL_Surface * screen)
{
    /* Lock the screen if necessary */
    if (screen == NULL) {
        return;
    }
    if (SDL_MUSTLOCK(screen)) {
        if (SDL_LockSurface(screen) < 0) {
            return;
        }
    }

    SDL_EraseCursorNoLock(screen);

    /* Unlock the screen and update if necessary */
    if (SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }
    if ((screen->flags & SDL_SCREEN_SURFACE) &&
        !(screen->flags & SDL_HWSURFACE)) {
        SDL_VideoDevice *_this = SDL_GetVideoDevice();
        SDL_Window *window;
        SDL_Rect area;

        window = SDL_GetWindowFromSurface(screen);
        if (!window) {
            return;
        }

        SDL_MouseRect(&area);

        if (_this->UpdateWindowSurface) {
            _this->UpdateWindowSurface(_this, window, 1, &area);
        }
    }
}

/* Reset the cursor on video mode change
   FIXME:  Keep track of all cursors, and reset them all.
 */
void
SDL_ResetCursor(void)
{
    int savelen;

    if (SDL_cursor) {
        savelen = SDL_cursor->area.w * 4 * SDL_cursor->area.h;
        SDL_cursor->area.x = 0;
        SDL_cursor->area.y = 0;
        SDL_memset(SDL_cursor->save[0], 0, savelen);
    }
}

SDL_Overlay *
SDL_CreateYUVOverlay(int w, int h, Uint32 format, SDL_Surface * display)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Window *window;
    const char *yuv_hwaccel;
    SDL_Overlay *overlay;

    window = SDL_GetWindowFromSurface(display);
    if (window && (window->flags & SDL_WINDOW_OPENGL)) {
        SDL_SetError("YUV overlays are not supported in OpenGL mode");
        return NULL;
    }

    /* Display directly on video surface, if possible */
    if (SDL_getenv("SDL_VIDEO_YUV_DIRECT")) {
        if (window &&
            ((window->surface->format->BytesPerPixel == 2) ||
             (window->surface->format->BytesPerPixel == 4))) {
            display = window->surface;
        }
    }
    overlay = NULL;
    yuv_hwaccel = SDL_getenv("SDL_VIDEO_YUV_HWACCEL");
    if (((display->flags & SDL_SCREEN_SURFACE) && _this->CreateYUVOverlay) &&
        (!yuv_hwaccel || (SDL_atoi(yuv_hwaccel) > 0))) {
        overlay = _this->CreateYUVOverlay(_this, w, h, format, display);
    }
    /* If hardware YUV overlay failed ... */
    if (overlay == NULL) {
        overlay = SDL_CreateYUV_SW(_this, w, h, format, display);
    }
    return overlay;
}

int
SDL_LockYUVOverlay(SDL_Overlay * overlay)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    return overlay->hwfuncs->Lock(_this, overlay);
}

void
SDL_UnlockYUVOverlay(SDL_Overlay * overlay)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    overlay->hwfuncs->Unlock(_this, overlay);
}

int
SDL_DisplayYUVOverlay(SDL_Overlay * overlay, SDL_Rect * dstrect)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
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
    return overlay->hwfuncs->Display(_this, overlay, &src, &dst);
}

void
SDL_FreeYUVOverlay(SDL_Overlay * overlay)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (overlay) {
        if (overlay->hwfuncs) {
            overlay->hwfuncs->FreeHW(_this, overlay);
        }
        SDL_free(overlay);
    }
}

/* vi: set ts=4 sw=4 expandtab: */
