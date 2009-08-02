/*
 * SDL test suite framework code.
 *
 * Written by Edgar Simo "bobbens"
 *
 * Released under Public Domain.
 */


#include "platform/platform.h"
#include "rwops/rwops.h"
#include "surface/surface.h"
#include "render/render.h"


int main( int argc, char *argv[] )
{
   (void) argc;
   (void) argv;

   test_platform();
   test_rwops();
   test_surface();
   test_render();

   return 0;
}

