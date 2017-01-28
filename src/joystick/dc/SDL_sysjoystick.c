/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    BERO
    bero@geocities.co.jp

    based on win32/SDL_mmjoystick.c

    Sam Lantinga
    slouken@libsdl.org

    Modified by Lawrence Sebald <bluecrab2887@netscape.net>
*/

/* Win32 MultiMedia Joystick driver, contributed by Andrei de A. Formiga */

#include <stdlib.h>
#include <stdio.h>		/* For the definition of NULL */
#include <string.h>

#include "SDL_error.h"
#include "SDL_joystick.h"
#include "../SDL_sysjoystick.h"
#include "SDL_joystick_c.h"

#include <dc/maple.h>
#include <dc/maple/controller.h>

#define MAX_JOYSTICKS	8	/* only 2 are supported in the multimedia API */
#define MAX_AXES	6	/* each joystick can have up to 6 axes */
#define MAX_BUTTONS	8	/* and 8 buttons                      */
#define	MAX_HATS	2

#define	JOYNAMELEN	8

/* array to hold joystick ID values */
static maple_device_t *SYS_Joystick_addr[MAX_JOYSTICKS];

/* The private structure used to keep track of a joystick */
typedef struct joystick_hwdata {
    cont_state_t prev_state;
} jhwdata_t;

/* Function to scan the system for joysticks.
 * This function should set SDL_numjoysticks to the number of available
 * joysticks.  Joystick 0 should be the system default joystick.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */
int SDL_SYS_JoystickInit(void) {
	int numdevs = 0, i;
    maple_device_t *dev;

    for(i = 0; i < MAX_JOYSTICKS; ++i) {
        if((dev = maple_enum_type(i, MAPLE_FUNC_CONTROLLER))) {
            SYS_Joystick_addr[numdevs++] = dev;
        }
    }

	return numdevs;
}

/* Function to get the device-dependent name of a joystick */
const char *SDL_SYS_JoystickName(int index) {
	maple_device_t *dev;

	if(index > MAX_JOYSTICKS || !(dev = SYS_Joystick_addr[index])) {
        return NULL;
    }

	return dev->info.product_name;
}

/* Function to open a joystick for use.
   The joystick to open is specified by the index field of the joystick.
   This should fill the nbuttons and naxes fields of the joystick structure.
   It returns 0, or -1 if there is an error.
 */
int SDL_SYS_JoystickOpen(SDL_Joystick *joystick) {
    /* allocate memory for system specific hardware data */
    joystick->hwdata = (struct joystick_hwdata *)malloc(sizeof(jhwdata_t));

    if(joystick->hwdata == NULL) {
        SDL_OutOfMemory();
        return(-1);
    }

    memset(joystick->hwdata, 0, sizeof(jhwdata_t));

    /* fill nbuttons, naxes, and nhats fields */
    joystick->nbuttons = MAX_BUTTONS;
    joystick->naxes = MAX_AXES;
    joystick->nhats = MAX_HATS;

    return 0;
}


/* Function to update the state of a joystick - called as a device poll.
 * This function shouldn't update the joystick structure directly,
 * but instead should call SDL_PrivateJoystick*() to deliver events
 * and update joystick device state.
 */

static const int sdl_buttons[] = {
    CONT_A,
    CONT_B,
    CONT_X,
    CONT_Y,
    CONT_START,
    CONT_C,
    CONT_D,
    CONT_Z,
};

void SDL_SYS_JoystickUpdate(SDL_Joystick *joystick) {
	maple_device_t *dev;
    cont_state_t *state, *prev_state;
	int buttons, i, changed, hat;

	dev = SYS_Joystick_addr[joystick->index];

	if(!(state = (cont_state_t *)maple_dev_status(dev))) {
        return;
    }

	buttons = state->buttons;
    prev_state = &joystick->hwdata->prev_state;
	changed = buttons ^ prev_state->buttons;

	if(changed & (CONT_DPAD_UP | CONT_DPAD_DOWN | CONT_DPAD_LEFT |
                  CONT_DPAD_RIGHT)) {
		hat = SDL_HAT_CENTERED;

		if(buttons & CONT_DPAD_UP)
            hat |= SDL_HAT_UP;
		if(buttons & CONT_DPAD_DOWN)
            hat |= SDL_HAT_DOWN;
		if(buttons & CONT_DPAD_LEFT)
            hat |= SDL_HAT_LEFT;
		if(buttons & CONT_DPAD_RIGHT)
            hat |= SDL_HAT_RIGHT;

		SDL_PrivateJoystickHat(joystick, 0, hat);
	}
	if(changed & (CONT_DPAD2_UP | CONT_DPAD2_DOWN | CONT_DPAD2_LEFT |
                  CONT_DPAD2_RIGHT)) {
		hat = SDL_HAT_CENTERED;

		if(buttons & CONT_DPAD2_UP)
            hat |= SDL_HAT_UP;
		if(buttons & CONT_DPAD2_DOWN)
            hat |= SDL_HAT_DOWN;
		if(buttons & CONT_DPAD2_LEFT)
            hat |= SDL_HAT_LEFT;
		if(buttons & CONT_DPAD2_RIGHT)
            hat |= SDL_HAT_RIGHT;

		SDL_PrivateJoystickHat(joystick, 1, hat);
	}

	for(i = 0; i < sizeof(sdl_buttons) / sizeof(sdl_buttons[0]); ++i) {
		if(changed & sdl_buttons[i]) {
			SDL_PrivateJoystickButton(joystick, i, (buttons & sdl_buttons[i]) ?
                                      1 : 0);
		}
	}

	if(state->joyx != prev_state->joyx)
		SDL_PrivateJoystickAxis(joystick, 0, state->joyx);
	if(state->joyy != prev_state->joyy)
		SDL_PrivateJoystickAxis(joystick, 1, state->joyy);
	if(state->rtrig != prev_state->rtrig)
		SDL_PrivateJoystickAxis(joystick, 2, state->rtrig);
	if(state->ltrig != prev_state->ltrig)
		SDL_PrivateJoystickAxis(joystick, 3, state->ltrig);
	if(state->joy2x != prev_state->joy2x)
		SDL_PrivateJoystickAxis(joystick, 4, state->joy2x);
	if(state->joy2y != prev_state->joy2y)
		SDL_PrivateJoystickAxis(joystick, 5, state->joy2y);

	joystick->hwdata->prev_state = *state;
}

/* Function to close a joystick after use */
void SDL_SYS_JoystickClose(SDL_Joystick *joystick) {
    if (joystick->hwdata != NULL) {
        /* free system specific hardware data */
        free(joystick->hwdata);
    }
}

/* Function to perform any system-specific joystick related cleanup */
void SDL_SYS_JoystickQuit(void) {
}
