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

#include "SDL_win32video.h"


static LRESULT CALLBACK
WinMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return CallWindowProc(DefWindowProc, hwnd, msg, wParam, lParam);
}

void
WIN_PumpEvents(_THIS)
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

static int app_registered = 0;
LPTSTR SDL_Appname = NULL;
Uint32 SDL_Appstyle = 0;
HINSTANCE SDL_Instance = NULL;

/* Register the class for this application */
int
SDL_RegisterApp(char *name, Uint32 style, void *hInst)
{
    WNDCLASS class;

    /* Only do this once... */
    if (app_registered) {
        ++app_registered;
        return (0);
    }
    if (!name && !SDL_Appname) {
        name = "SDL_app";
        SDL_Appstyle = (CS_BYTEALIGNCLIENT | CS_OWNDC);
        SDL_Instance = hInst ? hInst : GetModuleHandle(NULL);
    }

    if (name) {
        SDL_Appname = SDL_iconv_utf8_ucs2(name);
        SDL_Appstyle = style;
        SDL_Instance = hInst ? hInst : GetModuleHandle(NULL);
    }

    /* Register the application class */
    class.hCursor = NULL;
    class.hIcon = LoadImage(SDL_Instance, SDL_Appname,
                            IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    class.lpszMenuName = NULL;
    class.lpszClassName = SDL_Appname;
    class.hbrBackground = NULL;
    class.hInstance = SDL_Instance;
    class.style = SDL_Appstyle;
    class.lpfnWndProc = WinMessage;
    class.cbWndExtra = 0;
    class.cbClsExtra = 0;
    if (!RegisterClass(&class)) {
        SDL_SetError("Couldn't register application class");
        return (-1);
    }

    app_registered = 1;
    return (0);
}

/* Unregisters the windowclass registered in SDL_RegisterApp above. */
void
SDL_UnregisterApp()
{
    WNDCLASS class;

    /* SDL_RegisterApp might not have been called before */
    if (!app_registered) {
        return;
    }
    --app_registered;
    if (app_registered == 0) {
        /* Check for any registered window classes. */
        if (GetClassInfo(SDL_Instance, SDL_Appname, &class)) {
            UnregisterClass(SDL_Appname, SDL_Instance);
        }
        SDL_free(SDL_Appname);
        SDL_Appname = NULL;
    }
}

/* vi: set ts=4 sw=4 expandtab: */
