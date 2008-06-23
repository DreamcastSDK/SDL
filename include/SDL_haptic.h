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

/**
 * \file SDL_haptic.h
 *
 * Include file for SDL haptic subsystem
 */

#ifndef _SDL_haptic_h
#define _SDL_haptic_h

#include "SDL_stdinc.h"
#include "SDL_error.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
extern "C" {
   /* *INDENT-ON* */                                                         
#endif

/* The haptic structure used to identify an SDL haptic */
struct _SDL_Haptic;                                                     
typedef struct _SDL_Haptic SDL_Haptic;


/* Different effects that can be generated */
#define SDL_HAPTIC_CONSTANT   (1<<0)
#define SDL_HAPTIC_PERIODIC   (1<<1)
#define SDL_HAPTIC_RAMP       (1<<2)
#define SDL_HAPTIC_SPRING     (1<<3)
#define SDL_HAPTIC_FRICTION   (1<<4)
#define SDL_HAPTIC_DAMPER     (1<<5)
#define SDL_HAPTIC_RUMBLE     (1<<6)
#define SDL_HAPTIC_INERTIA    (1<<7)
#define SDL_HAPTIC_GAIN       (1<<8)
#define SDL_HAPTIC_AUTOCENTER (1<<9)


/* Function prototypes */
/*
 * Count the number of joysticks attached to the system
 */
extern DECLSPEC int SDLCALL SDL_NumHaptics(void);

/*
 * Get the implementation dependent name of a Haptic device.
 * This can be called before any joysticks are opened.
 * If no name can be found, this function returns NULL.
 */
extern DECLSPEC const char *SDLCALL SDL_HapticName(int device_index);

/*
 * Opens a Haptic device for usage - the index passed as an
 * argument refers to the N'th Haptic device on this system.
 *
 * This function returns a Haptic device identifier, or Null
 * if an error occurred.
 */
extern DECLSPEC SDL_Haptic * SDL_HapticOpen(int device_index);

/* 
 * Closes a Haptic device previously opened with SDL_HapticOpen.
 */
extern DECLSPEC void SDL_HapticClose(SDL_Haptic * haptic);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif
#include "close_code.h"

#endif /* _SDL_haptic_h */

/* vi: set ts=4 sw=4 expandtab: */


