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
#include "SDL_config.h"

#include "spulibs/spu_common.h"

#include <libspe2.h>

#ifndef _SDL_ps3spe_h
#define _SDL_ps3spe_h

/* SPU thread data */
typedef struct spu_data {
    spe_context_ptr_t ctx;
    spe_program_handle_t program;
    pthread_t thread;
    char * program_name;
    unsigned int booted;
    unsigned int keepalive;
    unsigned int entry;
    int error_code;
    void * argp;
} spu_data_t;

/* SPU specific functions */
int SPE_Start(spu_data_t * spe_data);
int SPE_Stop(spu_data_t * spe_data);
int SPE_Boot(spu_data_t * spe_data);
int SPE_Shutdown(spu_data_t * spe_data);
int SPE_SendMsg(spu_data_t * spe_data, unsigned int msg);
int SPE_WaitForMsg(spu_data_t * spe_data, unsigned int msg);
void SPE_RunContext(void *thread_argp);

#endif /* _SDL_ps3spe_h */

/* vi: set ts=4 sw=4 expandtab: */
