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

#include "SDL_video.h"
#include "SDL_syswm.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
extern "C" {
/* *INDENT-ON* */
#endif

#define SDL_SWSURFACE       0x00000000
#define SDL_ANYFORMAT       0x00100000
#define SDL_HWPALETTE       0x00200000
#define SDL_DOUBLEBUF       0x00400000
#define SDL_FULLSCREEN      0x00800000
#define SDL_RESIZABLE       0x01000000
#define SDL_NOFRAME         0x02000000
#define SDL_OPENGL          0x04000000
#define SDL_ASYNCBLIT       0x00000000
#define SDL_HWACCEL         0x00000000
#define SDL_SCREEN_SURFACE  0x10000000  /* Surface is a window screen surface */
#define SDL_SHADOW_SURFACE  0x20000000  /* Surface is a window shadow surface */

#define SDL_APPMOUSEFOCUS	0x01
#define SDL_APPINPUTFOCUS	0x02
#define SDL_APPACTIVE		0x04

#define SDL_LOGPAL 0x01
#define SDL_PHYSPAL 0x02

#define SDL_ACTIVEEVENT	SDL_EVENT_RESERVED1
#define SDL_VIDEORESIZE	SDL_EVENT_RESERVED2
#define SDL_VIDEOEXPOSE	SDL_EVENT_RESERVED3

typedef struct SDL_VideoInfo
{
    Uint32 hw_available:1;
    Uint32 wm_available:1;
    Uint32 UnusedBits1:6;
    Uint32 UnusedBits2:1;
    Uint32 blit_hw:1;
    Uint32 blit_hw_CC:1;
    Uint32 blit_hw_A:1;
    Uint32 blit_sw:1;
    Uint32 blit_sw_CC:1;
    Uint32 blit_sw_A:1;
    Uint32 blit_fill:1;
    Uint32 UnusedBits3:16;
    Uint32 video_mem;

    SDL_PixelFormat *vfmt;
} SDL_VideoInfo;

typedef enum
{
    SDL_GRAB_QUERY = -1,
    SDL_GRAB_OFF = 0,
    SDL_GRAB_ON = 1
} SDL_GrabMode;

#define SDL_AllocSurface    SDL_CreateRGBSurface

extern DECLSPEC const SDL_version *SDLCALL SDL_Linked_Version(void);
extern DECLSPEC char *SDLCALL SDL_AudioDriverName(char *namebuf, int maxlen);
extern DECLSPEC char *SDLCALL SDL_VideoDriverName(char *namebuf, int maxlen);
extern DECLSPEC const SDL_VideoInfo *SDLCALL SDL_GetVideoInfo(void);
extern DECLSPEC int SDLCALL SDL_VideoModeOK(int width, int height, int bpp,
                                            Uint32 flags);
extern DECLSPEC SDL_Rect **SDLCALL SDL_ListModes(SDL_PixelFormat * format,
                                                 Uint32 flags);
extern DECLSPEC SDL_Surface *SDLCALL SDL_SetVideoMode(int width, int height,
                                                      int bpp, Uint32 flags);
extern DECLSPEC SDL_Surface *SDLCALL SDL_GetVideoSurface(void);
extern DECLSPEC void SDLCALL SDL_UpdateRects(SDL_Surface * screen,
                                             int numrects, SDL_Rect * rects);
extern DECLSPEC void SDLCALL SDL_UpdateRect(SDL_Surface * screen, Sint32 x,
                                            Sint32 y, Uint32 w, Uint32 h);
extern DECLSPEC int SDLCALL SDL_Flip(SDL_Surface * screen);
extern DECLSPEC SDL_Surface *SDLCALL SDL_DisplayFormat(SDL_Surface * surface);
extern DECLSPEC SDL_Surface *SDLCALL SDL_DisplayFormatAlpha(SDL_Surface *
                                                            surface);
extern DECLSPEC void SDLCALL SDL_WM_SetCaption(const char *title,
                                               const char *icon);
extern DECLSPEC void SDLCALL SDL_WM_GetCaption(char **title, char **icon);
extern DECLSPEC void SDLCALL SDL_WM_SetIcon(SDL_Surface * icon, Uint8 * mask);
extern DECLSPEC int SDLCALL SDL_WM_IconifyWindow(void);
extern DECLSPEC int SDLCALL SDL_WM_ToggleFullScreen(SDL_Surface * surface);
extern DECLSPEC SDL_GrabMode SDLCALL SDL_WM_GrabInput(SDL_GrabMode mode);
extern DECLSPEC Uint8 SDLCALL SDL_GetAppState(void);
extern DECLSPEC int SDLCALL SDL_SetPalette(SDL_Surface * surface, int flags,
                                           SDL_Color * colors,
                                           int firstcolor, int ncolors);
extern DECLSPEC int SDLCALL SDL_SetScreenColors(SDL_Surface * screen,
                                                SDL_Color * colors,
                                                int firstcolor, int ncolors);
extern DECLSPEC int SDLCALL SDL_GetWMInfo(SDL_SysWMinfo * info);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif
#include "close_code.h"

#endif /* _SDL_compat_h */

/* vi: set ts=4 sw=4 expandtab: */
extern DECLSPEC void SDLCALL SDL_WarpMouse(Uint16 x, Uint16 y);
