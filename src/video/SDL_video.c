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

/* The high-level video driver subsystem */

#include "SDL.h"
#include "SDL_sysvideo.h"
#include "SDL_blit.h"
#include "SDL_pixels_c.h"
#include "SDL_cursor_c.h"
#include "../events/SDL_sysevents.h"
#include "../events/SDL_events_c.h"

/* Available video drivers */
static VideoBootStrap *bootstrap[] = {
#if SDL_VIDEO_DRIVER_QUARTZ
    &QZ_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_X11
    &X11_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_DGA
    &DGA_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_NANOX
    &NX_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_IPOD
    &iPod_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_QTOPIA
    &Qtopia_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_WSCONS
    &WSCONS_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_FBCON
    &FBCON_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_DIRECTFB
    &DirectFB_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_PS2GS
    &PS2GS_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_GGI
    &GGI_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_VGL
    &VGL_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_SVGALIB
    &SVGALIB_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_GAPI
    &GAPI_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_WINDIB
    &WINDIB_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_DDRAW
    &DIRECTX_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_BWINDOW
    &BWINDOW_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_TOOLBOX
    &TOOLBOX_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_DRAWSPROCKET
    &DSp_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_CYBERGRAPHICS
    &CGX_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_PHOTON
    &ph_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_EPOC
    &EPOC_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_XBIOS
    &XBIOS_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_GEM
    &GEM_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_PICOGUI
    &PG_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_DC
    &DC_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_RISCOS
    &RISCOS_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_OS2FS
    &OS2FSLib_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_AALIB
    &AALIB_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_DUMMY
    &DUMMY_bootstrap,
#endif
#if SDL_VIDEO_DRIVER_GLSDL
    &glSDL_bootstrap,
#endif
    NULL
};

static SDL_VideoDevice *_this = NULL;

/* Various local functions */
int SDL_VideoInit (const char *driver_name, Uint32 flags);
void SDL_VideoQuit (void);

static int
cmpmodes (const void *A, const void *B)
{
    SDL_DisplayMode a = *(const SDL_DisplayMode *) A;
    SDL_DisplayMode b = *(const SDL_DisplayMode *) B;

    if (a.w != b.w) {
        return b.w - a.w;
    }
    if (a.h != b.h) {
        return b.h - a.h;
    }
    if (SDL_BITSPERPIXEL (a.format) != SDL_BITSPERPIXEL (b.format)) {
        return SDL_BITSPERPIXEL (b.format) - SDL_BITSPERPIXEL (a.format);
    }
    if (a.refresh_rate != b.refresh_rate) {
        return b.refresh_rate - a.refresh_rate;
    }
    return 0;
}

int
SDL_GetNumVideoDrivers (void)
{
    return SDL_arraysize (bootstrap) - 1;
}

const char *
SDL_GetVideoDriver (int index)
{
    if (index >= 0 && index < SDL_GetNumVideoDrivers ()) {
        return bootstrap[index]->name;
    }
    return NULL;
}

/*
 * Initialize the video and event subsystems -- determine native pixel format
 */
int
SDL_VideoInit (const char *driver_name, Uint32 flags)
{
    SDL_VideoDevice *video;
    int index;
    int i;
    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;

    /* Toggle the event thread flags, based on OS requirements */
#if defined(MUST_THREAD_EVENTS)
    flags |= SDL_INIT_EVENTTHREAD;
#elif defined(CANT_THREAD_EVENTS)
    if ((flags & SDL_INIT_EVENTTHREAD) == SDL_INIT_EVENTTHREAD) {
        SDL_SetError ("OS doesn't support threaded events");
        return -1;
    }
#endif

    /* Check to make sure we don't overwrite '_this' */
    if (_this != NULL) {
        SDL_VideoQuit ();
    }

    /* Select the proper video driver */
    index = 0;
    video = NULL;
    if (driver_name != NULL) {
        for (i = 0; bootstrap[i]; ++i) {
            if (SDL_strncmp (bootstrap[i]->name, driver_name,
                             SDL_strlen (bootstrap[i]->name)) == 0) {
                if (bootstrap[i]->available ()) {
                    video = bootstrap[i]->create (index);
                }
                break;
            }
        }
    } else {
        for (i = 0; bootstrap[i]; ++i) {
            if (bootstrap[i]->available ()) {
                video = bootstrap[i]->create (index);
                if (video != NULL) {
                    break;
                }
            }
        }
    }
    if (video == NULL) {
        if (driver_name) {
            SDL_SetError ("%s not available", driver_name);
        } else {
            SDL_SetError ("No available video device");
        }
        return -1;
    }
    _this = video;
    _this->name = bootstrap[i]->name;
    _this->next_window_id = 1;

    /* Set some very sane GL defaults */
    _this->gl_config.driver_loaded = 0;
    _this->gl_config.dll_handle = NULL;
    _this->gl_config.red_size = 3;
    _this->gl_config.green_size = 3;
    _this->gl_config.blue_size = 2;
    _this->gl_config.alpha_size = 0;
    _this->gl_config.buffer_size = 0;
    _this->gl_config.depth_size = 16;
    _this->gl_config.stencil_size = 0;
    _this->gl_config.double_buffer = 1;
    _this->gl_config.accum_red_size = 0;
    _this->gl_config.accum_green_size = 0;
    _this->gl_config.accum_blue_size = 0;
    _this->gl_config.accum_alpha_size = 0;
    _this->gl_config.stereo = 0;
    _this->gl_config.multisamplebuffers = 0;
    _this->gl_config.multisamplesamples = 0;
    _this->gl_config.accelerated = -1;  /* not known, don't set */
    _this->gl_config.swap_control = -1; /* not known, don't set */

    /* Initialize the video subsystem */
    if (_this->VideoInit (_this) < 0) {
        SDL_VideoQuit ();
        return -1;
    }

    /* Make sure some displays were added */
    if (_this->num_displays == 0) {
        SDL_SetError ("The video driver did not add any displays");
        SDL_VideoQuit ();
        return (-1);
    }

    /* Sort the video modes */
    for (i = 0; i < _this->num_displays; ++i) {
        SDL_qsort (_this->displays[i].display_modes,
                   _this->displays[i].num_display_modes,
                   sizeof (SDL_DisplayMode), cmpmodes);
    }

    /* Start the event loop */
    if (SDL_StartEventLoop (flags) < 0) {
        SDL_VideoQuit ();
        return -1;
    }
    SDL_CursorInit (flags & SDL_INIT_EVENTTHREAD);

    /* We're ready to go! */
    return 0;
}

