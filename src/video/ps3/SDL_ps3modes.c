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

#include "SDL_ps3video.h"

void
PS3_InitModes(_THIS)
{
    deprintf(1, "+PS3_InitModes()\n");
    SDL_VideoDisplay display;
    SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
    SDL_DisplayMode mode;
    PS3_DisplayModeData *modedata;
    unsigned long vid = 0;

    modedata = (PS3_DisplayModeData *) SDL_malloc(sizeof(*modedata));
    if (!displaydata) {
        return;
    }

    /* Setting up the DisplayMode */
    struct ps3fb_ioctl_res res;
    if (ioctl(data->fbdev, PS3FB_IOCTL_SCREENINFO, &res)) {
        SDL_SetError("Can't get PS3FB_IOCTL_SCREENINFO");
    }
    mode.format = SDL_PIXELFORMAT_RGB888;
    mode.refresh_rate = 0;
    mode.w = res.xres;
    mode.h = res.yres;

    /* Setting up driver specific mode data */
    if (ioctl(data->fbdev, PS3FB_IOCTL_GETMODE, (unsigned long)&vid)) {
        SDL_SetError("Can't get PS3FB_IOCTL_GETMODE");
    }
    deprintf(2, "PS3FB_IOCTL_GETMODE = %u\n", vid);
    modedata->mode = vid;
    mode.driverdata = modedata;

    SDL_zero(display);
    display.desktop_mode = mode;
    display.current_mode = mode;

    SDL_AddVideoDisplay(&display);
    deprintf(1, "-PS3_InitModes()\n");
}

void
PS3_GetDisplayModes(_THIS) {
    deprintf(1, "+PS3_GetDisplayModes()\n");
    deprintf(1, "-PS3_GetDisplayModes()\n");
}

static int
PS3_SetDisplayMode(_THIS, SDL_DisplayMode * mode)
{
    deprintf(1, "+PS3_SetDisplayMode()\n");
    SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
    SDL_DisplayData *dispdata = (SDL_DisplayData *) mode->driverdata;

    /* We don't care about the current DisplayMode for now */
    if (ioctl(data->fbdev, PS3FB_IOCTL_SETMODE, (unsigned long)&dispdata->mode)) {
        SDL_SetError("Could not set videomode\n");
        return -1;
    }

    deprintf(1, "-PS3_SetDisplayMode()\n");
    return 0;
}

void
PS3_QuitModes(_THIS) {
    deprintf(1, "+PS3_QuitModes()\n");
    deprintf(1, "-PS3_QuitModes()\n");
}

/* vi: set ts=4 sw=4 expandtab: */
