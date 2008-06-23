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

/* Useful functions and variables from SDL_touchscreen.c */
#include "SDL_touchscreen.h"

/* The number of available touchscreens on the system */
extern Uint8 SDL_numtouchscreens;

/* Internal event queueing functions */

/* the point index starts at 0
 * if point < 0, release all points and set this to be the first (only) point.
 * if point >= touchscreen->maxpoints, error.
 * otherwise, set or update the coordinates for the given point.
 * return -1 if fatal error, 0 on success
 */
extern int SDL_PrivateTouchPress(SDL_Touchscreen * touchscreen, int point,
                                 int x, int y, int pressure);
/* if point < 0, release all points. */
extern int SDL_PrivateTouchRelease(SDL_Touchscreen * touchscreen, int point);
/* vi: set ts=4 sw=4 expandtab: */