const char *
SDL_GetCurrentVideoDriver ()
{
    if (!_this) {
        return NULL;
    }
    return _this->name;
}

SDL_VideoDevice *
SDL_GetVideoDevice ()
{
    return _this;
}

const SDL_VideoInfo *
SDL_GetVideoInfo (void)
{
    if (!_this) {
        return NULL;
    }
    return &_this->info;
}

void
SDL_AddBasicVideoDisplay (const SDL_DisplayMode * desktop_mode)
{
    SDL_VideoDisplay display;

    SDL_zero (display);
    if (desktop_mode) {
        display.desktop_mode = *desktop_mode;
    }
    display.current_mode = display.desktop_mode;
    display.max_windows = 1;

    SDL_AddVideoDisplay (&display);
}

void
SDL_AddVideoDisplay (SDL_VideoDisplay * display)
{
    SDL_VideoDisplay *displays;

    displays =
        SDL_realloc (_this->displays,
                     (_this->num_displays + 1) * sizeof (*displays));
    if (displays) {
        displays[_this->num_displays] = *display;
        _this->displays = displays;
        _this->num_displays++;
    }
}

int
SDL_GetNumVideoDisplays (void)
{
    if (!_this) {
        return 0;
    }
    return _this->num_displays;
}

int
SDL_SelectVideoDisplay (int index)
{
    if (!_this) {
        SDL_SetError ("Video subsystem has not been initialized");
        return (-1);
    }
    if (index >= 0) {
        if (index >= _this->num_displays) {
            SDL_SetError ("index must be in the range 0 - %d",
                          _this->num_displays - 1);
            return -1;
        }
        _this->current_display = index;
    }
    return _this->current_display;
}

void
SDL_AddDisplayMode (int display, const SDL_DisplayMode * mode)
{
    SDL_DisplayMode *modes;
    int i, nmodes;

    /* Make sure we don't already have the mode in the list */
    modes = SDL_CurrentDisplay.display_modes;
    nmodes = SDL_CurrentDisplay.num_display_modes;
    for (i = 0; i < nmodes; ++i) {
        if (SDL_memcmp (mode, &modes[i], sizeof (*mode)) == 0) {
            return;
        }
    }

    /* Go ahead and add the new mode */
    modes = SDL_realloc (modes, (nmodes + 1) * sizeof (*mode));
    if (modes) {
        SDL_CurrentDisplay.display_modes = modes;
        modes[nmodes] = *mode;
        SDL_CurrentDisplay.num_display_modes++;
    }
}

int
SDL_GetNumDisplayModes ()
{
    if (_this) {
        return SDL_CurrentDisplay.num_display_modes;
    }
    return 0;
}

const SDL_DisplayMode *
SDL_GetDisplayMode (int index)
{
    if (index < 0 || index >= SDL_GetNumDisplayModes ()) {
        SDL_SetError ("index must be in the range of 0 - %d",
                      SDL_GetNumDisplayModes ());
        return NULL;
    }
    return &SDL_CurrentDisplay.display_modes[index];
}

