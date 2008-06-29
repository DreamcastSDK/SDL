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

/* This is the touchscreen API for Simple DirectMedia Layer */

#include "SDL_events.h"
#include "SDL_systouchscreen.h"
#include "SDL_touchscreen_c.h"
#if !SDL_EVENTS_DISABLED
#include "../events/SDL_events_c.h"
#endif

/* This is used for Quake III Arena */
#if SDL_EVENTS_DISABLED
#define SDL_Lock_EventThread()
#define SDL_Unlock_EventThread()
#endif

Uint8 SDL_numtouchscreens = 0;
SDL_Touchscreen **SDL_touchscreens = NULL;
static SDL_Touchscreen *default_touchscreen = NULL;

int
SDL_TouchscreenInit(void)
{
    int arraylen;
    int status;

    SDL_numtouchscreens = 0;
    status = SDL_SYS_TouchscreenInit();
    if (status >= 0) {
        arraylen = (status + 1) * sizeof(*SDL_touchscreens);
        SDL_touchscreens = (SDL_Touchscreen **) SDL_malloc(arraylen);
        if (SDL_touchscreens == NULL) {
            SDL_numtouchscreens = 0;
        } else {
            SDL_memset(SDL_touchscreens, 0, arraylen);
            SDL_numtouchscreens = status;
        }
        status = 0;
    }
    default_touchscreen = NULL;
    return (status);
}

/*
 * Count the number of touchscreens attached to the system
 */
int
SDL_NumTouchscreens(void)
{
    return SDL_numtouchscreens;
}

/*
 * Get the implementation dependent name of a touchscreen
 */
const char *
SDL_TouchscreenName(int device_index)
{
    if ((device_index < 0) || (device_index >= SDL_numtouchscreens)) {
        SDL_SetError("There are %d touchscreens available", SDL_numtouchscreens);
        return (NULL);
    }
    return (SDL_SYS_TouchscreenName(device_index));
}

/*
 * Open a touchscreen for use - the index passed as an argument refers to
 * the N'th touchscreen on the system.  This index is the value which will
 * identify this touchscreen in future touchscreen events.
 *
 * This function returns a touchscreen identifier, or NULL if an error occurred.
 */
SDL_Touchscreen *
SDL_TouchscreenOpen(int device_index)
{
    int i;
    SDL_Touchscreen *touchscreen;

    if ((device_index < 0) || (device_index >= SDL_numtouchscreens)) {
        SDL_SetError("There are %d touchscreens available", SDL_numtouchscreens);
        return (NULL);
    }

    /* If the touchscreen is already open, return it */
    for (i = 0; SDL_touchscreens[i]; ++i) {
        if (device_index == SDL_touchscreens[i]->index) {
            touchscreen = SDL_touchscreens[i];
            ++touchscreen->ref_count;
            return (touchscreen);
        }
    }

    /* Create and initialize the touchscreen */
    touchscreen = (SDL_Touchscreen *) SDL_malloc((sizeof *touchscreen));
    if (touchscreen != NULL) {
        SDL_memset(touchscreen, 0, (sizeof *touchscreen));
        touchscreen->index = device_index;
        if (SDL_SYS_TouchscreenOpen(touchscreen) < 0) {
            SDL_free(touchscreen);
            touchscreen = NULL;
        } else {
            if (touchscreen->maxpoints > 0) {
                touchscreen->points = (struct touchpoint *) SDL_malloc
                    (touchscreen->maxpoints * sizeof(*touchscreen->points));
            }
            if ((touchscreen->maxpoints > 0) && !touchscreen->points) {
                SDL_OutOfMemory();
                SDL_TouchscreenClose(touchscreen);
                touchscreen = NULL;
            }
            if (touchscreen->points) {
                SDL_memset(touchscreen->points, 0,
                           touchscreen->maxpoints * sizeof(*touchscreen->points));
            }
        }
    }
    if (touchscreen) {
        /* Add touchscreen to list */
        ++touchscreen->ref_count;
        SDL_Lock_EventThread();
        for (i = 0; SDL_touchscreens[i]; ++i)
            /* Skip to next touchscreen */ ;
        SDL_touchscreens[i] = touchscreen;
        SDL_Unlock_EventThread();
    }
    return (touchscreen);
}

