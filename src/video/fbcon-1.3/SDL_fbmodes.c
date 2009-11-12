/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

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

#include "SDL_fbvideo.h"

void
FB_InitModes(_THIS)
{
    deprintf(1, "+FB_InitModes()\n");
    SDL_VideoDisplay display;
    SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
    SDL_DisplayMode mode;
    deprintf(1, "-FB_InitModes()\n");
}

void
FB_GetDisplayModes(_THIS) {
    deprintf(1, "+FB_GetDisplayModes()\n");
    deprintf(1, "-FB_GetDisplayModes()\n");
}

int
FB_SetDisplayMode(_THIS, SDL_DisplayMode * mode)
{
    deprintf(1, "+FB_SetDisplayMode()\n");
    deprintf(1, "-FB_SetDisplayMode()\n");
    return 0;
}

void
FB_QuitModes(_THIS) {
    deprintf(1, "+FB_QuitModes()\n");

    /* There was no mem allocated for driverdata */
    int i, j;
    for (i = _this->num_displays; i--;) {
        SDL_VideoDisplay *display = &_this->displays[i];
        for (j = display->num_display_modes; j--;) {
            display->display_modes[j].driverdata = NULL;
        }
    }

    deprintf(1, "-FB_QuitModes()\n");
}

/* vi: set ts=4 sw=4 expandtab: */
