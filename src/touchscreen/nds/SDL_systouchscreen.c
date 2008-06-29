/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@devolution.com
*/

#include "SDL_config.h"

#ifdef SDL_TOUCHSCREEN_NDS

/* This is the system specific header for the SDL touchscreen API */
#include <nds.h>
#include <stdio.h>              /* For the definition of NULL */

#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_touchscreen.h"
#include "../SDL_systouchscreen.h"
#include "../SDL_touchscreen_c.h"

#include "../../video/nds/SDL_ndsevents_c.h"

/* Function to scan the system for touchscreen.
 * This function should set SDL_numtouchscreens to the number of available
 * touchscreen.  Touchscreen 0 should be the system default touchscreen.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */
int
SDL_SYS_TouchscreenInit(void)
{
    SDL_numtouchscreens = 1;
    return (1);
}

/* Function to get the device-dependent name of a touchscreen */
const char *
SDL_SYS_TouchscreenName(int index)
{
    if (!index)
        return "NDS builtin touchscreen";
    SDL_SetError("No touchscreen available with that index");
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
    touchscreen->maxpoints = 1;
    touchscreen->npoints = 0;
    /* do I call SDL_TouchscreenOpen here? */
    return 0;
}


/* Function to update the state of a touchscreen - called as a device poll.
 * This function shouldn't update the touchscreen structure directly,
 * but instead should call SDL_PrivateTouch*() to deliver events
 * and update touchscreen device state.
 */
void
SDL_SYS_TouchscreenUpdate(SDL_Touchscreen * touchscreen)
{
    u32 keysd, keysu, keysh;
    Uint16 xpos=0, ypos=0, pressure = 32767;
    touchPosition touch;

    scanKeys();
    keysd = keysDown();
    keysh = keysHeld();
    keysu = keysUp();
    touch=touchReadXY();
    xpos = (touch.px << 8) + 1;
    ypos = (touch.py << 16) / 192 + 1;
    /* TODO uses touch.x and touch.y for something.
       we discussed in the mailing list having both "hardware x/y" and
       "screen x/y" coordinates. */

    if ((keysd & KEY_TOUCH)) {
        SDL_PrivateTouchPress(touchscreen, 0, xpos, ypos, pressure);
    }
    if ((keysh & KEY_TOUCH)) {
    	SDL_PrivateTouchMove(touchscreen, 0, xpos, ypos, pressure); 
    }
    if ((keysu & KEY_TOUCH)) {
        SDL_PrivateTouchRelease(touchscreen, 0);
    }}

/* Function to close a touchscreen after use */
void
SDL_SYS_TouchscreenClose(SDL_Touchscreen * touchscreen)
{
}

/* Function to perform any system-specific touchscreen related cleanup */
void
SDL_SYS_TouchscreenQuit(void)
{
}
#endif /* SDL_TOUCHSCREEN_NDS */

