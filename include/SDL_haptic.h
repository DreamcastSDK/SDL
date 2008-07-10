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
 * \brief The SDL Haptic subsystem allows you to control haptic (force feedback)
 *  devices.
 *
 * The basic usage is as follows:
 *   - Initialize the Subsystem (SDL_INIT_HAPTIC).
 *   - Open a Haptic Device.
 *     - SDL_HapticOpen(...) to open from index.
 *     - SDL_HapticOpenFromJoystick(...) to open from an existing joystick.
 *   - Create an effect (SDL_HapticEffect).
 *   - Upload the effect with SDL_HapticNewEffect(...).
 *   - Run the effect with SDL_HapticRunEffect(...).
 *   - (optional) Free the effect with SDL_HapticDestroyEffect(...).
 *   - Close the haptic device with SDL_HapticClose(...).
 *
 * Example:
 *
 * \code
 * int test_haptic( SDL_Joystick * joystick ) {
 *    SDL_Haptic *haptic;
 *    SDL_HapticEffect effect;
 *    int effect_id;
 *
 *    // Open the device
 *    haptic = SDL_HapticOpenFromJoystick( joystick );
 *    if (haptic == NULL) return -1; // Most likely joystick isn't haptic
 *
 *    // See if it can do sine waves
 *    if ((SDL_HapticQuery(haptic) & SDL_HAPTIC_SINE)==0) {
 *       SDL_HapticClose(haptic); // No sine effect
 *       return -1;
 *    }
 *
 *    // Create the effect
 *    memset( &effect, 0, sizeof(SDL_HapticEffect) ); // 0 is safe default
 *    effect.type = SDL_HAPTIC_SINE;
 *    effect.periodic.direction.type = SDL_HAPTIC_POLAR; // Polar coordinates
 *    effect.periodic.direction.dir[0] = 18000; // Force comes from south
 *    effect.periodic.period = 1000; // 1000 ms
 *    effect.periodic.magnitude = 20000; // 20000/32767 strength
 *    effect.periodic.length = 5000; // 5 seconds long
 *    effect.periodic.attack_length = 1000; // Takes 1 second to get max strength
 *    effect.periodic.fade_length = 1000; // Takes 1 second to fade away
 *
 *    // Upload the effect
 *    effect_id = SDL_HapticNewEffect( haptic, &effect );
 *
 *    // Test the effect
 *    SDL_HapticRunEffect( haptic, effect_id, 1 );
 *    SDL_Delay( 5000); // Wait for the effect to finish
 *
 *    // We destroy the effect, although closing the device also does this
 *    SDL_HapticDestroyEffect( haptic, effect_id );
 *
 *    // Close the device
 *    SDL_HapticClose(haptic);
 *
 *    return 0; // Success
 * }
 * \endcode
 *
 * \author Edgar Simo Serra
 */

