/*
 *  jump.h
 *  iPodSDL
 *
 *  Created by Holmes Futrell on 7/2/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "setjmp.h"

extern jmp_buf* jump_env();
extern int canJump;