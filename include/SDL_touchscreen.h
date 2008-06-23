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

/**
 * \file SDL_touchscreen.h
 *
 * Include file for SDL touchscreen event handling
 */

#ifndef _SDL_touchscreen_h
#define _SDL_touchscreen_h

#include "SDL_stdinc.h"
#include "SDL_error.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
extern "C" {
/* *INDENT-ON* */
#endif

/* In order to use these functions, SDL_Init() must have been called
   with the SDL_INIT_TOUCHSCREEN flag.  This causes SDL to scan the system
   for touchscreens, and load appropriate drivers.
*/

/* The touchscreen structure used to identify an SDL touchscreen */
struct _SDL_Touchscreen;
typedef struct _SDL_Touchscreen SDL_Touchscreen;


/* Function prototypes */
/*
 * Count the number of touchscreens attached to the system
 */
extern DECLSPEC int SDLCALL SDL_NumTouchscreens(void);

/*
 * Get the implementation dependent name of a touchscreen.
 * This can be called before any touchscreens are opened.
 * If no name can be found, this function returns NULL.
 */
extern DECLSPEC const char *SDLCALL SDL_TouchscreenName(int device_index);

/*
 * Open a touchscreen for use - the index passed as an argument refers to
 * the N'th touchscreen on the system.  This index is the value which will
 * identify this touchscreen in future touchscreen events.
 *
 * This function returns a touchscreen identifier, or NULL if an error occurred.
 */
extern DECLSPEC SDL_Touchscreen *SDLCALL SDL_TouchscreenOpen(int device_index);

/*
 * Returns 1 if the touchscreen has been opened, or 0 if it has not.
 */
extern DECLSPEC int SDLCALL SDL_TouchscreenOpened(int device_index);

/*
 * Get the device index of an opened touchscreen.
 */
extern DECLSPEC int SDLCALL SDL_TouchscreenIndex(SDL_Touchscreen * touchscreen);

/*
 * Get the number of points a touchscreen can register at a time
 * (single or multi-touch.)
 */
extern DECLSPEC int SDLCALL SDL_TouchscreenMaxPoints(SDL_Touchscreen * touchscreen);

/*
 * Update the current state of the open touchscreens.
 * This is called automatically by the event loop if any touchscreen
 * events are enabled.
 */
extern DECLSPEC void SDLCALL SDL_TouchscreenUpdate(void);

/*
 * Enable/disable touchscreen event polling.
 * If touchscreen events are disabled, you must call SDL_TouchscreenUpdate()
 * yourself and check the state of the touchscreen when you want touchscreen
 * information.
 * The state can be one of SDL_QUERY, SDL_ENABLE or SDL_IGNORE.
 */
extern DECLSPEC int SDLCALL SDL_TouchscreenEventState(int state);

/*
 * Get the current X,Y position of the indicated point on the touchscreen.
 *
 * If not NULL, *x is set to an integer in the range [-32768,32767]
 * where -32768 is the left edge, 0 is center, 32767 is right edge.
 * Similarly with *y, for the top, center, and bottom, respectively.
 * The returned value is an unsigned integer in the range [1,32767]
 * that represents the pressure of the touch.
 * If not being touched, 0 is returned and *x and *y are unmodified.
 *
 * The touch point indices start at index 0.
 * If given a negative value for point, all the X,Y, and pressures
 * for all the currently touched points on a multitouch device
 * should be averaged and used for *x, *y, and the return value.
 * If the value of point exceeds the number of points currently touched,
 * 0 should be returned, and *x, *y should be left unmodified.
 *
 * On error, -1 is returned.
 */
extern DECLSPEC int SDLCALL SDL_TouchscreenGetXY(SDL_Touchscreen *touchscreen,
                                                    int point, int *x, int *y);

/*
 * Get the number of currently touched points on a touchscreen.
 * This is either 0 or 1 for a single-touch device.
 * On error, -1 is returned.
 */
extern DECLSPEC int SDLCALL SDL_TouchscreenGetPoints(SDL_Touchscreen * touchscreen);

/*
 * Close a touchscreen previously opened with SDL_TouchscreenOpen()
 */
extern DECLSPEC void SDLCALL SDL_TouchscreenClose(SDL_Touchscreen * touchscreen);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif
#include "close_code.h"

#endif /* _SDL_touchscreen_h */

/* vi: set ts=4 sw=4 expandtab: */
