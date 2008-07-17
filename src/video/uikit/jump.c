/*
 *  jump.c
 *  iPodSDL
 *
 *  Created by Holmes Futrell on 7/2/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "jump.h"

jmp_buf env;
jmp_buf *jump_env() { return &env; }