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
#include "../../events/SDL_events_c.h"


LRESULT CALLBACK
WIN_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SDL_WindowData *data;

    /* Get the window data for the window */
    data = (SDL_WindowData *) GetProp(hwnd, TEXT("SDL_WindowData"));
    if (!data) {
        return CallWindowProc(DefWindowProc, hwnd, msg, wParam, lParam);
    }
#if 0
    switch (msg) {

    case WM_ACTIVATE:
        {
            BOOL minimized;

            minimized = HIWORD(wParam);
            if (!minimized && (LOWORD(wParam) != WA_INACTIVE)) {
                SDL_PrivateWindowEvent(data->windowID, SDL_WINDOWEVENT_SHOWN,
                                       0, 0);
                SDL_PrivateWindowEvent(data->windowID,
                                       SDL_WINDOWEVENT_RESTORED, 0, 0);
                if (IsZoomed(hwnd)) {
                    SDL_PrivateWindowEvent(data->windowID,
                                           SDL_WINDOWEVENT_MAXIMIZED, 0, 0);
                }
                SDL_PrivateWindowEvent(data->windowID,
                                       SDL_WINDOWEVENT_FOCUS_GAINED, 0, 0);
                /* FIXME: Restore mode state (mode, gamma, grab) */
                /* FIXME: Update keyboard state */
            } else {
                SDL_PrivateWindowEvent(data->windowID,
                                       SDL_WINDOWEVENT_FOCUS_LOST, 0, 0);
                if (minimized) {
                    SDL_PrivateWindowEvent(data->windowID,
                                           SDL_WINDOWEVENT_MINIMIZED, 0, 0);
                }
                /* FIXME: Restore desktop state (mode, gamma, grab) */
            }
            return (0);
        }
        break;

    case WM_MOUSEMOVE:
        {
            int index;
            SDL_Mouse *mouse;

            if (!
                (SDL_GetWindowFlags(data->windowID) & SDL_WINDOW_MOUSE_FOCUS))
            {
                /* mouse has entered the window */
                TRACKMOUSEEVENT tme;

                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);

                SDL_PrivateWindowEvent(data->windowID, SDL_WINDOWEVENT_ENTER,
                                       0, 0);
            }

            index = data->videodata->mouse;
            mouse = SDL_GetMouse(index);
            if (mouse) {
                int x, y;
                /* mouse has moved within the window */
                x = LOWORD(lParam);
                y = HIWORD(lParam);
                if (mouse->relative_mode) {
                    int w, h;
                    POINT center;
                    SDL_GetWindowSize(data->windowID, &w, &h);
                    center.x = (w / 2);
                    center.y = (h / 2);
                    x -= center.x;
                    y -= center.y;
                    if (x || y) {
                        ClientToScreen(SDL_Window, &center);
                        SetCursorPos(center.x, center.y);
                        SDL_SendMouseMotion(index, data->windowID, 1, x, y);
                    }
                } else {
                    SDL_SendMouseMotion(index, data->windowID, 0, x, y);
                }
            }
        }
        return (0);

    case WM_MOUSELEAVE:
        {
            SDL_PrivateWindowEvent(data->windowID, SDL_WINDOWEVENT_LEAVE, 0,
                                   0);
        }
        return (0);

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        {
            int x, y;
            Uint8 button, state;

            /* DJM:
               We want the SDL window to take focus so that
               it acts like a normal windows "component"
               (e.g. gains keyboard focus on a mouse click).
             */
            SetFocus(SDL_Window);

            /* Figure out which button to use */
            switch (msg) {
            case WM_LBUTTONDOWN:
                button = SDL_BUTTON_LEFT;
                state = SDL_PRESSED;
                break;
            case WM_LBUTTONUP:
                button = SDL_BUTTON_LEFT;
                state = SDL_RELEASED;
                break;
            case WM_MBUTTONDOWN:
                button = SDL_BUTTON_MIDDLE;
                state = SDL_PRESSED;
                break;
            case WM_MBUTTONUP:
                button = SDL_BUTTON_MIDDLE;
                state = SDL_RELEASED;
                break;
            case WM_RBUTTONDOWN:
                button = SDL_BUTTON_RIGHT;
                state = SDL_PRESSED;
                break;
            case WM_RBUTTONUP:
                button = SDL_BUTTON_RIGHT;
                state = SDL_RELEASED;
                break;
            default:
                /* Eh? Unknown button? */
                return (0);
            }
            if (state == SDL_PRESSED) {
                /* Grab mouse so we get up events */
                if (++mouse_pressed > 0) {
                    SetCapture(hwnd);
                }
            } else {
                /* Release mouse after all up events */
                if (--mouse_pressed <= 0) {
                    ReleaseCapture();
                    mouse_pressed = 0;
                }
            }
            x = LOWORD(lParam);
            y = HIWORD(lParam);
#ifdef _WIN32_WCE
            if (SDL_VideoSurface)
                GapiTransform(this->hidden->userOrientation,
                              this->hidden->hiresFix, &x, &y);
#endif
            posted = SDL_PrivateMouseButton(state, button, x, y);
        }

        return (0);


#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
    case WM_MOUSEWHEEL:
        if (SDL_VideoSurface && !DINPUT_FULLSCREEN()) {
            int move = (short) HIWORD(wParam);
            if (move) {
                Uint8 button;
                if (move > 0)
                    button = SDL_BUTTON_WHEELUP;
                else
                    button = SDL_BUTTON_WHEELDOWN;
                posted = SDL_PrivateMouseButton(SDL_PRESSED, button, 0, 0);
                posted |= SDL_PrivateMouseButton(SDL_RELEASED, button, 0, 0);
            }
        }
        return (0);