/*
 * Returns 1 if the touchscreen has been opened, or 0 if it has not.
 */
int
SDL_TouchscreenOpened(int device_index)
{
    int i, opened;

    opened = 0;
    for (i = 0; SDL_touchscreens[i]; ++i) {
        if (SDL_touchscreens[i]->index == (Uint8) device_index) {
            opened = 1;
            break;
        }
    }
    return (opened);
}

static int
ValidTouchscreen(SDL_Touchscreen ** touchscreen)
{
    int valid;

    if (*touchscreen == NULL) {
        *touchscreen = default_touchscreen;
    }
    if (*touchscreen == NULL) {
        SDL_SetError("Touchscreen hasn't been opened yet");
        valid = 0;
    } else {
        valid = 1;
    }
    return valid;
}

/*
 * Get the device index of an opened touchscreen.
 */
int
SDL_TouchscreenIndex(SDL_Touchscreen * touchscreen)
{
    if (!ValidTouchscreen(&touchscreen)) {
        return (-1);
    }
    return (touchscreen->index);
}

/*
 * Get the max number of points on a multi-touch screen (or 1 on a single-touch)
 */
int
SDL_TouchscreenMaxPoints(SDL_Touchscreen * touchscreen)
{
    if (!ValidTouchscreen(&touchscreen)) {
        return (-1);
    }
    return (touchscreen->maxpoints);
}

/*
 * Get the current X,Y position of the indicated point on the touchscreen
 */
Uint16
SDL_TouchscreenGetXY(SDL_Touchscreen *touchscreen, int point, Uint16 *x, Uint16 *y)
{
    int retval;

    if (!ValidTouchscreen(&touchscreen)) {
        return (-1);
    }

    retval = 0;
    if (point < 0) {
        int i; long avg;
        if(x) {
            avg = 0;
            for(i = 0; i < touchscreen->npoints; ++i) {
                avg += touchscreen->points[i].x;
            }
            *x = avg;
        }
        if(y) {
            avg = 0;
            for(i = 0; i < touchscreen->npoints; ++i) {
                avg += touchscreen->points[i].y;
            }
            *y = avg;
        }
        avg = 0;
        for(i = 0; i < touchscreen->npoints; ++i) {
            avg += touchscreen->points[i].pressure;
        }
        return (int)avg;
    } else if (point < touchscreen->maxpoints) {
        if (x) {
            *x = touchscreen->points[point].x;
        }
        if (y) {
            *y = touchscreen->points[point].y;
        }
        retval = touchscreen->points[point].pressure;
    } else {
        SDL_SetError("Touchscreen only can have %d points", touchscreen->maxpoints);
        retval = -1;
    }
    return (retval);
}

int
SDL_TouchscreenGetPoints(SDL_Touchscreen *touchscreen) {
    if (!ValidTouchscreen(&touchscreen)) {
        return (-1);
    }

    return touchscreen->npoints;
}

/*
 * Close a touchscreen previously opened with SDL_TouchscreenOpen()
 */
void
SDL_TouchscreenClose(SDL_Touchscreen * touchscreen)
{
    int i;

    if (!ValidTouchscreen(&touchscreen)) {
        return;
    }

    /* First decrement ref count */
    if (--touchscreen->ref_count > 0) {
        return;
    }

    /* Lock the event queue - prevent touchscreen polling */
    SDL_Lock_EventThread();

    if (touchscreen == default_touchscreen) {
        default_touchscreen = NULL;
    }
    SDL_SYS_TouchscreenClose(touchscreen);

    /* Remove touchscreen from list */
    for (i = 0; SDL_touchscreens[i]; ++i) {
        if (touchscreen == SDL_touchscreens[i]) {
            SDL_memcpy(&SDL_touchscreens[i], &SDL_touchscreens[i + 1],
                       (SDL_numtouchscreens - i) * sizeof(touchscreen));
            break;
        }
    }

    /* Let the event thread keep running */
    SDL_Unlock_EventThread();

    /* Free the data associated with this touchscreen */
    if (touchscreen->points) {
        SDL_free(touchscreen->points);
    }
    SDL_free(touchscreen);
}