const SDL_DisplayMode *
SDL_GetDesktopDisplayMode (void)
{
    if (_this) {
        return &SDL_CurrentDisplay.desktop_mode;
    }
    return NULL;
}

const SDL_DisplayMode *
SDL_GetCurrentDisplayMode (void)
{
    if (_this) {
        return &SDL_CurrentDisplay.current_mode;
    }
    return NULL;
}

SDL_DisplayMode *
SDL_GetClosestDisplayMode (const SDL_DisplayMode * mode,
                           SDL_DisplayMode * closest)
{
    Uint32 target_format;
    int target_refresh_rate;
    int i;
    SDL_DisplayMode *current, *match;

    if (!_this || !mode || !closest) {
        return NULL;
    }

    /* Default to the desktop format */
    if (mode->format) {
        target_format = mode->format;
    } else {
        target_format = SDL_CurrentDisplay.desktop_mode.format;
    }

    /* Default to the desktop refresh rate */
    if (mode->refresh_rate) {
        target_refresh_rate = mode->refresh_rate;
    } else {
        target_refresh_rate = SDL_CurrentDisplay.desktop_mode.refresh_rate;
    }

    match = NULL;
    for (i = 0; i < SDL_CurrentDisplay.num_display_modes; ++i) {
        current = &SDL_CurrentDisplay.display_modes[i];

        if ((current->w && current->h) &&
            (current->w < mode->w || current->h < mode->h)) {
            /* Out of sorted modes large enough here */
            break;
        }
        if (!match || current->w < match->w || current->h < match->h) {
            match = current;
            continue;
        }
        if (current->format != match->format) {
            /* Sorted highest depth to lowest */
            if (current->format == target_format ||
                (SDL_BITSPERPIXEL (current->format) >=
                 SDL_BITSPERPIXEL (target_format)
                 && SDL_PIXELTYPE (current->format) ==
                 SDL_PIXELTYPE (target_format))) {
                match = current;
            }
            continue;
        }
        if (current->refresh_rate != match->refresh_rate) {
            /* Sorted highest refresh to lowest */
            if (current->refresh_rate >= target_refresh_rate) {
                match = current;
            }
        }
    }
    if (match) {
        if (match->format) {
            closest->format = match->format;
        } else {
            closest->format = mode->format;
        }
        if (match->w && match->h) {
            closest->w = match->w;
            closest->h = match->h;
        } else {
            closest->w = mode->w;
            closest->h = mode->h;
        }
        if (match->refresh_rate) {
            closest->refresh_rate = match->refresh_rate;
        } else {
            closest->refresh_rate = mode->refresh_rate;
        }
        return closest;
    }
    return NULL;
}

int
SDL_SetDisplayMode (const SDL_DisplayMode * mode)
{
    SDL_VideoDisplay *display;
    SDL_DisplayMode display_mode;
    int i;

    if (!_this) {
        SDL_SetError ("Video subsystem has not been initialized");
        return -1;
    }

    /* Make sure there's an actual display mode to set */
    if (!mode) {
        SDL_SetError ("No mode passed to SDL_SetDisplayMode");
        return -1;
    }
    display = &SDL_CurrentDisplay;
    display_mode = *mode;

    /* Default to the current mode */
    if (!display_mode.format) {
        display_mode.format = display->current_mode.format;
    }
    if (!display_mode.w) {
        display_mode.w = display->current_mode.w;
    }
    if (!display_mode.h) {
        display_mode.h = display->current_mode.h;
    }
    if (!display_mode.refresh_rate) {
        display_mode.refresh_rate = display->current_mode.refresh_rate;
    }

    /* Get a good video mode, the closest one possible */
    if (!SDL_GetClosestDisplayMode (&display_mode, &display_mode)) {
        SDL_SetError ("No video mode large enough for %dx%d",
                      display_mode.w, display_mode.h);
        return -1;
    }

    /* See if there's anything left to do */
    if (SDL_memcmp
        (&display_mode, SDL_GetCurrentDisplayMode (),
         sizeof (display_mode)) == 0) {
        return 0;
    }

    /* Free any previous window surfaces */
    for (i = 0; i < display->num_windows; ++i) {
        SDL_Window *window = &display->windows[i];
        if (window->shadow) {
            SDL_FreeSurface (window->shadow);
            window->shadow = NULL;
        }
        if (window->surface) {
            SDL_FreeSurface (window->surface);
            window->surface = NULL;
        }
    }

    return _this->SetDisplayMode (_this, &display_mode);
}

