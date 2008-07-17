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

/* Dummy SDL video driver implementation; this is just enough to make an
 *  SDL-based application THINK it's got a working video driver, for
 *  applications that call SDL_Init(SDL_INIT_VIDEO) when they don't need it,
 *  and also for use as a collection of stubs when porting SDL to a new
 *  platform for which you haven't yet written a valid video driver.
 *
 * This is also a great way to determine bottlenecks: if you think that SDL
 *  is a performance problem for a given platform, enable this driver, and
 *  then see if your application runs faster without video overhead.
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "DUMMY" by Sam Lantinga.
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_uikitvideo.h"
#include "SDL_uikitevents.h"
#include "SDL_uikitwindow.h"
#import "SDL_uikitappdelegate.h"

#import "SDL_uikitopenglview.h"
#import "SDL_renderer_sw.h"

#include <UIKit/UIKit.h>
#include <Foundation/Foundation.h>



extern UIWindow *uikitWindow;
extern SDL_uikitopenglview *uikitEAGLView;

int UIKit_CreateWindow(_THIS, SDL_Window *window) {
	
	printf("Create window! UIKIT!\n");
			
	uikitWindow = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
		
	if (window->flags & SDL_WINDOW_BORDERLESS) {
		/* hide status bar */
		[UIApplication sharedApplication].statusBarHidden = YES;
	}
		
	window->flags &= ~SDL_WINDOW_RESIZABLE;		/* window is NEVER resizeable */
	window->flags |= SDL_WINDOW_OPENGL;			/* window is always OpenGL */
	window->flags |= SDL_WINDOW_FULLSCREEN;		/* window is always fullscreen */
	window->flags |= SDL_WINDOW_SHOWN;			/* only one window on iPod touch, always shown */
	window->flags |= SDL_WINDOW_INPUT_FOCUS;	/* always has input focus */
	
	return 1;
	
}

void UIKit_DestroyWindow(_THIS, SDL_Window * window) {
	;
}



/* vi: set ts=4 sw=4 expandtab: */
