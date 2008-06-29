/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is SDL_free software; you can redistribute it and/or
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

/* This is the system specific header for the SDL touchscreen API */

#include "SDL_touchscreen.h"

/* The SDL touchscreen structure */
struct _SDL_Touchscreen
{
    Uint8 index;                /* Device index */
    const char *name;           /* Touchscreen name - system dependent */

    int maxpoints;              /* Max # multi-touch points, -1 if unlimited. */
    int npoints;                /* Number of points currently touched */
    struct touchpoint
    {
        Uint16 x;
        Uint16 y;
        Uint16 pressure;
    } *points;                   /* Current ball motion deltas */

    struct touchscreen_hwdata *hwdata;     /* Driver dependent information */

    int ref_count;              /* Reference count for multiple opens */
};

/* Function to scan the system for touchscreens.
 * Touchscreen 0 should be the system default touchscreen.
 * This function should return the number of available touchscreens, or -1
 * on an unrecoverable fatal error.
 */
extern int SDL_SYS_TouchscreenInit(void);

/* Function to get the device-dependent name of a touchscreen */
extern const char *SDL_SYS_TouchscreenName(int index);

/* Function to open a touchscreen for use.
   The touchscreen to open is specified by the index field of the touchscreen.
   This should fill the maxpoints field of the touchscreen structure.
   It returns 0, or -1 if there is an error.
 */
extern int SDL_SYS_TouchscreenOpen(SDL_Touchscreen * touchscreen);

/* Function to update the state of a touchscreen - called as a device poll.
 * This function shouldn't update the touchscreen structure directly,
 * but instead should call SDL_PrivateTouchscreen*() to deliver events
 * and update touchscreen device state.
 */
extern void SDL_SYS_TouchscreenUpdate(SDL_Touchscreen * touchscreen);

/* Function to close a touchscreen after use */
extern void SDL_SYS_TouchscreenClose(SDL_Touchscreen * touchscreen);

/* Function to perform any system-specific touchscreen related cleanup */
extern void SDL_SYS_TouchscreenQuit(void);

/* vi: set ts=4 sw=4 expandtab: */