SDL_WindowID
SDL_CreateWindow (const char *title, int x, int y, int w, int h, Uint32 flags)
{
    const Uint32 allowed_flags = (SDL_WINDOW_FULLSCREEN |
                                  SDL_WINDOW_BORDERLESS |
                                  SDL_WINDOW_SHOWN |
                                  SDL_WINDOW_OPENGL |
                                  SDL_WINDOW_RESIZABLE |
                                  SDL_WINDOW_MAXIMIZED |
                                  SDL_WINDOW_MINIMIZED |
                                  SDL_WINDOW_INPUT_GRABBED);
    SDL_Window window;
    int num_windows;
    SDL_Window *windows;

    if (!_this) {
        SDL_SetError ("Video subsystem has not been initialized");
        return 0;
    }

    SDL_zero (window);
    window.id = _this->next_window_id++;
    window.title = title ? SDL_strdup (title) : NULL;
    window.x = x;
    window.y = y;
    window.w = w;
    window.h = h;
    window.flags = (flags & allowed_flags);

    if (_this->CreateWindow && _this->CreateWindow (_this, &window) < 0) {
        if (window.title) {
            SDL_free (window.title);
        }
        return 0;
    }

    num_windows = SDL_CurrentDisplay.num_windows;
    windows =
        SDL_realloc (SDL_CurrentDisplay.windows,
                     (num_windows + 1) * sizeof (*windows));
    if (!windows) {
        if (_this->DestroyWindow) {
            _this->DestroyWindow (_this, &window);
        }
        if (window.title) {
            SDL_free (window.title);
        }
        return 0;
    }
    windows[num_windows] = window;
    SDL_CurrentDisplay.windows = windows;
    SDL_CurrentDisplay.num_windows++;

    return window.id;
}

SDL_WindowID
SDL_CreateWindowFrom (void *data)
{
    SDL_Window window;
    int num_windows;
    SDL_Window *windows;

    if (!_this) {
        SDL_SetError ("Video subsystem has not been initialized");
        return (0);
    }

    SDL_zero (window);
    window.id = _this->next_window_id++;

    if (!_this->CreateWindowFrom ||
        _this->CreateWindowFrom (_this, &window, data) < 0) {
        return 0;
    }

    num_windows = SDL_CurrentDisplay.num_windows;
    windows =
        SDL_realloc (SDL_CurrentDisplay.windows,
                     (num_windows + 1) * sizeof (*windows));
    if (!windows) {
        if (_this->DestroyWindow) {
            _this->DestroyWindow (_this, &window);
        }
        if (window.title) {
            SDL_free (window.title);
        }
        return 0;
    }
    windows[num_windows] = window;
    SDL_CurrentDisplay.windows = windows;
    SDL_CurrentDisplay.num_windows++;

    return window.id;
}

static __inline__ SDL_Window *
SDL_GetWindowFromID (SDL_WindowID windowID)
{
    int i, j;

    if (!_this) {
        return NULL;
    }

    for (i = 0; i < _this->num_displays; ++i) {
        SDL_VideoDisplay *display = &_this->displays[i];
        for (j = 0; j < display->num_windows; ++j) {
            SDL_Window *window = &display->windows[j];
            if (window->id == windowID) {
                return window;
            }
        }
    }
    return NULL;
}

SDL_Window *
SDL_GetWindowFromSurface (SDL_Surface * surface)
{
    int i, j;

    if (!_this || !surface) {
        return NULL;
    }

    for (i = 0; i < _this->num_displays; ++i) {
        SDL_VideoDisplay *display = &_this->displays[i];
        for (j = 0; j < display->num_windows; ++j) {
            SDL_Window *window = &display->windows[j];
            if (surface == window->surface || surface == window->shadow) {
                return window;
            }
        }
    }
    return NULL;
}


Uint32
SDL_GetWindowFlags (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return 0;
    }
    return window->flags;
}

void
SDL_SetWindowTitle (SDL_WindowID windowID, const char *title)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return;
    }
    if (window->title) {
        SDL_free (window->title);
    }
    window->title = SDL_strdup (title);

    if (_this->SetWindowTitle) {
        _this->SetWindowTitle (_this, window);
    }
}

const char *
SDL_GetWindowTitle (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return NULL;
    }
    return window->title;
}

void
SDL_SetWindowData (SDL_WindowID windowID, void *userdata)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return;
    }
    window->userdata = userdata;
}

void *
SDL_GetWindowData (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return NULL;
    }
    return window->userdata;
}

void
SDL_SetWindowPosition (SDL_WindowID windowID, int x, int y)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return;
    }

    window->x = x;
    window->y = y;

    if (_this->SetWindowPosition) {
        _this->SetWindowPosition (_this, window);
    }
}

void
SDL_GetWindowPosition (SDL_WindowID windowID, int *x, int *y)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return;
    }
    if (x) {
        *x = window->x;
    }
    if (y) {
        *y = window->y;
    }
}

void
SDL_SetWindowSize (SDL_WindowID windowID, int w, int h)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return;
    }

    window->w = w;
    window->h = h;

    if (_this->SetWindowSize) {
        _this->SetWindowSize (_this, window);
    }
}

