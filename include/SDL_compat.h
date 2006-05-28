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

/* This file contains functions for backwards compatibility with SDL 1.2 */

#ifndef _SDL_compat_h
#define _SDL_compat_h

#include "SDL_syswm.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C"
{
#endif

    extern DECLSPEC char *SDLCALL SDL_AudioDriverName (char *namebuf,
                                                       int maxlen);
    extern DECLSPEC char *SDLCALL SDL_VideoDriverName (char *namebuf,
                                                       int maxlen);
    extern DECLSPEC int SDLCALL SDL_VideoModeOK (int width, int height,
                                                 int bpp, Uint32 flags);
    extern DECLSPEC SDL_Rect **SDLCALL SDL_ListModes (SDL_PixelFormat *
                                                      format, Uint32 flags);
    extern DECLSPEC SDL_Surface *SDLCALL SDL_SetVideoMode (int width,
                                                           int height,
                                                           int bpp,
                                                           Uint32 flags);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif                          /* _SDL_compat_h */

extern DECLSPEC SDL_Surface *SDLCALL SDL_GetVideoSurface (void);

/* These are the currently supported flags for the SDL_surface */
/* Available for SDL_CreateRGBSurface() or SDL_SetVideoMode() */
#define SDL_SWSURFACE	0x00000000      /* Surface is in system memory */
#define SDL_HWSURFACE	0x00000001      /* Surface is in video memory */
#define SDL_ASYNCBLIT	0x00000004      /* Use asynchronous blits if possible */
/* Available for SDL_SetVideoMode() */
#define SDL_ANYFORMAT	0x10000000      /* Allow any video depth/pixel-format */
#define SDL_HWPALETTE	0x20000000      /* Surface has exclusive palette */
#define SDL_DOUBLEBUF	0x40000000      /* Set up double-buffered video mode */
#define SDL_FULLSCREEN	0x80000000      /* Surface is a full screen display */
#define SDL_OPENGL      0x00000002      /* Create an OpenGL rendering context */
#define SDL_INTERNALOPENGL 0x00000008   /* SDL uses OpenGL internally for this window */
#define SDL_RESIZABLE	0x00000010      /* This video mode may be resized */
#define SDL_NOFRAME	0x00000020      /* No window caption or edge frame */
/* Used internally (read-only) */
#define SDL_HWACCEL	0x00000100      /* Blit uses hardware acceleration */
#define SDL_SRCCOLORKEY	0x00001000      /* Blit uses a source color key */
#define SDL_RLEACCELOK	0x00002000      /* Private flag */
#define SDL_RLEACCEL	0x00004000      /* Surface is RLE encoded */
#define SDL_SRCALPHA	0x00010000      /* Blit uses source alpha blending */

extern DECLSPEC void SDLCALL SDL_WM_SetCaption (const char *title,
                                                const char *icon);
extern DECLSPEC void SDLCALL SDL_WM_GetCaption (char **title, char **icon);
extern DECLSPEC void SDLCALL SDL_WM_SetIcon (SDL_Surface * icon,
                                             Uint8 * mask);
extern DECLSPEC int SDLCALL SDL_WM_IconifyWindow (void);
extern DECLSPEC int SDLCALL SDL_WM_ToggleFullScreen (SDL_Surface * surface);
typedef enum
{
    SDL_GRAB_QUERY = -1,
    SDL_GRAB_OFF = 0,
    SDL_GRAB_ON = 1
} SDL_GrabMode;
extern DECLSPEC SDL_GrabMode SDLCALL SDL_WM_GrabInput (SDL_GrabMode mode);

/* The available application states */
#define SDL_APPMOUSEFOCUS	0x01    /* The app has mouse coverage */
#define SDL_APPINPUTFOCUS	0x02    /* The app has input focus */
#define SDL_APPACTIVE		0x04    /* The application is active */

extern DECLSPEC Uint8 SDLCALL SDL_GetAppState (void);
extern DECLSPEC const SDL_version *SDLCALL SDL_Linked_Version (void);
/* flags for SDL_SetPalette() */
#define SDL_LOGPAL 0x01
#define SDL_PHYSPAL 0x02

/*
 * Sets a portion of the colormap for a given 8-bit surface.
 * 'flags' is one or both of:
 * SDL_LOGPAL  -- set logical palette, which controls how blits are mapped
 *                to/from the surface,
 * SDL_PHYSPAL -- set physical palette, which controls how pixels look on
 *                the screen
 * Only screens have physical palettes. Separate change of physical/logical
 * palettes is only possible if the screen has SDL_HWPALETTE set.
 *
 * The return value is 1 if all colours could be set as requested, and 0
 * otherwise.
 *
 * SDL_SetColors() is equivalent to calling this function with
 *     flags = (SDL_LOGPAL|SDL_PHYSPAL).
 */
extern DECLSPEC int SDLCALL SDL_SetPalette (SDL_Surface * surface, int flags,
                                            SDL_Color * colors,
                                            int firstcolor, int ncolors);
extern DECLSPEC int SDLCALL SDL_GetWMInfo (SDL_SysWMinfo * info);