#endif

#ifdef WM_GETMINMAXINFO
        /* This message is sent as a way for us to "check" the values
         * of a position change.  If we don't like it, we can adjust
         * the values before they are changed.
         */
    case WM_GETMINMAXINFO:
        {
            MINMAXINFO *info;
            RECT size;
            int x, y;
            int style;
            int width;
            int height;

            /* We don't want to clobber an internal resize */
            if (SDL_resizing)
                return (0);

            /* We allow resizing with the SDL_RESIZABLE flag */
            if (SDL_PublicSurface
                && (SDL_PublicSurface->flags & SDL_RESIZABLE)) {
                return (0);
            }

            /* Get the current position of our window */
            GetWindowRect(SDL_Window, &size);
            x = size.left;
            y = size.top;

            /* Calculate current width and height of our window */
            size.top = 0;
            size.left = 0;
            if (SDL_PublicSurface != NULL) {
                size.bottom = SDL_PublicSurface->h;
                size.right = SDL_PublicSurface->w;
            } else {
                size.bottom = 0;
                size.right = 0;
            }

            /* DJM - according to the docs for GetMenu(), the
               return value is undefined if hwnd is a child window.
               Aparently it's too difficult for MS to check
               inside their function, so I have to do it here.
             */
            style = GetWindowLong(hwnd, GWL_STYLE);
            AdjustWindowRect(&size,
                             style,
                             style & WS_CHILDWINDOW ? FALSE : GetMenu(hwnd) !=
                             NULL);

            width = size.right - size.left;
            height = size.bottom - size.top;

            /* Fix our size to the current size */
            info = (MINMAXINFO *) lParam;
            info->ptMaxSize.x = width;
            info->ptMaxSize.y = height;
            info->ptMaxPosition.x = x;
            info->ptMaxPosition.y = y;
            info->ptMinTrackSize.x = width;
            info->ptMinTrackSize.y = height;
            info->ptMaxTrackSize.x = width;
            info->ptMaxTrackSize.y = height;
        }

        return (0);
#endif /* WM_GETMINMAXINFO */

    case WM_WINDOWPOSCHANGED:
        {
            SDL_VideoDevice *this = current_video;
            int w, h;

            GetClientRect(SDL_Window, &SDL_bounds);
            ClientToScreen(SDL_Window, (LPPOINT) & SDL_bounds);
            ClientToScreen(SDL_Window, (LPPOINT) & SDL_bounds + 1);
            if (!SDL_resizing && !IsZoomed(SDL_Window) &&
                SDL_PublicSurface
                && !(SDL_PublicSurface->flags & SDL_FULLSCREEN)) {
                SDL_windowX = SDL_bounds.left;
                SDL_windowY = SDL_bounds.top;
            }
            w = SDL_bounds.right - SDL_bounds.left;
            h = SDL_bounds.bottom - SDL_bounds.top;
            if (this->input_grab != SDL_GRAB_OFF) {
                ClipCursor(&SDL_bounds);
            }
            if (SDL_PublicSurface
                && (SDL_PublicSurface->flags & SDL_RESIZABLE)) {
                SDL_PrivateResize(w, h);
            }
        }

        break;

        /* We need to set the cursor */
    case WM_SETCURSOR:
        {
            Uint16 hittest;

            hittest = LOWORD(lParam);
            if (hittest == HTCLIENT) {
                SetCursor(SDL_hcursor);
                return (TRUE);
            }
        }

        break;

        /* We are about to get palette focus! */
    case WM_QUERYNEWPALETTE:
        {
            WIN_RealizePalette(current_video);
            return (TRUE);
        }

        break;

        /* Another application changed the palette */
    case WM_PALETTECHANGED:
        {
            WIN_PaletteChanged(current_video, (HWND) wParam);
        }

        break;

        /* We were occluded, refresh our display */
    case WM_PAINT:
        {
            HDC hdc;
            PAINTSTRUCT ps;

            hdc = BeginPaint(SDL_Window, &ps);
            if (current_video->screen &&
                !(current_video->screen->flags & SDL_INTERNALOPENGL)) {
                WIN_WinPAINT(current_video, hdc);
            }
            EndPaint(SDL_Window, &ps);
        }

        return (0);

        /* DJM: Send an expose event in this case */
    case WM_ERASEBKGND:
        {
            posted = SDL_PrivateExpose();
        }

        return (0);

    case WM_CLOSE:
        {
            if ((posted = SDL_PrivateQuit()))
                PostQuitMessage(0);
        }

        return (0);

    case WM_DESTROY:
        {
            PostQuitMessage(0);
        }

        return (0);
    }
#endif
    return CallWindowProc(data->wndproc, hwnd, msg, wParam, lParam);
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
        SDL_Appname = WIN_UTF8ToString(name);
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
    class.lpfnWndProc = DefWindowProc;
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

/* Sets an error message based on GetLastError() */
void
WIN_SetError(const char *prefix)
{
    TCHAR buffer[1024];
    char *message;

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  GetLastError(), 0, buffer, SDL_arraysize(buffer), NULL);

    message = WIN_StringToUTF8(buffer);
    SDL_SetError("%s%s%s", prefix ? prefix : "", prefix ? ":" : "", message);
    SDL_free(message);
}

/* vi: set ts=4 sw=4 expandtab: */