void
SDL_GetWindowSize (SDL_WindowID windowID, int *w, int *h)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return;
    }
    if (w) {
        *w = window->w;
    }
    if (h) {
        *h = window->h;
    }
}

void
SDL_ShowWindow (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window || (window->flags & SDL_WINDOW_SHOWN)) {
        return;
    }

    window->flags |= SDL_WINDOW_SHOWN;

    if (_this->ShowWindow) {
        _this->ShowWindow (_this, window);
    }
}

void
SDL_HideWindow (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window || !(window->flags & SDL_WINDOW_SHOWN)) {
        return;
    }

    window->flags &= ~SDL_WINDOW_SHOWN;

    if (_this->HideWindow) {
        _this->HideWindow (_this, window);
    }
}

void
SDL_RaiseWindow (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return;
    }

    if (_this->RaiseWindow) {
        _this->RaiseWindow (_this, window);
    }
}

void
SDL_MaximizeWindow (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window || (window->flags & SDL_WINDOW_MAXIMIZED)) {
        return;
    }

    window->flags |= SDL_WINDOW_MAXIMIZED;

    if (_this->MaximizeWindow) {
        _this->MaximizeWindow (_this, window);
    }
}

void
SDL_MinimizeWindow (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window || (window->flags & SDL_WINDOW_MINIMIZED)) {
        return;
    }

    window->flags |= SDL_WINDOW_MINIMIZED;

    if (_this->MinimizeWindow) {
        _this->MinimizeWindow (_this, window);
    }
}

void
SDL_RestoreWindow (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window
        || (window->flags & (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED))) {
        return;
    }

    window->flags &= ~(SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED);

    if (_this->RestoreWindow) {
        _this->RestoreWindow (_this, window);
    }
}

void
SDL_SetWindowGrab (SDL_WindowID windowID, int mode)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window || (!!mode == !!(window->flags & SDL_WINDOW_INPUT_GRABBED))) {
        return;
    }

    if (mode) {
        window->flags |= SDL_WINDOW_INPUT_GRABBED;
    } else {
        window->flags &= ~SDL_WINDOW_INPUT_GRABBED;
    }

    if (_this->SetWindowGrab) {
        _this->SetWindowGrab (_this, window);
    }
}

int
SDL_GetWindowGrab (SDL_WindowID windowID)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window) {
        return 0;
    }

    return ((window->flags & SDL_WINDOW_INPUT_GRABBED) != 0);
}

void
SDL_DestroyWindow (SDL_WindowID windowID)
{
    int i, j;

    if (!_this) {
        return;
    }

    for (i = 0; i < _this->num_displays; ++i) {
        SDL_VideoDisplay *display = &_this->displays[i];
        for (j = 0; j < display->num_windows; ++j) {
            SDL_Window *window = &display->windows[j];
            if (window->id != windowID) {
                continue;
            }
            if (window->flags & SDL_WINDOW_INPUT_GRABBED) {
                window->flags &= ~SDL_WINDOW_INPUT_GRABBED;
                _this->SetWindowGrab (_this, window);
            }
            if (window->shadow) {
                SDL_FreeSurface (window->shadow);
            }
            if (window->surface) {
                SDL_FreeSurface (window->surface);
            }
            if (_this->DestroyWindow) {
                _this->DestroyWindow (_this, window);
            }
            if (window->title) {
                SDL_free (window->title);
            }
            if (window->gamma) {
                SDL_free (window->gamma);
            }
            if (j != display->num_windows - 1) {
                SDL_memcpy (&display->windows[i],
                            &display->windows[i + 1],
                            (display->num_windows - i -
                             1) * sizeof (*window));
            }
            --display->num_windows;
            return;
        }
    }
}

