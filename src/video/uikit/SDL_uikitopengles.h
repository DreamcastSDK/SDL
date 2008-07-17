/*
 *  SDL_uikitopengles.h
 *  iPodSDL
 *
 *  Created by Holmes Futrell on 5/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "SDL_config.h"

#ifndef _SDL_uikitopengles
#define _SDL_uikitopengles

#include "SDL_uikitvideo.h"

extern int UIKit_GL_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context);
extern void UIKit_GL_SwapWindow(_THIS, SDL_Window * window);
extern SDL_GLContext UIKit_GL_CreateContext(_THIS, SDL_Window * window);
extern void UIKit_GL_DeleteContext(_THIS, SDL_GLContext context);
extern void *UIKit_GL_GetProcAddress(_THIS, const char *proc);
extern int UIKit_GL_LoadLibrary(_THIS, const char *path);
#endif