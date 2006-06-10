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

#ifndef _SDL_keyboard_c_h
#define _SDL_keyboard_c_h

typedef struct SDL_Keyboard SDL_Keyboard;

struct SDL_Keyboard
{
    /* Free the keyboard when it's time */
    void (*FreeKeyboard) (SDL_Keyboard * keyboard);

    SDLMod modstate;
    Uint8 keystate[SDLK_LAST];

    struct
    {
        int firsttime;          /* if we check against the delay or repeat value */
        int delay;              /* the delay before we start repeating */
        int interval;           /* the delay between key repeat events */
        Uint32 timestamp;       /* the time the first keydown event occurred */

        SDL_Event evt;          /* the event we are supposed to repeat */
    } keyrepeat;

    void *driverdata;
};


/* Initialize the keyboard subsystem */
extern int SDL_KeyboardInit(void);

/* Get the keyboard at an index */
extern SDL_Keyboard *SDL_GetKeyboard(int index);

/* Add a keyboard, possibly reattaching at a particular index (or -1),
   returning the index of the keyboard, or -1 if there was an error.
 */
extern int SDL_AddKeyboard(const SDL_Keyboard * keyboard, int index);

/* Remove a keyboard at an index, clearing the slot for later */
extern void SDL_DelKeyboard(int index);

/* Clear the state of a keyboard at an index */
extern void SDL_ResetKeyboard(int index);

/* Send a keyboard event for a keyboard at an index */
extern int SDL_SendKeyboardKey(int index, Uint8 state,
                               const SDL_keysym * keysym);

/* Shutdown the keyboard subsystem */
extern void SDL_KeyboardQuit(void);

#endif /* _SDL_keyboard_c_h */

/* vi: set ts=4 sw=4 expandtab: */
