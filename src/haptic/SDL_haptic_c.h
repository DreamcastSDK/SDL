/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 2008 Edgar Simo

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

#include "SDL_haptic.h"

struct _SDL_Haptic;

extern int SDL_HapticInit(void);
extern int SDL_NumHaptics(void);
extern const char * SDL_HapticName(int device_index);
extern struct _SDL_Haptic * SDL_HapticOpen(int device_index);
extern int SDL_HapticOpened(int device_index);
extern int SDL_HapticIndex(SDL_Haptic * haptic);
extern void SDL_HapticClose(SDL_Haptic * haptic);
extern void SDL_HapticQuit(void);
