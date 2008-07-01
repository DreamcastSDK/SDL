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
#define SDL_HAPTIC_INERTIA    (1<<6)
#define SDL_HAPTIC_GAIN       (1<<7)
#define SDL_HAPTIC_AUTOCENTER (1<<8)

typedef enum SDL_waveform {
   SDL_WAVEFORM_SINE,
   SDL_WAVEFORM_SQUARE,
   SDL_WAVEFORM_TRIANGLE,
   SDL_WAVEFORM_SAWTOOTHUP,
   SDL_WAVEFORM_SAWTOOTHDOWN
} SDL_waveform;


/*
 * All values max at 32767 (0x7fff).  Signed values also can be negative.
 * Time values unless specified otherwise are in milliseconds.
 *
 * Common parts:
 * 
 * Replay:
 *    Uint16 length;    Duration of effect.
 *    Uint16 delay;     Delay before starting effect.
 *
 * Trigger:
 *    Uint16 button;    Button that triggers effect.
 *    Uint16 interval;  How soon before effect can be triggered again.
 *
 * Envelope:
 *    Uint16 attack_length;   Duration of the attack.
 *    Uint16 attack_level;    Level at the start of the attack.
 *    Uint16 fade_length;     Duration of the fade out.
 *    Uint16 fade_level;      Level at the end of the fade.
 */
typedef struct SDL_HapticConstant {
   /* Header */
   Uint16 type; /* SDL_HAPTIC_CONSTANT */
   Uint16 direction;

   /* Replay */
   Uint16 length;
   Uint16 delay;

   /* Trigger */
   Uint16 button;
   Uint16 interval;

   /* Constant */
   Sint16 level; /* Strength of the constant effect. */

   /* Envelope */
   Uint16 attack_length;
   Uint16 attack_level;
   Uint16 fade_length;
   Uint16 fade_level;
} SDL_HapticConstant;
typedef struct SDL_HapticPeriodic {
   /* Header */
   Uint16 type; /* SDL_HAPTIC_PERIODIC */
   Uint16 direction;

   /* Replay */
   Uint16 length;
   Uint16 delay;

   /* Trigger */
   Uint16 button;
   Uint16 interval;

   /* Periodic */
   SDL_waveform waveform; /* Type of effect */
   Uint16 period; /* Period of the wave */
   Sint16 magnitude; /* Peak value */
   Sint16 offset; /* Mean value of the wave */
   Uint16 phase; /* Horizontal shift */

   /* Envelope */
   Uint16 attack_length;
   Uint16 attack_level;
   Uint16 fade_length;
   Uint16 fade_level;
} SDL_HapticPeriodic;
typedef struct SDL_HapticCondition {
   /* Header */
   Uint16 type; /* SDL_HAPTIC_{SPRING,DAMPER,INERTIA,FRICTION} */
   Uint16 direction;

   /* Replay */
   Uint16 length;
   Uint16 delay;

   /* Trigger */
   Uint16 button;
   Uint16 interval;

   /* Condition */
   Uint16 right_sat; /* Level when joystick is to the right. */
   Uint16 left_sat; /* Level when joystick is to the left */
   Sint16 right_coeff; /* How fast to increase the force towards the right */
   Sint16 left_coeff; /* How fast to increase the force towards the left */
   Uint16 deadband; /* Size of the dead zone */
   Sint16 center; /* Position of the dead zone */

} SDL_HapticCondition;
typedef struct SDL_HapticRamp {
   /* Header */
   Uint16 type; /* SDL_HAPTIC_RAMP */
   Uint16 direction;

   /* Replay */
   Uint16 length;
   Uint16 delay;

   /* Trigger */
   Uint16 button;
   Uint16 interval;

   /* Ramp */
   Sint16 start;
   Sint16 end;

   /* Envelope */
   Uint16 attack_length;
   Uint16 attack_level;
   Uint16 fade_length;
   Uint16 fade_level;

} SDL_HapticRamp;

typedef union SDL_HapticEffect {
   /* Common for all force feedback effects */
   Uint16 type; /* Effect type */
   SDL_HapticConstant constant; /* Constant effect */
   SDL_HapticPeriodic periodic; /* Periodic effect */
   SDL_HapticCondition condition; /* Condition effect */
   SDL_HapticRamp ramp; /* Ramp effect */
} SDL_HapticEffect;


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

/*
 * Returns the number of effects a haptic device can store.
 */
extern DECLSPEC int SDL_HapticNumEffects(SDL_Haptic * haptic);

/*
 * Returns the supported effects.  Individual effects can be queried by
 * bitwise operators.
 *
 * Example:  (SDL_HapticQueryEffects(haptic) & SDL_HAPTIC_CONSTANT)
 */
extern DECLSPEC unsigned int SDL_HapticQueryEffects(SDL_Haptic * haptic);

/*
 * Creates a new haptic effect on the device.
 *
 * Returns the id of the effect on success, -1 on failure.
 */
extern DECLSPEC int SDL_HapticNewEffect(SDL_Haptic * haptic, SDL_HapticEffect * effect);

/*
 * Runs the haptic effect on it's assosciated haptic device.
 *
 * Returns 0 on success or -1 on failure.
 */
extern DECLSPEC int SDL_HapticRunEffect(SDL_Haptic * haptic, int effect);

/*
 * Stops the haptic effect on it's assosciated haptic device.
 *
 * Returns 0 on success or -1 on failure.
 */
extern DECLSPEC int SDL_HapticStopEffect(SDL_Haptic * haptic, int effect);

/*
 * Destroys a haptic effect on the device.  This will stop the effect if it's
 * running.
 */
extern DECLSPEC void SDL_HapticDestroyEffect(SDL_Haptic * haptic, int effect);

/*
 * Sets the global gain of the device.  Gain should be between 0 and 100.
 *
 * Returns 0 on success or -1 on failure.
 */
extern DECLSPEC int SDL_HapticSetGain(SDL_Haptic * haptic, int gain);

/*
 * Sets the global autocenter of the device.  Autocenter should be between
 * 0 and 100.  Setting it to 0 will disable autocentering.
 *
 * Returns 0 on success or -1 on failure.
 */
extern DECLSPEC int SDL_HapticSetAutocenter(SDL_Haptic * haptic, int autocenter);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif
#include "close_code.h"

#endif /* _SDL_haptic_h */

/* vi: set ts=4 sw=4 expandtab: */


