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

#include "SDL_main.h"
#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"

#include "SDL_win32video.h"
#include "SDL_dibrender.h"

/* Initialization/Query functions */
static int WIN_VideoInit(_THIS);
static int WIN_SetDisplayMode(_THIS, const SDL_DisplayMode * mode);
static void WIN_VideoQuit(_THIS);

/* WIN32 driver bootstrap functions */

static int
WIN_Available(void)
{
    return (1);
}

static void
WIN_DeleteDevice(SDL_VideoDevice * device)
{
    SDL_UnregisterApp();
    SDL_free(device->driverdata);
    SDL_free(device);
}

static SDL_VideoDevice *
WIN_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;

    SDL_RegisterApp(NULL, 0, NULL);

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (device) {
        device->driverdata =
            (struct SDL_VideoData *) SDL_calloc(1, sizeof(SDL_VideoData));
    }
    if (!device || !device->driverdata) {
        SDL_OutOfMemory();
        if (device) {
            SDL_free(device);
        }
        return NULL;
    }

    /* Set the function pointers */
    device->VideoInit = WIN_VideoInit;
    device->SetDisplayMode = WIN_SetDisplayMode;
    device->VideoQuit = WIN_VideoQuit;
    device->PumpEvents = WIN_PumpEvents;

#undef CreateWindow
    device->CreateWindow = WIN_CreateWindow;
    device->CreateWindowFrom = WIN_CreateWindowFrom;
    device->SetWindowTitle = WIN_SetWindowTitle;
    device->SetWindowPosition = WIN_SetWindowPosition;
    device->SetWindowSize = WIN_SetWindowSize;
    device->ShowWindow = WIN_ShowWindow;
    device->HideWindow = WIN_HideWindow;
    device->RaiseWindow = WIN_RaiseWindow;
    device->MaximizeWindow = WIN_MaximizeWindow;
    device->MinimizeWindow = WIN_MinimizeWindow;
    device->RestoreWindow = WIN_RestoreWindow;
    device->SetWindowGrab = WIN_SetWindowGrab;
    device->DestroyWindow = WIN_DestroyWindow;
    device->GetWindowWMInfo = WIN_GetWindowWMInfo;

    device->free = WIN_DeleteDevice;

    return device;
}

VideoBootStrap WIN32_bootstrap = {
    "win32", "SDL Win32/64 video driver",
    WIN_Available, WIN_CreateDevice
};


int
WIN_VideoInit(_THIS)
{
    int bmi_size;
    LPBITMAPINFO bmi;
    SDL_DisplayMode mode;

    /* Find out the desktop mode */
    mode.format = SDL_PixelFormat_Unknown;
    mode.w = GetSystemMetrics(SM_CXSCREEN);
    mode.h = GetSystemMetrics(SM_CYSCREEN);
    mode.refresh_rate = 0;

    bmi_size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
    bmi = (LPBITMAPINFO) SDL_malloc(bmi_size);
    if (bmi) {
        HDC hdc;
        HBITMAP hbm;

        SDL_memset(bmi, 0, bmi_size);
        bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        hdc = GetDC(NULL);
        hbm = CreateCompatibleBitmap(hdc, 1, 1);
        GetDIBits(hdc, hbm, 0, 1, NULL, bmi, DIB_RGB_COLORS);
        GetDIBits(hdc, hbm, 0, 1, NULL, bmi, DIB_RGB_COLORS);
        DeleteObject(hbm);
        ReleaseDC(NULL, hdc);
        if (bmi->bmiHeader.biCompression == BI_BITFIELDS) {
            switch (*(Uint32 *) bmi->bmiColors) {
            case 0x00FF0000:
                mode.format = SDL_PixelFormat_RGB888;
                break;
            case 0x000000FF:
                mode.format = SDL_PixelFormat_BGR888;
                break;
            case 0xF800:
                mode.format = SDL_PixelFormat_RGB565;
                break;
            case 0x7C00:
                mode.format = SDL_PixelFormat_RGB555;
                break;
            }
        } else if (bmi->bmiHeader.biBitCount == 8) {
            mode.format = SDL_PixelFormat_Index8;
        }
    }
    SDL_AddBasicVideoDisplay(&mode);
    SDL_AddRenderDriver(0, &SDL_DIB_RenderDriver);

    SDL_zero(mode);
    SDL_AddDisplayMode(0, &mode);

    WIN_AddKeyboard(_this);
    WIN_AddMouse(_this);

    /* We're done! */
    return 0;
}

static int
WIN_SetDisplayMode(_THIS, const SDL_DisplayMode * mode)
{
    SDL_CurrentDisplay.current_mode = *mode;
    return 0;
}

void
WIN_VideoQuit(_THIS)
{
    WIN_DelKeyboard(_this);
    WIN_DelMouse(_this);
}

/* vim: set ts=4 sw=4 expandtab: */