#ifndef _SDL_haptic_h
#define _SDL_haptic_h

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_joystick.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
extern "C" {
   /* *INDENT-ON* */                                                         
#endif /* __cplusplus */

/**
 * \typedef SDL_Haptic
 *
 * \brief The haptic structure used to identify an SDL haptic.
 *
 * \sa SDL_HapticOpen
 * \sa SDL_HapticOpenFromJoystick
 * \sa SDL_HapticClose
 */
struct _SDL_Haptic;                                                     
typedef struct _SDL_Haptic SDL_Haptic;


/*
 * Different haptic features a device can have.
 */
/**
 * \def SDL_HAPTIC_CONSTANT
 *
 * \brief Constant haptic effect.
 *
 * \sa SDL_HapticCondition
 */
#define SDL_HAPTIC_CONSTANT   (1<<0) /* Constant effect supported */
/**
 * \def SDL_HAPTIC_SINE
 *
 * \brief Periodic haptic effect that simulates sine waves.
 *
 * \sa SDL_HapticPeriodic
 */
#define SDL_HAPTIC_SINE       (1<<1) /* Sine wave effect supported */
/**
 * \def SDL_HAPTIC_SQUARE
 *
 * \brief Periodic haptic effect that simulates square waves.
 * 
 * \sa SDL_HapticPeriodic
 */
#define SDL_HAPTIC_SQUARE     (1<<2) /* Square wave effect supported */
/**
 * \def SDL_HAPTIC_TRIANGLE
 *
 * \brief Periodic haptic effect that simulates triangular waves.
 *
 * \sa SDL_HapticPeriodic
 */
#define SDL_HAPTIC_TRIANGLE   (1<<3) /* Triangle wave effect supported */
/**
 * \def SDL_HAPTIC_SAWTOOTHUP
 *
 * \brief Periodic haptic effect that simulates saw tooth up waves.
 *
 * \sa SDL_HapticPeriodic
 */
#define SDL_HAPTIC_SAWTOOTHUP (1<<4) /* Sawtoothup wave effect supported */
/**
 * \def SDL_HAPTIC_SAWTOOTHDOWN
 *
 * \brief Periodic haptic effect that simulates saw tooth down waves.
 *
 * \sa SDL_HapticPeriodic
 */
#define SDL_HAPTIC_SAWTOOTHDOWN (1<<5) /* Sawtoothdown wave effect supported */
/**
 * \def SDL_HAPTIC_RAMP
 *
 * \brief Ramp haptic effect.
 *
 * \sa SDL_HapticRamp
 */
#define SDL_HAPTIC_RAMP       (1<<6) /* Ramp effect supported */
/**
 * \def SDL_HAPTIC_SPRING
 *
 * \brief Condition haptic effect that simulates a spring.  Effect is based on the
 * axes position.
 *
 * \sa SDL_HapticCondition
 */
#define SDL_HAPTIC_SPRING     (1<<7) /* Spring effect supported - uses axes position */
/**
 * \def SDL_HAPTIC_DAMPER
 *
 * \brief Condition haptic effect that simulates dampening.  Effect is based on the
 * axes velocity.
 *
 * \sa SDL_HapticCondition
 */
#define SDL_HAPTIC_DAMPER     (1<<8) /* Damper effect supported - uses axes velocity */
/**
 * \def SDL_HAPTIC_INERTIA
 *
 * \brief Condition haptic effect that simulates inertia.  Effect is based on the axes
 * acceleration.
 *
 * \sa SDL_HapticCondition
 */
#define SDL_HAPTIC_INERTIA    (1<<9) /* Inertia effect supported - uses axes acceleration */
/**
 * \def SDL_HAPTIC_FRICTION
 *
 * \brief Condition haptic effect that simulates friction.  Effect is based on the axes
 * movement.
 *
 * \sa SDL_HapticCondition
 */
#define SDL_HAPTIC_FRICTION   (1<<10) /* Friction effect supported - uses axes movement */
/**
 * \def SDL_HAPTIC_CUSTOM
 *
 * \brief User defined custom haptic effect. TODO.
 */
#define SDL_HAPTIC_CUSTOM     (1<<11) /* Custom effect is supported */
/* These last two are features the device has, not effects */
/**
 * \def SDL_HAPTIC_GAIN
 *
 * \brief Device supports setting the global gain.
 *
 * \sa SDL_HapticSetGain
 */
#define SDL_HAPTIC_GAIN       (1<<12) /* Device can set global gain */
/**
 * \def SDL_HAPTIC_AUTOCENTER
 *
 * \brief Device supports setting autocenter.
 *
 * \sa SDL_HapticSetAutocenter
 */
#define SDL_HAPTIC_AUTOCENTER (1<<13) /* Device can set autocenter */
/**
 * \def SDL_HAPTIC_STATUS
 *
 * \brief Device can be queried for effect status.
 *
 * \sa SDL_HapticGetEffectStatus
 */
#define SDL_HAPTIC_STATUS     (1<<14) /* Device can be queried for effect status */


/*
 * Direction encodings
 */
/**
 * \def SDL_HAPTIC_POLAR
 *
 * \brief Uses polar coordinates for the direction.
 *
 * \sa SDL_HapticDirection
 */
#define SDL_HAPTIC_POLAR      0
/**
 * \def SDL_HAPTIC_CARTESIAN
 *
 * \brief Uses cartesian coordinates for the direction.
 *
 * \sa SDL_HapticDirection
 */
#define SDL_HAPTIC_CARTESIAN  1


/*
 * Misc defines.
 */
/**
 * \def SDL_HAPTIC_INFINITY
 *
 * \brief Used to play a device an infinite number of times.
 *
 * \sa SDL_HapticRunEffect
 */
#define SDL_HAPTIC_INFINITY   -1


/**
 * \struct SDL_HapticDirection
 *
 * \brief Structure that represents a haptic direction.
 *
 * Directions can be specified by:
 *   - SDL_HAPTIC_POLAR : Specified by polar coordinates.
 *   - SDL_HAPTIC_CARTESIAN : Specified by cartesian coordinates.
 *
 * Cardinal directions of the haptic device are relative to the positioning
 *  of the device.  North is considered to be away from the user.
 *
 * The following diagram represents the cardinal directions:
 * \code
 *            .--.
 *            |__| .-------.
 *            |=.| |.-----.|
 *            |--| ||     ||
 *            |  | |'-----'|
 *            |__|~')_____('
 *              [ COMPUTER ]
 *
 *
 *                  North (-1)
 *                    ^
 *                    |
 *                    |
 * (1)  West <----[ HAPTIC ]----> East (-1)
 *                    |
 *                    |
 *                    v
 *                  South (1)
 *
 *
 *                 [ USER ]
 *                   \|||/
 *                   (o o)
 *             ---ooO-(_)-Ooo---
 * \endcode
 *
 * If type is SDL_HAPTIC_POLAR, direction is encoded by hundredths of a 
 *  degree starting north and turning clockwise.  The cardinal directions would be:
 *   - North: 0 (0 degrees)
 *   - East: 9000 (90 degrees)
 *   - South: 18000 (180 degrees)
 *   - West: 27000 (270 degrees)
 *
 * If type is SDL_HAPTIC_CARTESIAN, direction is encoded by position.
 *  The cardinal directions would be:
 *   - North:  0,-1
 *   - East:  -1, 0
 *   - South:  0, 1
 *   - West:   1, 0
 *
 *
 * Example:
 * \code
 * SDL_HapticDirection direction;
 *
 * direction.type = SDL_HAPTIC_POLAR; // We'll be using polar direction encoding.
 * direction.dir = 18000; // Force comes from the south meaning the user will
 *                         // have to pull the stick to counteract.
 * \endcode
 *
 * \sa SDL_HAPTIC_POLAR
 * \sa SDL_HAPTIC_CARTESIAN
 * \sa SDL_HapticEffect
 */
typedef struct SDL_HapticDirection {
   Uint8 type; /**< The type of encoding. */
   Uint16 dir[2]; /**< The encoded direction. */
} SDL_HapticDirection;


/**
 * \struct SDL_HapticConstant
 *
 * \brief A structure containing a template for a Constant effect.
 *
 * The struct is exclusive to the SDL_HAPTIC_CONSTANT effect.
 *
 * A constant effect applies a constant force in the specified direction
 *  to the joystick.
 *
 * \sa SDL_HAPTIC_CONSTANT
 * \sa SDL_HapticEffect
 */
typedef struct SDL_HapticConstant {
   /* Header */
   Uint16 type; /**< SDL_HAPTIC_CONSTANT */
   SDL_HapticDirection direction; /**< Direction of the effect. */

   /* Replay */
   Uint16 length; /**< Duration of the effect. */
   Uint16 delay; /**< Delay before starting the effect. */

   /* Trigger */
   Uint16 button; /**< Button that triggers the effect. */
   Uint16 interval; /**< How soon it can be triggered again after button. */

   /* Constant */
   Sint16 level; /**< Strength of the constant effect. */

   /* Envelope */
   Uint16 attack_length; /**< Duration of the attack. */
   Uint16 attack_level; /**< Level at the start of the attack. */
   Uint16 fade_length; /**< Duration of the fade. */
   Uint16 fade_level; /**< Level at the end of the fade. */
} SDL_HapticConstant;
/**
 * \struct SDL_HapticPeriodic
 *
 * \brief A structure containing a template for a Periodic effect.
 *
 * The struct handles the following effects:
 *   - SDL_HAPTIC_SINE
 *   - SDL_HAPTIC_SQUARE
 *   - SDL_HAPTIC_TRIANGLE
 *   - SDL_HAPTIC_SAWTOOTHUP
 *   - SDL_HAPTIC_SAWTOOTHDOWN
 *
 * A periodic effect consists in a wave-shaped effect that repeats itself
 *  over time.  The type determines the shape of the wave and the parameters
 *  determine the dimensions of the wave.
 *
 * Examples:
 * \code
 * SDL_HAPTIC_SINE
 *   __      __      __      __
 *  /  \    /  \    /  \    /
 * /    \__/    \__/    \__/
 *
 * SDL_HAPTIC_SQUARE
 *  __    __    __    __    __
 * |  |  |  |  |  |  |  |  |  |
 * |  |__|  |__|  |__|  |__|  |
 *
 * SDL_HAPTIC_TRIANGLE
 *   /\    /\    /\    /\    /\
 *  /  \  /  \  /  \  /  \  /
 * /    \/    \/    \/    \/
 *
 * SDL_HAPTIC_SAWTOOTHUP
 *   /|  /|  /|  /|  /|  /|  /|
 *  / | / | / | / | / | / | / |
 * /  |/  |/  |/  |/  |/  |/  |
 *
 * SDL_HAPTIC_SAWTOOTHDOWN
 * \  |\  |\  |\  |\  |\  |\  |
 *  \ | \ | \ | \ | \ | \ | \ |
 *   \|  \|  \|  \|  \|  \|  \|
 * \endcode
 *
 * \sa SDL_HAPTIC_SINE
 * \sa SDL_HAPTIC_SQUARE
 * \sa SDL_HAPTIC_TRIANGLE
 * \sa SDL_HAPTIC_SAWTOOTHUP
 * \sa SDL_HAPTIC_SAWTOOTHDOWN
 * \sa SDL_HapticEffect
 */
typedef struct SDL_HapticPeriodic {
   /* Header */
   Uint16 type; /**< SDL_HAPTIC_SINE, SDL_HAPTIC_SQUARE,
                     SDL_HAPTIC_TRIANGLE, SDL_HAPTIC_SAWTOOTHUP or
                     SDL_HAPTIC_SAWTOOTHDOWN */
   SDL_HapticDirection direction; /**< Direction of the effect. */

   /* Replay */
   Uint16 length; /**< Duration of the effect. */
   Uint16 delay; /**< Delay before starting the effect. */

   /* Trigger */
   Uint16 button; /**< Button that triggers the effect. */
   Uint16 interval; /**< How soon it can be triggered again after button. */

   /* Periodic */
   Uint16 period; /**< Period of the wave. */
   Sint16 magnitude; /**< Peak value. */
   Sint16 offset; /**< Mean value of the wave. */
   Uint16 phase; /**< Horizontal shift. */

   /* Envelope */
   Uint16 attack_length; /**< Duration of the attack. */
   Uint16 attack_level; /**< Level at the start of the attack. */
   Uint16 fade_length; /**< Duration of the fade. */
   Uint16 fade_level; /**< Level at the end of the fade. */
} SDL_HapticPeriodic;
/**
 * \struct SDL_HapticCondition
 *
 * \brief A structure containing a template for a Condition effect.
 *
 * The struct handles the following effects:
 *   - SDL_HAPTIC_SPRING: Effect based on axes position.
 *   - SDL_HAPTIC_DAMPER: Effect based on axes velocity.
 *   - SDL_HAPTIC_INERTIA: Effect based on axes acceleration.
 *   - SDL_HAPTIC_FRICTION: Effect based on axes movement.
 *
 * Direction is handled by condition internals instead of a direction member.
 *  The condition effect specific members have two parameters.  The first
 *  refers to the X axis, the second refers to the Y axis.  The right terms
 *  refer to the positive side of the axis and the left terms refer to the
 *  negative side of the axis.  Please refer to the SDL_HapticDirection 
 *  diagram for which side is positive and which is negative.
 *
 * \sa SDL_HapticDirection
 * \sa SDL_HAPTIC_SPRING
 * \sa SDL_HAPTIC_DAMPER
 * \sa SDL_HAPTIC_INERTIA
 * \sa SDL_HAPTIC_FRICTION
 * \sa SDL_HapticEffect
 */
typedef struct SDL_HapticCondition {
   /* Header */
   Uint16 type; /**< SDL_HAPTIC_SPRING, SDL_HAPTIC_DAMPER,
                     SDL_HAPTIC_INERTIA or SDL_HAPTIC_FRICTION */

   /* Replay */
   Uint16 length; /**< Duration of the effect. */
   Uint16 delay; /**< Delay before starting the effect. */

   /* Trigger */
   Uint16 button; /**< Button that triggers the effect. */
   Uint16 interval; /**< How soon it can be triggered again after button. */

   /* Condition */
   Uint16 right_sat[2]; /**< Level when joystick is to the positive side. */
   Uint16 left_sat[2]; /**< Level when joystick is to the negative side. */
   Sint16 right_coeff[2]; /**< How fast to increase the force towards the positive side. */
   Sint16 left_coeff[2]; /**< How fast to increase the force towards the negative side. */
   Uint16 deadband[2]; /**< Size of the dead zone. */
   Sint16 center[2]; /**< Position of the dead zone. */
} SDL_HapticCondition;
/**
 * \struct SDL_HapticRamp
 *
 * \brief A structure containing a template for a Ramp effect.
 *
 * This struct is exclusively for the SDL_HAPTIC_RAMP effect.
 *
 * The ramp effect starts at start strength and ends at end strength.
 *  It augments in linear fashion.  If you use attack and fade with a ramp
 *  they effects get added to the ramp effect making the effect become
 *  quadratic instead of linear.
 *
 * \sa SDL_HAPTIC_RAMP
 * \sa SDL_HapticEffect
 */
typedef struct SDL_HapticRamp {
   /* Header */
   Uint16 type; /**< SDL_HAPTIC_RAMP */
   SDL_HapticDirection direction; /**< Direction of the effect. */

   /* Replay */
   Uint16 length; /**< Duration of the effect. */
   Uint16 delay; /**< Delay before starting the effect. */

   /* Trigger */
   Uint16 button; /**< Button that triggers the effect. */
   Uint16 interval; /**< How soon it can be triggered again after button. */

   /* Ramp */
   Sint16 start; /**< Beginning strength level. */
   Sint16 end; /**< Ending strength level. */

   /* Envelope */
   Uint16 attack_length; /**< Duration of the attack. */
   Uint16 attack_level; /**< Level at the start of the attack. */
   Uint16 fade_length; /**< Duration of the fade. */
   Uint16 fade_level; /**< Level at the end of the fade. */
} SDL_HapticRamp;
/**
 * \union SDL_HapticEffect
 *
 * \brief The generic template for any haptic effect.
 *
 * All values max at 32767 (0x7FFF).  Signed values also can be negative.
 *  Time values unless specified otherwise are in milliseconds.
 *
 * Common parts:
 * \code
 * // Replay - All effects have this
 * Uint16 length;        // Duration of effect (ms).
 * Uint16 delay;         // Delay before starting effect.
 *
 * // Trigger - All effects have this
 * Uint16 button;        // Button that triggers effect.
 * Uint16 interval;      // How soon before effect can be triggered again.
 *
 * // Envelope - All effects except condition effects have this
 * Uint16 attack_length; // Duration of the attack (ms).
 * Uint16 attack_level;  // Level at the start of the attack.
 * Uint16 fade_length;   // Duration of the fade out (ms).
 * Uint16 fade_level;    // Level at the end of the fade.
 * \endcode
 *
 *
 * Here we have an example of a constant effect evolution in time:
 *
 * \code
 * Strength
 * ^
 * |
 * |    effect level -->  _________________
 * |                     /                 \
 * |                    /                   \
 * |                   /                     \
 * |                  /                       \ 
 * | attack_level --> |                        \
 * |                  |                        |  <---  fade_level
 * |
 * +--------------------------------------------------> Time
 *                    [--]                 [---]
 *                    attack_length        fade_length
 * 
 * [------------------][-----------------------]
 * delay               length
 * \endcode
 *
 * Note either the attack_level or the fade_level may be above the actual
 *  effect level.
 *
 * \sa SDL_HapticConstant
 * \sa SDL_HapticPeriodic
 * \sa SDL_HapticCondition
 * \sa SDL_HapticRamp
 */
typedef union SDL_HapticEffect {
   /* Common for all force feedback effects */
   Uint16 type; /**< Effect type. */
   SDL_HapticConstant constant; /**< Constant effect. */
   SDL_HapticPeriodic periodic; /**< Periodic effect. */
   SDL_HapticCondition condition; /**< Condition effect. */
   SDL_HapticRamp ramp; /**< Ramp effect. */
} SDL_HapticEffect;


/* Function prototypes */
/**
 * \fn int SDL_NumHaptics(void)
 *
 * \brief Count the number of joysticks attached to the system.
 *
 *    \return Number of haptic devices detected on the system.
 */
extern DECLSPEC int SDLCALL SDL_NumHaptics(void);

/**
 * \fn const char * SDL_HapticName(int device_index)
 *
 * \brief Get the implementation dependent name of a Haptic device.
 * This can be called before any joysticks are opened.
 * If no name can be found, this function returns NULL.
 *
 *    \param device_index Index of the device to get it's name.
 *    \return Name of the device or NULL on error.
 *
 * \sa SDL_NumHaptics
 */
extern DECLSPEC const char *SDLCALL SDL_HapticName(int device_index);

/**
 * \fn SDL_Haptic * SDL_HapticOpen(int device_index)
 *
 * \brief Opens a Haptic device for usage - the index passed as an
 * argument refers to the N'th Haptic device on this system.
 *
 * When opening a haptic device, it's gain will be set to maximum and
 *  autocenter will be disabled.  To modify these values use
 *  SDL_HapticSetGain and SDL_HapticSetAutocenter
 *
 *    \param device_index Index of the device to open.
 *    \return Device identifier or NULL on error.
 *
 * \sa SDL_HapticIndex
 * \sa SDL_HapticOpenFromMouse
 * \sa SDL_HapticOpenFromJoystick
 * \sa SDL_HapticClose
 * \sa SDL_HapticSetGain
 * \sa SDL_HapticSetAutocenter
 */
extern DECLSPEC SDL_Haptic * SDL_HapticOpen(int device_index);

/**
 * \fn int SDL_HapticOpened(int device_index)
 *
 * \brief Checks if the haptic device at index has been opened.
 *
 *    \param device_index Index to check to see if it has been opened.
 *    \return 1 if it has been opened or 0 if it hasn't.
 * 
 * \sa SDL_HapticOpen
 * \sa SDL_HapticIndex
 */
extern DECLSPEC int SDL_HapticOpened(int device_index);

/**
 * \fn int SDL_HapticIndex(SDL_Haptic * haptic)
 *
 * \brief Gets the index of a haptic device.
 *
 *    \param haptic Haptic device to get the index of.
 *    \return The index of the haptic device or -1 on error.
 *
 * \sa SDL_HapticOpen
 * \sa SDL_HapticOpened
 */
extern DECLSPEC int SDL_HapticIndex(SDL_Haptic * haptic);

/**
 * \fn int SDL_MouseIsHaptic(void)
 *
 * \brief Gets whether or not the current mouse has haptic capabilities.
 *
 *    \return SDL_TRUE if the mouse is haptic, SDL_FALSE if it isn't.
 *
 * \sa SDL_HapticOpenFromMouse
 */
extern DECLSPEC int SDL_MouseIsHaptic(void);

/**
 * \fn SDL_Haptic * SDL_HapticOpenFromMouse(void)
 *
 * \brief Tries to open a haptic device from the current mouse.
 *
 *    \return The haptic device identifier or NULL on error.
 *
 * \sa SDL_MouseIsHaptic
 * \sa SDL_HapticOpen
 */
extern DECLSPEC SDL_Haptic * SDL_HapticOpenFromMouse(void);

/**
 * \fn int SDL_JoystickIsHaptic(SDL_Joystick * joystick)
 *
 * \brief Checks to see if a joystick has haptic features.
 *
 *    \param joystick Joystick to test for haptic capabilities.
 *    \return SDL_TRUE if the joystick is haptic, SDL_FALSE if it isn't
 *            or -1 if an error ocurred.
 *
 * \sa SDL_HapticOpenFromJoystick
 */
extern DECLSPEC int SDL_JoystickIsHaptic(SDL_Joystick * joystick);

/**
 * \fn SDL_Haptic * SDL_HapticOpenFromJoystick(SDL_Joystick * joystick)
 *
 * \brief Opens a Haptic device for usage from a Joystick device.  Still has
 * to be closed seperately to the joystick.
 *
 *    \param joystick Joystick to create a haptic device from.
 *    \return A valid haptic device identifier on success or NULL on error.
 *
 * \sa SDL_HapticOpen
 * \sa SDL_HapticClose
 */
extern DECLSPEC SDL_Haptic * SDL_HapticOpenFromJoystick(SDL_Joystick * joystick);

/**
 * \fn void SDL_HapticClose(SDL_Haptic * haptic)
 *
 * \brief Closes a Haptic device previously opened with SDL_HapticOpen.
 *
 *    \param haptic Haptic device to close.
 */
extern DECLSPEC void SDL_HapticClose(SDL_Haptic * haptic);

/**
 * \fn int SDL_HapticNumEffects(SDL_Haptic * haptic)
 *
 * \brief Returns the number of effects a haptic device can store.
 *
 *    \param haptic The haptic device to query effect max.
 *    \return The number of effects the haptic device can store or
 *            -1 on error.
 *
 * \sa SDL_HapticNumEffectsPlaying
 * \sa SDL_HapticQuery
 */
extern DECLSPEC int SDL_HapticNumEffects(SDL_Haptic * haptic);

/**
 * \fn int SDL_HapticNumEffectsPlaying(SDL_Haptic * haptic)
 *
 * \brief Returns the number of effects a haptic device can play at the same time.
 *
 *    \param haptic The haptic device to query maximum playing effect.s
 *    \return The number of effects the haptic device can play at the same time
 *            or -1 on error.
 *
 * \sa SDL_HapticNumEffects
 * \sa SDL_HapticQuery
 */
extern DECLSPEC int SDL_HapticNumEffectsPlaying(SDL_Haptic * haptic);

/**
 * \fn unsigned int SDL_HapticQuery(SDL_Haptic * haptic)
 *
 * \brief Gets the haptic devices supported features in bitwise matter.
 *
 * Example: 
 * \code
 * if (SDL_HapticQueryEffects(haptic) & SDL_HAPTIC_CONSTANT) {
 *    printf("We have constant haptic effect!");
 * }
 * \endcode
 *    
 *
 *    \param haptic The haptic device to query.
 *    \return Haptic features in bitwise manner (OR'd).
 *
 * \sa SDL_HapticNumEffects
 * \sa SDL_HapticEffectSupported
 */
extern DECLSPEC unsigned int SDL_HapticQuery(SDL_Haptic * haptic);

/**
 * \fn int SDL_HapticEffectSupported(SDL_Haptic * haptic, SDL_HapticEffect * effect)
 *
 * \brief Checks to see if effect is supported by haptic.
 *
 *    \param haptic Haptic device to check on.
 *    \param effect Effect to check to see if it is supported.
 *    \return SDL_TRUE if effect is supported, SDL_FALSE if it isn't or 
 *            -1 on error.
 * 
 * \sa SDL_HapticQuery
 * \sa SDL_HapticNewEffect
 */
extern DECLSPEC int SDL_HapticEffectSupported(SDL_Haptic * haptic, SDL_HapticEffect * effect);

/**
 * \fn int SDL_HapticNewEffect(SDL_Haptic * haptic, SDL_HapticEffect * effect)
 *
 * \brief Creates a new haptic effect on the device.
 *
 *    \param haptic Haptic device to create the effect on.
 *    \param effect Properties of the effect to create.
 *    \return The id of the effect on success or -1 on error.
 *
 * \sa SDL_HapticUpdateEffect
 * \sa SDL_HapticRunEffect
 * \sa SDL_HapticDestroyEffect
 */
extern DECLSPEC int SDL_HapticNewEffect(SDL_Haptic * haptic, SDL_HapticEffect * effect);

/**
 * \fn int SDL_HapticUpdateEffect(SDL_Haptic * haptic, int effect, SDL_HapticEffect * data)
 *
 * \brief Updates an effect.  Can be used dynamically, although behaviour when
 * dynamically changing direction may be strange.  Specifically the effect
 * may reupload itself and start playing from the start.  You cannot change
 * the type either when running UpdateEffect.
 *
 *    \param haptic Haptic device that has the effect.
 *    \param effect Effect to update.
 *    \param data New effect properties to use.
 *    \return The id of the effect on success or -1 on error.
 *
 * \sa SDL_HapticNewEffect
 * \sa SDL_HapticRunEffect
 * \sa SDL_HapticDestroyEffect
 */
extern DECLSPEC int SDL_HapticUpdateEffect(SDL_Haptic * haptic, int effect, SDL_HapticEffect * data);

/**
 * \fn int SDL_HapticRunEffect(SDL_Haptic * haptic, int effect, int iterations)
 *
 * \brief Runs the haptic effect on it's assosciated haptic device.
 *
 *    \param haptic Haptic device to run the effect on.
 *    \param effect Identifier of the haptic effect to run.
 *    \param iterations Number of iterations to run the effect. Use
 *           SDL_HAPTIC_INFINITY for infinity.
 *    \return 0 on success or -1 on error.
 *
 * \sa SDL_HapticStopEffect
 * \sa SDL_HapticDestroyEffect
 * \sa SDL_HapticGetEffectStatus
 */
extern DECLSPEC int SDL_HapticRunEffect(SDL_Haptic * haptic, int effect, int iterations);

/**
 * \fn int SDL_HapticStopEffect(SDL_Haptic * haptic, int effect)
 *
 * \brief Stops the haptic effect on it's assosciated haptic device.
 *
 *    \param haptic Haptic device to stop the effect on.
 *    \param effect Identifier of the effect to stop.
 *    \return 0 on success or -1 on error.
 *
 * \sa SDL_HapticRunEffect
 * \sa SDL_HapticDestroyEffect
 */
extern DECLSPEC int SDL_HapticStopEffect(SDL_Haptic * haptic, int effect);

/**
 * \fn void SDL_HapticDestroyEffect(SDL_Haptic * haptic, int effect)
 *
 * \brief Destroys a haptic effect on the device.  This will stop the effect
 * if it's running.  Effects are automatically destroyed when the device is
 * closed.
 *
 *    \param haptic Device to destroy the effect on.
 *    \param effect Identifier of the effect to destroy.
 * 
 * \sa SDL_HapticNewEffect
 */
extern DECLSPEC void SDL_HapticDestroyEffect(SDL_Haptic * haptic, int effect);

/**
 * \fn int SDL_HapticGetEffectStatus(SDL_Haptic *haptic, int effect)
 *
 * \brief Gets the status of the current effect on the haptic device.
 *
 * Device must support the SDL_HAPTIC_STATUS feature.
 *
 *    \param haptic Haptic device to query the effect status on.
 *    \param effect Identifier of the effect to query it's status.
 *    \return 0 if it isn't playing, SDL_HAPTIC_PLAYING if it is playing
 *            or -1 on error.
 *
 * \sa SDL_HapticRunEffect
 * \sa SDL_HapticStopEffect
 */
extern DECLSPEC int SDL_HapticGetEffectStatus(SDL_Haptic *haptic, int effect);

/**
 * \fn int SDL_HapticSetGain(SDL_Haptic * haptic, int gain)
 *
 * \brief Sets the global gain of the device.  Gain should be between 0 and 100.
 *
 * Device must support the SDL_HAPTIC_GAIN feature.
 *
 * The user may specify the maxmimum gain by setting the environment variable
 *  SDL_HAPTIC_GAIN_MAX which should be between 0 and 100.  All calls to
 *  SDL_HapticSetGain will scale linearly using SDL_HAPTIC_GAIN_MAX as the
 *  maximum.
 *
 *    \param haptic Haptic device to set the gain on.
 *    \param gain Value to set the gain to, should be between 0 and 100.
 *    \return 0 on success or -1 on error.
 *
 * \sa SDL_HapticQuery
 */
extern DECLSPEC int SDL_HapticSetGain(SDL_Haptic * haptic, int gain);

/**
 * \fn int SDL_HapticSetAutocenter(SDL_Haptic * haptic, int autocenter)
 *
 * \brief Sets the global autocenter of the device.  Autocenter should be between
 * 0 and 100.  Setting it to 0 will disable autocentering.
 *
 * Device must support the SDL_HAPTIC_AUTOCENTER feature.
 *
 *    \param haptic Haptic device to set autocentering on.
 *    \param autocenter Value to set autocenter to, 0 disables autocentering.
 *    \return 0 on success or -1 on error.
 *
 * \sa SDL_HapticQuery
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


