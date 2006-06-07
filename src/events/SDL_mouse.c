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

/* General mouse handling code for SDL */

#include "SDL_events.h"
#include "SDL_events_c.h"
#include "SDL_mouse_c.h"


static int SDL_num_mice;
static int SDL_current_mouse;
static SDL_Mouse *SDL_mice;


/* Public functions */
int
SDL_MouseInit(void)
{
    return (0);
}

int
SDL_AddMouse(SDL_WindowID focus, int x, int y, Uint8 buttonstate)
{
    SDL_Mouse *new_mice;
    int index;
    SDL_Mouse *mouse;

    new_mice =
        (SDL_Mouse *) SDL_realloc(SDL_mice,
                                  (SDL_num_mice + 1) * sizeof(*new_mice));
    if (!new_mice) {
        SDL_OutOfMemory();
        return -1;
    }

    index = SDL_num_mice++;
    mouse = &SDL_mice[index];
    mouse->focus = focus;
    mouse->x = x;
    mouse->y = y;
    mouse->xdelta = 0;
    mouse->ydelta = 0;
    mouse->buttonstate = buttonstate;

    return index;
}

SDL_Mouse *
SDL_GetMouse(int index)
{
    if (index < 0 || index >= SDL_num_mice) {
        return NULL;
    }
    return &SDL_mice[index];
}

void
SDL_MouseQuit(void)
{
    SDL_num_mice = 0;
    SDL_current_mouse = 0;

    if (SDL_mice) {
        SDL_free(SDL_mice);
        SDL_mice = NULL;
    }
}

int
SDL_GetNumMice(void)
{
    return SDL_num_mice;
}

int
SDL_SelectMouse(int index)
{
    if (index >= 0 && index < SDL_num_mice) {
        SDL_current_mouse = index;
    }
    return SDL_current_mouse;
}

SDL_WindowID
SDL_GetMouseFocusWindow()
{
    SDL_Mouse *mouse = SDL_GetMouse(SDL_current_mouse);

    if (!mouse) {
        return 0;
    }
    return mouse->focus;
}

Uint8
SDL_GetMouseState(int *x, int *y)
{
    SDL_Mouse *mouse = SDL_GetMouse(SDL_current_mouse);

    if (!mouse) {
        if (x) {
            *x = 0;
        }
        if (y) {
            *y = 0;
        }
        return 0;
    }

    if (x) {
        *x = mouse->x;
    }
    if (y) {
        *y = mouse->y;
    }
    return mouse->buttonstate;
}

Uint8
SDL_GetRelativeMouseState(int *x, int *y)
{
    SDL_Mouse *mouse = SDL_GetMouse(SDL_current_mouse);

    if (!mouse) {
        if (x) {
            *x = 0;
        }
        if (y) {
            *y = 0;
        }
        return 0;
    }

    if (x) {
        *x = mouse->xdelta;
    }
    if (y) {
        *y = mouse->ydelta;
    }
    mouse->xdelta = 0;
    mouse->ydelta = 0;
    return mouse->buttonstate;
}

int
SDL_SendMouseMotion(int index, SDL_WindowID windowID, int relative, int x,
                    int y)
{
    SDL_Mouse *mouse = SDL_GetMouse(index);
    int posted;
    int xrel;
    int yrel;

    if (!mouse) {
        return 0;
    }

    if (windowID) {
        mouse->focus = windowID;
    }

    if (relative) {
        /* Push the cursor around */
        xrel = x;
        yrel = y;
        x = (mouse->x + xrel);
        y = (mouse->y + yrel);
    } else {
        xrel = x - mouse->x;
        yrel = y - mouse->y;
    }

    /* Drop events that don't change state */
    if (!xrel && !yrel) {
#if 0
        printf("Mouse event didn't change state - dropped!\n");
#endif
        return 0;
    }

    /* Update internal mouse state */
    mouse->xdelta += xrel;
    mouse->ydelta += yrel;

    /* Post the event, if desired */
    posted = 0;
    if (SDL_ProcessEvents[SDL_MOUSEMOTION] == SDL_ENABLE) {
        SDL_Event event;
        event.motion.type = SDL_MOUSEMOTION;
        event.motion.which = (Uint8) index;
        event.motion.state = mouse->buttonstate;
        event.motion.x = mouse->x;
        event.motion.y = mouse->y;
        event.motion.xrel = xrel;
        event.motion.yrel = yrel;
        event.motion.windowID = mouse->focus;
        if ((SDL_EventOK == NULL) || (*SDL_EventOK) (&event)) {
            posted = 1;
            SDL_PushEvent(&event);
        }
    }
    return posted;
}

int
SDL_PrivateMouseButton(int index, SDL_WindowID windowID, Uint8 state,
                       Uint8 button)
{
    SDL_Mouse *mouse = SDL_GetMouse(index);
    int posted;
    Uint8 type;

    if (!mouse) {
        return 0;
    }

    if (windowID) {
        mouse->focus = windowID;
    }

    /* Figure out which event to perform */
    switch (state) {
    case SDL_PRESSED:
        if (mouse->buttonstate & SDL_BUTTON(button)) {
            /* Ignore this event, no state change */
            return 0;
        }
        type = SDL_MOUSEBUTTONDOWN;
        mouse->buttonstate |= SDL_BUTTON(button);
        break;
    case SDL_RELEASED:
        if (!(mouse->buttonstate & SDL_BUTTON(button))) {
            /* Ignore this event, no state change */
            return 0;
        }
        type = SDL_MOUSEBUTTONUP;
        mouse->buttonstate &= ~SDL_BUTTON(button);
        break;
    default:
        /* Invalid state -- bail */
        return 0;
    }

    /* Post the event, if desired */
    posted = 0;
    if (SDL_ProcessEvents[type] == SDL_ENABLE) {
        SDL_Event event;
        event.type = type;
        event.button.which = (Uint8) index;
        event.button.state = state;
        event.button.button = button;
        event.button.x = mouse->x;
        event.button.y = mouse->y;
        event.button.windowID = windowID;
        if ((SDL_EventOK == NULL) || (*SDL_EventOK) (&event)) {
            posted = 1;
            SDL_PushEvent(&event);
        }
    }
    return posted;
}

/* vi: set ts=4 sw=4 expandtab: */
