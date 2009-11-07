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

/* SDL fbcon video driver implementation based on dummy, ps3 video driver
 * and sourcecode of the fbcon implementation in sdl-1.2
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "DUMMY" by Sam Lantinga.
 */

#include "SDL_video.h"
#include "../SDL_sysvideo.h"

#include "SDL_fbvideo.h"
#include "SDL_fbevents_c.h"
#include "SDL_fbrender_c.h"
#include "SDL_fbmodes_c.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#ifndef HAVE_GETPAGESIZE
	#include <asm/page.h>           /* For definition of PAGE_SIZE */
#endif

#include <linux/vt.h>

#define FBVID_DRIVER_NAME "fbcon"

/* Initialization/Query functions */
static int FB_VideoInit(_THIS);
static int FB_SetDisplayMode(_THIS, SDL_DisplayMode * mode);
static void FB_VideoQuit(_THIS);

/* bootstrap functions */
static int
FB_Available(void)
{
	deprintf(1, "+FB_Available()\n");
	const char *envr = SDL_getenv("SDL_VIDEODRIVER");
	if ((envr) && (SDL_strcmp(envr, FBVID_DRIVER_NAME) == 0)) {
		return (1);
	}

    int console = -1;
    /* Added check for /fb/0 (devfs) */
    /* but - use environment variable first... if it fails, still check defaults */
    int idx = 0;
    const char *SDL_fbdevs[4] = { NULL, "/dev/fb0", "/dev/fb/0", NULL };

    SDL_fbdevs[0] = SDL_getenv("SDL_FBDEV");
    if (!SDL_fbdevs[0]) {
        idx++;
	}
    for (; SDL_fbdevs[idx]; idx++) {
        console = open(SDL_fbdevs[idx], O_RDWR, 0);
        if (console >= 0) {
            close(console);
            break;
        }
    }
	deprintf(1, "-FB_Available()\n");
    return (console >= 0);
}

static void
FB_DeleteDevice(SDL_VideoDevice * device)
{
	deprintf(1, "+FB_DeleteDevice()\n");
	SDL_free(device->driverdata);
	SDL_free(device);
	deprintf(1, "-FB_DeleteDevice()\n");
}

static SDL_VideoDevice *
FB_CreateDevice(int devindex)
{
	deprintf(1, "+FB_CreateDevice()\n");
    SDL_VideoDevice *device;
    SDL_VideoData *data;

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        if (device) {
            SDL_free(device);
        }
        return (0);
    }
    data = (struct SDL_VideoData *) SDL_calloc(1, sizeof(SDL_VideoData));
    if (!data) {
        SDL_OutOfMemory();
        SDL_free(device);
        return (0);
    }
    device->driverdata = data;

    /* Set the function pointers */
    device->VideoInit = FB_VideoInit;
    device->VideoQuit = FB_VideoQuit;
    device->SetDisplayMode = FB_SetDisplayMode;
    device->PumpEvents = FB_PumpEvents;

    device->free = FB_DeleteDevice;

    deprintf(1, "-FB_CreateDevice()\n");
    return device;
}

VideoBootStrap FB_bootstrap = {
	FBVID_DRIVER_NAME, "Linux framebuffer video driver",
	FB_Available, FB_CreateDevice
};

int
FB_VideoInit(_THIS)
{
    deprintf(1, "+FB_VideoInit()\n");
    SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
	SDL_DisplayMode mode;
    struct fb_fix_screeninfo finfo;
    const char *SDL_fbdev;

    /* Initialize the library */
    SDL_fbdev = SDL_getenv("SDL_FBDEV");
    if (SDL_fbdev == NULL) {
        SDL_fbdev = "/dev/fb0";
    }
    data->console_fd = open(SDL_fbdev, O_RDWR, 0);
    if (data->console_fd < 0) {
        SDL_SetError("Unable to open %s", SDL_fbdev);
        return -1;
    }
#if !SDL_THREADS_DISABLED
    /* Create the hardware surface lock mutex */
    data->hw_lock = SDL_CreateMutex();
    if (data->hw_lock == NULL) {
        SDL_SetError("Unable to create lock mutex");
        FB_VideoQuit(_this);
        return -1;
    }
#endif

    /* Get the type of video hardware */
    if (ioctl(data->console_fd, FBIOGET_FSCREENINFO, &finfo) < 0) {
        SDL_SetError("Couldn't get console hardware info");
        FB_VideoQuit(_this);
        return -1;
    }

	/* Use a fake 32-bpp desktop mode */
	mode.format = SDL_PIXELFORMAT_RGB888;
	mode.w = 1024;
	mode.h = 768;
	mode.refresh_rate = 0;
	mode.driverdata = NULL;
	SDL_AddBasicVideoDisplay(&mode);
	SDL_AddRenderDriver(0, &SDL_FB_RenderDriver);

	SDL_zero(mode);
	SDL_AddDisplayMode(0, &mode);

	/* We're done! */
    deprintf(1, "-FB_VideoInit()\n");
	return 0;
}

static int
FB_SetDisplayMode(_THIS, SDL_DisplayMode * mode)
{
    deprintf(1, "+FB_SetDisplayMode()\n");
    deprintf(1, "-FB_SetDisplayMode()\n");
	return 0;
}

void
FB_VideoQuit(_THIS)
{
    deprintf(1, "+FB_VideoQuit()\n");
    deprintf(1, "-FB_VideoQuit()\n");
}

