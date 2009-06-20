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
 * @brief Does a generic rwops test.
 *
 * RWops should have "Hello World!" in it already if write is disabled.
 *
 *    @param write Test writing also.
 *    @return 1 if an assert is failed.
 */
static int rwops_testGeneric( SDL_RWops *rw, int write )
{
   char buf[sizeof(hello_world)];
   int i;

   if (write) {
      /* Test write. */
      i = SDL_RWwrite( rw, hello_world, sizeof(hello_world)-1, 1 );
      if (SDL_ATassert( "Writing with SDL_RWwrite", i == 1 ))
         return 1;
   }

   /* Test seek. */
   i = SDL_RWseek( rw, 6, RW_SEEK_SET );
   if (SDL_ATassert( "Seeking with SDL_RWseek", i == 6 ))
      return 1;

   /* Test seek. */
   i = SDL_RWseek( rw, 0, RW_SEEK_SET );
   if (SDL_ATassert( "Seeking with SDL_RWseek", i == 0 ))
      return 1;

   /* Test read. */
   i = SDL_RWread( rw, buf, 1, sizeof(hello_world)-1 );
   if (i != sizeof(hello_world)-1)
      printf("%s\n", SDL_GetError());
   if (SDL_ATassert( "Reading with SDL_RWread", i == sizeof(hello_world)-1 ))
      return 1;
   if (SDL_ATassert( "Memory read does not match memory written",
            memcmp( buf, hello_world, sizeof(hello_world)-1 ) == 0 ))
      return 1;

   return 0;
}


/**
 * @brief Tests opening from memory.
 */
static void rwops_testMem (void)
{
   char mem[sizeof(hello_world)];
   SDL_RWops *rw;

   /* Begin testcase. */
   SDL_ATbegin( "SDL_RWFromMem" );

   /* Open. */
   rw = SDL_RWFromMem( mem, sizeof(mem) );
   if (SDL_ATassert( "Opening memory with SDL_RWFromMem", rw != NULL ))
      return;

   /* Run generic tests. */
   if (rwops_testGeneric( rw, 1 ))
      return;

   /* Close. */
   SDL_FreeRW( rw );

   /* End testcase. */
   SDL_ATend();
}


static const char const_mem[] = "Hello World!";
/**
 * @brief Tests opening from memory.
 */
static void rwops_testConstMem (void)
{
   SDL_RWops *rw;

   /* Begin testcase. */
   SDL_ATbegin( "SDL_RWFromConstMem" );

   /* Open. */
   rw = SDL_RWFromConstMem( const_mem, sizeof(const_mem) );
   if (SDL_ATassert( "Opening memory with SDL_RWFromConstMem", rw != NULL ))
      return;

   /* Run generic tests. */
   if (rwops_testGeneric( rw, 0 ))
      return;

   /* Close. */
   SDL_FreeRW( rw );

   /* End testcase. */
   SDL_ATend();
}


/**
 * @brief Tests opening from memory.
 */
static void rwops_testFile (void)
{
   SDL_RWops *rw;
   int i;

   /* Begin testcase. */
   SDL_ATbegin( "SDL_RWFromFile" );

   /* Open. */
   rw = SDL_RWFromFile( "rwops/read", "r" );
   if (SDL_ATassert( "Opening memory with SDL_RWFromFile", rw != NULL ))
      return;

   /* Test writing. */
   i = SDL_RWwrite( rw, hello_world, sizeof(hello_world), 1 );
   if (SDL_ATassert( "Writing with SDL_RWwrite", i == 0 ))
      return;

   /* Run generic tests. */
   if (rwops_testGeneric( rw, 0 ))
      return;

   /* Close. */
   SDL_FreeRW( rw );

   /* End testcase. */
   SDL_ATend();
}


/**
 * @brief Tests opening from memory.
 */
static void rwops_testFP (void)
{
#ifdef HAVE_STDIO_H
   FILE *fp;
   SDL_RWops *rw;
   int i;

   /* Begin testcase. */
   SDL_ATbegin( "SDL_RWFromFP" );

   /* Open. */
   fp = fopen( "rwops/write", "w+" );
   if (fp == NULL) {
      SDL_ATprint("Failed to open file rwops/write");
      SDL_ATend();
      return;
   }
   rw = SDL_RWFromFP( fp, 1 );
   if (SDL_ATassert( "Opening memory with SDL_RWFromFP", rw != NULL ))
      return;

   /* Run generic tests. */
   if (rwops_testGeneric( rw, 1 ))
      return;

   /* Close. */
   SDL_FreeRW( rw );

   /* End testcase. */
   SDL_ATend();
#endif /* HAVE_STDIO_H */
}


/**
 * @brief Entry point.
 */
int main( int argc, const char *argv[] )
{
   SDL_ATinit( "SDL_RWops" );

   rwops_testMem();
   rwops_testConstMem();
   rwops_testFile();
   rwops_testFP();

   return SDL_ATfinish(1);
}