SDL_Surface *
SDL_CreateWindowSurface (SDL_WindowID windowID, Uint32 format, Uint32 flags)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);
    Uint32 black;
    SDL_Surface *surface;

    if (!window) {
        return NULL;
    }

    if (!_this->CreateWindowSurface) {
        return NULL;
    }

    if (!window->surface) {
        _this->CreateWindowSurface (_this, window, flags);
        if (!window->surface) {
            return NULL;
        }
        window->surface->flags |= SDL_SCREEN_SURFACE;
    }
    surface = window->surface;

    if (window->shadow) {
        SDL_FreeSurface (window->shadow);
        window->shadow = NULL;
    }

    /* Create a shadow surface if necessary */
    if ((!(flags & SDL_ANYFORMAT)
         && (format != SDL_GetCurrentDisplayMode ()->format))
        || ((flags & SDL_HWPALETTE)
            && !(window->surface->flags & SDL_HWPALETTE))) {
        int bpp;
        Uint32 Rmask, Gmask, Bmask, Amask;

        SDL_PixelFormatEnumToMasks (format, &bpp, &Amask, &Gmask, &Bmask,
                                    &Amask);
        window->shadow =
            SDL_CreateRGBSurface (SDL_SWSURFACE, surface->w, surface->h, bpp,
                                  Rmask, Gmask, Bmask, Amask);
        if (window->shadow == NULL) {
            return NULL;
        }
        window->shadow->flags |= SDL_SHADOW_SURFACE;
        surface = window->shadow;

        /* 8-bit shadow surfaces report that they have exclusive palette */
        if (surface->format->palette) {
            surface->flags |= SDL_HWPALETTE;
            if (format == SDL_GetCurrentDisplayMode ()->format) {
                SDL_memcpy (surface->format->palette->colors,
                            window->surface->format->palette->colors,
                            window->surface->format->palette->ncolors *
                            sizeof (SDL_Color));
            } else {
                SDL_DitherColors (surface->format->palette->colors, bpp);
            }
        }
    }

    /* Clear the surface for display */
    {
        Uint32 black = SDL_MapRGB (surface->format, 0, 0, 0);
        SDL_FillRect (surface, NULL, black);
        if (surface->flags & SDL_DOUBLEBUF) {
            SDL_Flip (surface);
            SDL_FillRect (surface, NULL, black);
        }
        SDL_Flip (surface);
    }

    return surface;
}

/* 
 * Convert a surface into the video pixel format.
 */
SDL_Surface *
SDL_DisplayFormat (SDL_Surface * surface)
{
    Uint32 flags;

    if (!SDL_PublicSurface) {
        SDL_SetError ("No video mode has been set");
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
    return (SDL_ConvertSurface (surface, SDL_PublicSurface->format, flags));
}

/*
 * Convert a surface into a format that's suitable for blitting to
 * the screen, but including an alpha channel.
 */
SDL_Surface *
SDL_DisplayFormatAlpha (SDL_Surface * surface)
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
        SDL_SetError ("No video mode has been set");
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
    format = SDL_AllocFormat (32, rmask, gmask, bmask, amask);
    flags = SDL_PublicSurface->flags & SDL_HWSURFACE;
    flags |= surface->flags & (SDL_SRCALPHA | SDL_RLEACCELOK);
    converted = SDL_ConvertSurface (surface, format, flags);
    SDL_FreeFormat (format);
    return (converted);
}

/*
 * Update a specific portion of the physical screen
 */
void
SDL_UpdateRect (SDL_Surface * screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
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
        SDL_UpdateRects (screen, 1, &rect);
    }
}
void
SDL_UpdateRects (SDL_Surface * screen, int numrects, SDL_Rect * rects)
{
    int i;
    SDL_Window *window;

    /* Find the window corresponding to this surface */
    window = SDL_GetWindowFromSurface (screen);
    if (!window) {
        SDL_SetError ("Couldn't find window associated with surface");
        return;
    }

    if (screen->flags & SDL_SHADOW_SURFACE) {
        if (SHOULD_DRAWCURSOR (SDL_cursorstate)) {
            SDL_LockCursor ();
            SDL_DrawCursor (screen);
            for (i = 0; i < numrects; ++i) {
                SDL_LowerBlit (screen, &rects[i], window->surface, &rects[i]);
            }
            SDL_EraseCursor (screen);
            SDL_UnlockCursor ();
        } else {
            for (i = 0; i < numrects; ++i) {
                SDL_LowerBlit (screen, &rects[i], window->surface, &rects[i]);
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
            _this->UpdateWindowSurface (_this, window, numrects, rects);
            for (i = 0; i < numrects; ++i) {
                rects[i].x -= offset_x;
                rects[i].y -= offset_y;
            }
        } else {
            _this->UpdateWindowSurface (_this, window, numrects, rects);
        }
    }
}

/*
 * Performs hardware double buffering, if possible, or a full update if not.
 */
int
SDL_Flip (SDL_Surface * screen)
{
    SDL_Window *window;

    /* Find the window corresponding to this surface */
    window = SDL_GetWindowFromSurface (screen);
    if (!window) {
        SDL_SetError ("Couldn't find window associated with surface");
        return;
    }

    /* Copy the shadow surface to the video surface */
    if (screen->flags & SDL_SHADOW_SURFACE) {
        SDL_Rect rect;

        rect.x = 0;
        rect.y = 0;
        rect.w = screen->w;
        rect.h = screen->h;
        if (SHOULD_DRAWCURSOR (SDL_cursorstate)) {
            SDL_LockCursor ();
            SDL_DrawCursor (screen);
            SDL_LowerBlit (screen, &rect, window->surface, &rect);
            SDL_EraseCursor (screen);
            SDL_UnlockCursor ();
        } else {
            SDL_LowerBlit (screen, &rect, window->surface, &rect);
        }

        /* Fall through to video surface update */
        screen = window->surface;
    }
    if (screen->flags & SDL_DOUBLEBUF) {
        _this->FlipWindowSurface (_this, window);
    } else {
        SDL_UpdateRect (screen, 0, 0, 0, 0);
    }
    return (0);
}

int
SDL_SetColors (SDL_Surface * screen, SDL_Color * colors, int firstcolor,
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

    if (colors != (pal->colors + firstcolor)) {
        SDL_memcpy (pal->colors + firstcolor, colors,
                    ncolors * sizeof (*colors));
    }
    SDL_FormatChanged (screen);

    if (screen->flags & (SDL_SHADOW_SURFACE | SDL_SCREEN_SURFACE)) {
        window = SDL_GetWindowFromSurface (screen);
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
            SDL_memcpy (vidpal->colors + firstcolor, colors,
                        ncolors * sizeof (*colors));
        }
        if (window->surface->flags & SDL_HWPALETTE) {
            /* Set the physical palette */
            screen = window->surface;
        } else {
            SDL_UpdateRect (screen, 0, 0, 0, 0);
        }
    }

    if (screen->flags & SDL_SCREEN_SURFACE) {
        if (_this->SetWindowColors) {
            gotall =
                _this->SetWindowColors (_this, window, firstcolor, ncolors,
                                        colors);
            if (!gotall) {
                /* The video flags shouldn't have SDL_HWPALETTE, and
                   the video driver is responsible for copying back the
                   correct colors into the video surface palette.
                 */
                ;
            }
        }
        SDL_CursorPaletteChanged ();
    }

    return gotall;
}

