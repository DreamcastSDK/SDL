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

/* General cursor handling code for SDL */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "SDL_sysvideo.h"
#include "SDL_cursor_c.h"
#include "default_cursor.h"

/* These are static for our cursor handling code */
/* FIXME: Add support for multiple simultaneous cursors */
volatile int SDL_cursorstate = CURSOR_VISIBLE;
SDL_Cursor *SDL_cursor = NULL;
static SDL_Cursor *SDL_defcursor = NULL;

/* Public functions */
void
SDL_CursorQuit(void)
{
    if (SDL_cursor != NULL) {
        SDL_Cursor *cursor;

        SDL_cursorstate &= ~CURSOR_VISIBLE;
        if (SDL_cursor != SDL_defcursor) {
            SDL_FreeCursor(SDL_cursor);
        }
        SDL_cursor = NULL;
        if (SDL_defcursor != NULL) {
            cursor = SDL_defcursor;
            SDL_defcursor = NULL;
            SDL_FreeCursor(cursor);
        }
    }
}

int
SDL_CursorInit(void)
{
    /* We don't have mouse focus, and the cursor isn't drawn yet */
#ifndef IPOD
    SDL_cursorstate = CURSOR_VISIBLE;
#endif

    /* Create the default cursor */
    if (SDL_defcursor == NULL) {
        SDL_defcursor = SDL_CreateCursor(default_cdata, default_cmask,
                                         DEFAULT_CWIDTH, DEFAULT_CHEIGHT,
                                         DEFAULT_CHOTX, DEFAULT_CHOTY);
        SDL_SetCursor(SDL_defcursor);
    }

    /* That's it! */
    return (0);
}

SDL_Cursor *
SDL_CreateCursor(Uint8 * data, Uint8 * mask,
                 int w, int h, int hot_x, int hot_y)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Cursor *cursor;

    /* Make sure the width is a multiple of 8 */
    w = ((w + 7) & ~7);

    /* Sanity check the hot spot */
    if ((hot_x < 0) || (hot_y < 0) || (hot_x >= w) || (hot_y >= h)) {
        SDL_SetError("Cursor hot spot doesn't lie within cursor");
        return (NULL);
    }

    if (_this->CreateWMCursor) {
        cursor = _this->CreateWMCursor(_this, data, mask, w, h, hot_x, hot_y);
    } else {
        cursor = NULL;
    }
    return (cursor);
}

/* SDL_SetCursor(NULL) can be used to force the cursor redraw,
   if this is desired for any reason.  This is used when setting
   the video mode and when the SDL window gains the mouse focus.
 */
void
SDL_SetCursor(SDL_Cursor * cursor)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    /* Make sure that the video subsystem has been initialized */
    if (!_this) {
        return;
    }

    /* Set the new cursor */
    if (cursor && (cursor != SDL_cursor)) {
        SDL_cursor = cursor;
    }

    /* Draw the new mouse cursor */
    if (SDL_cursor && (SDL_cursorstate & CURSOR_VISIBLE)) {
        if (_this->ShowWMCursor) {
            _this->ShowWMCursor(_this, SDL_cursor);
        }
    } else {
        /* Erase window manager mouse (cursor not visible) */
        if (_this->ShowWMCursor) {
            _this->ShowWMCursor(_this, NULL);
        }
    }
}

SDL_Cursor *
SDL_GetCursor(void)
{
    return (SDL_cursor);
}

void
SDL_FreeCursor(SDL_Cursor * cursor)
{
    if (cursor) {
        if (cursor == SDL_cursor) {
            SDL_SetCursor(SDL_defcursor);
        }
        if (cursor != SDL_defcursor) {
            SDL_VideoDevice *_this = SDL_GetVideoDevice();

            if (_this && _this->FreeWMCursor) {
                _this->FreeWMCursor(_this, cursor);
            }
        }
    }
}

int
SDL_ShowCursor(int toggle)
{
    int showing;

    showing = (SDL_cursorstate & CURSOR_VISIBLE);
    if (toggle >= 0) {
        if (toggle) {
            SDL_cursorstate |= CURSOR_VISIBLE;
        } else {
            SDL_cursorstate &= ~CURSOR_VISIBLE;
        }
        if ((SDL_cursorstate & CURSOR_VISIBLE) != showing) {
            SDL_VideoDevice *_this = SDL_GetVideoDevice();

            SDL_SetCursor(NULL);
            if (_this && _this->CheckMouseMode) {
                _this->CheckMouseMode(_this);
            }
        }
    } else {
        /* Query current state */ ;
    }
    return (showing ? 1 : 0);
}

void
SDL_WarpMouseInWindow(SDL_WindowID windowID, int x, int y)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    /* FIXME: This should specify the target window */
    if (!_this || !SDL_CurrentDisplay.num_windows) {
        SDL_SetError("A window must be created before warping mouse");
        return;
    }

    if (!windowID) {
        windowID = SDL_CurrentWindow;
    }

    /* This generates a mouse motion event */
    if (_this->WarpWMCursor) {
        _this->WarpWMCursor(_this, windowID, x, y);
    }
}

/* vi: set ts=4 sw=4 expandtab: */