void
SDL_TouchscreenQuit(void)
{
    /* Stop the event polling */
    SDL_Lock_EventThread();
    SDL_numtouchscreens = 0;
    SDL_Unlock_EventThread();

    /* Quit the touchscreen setup */
    SDL_SYS_TouchscreenQuit();
    if (SDL_touchscreens) {
        SDL_free(SDL_touchscreens);
        SDL_touchscreens = NULL;
    }
}


/* These are global for SDL_systouchscreen.c and SDL_events.c */
int
SDL_PrivateTouchPress(SDL_Touchscreen * touchscreen, int point, Uint16 x, Uint16 y, Uint16 pressure) {
    int posted;

    if (!ValidTouchscreen(&touchscreen)) {
        return -1;
    }

    if(point >= touchscreen->maxpoints) {
        SDL_SetError("Touchscreen only can have %d points", touchscreen->maxpoints);
        return -1;
    }

    /* on neg. point, set the given args as the *only* point.
       so set the struct to have no points pressed, then continue as normal. */
    if(point < 0) {
        point = 0;
        touchscreen->npoints = 0;
        SDL_memset(touchscreen->points, 0,
                   touchscreen->maxpoints * sizeof(touchscreen->points[0]));
    }

    /* new touch point!  that means a TOUCHPRESSED event is due. */
    if(point >= touchscreen->npoints) {
        point = touchscreen->npoints;
        ++touchscreen->npoints;
    }

    /* no motion, no change, don't report an event. */
    if(touchscreen->points[point].pressure > 0) {
        SDL_SetError("Warning: touch point %d was already pressed", point);
        return -1;
    }

    /* Update internal touchscreen point state */
    touchscreen->points[point].x = x;
    touchscreen->points[point].y = y;
    touchscreen->points[point].pressure = pressure;

    /* Post the event, if desired */
    posted = 0;
#if !SDL_EVENTS_DISABLED
    if (SDL_ProcessEvents[SDL_TOUCHPRESSED] == SDL_ENABLE) {
        SDL_Event event;
        event.touch.type = SDL_TOUCHPRESSED;
        event.touch.which = touchscreen->index;
        event.touch.point = point;
        event.touch.xpos = x;
        event.touch.ypos = y;
        event.touch.pressure = pressure;
        if ((SDL_EventOK == NULL)
            || (*SDL_EventOK) (SDL_EventOKParam, &event)) {
            posted = 1;
            SDL_PushEvent(&event);
        }
    }
#endif /* !SDL_EVENTS_DISABLED */
    return (posted);
}

int
SDL_PrivateTouchMove(SDL_Touchscreen * touchscreen, int point,
                                 Uint16 x, Uint16 y, Uint16 pressure) {
    int posted;

    if (!ValidTouchscreen(&touchscreen)) {
        return -1;
    }

    if(point >= touchscreen->maxpoints) {
        SDL_SetError("Touchscreen only can have %d points", touchscreen->maxpoints);
        return -1;
    }

    /* on neg. point, set the given args as the *only* point.
       so set the struct to have no points pressed, then continue as normal. */
    if(point < 0) {
        point = 0;
        touchscreen->npoints = 0;
        SDL_memset(touchscreen->points, 0,
                   touchscreen->maxpoints * sizeof(touchscreen->points[0]));
    }

    /* new touch point!  that means a TOUCHPRESSED event is due. */
    if(point >= touchscreen->npoints || touchscreen->points[point].pressure == 0) {
        SDL_SetError("Touch point %d shouldn't move before it's pressed.", point);
        return -1;
    }

    /* no motion, no change, don't report an event. */
    if(touchscreen->points[point].x == x &&
       touchscreen->points[point].y == y &&
       touchscreen->points[point].pressure == pressure) {
        return 0;
    }

    /* Update internal touchscreen point state */
    touchscreen->points[point].x = x;
    touchscreen->points[point].y = y;
    touchscreen->points[point].pressure = pressure;

    /* Post the event, if desired */
    posted = 0;
#if !SDL_EVENTS_DISABLED
    if (SDL_ProcessEvents[SDL_TOUCHMOTION] == SDL_ENABLE) {
        SDL_Event event;
        event.touch.type = SDL_TOUCHMOTION;
        event.touch.which = touchscreen->index;
        event.touch.point = point;
        event.touch.xpos = x;
        event.touch.ypos = y;
        event.touch.pressure = pressure;
        if ((SDL_EventOK == NULL)
            || (*SDL_EventOK) (SDL_EventOKParam, &event)) {
            posted = 1;
            SDL_PushEvent(&event);
        }
    }
#endif /* !SDL_EVENTS_DISABLED */
    return (posted);
}