void
SDL_VideoQuit (void)
{
    int i, j;

    if (!_this) {
        return;
    }

    /* Halt event processing before doing anything else */
    SDL_StopEventLoop ();

    /* Clean up allocated window manager items */
    SDL_CursorQuit ();

    /* Clean up the system video */
    for (i = _this->num_displays; i--;) {
        SDL_VideoDisplay *display = &_this->displays[i];
        for (j = display->num_windows; j--;) {
            SDL_DestroyWindow (display->windows[i].id);
        }
        if (display->windows) {
            SDL_free (display->windows);
            display->windows = NULL;
        }
    }
    _this->VideoQuit (_this);
    if (_this->displays) {
        SDL_free (_this->displays);
    }
    _this->free (_this);
    _this = NULL;
}

/* Load the GL driver library */
int
SDL_GL_LoadLibrary (const char *path)
{
    int retval;

    retval = -1;
    if (_this == NULL) {
        SDL_SetError ("Video subsystem has not been initialized");
    } else {
        if (_this->GL_LoadLibrary) {
            retval = _this->GL_LoadLibrary (_this, path);
        } else {
            SDL_SetError ("No dynamic GL support in video driver");
        }
    }
    return (retval);
}

void *
SDL_GL_GetProcAddress (const char *proc)
{
    void *func;

    func = NULL;
    if (_this->GL_GetProcAddress) {
        if (_this->gl_config.driver_loaded) {
            func = _this->GL_GetProcAddress (_this, proc);
        } else {
            SDL_SetError ("No GL driver has been loaded");
        }
    } else {
        SDL_SetError ("No dynamic GL support in video driver");
    }
    return func;
}

/* Set the specified GL attribute for setting up a GL video mode */
int
SDL_GL_SetAttribute (SDL_GLattr attr, int value)
{
    int retval;

    retval = 0;
    switch (attr) {
    case SDL_GL_RED_SIZE:
        _this->gl_config.red_size = value;
        break;
    case SDL_GL_GREEN_SIZE:
        _this->gl_config.green_size = value;
        break;
    case SDL_GL_BLUE_SIZE:
        _this->gl_config.blue_size = value;
        break;
    case SDL_GL_ALPHA_SIZE:
        _this->gl_config.alpha_size = value;
        break;
    case SDL_GL_DOUBLEBUFFER:
        _this->gl_config.double_buffer = value;
        break;
    case SDL_GL_BUFFER_SIZE:
        _this->gl_config.buffer_size = value;
        break;
    case SDL_GL_DEPTH_SIZE:
        _this->gl_config.depth_size = value;
        break;
    case SDL_GL_STENCIL_SIZE:
        _this->gl_config.stencil_size = value;
        break;
    case SDL_GL_ACCUM_RED_SIZE:
        _this->gl_config.accum_red_size = value;
        break;
    case SDL_GL_ACCUM_GREEN_SIZE:
        _this->gl_config.accum_green_size = value;
        break;
    case SDL_GL_ACCUM_BLUE_SIZE:
        _this->gl_config.accum_blue_size = value;
        break;
    case SDL_GL_ACCUM_ALPHA_SIZE:
        _this->gl_config.accum_alpha_size = value;
        break;
    case SDL_GL_STEREO:
        _this->gl_config.stereo = value;
        break;
    case SDL_GL_MULTISAMPLEBUFFERS:
        _this->gl_config.multisamplebuffers = value;
        break;
    case SDL_GL_MULTISAMPLESAMPLES:
        _this->gl_config.multisamplesamples = value;
        break;
    case SDL_GL_ACCELERATED_VISUAL:
        _this->gl_config.accelerated = value;
        break;
    case SDL_GL_SWAP_CONTROL:
        _this->gl_config.swap_control = value;
        break;
    default:
        SDL_SetError ("Unknown OpenGL attribute");
        retval = -1;
        break;
    }
    return (retval);
}

