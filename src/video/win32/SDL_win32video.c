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

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"

#include "SDL_win32video.h"
#include "SDL_win32events.h"
#include "SDL_win32window.h"

/* Initialization/Query functions */
static int WIN32_VideoInit(_THIS);
static int WIN32_SetDisplayMode(_THIS, const SDL_DisplayMode * mode);
static void WIN32_VideoQuit(_THIS);

/* WIN32 driver bootstrap functions */

static int
WIN32_Available(void)
{
    return (1);
}

static void
WIN32_DeleteDevice(SDL_VideoDevice * device)
{
    SDL_free(device->hidden);
    SDL_free(device);
}

static SDL_VideoDevice *
WIN32_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_malloc(sizeof(SDL_VideoDevice));
    if (device) {
        SDL_memset(device, 0, (sizeof *device));
        device->hidden = (struct SDL_PrivateVideoData *)
            SDL_malloc((sizeof *device->hidden));
    }
    if ((device == NULL) || (device->hidden == NULL)) {
        SDL_OutOfMemory();
        if (device) {
            SDL_free(device);
        }
        return (0);
    }
    SDL_memset(device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = WIN32_VideoInit;
    device->SetDisplayMode = WIN32_SetDisplayMode;
    device->VideoQuit = WIN32_VideoQuit;
    device->PumpEvents = WIN32_PumpEvents;

#undef CreateWindow
    device->CreateWindow = WIN32_CreateWindow;
    device->CreateWindowFrom = WIN32_CreateWindowFrom;
    device->SetWindowTitle = WIN32_SetWindowTitle;
    device->SetWindowPosition = WIN32_SetWindowPosition;
    device->SetWindowSize = WIN32_SetWindowSize;
    device->ShowWindow = WIN32_ShowWindow;
    device->HideWindow = WIN32_HideWindow;
    device->RaiseWindow = WIN32_RaiseWindow;
    device->MaximizeWindow = WIN32_MaximizeWindow;
    device->MinimizeWindow = WIN32_MinimizeWindow;
    device->RestoreWindow = WIN32_RestoreWindow;
    device->SetWindowGrab = WIN32_SetWindowGrab;
    device->DestroyWindow = WIN32_DestroyWindow;
    device->GetWindowWMInfo = WIN32_GetWindowWMInfo;

    device->free = WIN32_DeleteDevice;

    return device;
}

VideoBootStrap WIN32_bootstrap = {
    "win32", "SDL Win32/64 video driver",
    WIN32_Available, WIN32_CreateDevice
};


int
WIN32_VideoInit(_THIS)
{
    SDL_DisplayMode mode;

    SDL_AddBasicVideoDisplay(NULL);
    //SDL_AddRenderDriver(0, &SDL_WIN32_RenderDriver);

    SDL_zero(mode);
    SDL_AddDisplayMode(0, &mode);

    /* We're done! */
    return 0;
}

static int
WIN32_SetDisplayMode(_THIS, const SDL_DisplayMode * mode)
{
    SDL_CurrentDisplay.current_mode = *mode;
    return 0;
}

void
WIN32_VideoQuit(_THIS)
{
}

/* vim: set ts=4 sw=4 expandtab: */
