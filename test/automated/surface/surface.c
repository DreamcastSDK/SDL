/**
 * Automated SDL_Surface test.
 *
 * Written by Edgar Simo "bobbens"
 *
 * Released under Public Domain.
 */


#include "SDL.h"
#include "SDL_at.h"


#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
#  define RMASK   0xff000000 /**< Red bit mask. */
#  define GMASK   0x00ff0000 /**< Green bit mask. */
#  define BMASK   0x0000ff00 /**< Blue bit mask. */
#  define AMASK   0x000000ff /**< Alpha bit mask. */
#else
#  define RMASK   0x000000ff /**< Red bit mask. */
#  define GMASK   0x0000ff00 /**< Green bit mask. */
#  define BMASK   0x00ff0000 /**< Blue bit mask. */
#  define AMASK   0xff000000 /**< Alpha bit mask. */
#endif


typedef struct SurfaceImage_s {
   unsigned int  width;
   unsigned int  height;
   unsigned int  bytes_per_pixel; /* 3:RGB, 4:RGBA */ 
   const unsigned char pixel_data[];
} SurfaceImage_t;


/*
 * Pull in images for testcases.
 */
#include "primitives.c"


/**
 * @brief Compares a surface and a surface image for equality.
 *
 *    @param sur Surface to compare.
 *    @param img Image to compare against.
 *    @return 0 if they are the same, -1 on error and positive if different.
 */
static int surface_compare( SDL_Surface *sur, const SurfaceImage_t *img )
{
   int ret;
   int i,j;
   Uint32 pix;
   int bpp;
   Uint8 *p, *pd;

   /* Make sure size is the same. */
   if ((sur->w != img->width) || (sur->h != img->height))
      return -1;

   SDL_LockSurface( sur );

   ret = 0;
   bpp = sur->format->BytesPerPixel;

   /* Compare image - should be same format. */
   for (j=0; j<sur->h; j++) {
      for (i=0; i<sur->w; i++) {
         p  = (Uint8 *)sur->pixels + j * sur->pitch + i * bpp;
         pd = (Uint8 *)img->pixel_data + (j*img->width + i) * img->bytes_per_pixel;
         switch (bpp) {
            case 1:
               /* Paletted not supported atm. */
               ret += 1;
               break;

            case 2:
               /* 16 BPP not supported atm. */
               ret += 1;
               break;

            case 3:
               /* 24 BPP not supported atm. */
               ret += 1;
               break;

            case 4:
               ret += !( (p[0] == pd[0]) &&
                         (p[1] == pd[1]) &&
                         (p[2] == pd[2]) );
               break;
         }
      }
   }
  
   SDL_UnlockSurface( sur );

   return ret;
}


/**
 * @brief Tests the SDL primitives for rendering.
 */
static void surface_testPrimitives (void)
{
   int ret;
   int x, y;
   SDL_Rect rect;
   SDL_Surface *testsur;

   SDL_ATbegin( "Primitives Test" );

   /* Create the surface. */
   testsur = SDL_CreateRGBSurface( 0, 80, 60, 32, 
         RMASK, GMASK, BMASK, AMASK );
   if (SDL_ATassert( "SDL_CreateRGBSurface", testsur != NULL))
      return;

   /* Draw a rectangle. */
   rect.x = 40;
   rect.y = 0;
   rect.w = 40;
   rect.h = 80;
   ret = SDL_FillRect( testsur, &rect,
         SDL_MapRGB( testsur->format, 13, 73, 200 ) );
   if (SDL_ATassert( "SDL_FillRect", ret == 0))
      return;

   /* Draw a rectangle. */
   rect.x = 10;
   rect.y = 10;
   rect.w = 60;
   rect.h = 40;
   ret = SDL_FillRect( testsur, &rect,
         SDL_MapRGB( testsur->format, 200, 0, 100 ) );
   if (SDL_ATassert( "SDL_FillRect", ret == 0))
      return;

   /* Draw some points like so:
    * X.X.X.X..
    * .X.X.X.X.
    * X.X.X.X.. */
   for (y=0; y<3; y++) {
      x = y % 2;
      for (; x<80; x+=2)
         ret = SDL_DrawPoint( testsur, x, y,
               SDL_MapRGB( testsur->format, x*y, x*y/2, x*y/3 ) );
      if (SDL_ATassert( "SDL_DrawPoint", ret == 0))
         return;
   }

   /* Draw some lines. */
   ret = SDL_DrawLine( testsur, 0, 30, 80, 30,
         SDL_MapRGB( testsur->format, 0, 255, 0 ) );
   if (SDL_ATassert( "SDL_DrawLine", ret == 0))
      return;
   ret = SDL_DrawLine( testsur, 40, 30, 40, 60,
         SDL_MapRGB( testsur->format, 55, 55, 5 ) );
   if (SDL_ATassert( "SDL_DrawLine", ret == 0))
      return;
   ret = SDL_DrawLine( testsur, 0, 60, 80, 0,
         SDL_MapRGB( testsur->format, 5, 105, 105 ) );
   if (SDL_ATassert( "SDL_DrawLine", ret == 0))
      return;

   /* See if it's the same. */
   if (SDL_ATassert( "Primitives output not the same.",
            surface_compare( testsur, &img_primitives )==0 ))
      return;

   /* Clean up. */
   SDL_FreeSurface( testsur );

   SDL_ATend();
}


/**
 * @brief Entry point.
 */
int main( int argc, const char *argv[] )
{
   SDL_ATinit( "SDL_Surface" );

   /* Initializes the SDL subsystems. */
   SDL_Init(0);

   surface_testPrimitives();
   /*surface_testPrimitivesAlpha();*/

   /* Exit SDL. */
   SDL_Quit();

   return SDL_ATfinish(1);
}
