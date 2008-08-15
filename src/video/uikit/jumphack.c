/*
 *  jumphack.c
 *  SDLiPhoneOS
 *
 */

#include "jump.h"

jmp_buf env;
jmp_buf *jump_env() { return &env; }