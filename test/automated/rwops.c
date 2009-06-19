/**
 * Automated SDL_RWops test.
 *
 * Written by Edgar Simo "bobbens"
 *
 * Released under Public Domain.
 */


#include "SDL.h"
#include "SDL_at.h"


static const char hello_world[] = "Hello World!";


/**
 * @brief Tests opening from memory.
 */
static void rwops_testMem (void)
{
   SDL_RWops *rw;
   char mem[sizeof(hello_world)], buf[sizeof(hello_world)];
   int i;

   /* Begin testcase. */
   SDL_ATbegin( "SDL_RWFromMem" );

   /* Open. */
   rw = SDL_RWFromMem( mem, sizeof(mem) );
   if (SDL_ATassert( "Opening memory with SDL_RWFromMem", rw != NULL ))
      return;

   /* Test write. */
   i = SDL_RWwrite( rw, hello_world, sizeof(hello_world), 1 );
   if (SDL_ATassert( "Writing with SDL_RWwrite", i == 1 ))
      return;

   /* Test seek. */
   i = SDL_RWseek( rw, 0, RW_SEEK_SET );
   if (SDL_ATassert( "Seeking with SDL_RWseek", i == 0 ))
      return;

   /* Test read. */
   i = SDL_RWread( rw, buf, sizeof(hello_world), 1 );
   if (SDL_ATassert( "Reading with SDL_RWread", i == 1 ))
      return;
   if (SDL_ATassert( "Memory read does not match memory written",
            memcmp( buf, hello_world, sizeof(hello_world) ) == 0 ))
      return;

   /* Close. */
   SDL_FreeRW( rw );

   /* End testcase. */
   SDL_ATend();
}


/**
 * @brief Entry point.
 */
int main( int argc, const char *argv[] )
{
   SDL_ATinit( "SDL_RWops" );

   rwops_testMem();

   return SDL_ATfinish(1);
}