int
SDL_PrivateTouchRelease(SDL_Touchscreen * touchscreen, int point) {
    int posted;
    int i;

    if (!ValidTouchscreen(&touchscreen)) {
        return -1;
    }

    if(point >= touchscreen->maxpoints) {
        SDL_SetError("Touchscreen only can have %d points", touchscreen->maxpoints);
        return -1;
    } else if(point >= touchscreen->npoints) {
        SDL_SetError("Point %d up when only %d were down", point, touchscreen->npoints);
        return -1;
    }

    /* on neg. point, clear all points.  so set the struct to have one point
       pressed, then continue as normal. */
    if(point < 0) {
        point = 0;
        touchscreen->npoints = 1;
        SDL_memset(&(touchscreen->points[1]), 0,
                   (touchscreen->maxpoints-1) * sizeof(touchscreen->points[0]));
    }

    /* Update internal touchscreen point state */
    touchscreen->points[point].pressure = 0;
    touchscreen->points[point].x = 0;
    touchscreen->points[point].y = 0;
    if(touchscreen->npoints >= 0) --touchscreen->npoints;
    for(i = point; i < touchscreen->npoints; ++i) {
        touchscreen->points[i] = touchscreen->points[i+1];
    }

    /* Post the event, if desired */
    posted = 0;
#if !SDL_EVENTS_DISABLED
    if (SDL_ProcessEvents[SDL_TOUCHRELEASED] == SDL_ENABLE) {
        SDL_Event event;
        event.touch.type = SDL_TOUCHRELEASED;
        event.touch.which = touchscreen->index;
        event.touch.point = point;
        if ((SDL_EventOK == NULL)
            || (*SDL_EventOK) (SDL_EventOKParam, &event)) {
            posted = 1;
            SDL_PushEvent(&event);
        }
    }
#endif /* !SDL_EVENTS_DISABLED */
    return (posted);
}

void
SDL_TouchscreenUpdate(void)
{
    int i;

    for (i = 0; SDL_touchscreens[i]; ++i) {
        SDL_SYS_TouchscreenUpdate(SDL_touchscreens[i]);
    }
}

int
SDL_TouchscreenEventState(int state)
{
#if SDL_EVENTS_DISABLED
    return SDL_IGNORE;
#else
    const Uint8 event_list[] = {
        SDL_TOUCHPRESSED, SDL_TOUCHRELEASED, SDL_TOUCHMOTION
    };
    unsigned int i;

    switch (state) {
    case SDL_QUERY:
        state = SDL_IGNORE;
        for (i = 0; i < SDL_arraysize(event_list); ++i) {
            state = SDL_EventState(event_list[i], SDL_QUERY);
            if (state == SDL_ENABLE) {
                break;
            }
        }
        break;
    default:
        for (i = 0; i < SDL_arraysize(event_list); ++i) {
            SDL_EventState(event_list[i], state);
        }
        break;
    }
    return (state);
#endif /* SDL_EVENTS_DISABLED */
}

/* vi: set ts=4 sw=4 expandtab: */
