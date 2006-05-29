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
SDL_AudioDriverName (char *namebuf, int maxlen)
{
    const char *name = SDL_GetCurrentAudioDriver ();
    if (name) {
        SDL_strlcpy (namebuf, name, maxlen);
        return namebuf;
    }
    return NULL;
}

char *
SDL_VideoDriverName (char *namebuf, int maxlen)
{
    const char *name = SDL_GetCurrentVideoDriver ();
    if (name) {
        SDL_strlcpy (namebuf, name, maxlen);
        return namebuf;
    }
    return NULL;
}

int
SDL_VideoModeOK (int width, int height, int bpp, Uint32 flags)
{
    int i, actual_bpp = 0;

    if (!SDL_GetVideoDevice ()) {
        return 0;
    }

    if (!(flags & SDL_FULLSCREEN)) {
        return SDL_BITSPERPIXEL (SDL_GetDesktopDisplayMode ()->format);
    }

    for (i = 0; i < SDL_GetNumDisplayModes (); ++i) {
        const SDL_DisplayMode *mode = SDL_GetDisplayMode (i);
        if (!mode->w || !mode->h || (width == mode->w && height == mode->h)) {
            if (!mode->format) {
                return bpp;
            }
            if (SDL_BITSPERPIXEL (mode->format) >= bpp) {
                actual_bpp = SDL_BITSPERPIXEL (mode->format);
            }
        }
    }
    return actual_bpp;
}

SDL_Rect **
SDL_ListModes (SDL_PixelFormat * format, Uint32 flags)
{
    int i, nmodes;
    SDL_Rect **modes;

    if (!SDL_GetVideoDevice ()) {
        return NULL;
    }

    if (!(flags & SDL_FULLSCREEN)) {
        return (SDL_Rect **) (-1);
    }

    /* Memory leak, but this is a compatibility function, who cares? */
    nmodes = 0;
    for (i = 0; i < SDL_GetNumDisplayModes (); ++i) {
        const SDL_DisplayMode *mode = SDL_GetDisplayMode (i);
        if (!mode->w || !mode->h) {
            return (SDL_Rect **) (-1);
        }
        if (SDL_BITSPERPIXEL (mode->format) != format->BitsPerPixel) {
            continue;
        }
        if (nmodes > 0 && modes[nmodes - 1]->w == mode->w
            && modes[nmodes - 1]->h == mode->h) {
            continue;
        }

        modes = SDL_realloc (modes, (nmodes + 2) * sizeof (*modes));
        if (!modes) {
            return NULL;
        }
        modes[nmodes] = (SDL_Rect *) SDL_malloc (sizeof (SDL_Rect));
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

SDL_Surface *
SDL_SetVideoMode (int width, int height, int bpp, Uint32 flags)
{
    SDL_DisplayMode mode;
    int i;
    Uint32 window_flags;
    Uint32 desktop_format;
    Uint32 desired_format;

    if (!SDL_GetVideoDevice ()) {
        if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
            return NULL;
        }
    }

    /* Destroy existing window */
    SDL_DestroyWindow (window);

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
    window = SDL_CreateWindow (wm_title, 0, 0, width, height, window_flags);
    if (!window) {
        return NULL;
    }

    /* Set up the desired display mode */
    desktop_format = SDL_GetDesktopDisplayMode ()->format;
    if ((bpp == SDL_BITSPERPIXEL(desktop_format)) ||
        (desktop_format && (flags & SDL_ANYFORMAT))) {
        desired_format = desktop_format;
    } else {
        switch (bpp) {
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
            SDL_SetError ("Unsupported bpp in SDL_SetVideoMode()");
            return NULL;
        }
    }
    mode.format = desired_format;
    mode.w = width;
    mode.h = height;
    mode.refresh_rate = 0;

    /* Set the desired display mode */
    if (flags & SDL_FULLSCREEN) {
        if (!SDL_GetClosestDisplayMode (&mode, &mode)) {
            return NULL;
        }
    } else {
        mode = *SDL_GetDesktopDisplayMode ();
    }
    if (SDL_SetDisplayMode (&mode) < 0) {
        return NULL;
    }

    /* Create the display surface */
    return SDL_CreateWindowSurface (window, desired_format, flags);
}

SDL_Surface *
SDL_GetVideoSurface (void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();

    return SDL_VideoSurface;
}

void
SDL_WM_SetCaption (const char *title, const char *icon)
{
    if (wm_title) {
        SDL_free (wm_title);
    } else {
        wm_title = SDL_strdup (title);
    }
    SDL_SetWindowTitle (window, wm_title);
}

void
SDL_WM_GetCaption (char **title, char **icon)
{
    if (title) {
        *title = wm_title;
    }
    if (icon) {
        *icon = "";
    }
}

void
SDL_WM_SetIcon (SDL_Surface * icon, Uint8 * mask)
{
    /* FIXME */
}

int
SDL_WM_IconifyWindow (void)
{
    SDL_MinimizeWindow (window);
}

int
SDL_WM_ToggleFullScreen (SDL_Surface * surface)
{
    return 0;
}

SDL_GrabMode
SDL_WM_GrabInput (SDL_GrabMode mode)
{
    if (mode != SDL_GRAB_QUERY) {
        SDL_SetWindowGrab (window, mode);
    }
    return (SDL_GrabMode) SDL_GetWindowGrab (window);
}

Uint8
SDL_GetAppState (void)
{
    Uint8 state = 0;
    Uint32 flags = 0;

    flags = SDL_GetWindowFlags (window);
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
SDL_Linked_Version (void)
{
    static SDL_version version;
    SDL_VERSION (&version);
    return &version;
}

int
SDL_SetPalette (SDL_Surface * surface, int flags, SDL_Color * colors,
                int firstcolor, int ncolors)
{
    SDL_SetColors (surface, colors, firstcolor, ncolors);
}

int
SDL_GetWMInfo (SDL_SysWMinfo * info)
{
    return SDL_GetWindowWMInfo (window, info);
}

/* vi: set ts=4 sw=4 expandtab: */
