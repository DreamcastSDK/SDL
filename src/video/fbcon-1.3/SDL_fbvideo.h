/*
	SDL - Simple DirectMedia Layer
	Copyright (C) 1997-2009 Sam Lantinga

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

#ifndef _SDL_fbvideo_h
#define _SDL_fbvideo_h

#include <sys/types.h>
#include <termios.h>
#include <linux/fb.h>

#include "SDL_mutex.h"
#include "../SDL_sysvideo.h"

/* Debugging
 * 0: No debug messages
 * 1: Video debug messages
 */
#define DEBUG_LEVEL 1

#ifdef DEBUG_LEVEL
#define deprintf( level, fmt, args... ) \
	do \
{ \
	if ( (unsigned)(level) <= DEBUG_LEVEL ) \
	{ \
		fprintf( stdout, fmt, ##args ); \
		fflush( stdout ); \
	} \
} while ( 0 )
#else
	#define deprintf( level, fmt, args... )
#endif


/* Private display data */
typedef struct SDL_VideoData
{
    /* Framebuffer device descriptor */
    int console_fd;
    /* hardware surface lock mutex */
    SDL_mutex *hw_lock;
    /* Framebuffer */
    char *mapped_mem;
    int mapped_memlen;
    int mapped_offset;
} SDL_VideoData;

#endif /* _SDL_fbvideo_h */

/* vi: set ts=4 sw=4 expandtab: */
