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

#if defined(SDL_TOUCHSCREEN_DUMMY) || defined(SDL_TOUCHSCREEN_DISABLED)

/* This is the system specific header for the SDL touchscreen API */

#include "SDL_touchscreen.h"
#include "../SDL_systouchscreen.h"
#include "../SDL_touchscreen_c.h"

/* Function to scan the system for touchscreen.
 * This function should set SDL_numtouchscreens to the number of available
 * touchscreens.  Touchscreen 0 should be the system default touchscreen.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */
int
SDL_SYS_TouchscreenInit(void)
{
    SDL_numtouchscreens = 0;
    return (0);
}

/* Function to get the device-dependent name of a touchscreen */
const char *
SDL_SYS_TouchscreenName(int index)
{
    SDL_SetError("Logic error: No touchscreens available");
    return (NULL);
}

/* Function to open a touchscreen for use.
   The touchscreen to open is specified by the index field of the touchscreen.
   This should fill the maxpoints field of the touchscreen structure.
   It returns 0, or -1 if there is an error.
 */
int
SDL_SYS_TouchscreenOpen(SDL_Touchscreen * touchscreen)
{
    SDL_SetError("Logic error: No touchscreens available");
    return (-1);
}

/* Function to update the state of a touchscreen - called as a device poll.
 * This function shouldn't update the touchscreen structure directly,
 * but instead should call SDL_PrivateTouchscreen*() to deliver events
 * and update touchscreen device state.
 */
void
SDL_SYS_TouchscreenUpdate(SDL_Touchscreen * touchscreen)
{
    return;
}

/* Function to close a touchscreen after use */
void
SDL_SYS_TouchscreenClose(SDL_Touchscreen * touchscreen)
{
    return;
}

/* Function to perform any system-specific touchscreen related cleanup */
void
SDL_SYS_TouchscreenQuit(void)
{
    return;
}

#endif /* SDL_TOUCHSCREEN_DUMMY || SDL_TOUCHSCREEN_DISABLED */
/* vi: set ts=4 sw=4 expandtab: */