/* Retrieve an attribute value from the windowing system. */
int
SDL_GL_GetAttribute (SDL_GLattr attr, int *value)
{
    int retval = -1;

    if (_this->GL_GetAttribute) {
        retval = _this->GL_GetAttribute (_this, attr, value);
    } else {
        *value = 0;
        SDL_SetError ("GL_GetAttribute not supported");
    }
    return retval;
}

/* Perform a GL buffer swap on the current GL context */
void
SDL_GL_SwapBuffers (void)
{
    if (SDL_VideoSurface->flags & SDL_INTERNALOPENGL) {
        _this->GL_SwapBuffers (_this);
    } else {
        SDL_SetError ("OpenGL video mode has not been set");
    }
}

#if 0                           // FIXME
/* Utility function used by SDL_WM_SetIcon();
 * flags & 1 for color key, flags & 2 for alpha channel. */
static void
CreateMaskFromColorKeyOrAlpha (SDL_Surface * icon, Uint8 * mask, int flags)
{
    int x, y;
    Uint32 colorkey;
#define SET_MASKBIT(icon, x, y, mask) \
	mask[(y*((icon->w+7)/8))+(x/8)] &= ~(0x01<<(7-(x%8)))

    colorkey = icon->format->colorkey;
    switch (icon->format->BytesPerPixel) {
    case 1:
        {
            Uint8 *pixels;
            for (y = 0; y < icon->h; ++y) {
                pixels = (Uint8 *) icon->pixels + y * icon->pitch;
                for (x = 0; x < icon->w; ++x) {
                    if (*pixels++ == colorkey) {
                        SET_MASKBIT (icon, x, y, mask);
                    }
                }
            }
        }
        break;

    case 2:
        {
            Uint16 *pixels;
            for (y = 0; y < icon->h; ++y) {
                pixels = (Uint16 *) icon->pixels + y * icon->pitch / 2;
                for (x = 0; x < icon->w; ++x) {
                    if ((flags & 1) && *pixels == colorkey) {
                        SET_MASKBIT (icon, x, y, mask);
                    } else if ((flags & 2)
                               && (*pixels & icon->format->Amask) == 0) {
                        SET_MASKBIT (icon, x, y, mask);
                    }
                    pixels++;
                }
            }
        }
        break;

    case 4:
        {
            Uint32 *pixels;
            for (y = 0; y < icon->h; ++y) {
                pixels = (Uint32 *) icon->pixels + y * icon->pitch / 4;
                for (x = 0; x < icon->w; ++x) {
                    if ((flags & 1) && *pixels == colorkey) {
                        SET_MASKBIT (icon, x, y, mask);
                    } else if ((flags & 2)
                               && (*pixels & icon->format->Amask) == 0) {
                        SET_MASKBIT (icon, x, y, mask);
                    }
                    pixels++;
                }
            }
        }
        break;
    }
}

/*
 * Sets the window manager icon for the display window.
 */
void
SDL_WM_SetIcon (SDL_Surface * icon, Uint8 * mask)
{
    if (icon && _this->SetIcon) {
        /* Generate a mask if necessary, and create the icon! */
        if (mask == NULL) {
            int mask_len = icon->h * (icon->w + 7) / 8;
            int flags = 0;
            mask = (Uint8 *) SDL_malloc (mask_len);
            if (mask == NULL) {
                return;
            }
            SDL_memset (mask, ~0, mask_len);
            if (icon->flags & SDL_SRCCOLORKEY)
                flags |= 1;
            if (icon->flags & SDL_SRCALPHA)
                flags |= 2;
            if (flags) {
                CreateMaskFromColorKeyOrAlpha (icon, mask, flags);
            }
            _this->SetIcon (_this, icon, mask);
            SDL_free (mask);
        } else {
            _this->SetIcon (_this, icon, mask);
        }
    }
}
#endif

SDL_bool
SDL_GetWindowWMInfo (SDL_WindowID windowID, SDL_SysWMinfo * info)
{
    SDL_Window *window = SDL_GetWindowFromID (windowID);

    if (!window || !_this->GetWindowWMInfo) {
        return SDL_FALSE;
    }
    return (_this->GetWindowWMInfo (_this, window, info));
}

/* vi: set ts=4 sw=4 expandtab: */
