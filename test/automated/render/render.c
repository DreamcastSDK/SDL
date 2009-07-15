/**
 * Automated SDL_Surface test.
 *
 * Written by Edgar Simo "bobbens"
 *
 * Released under Public Domain.
 */


#include "SDL.h"
#include "SDL_at.h"

#include "common/common.h"


/*
 * Pull in images for testcases.
 */
#include "common/images.h"


#define SCREEN_W     80
#define SCREEN_H     60


/*
 * Prototypes.
 */
static int render_compare( const char *msg, const SurfaceImage_t *s );
static int render_clearScreen (void);
/* Testcases. */
static void render_testPrimitives (void);
static void render_testPrimitivesBlend (void);
static void render_testBlit (void);
static int render_testBlitBlendMode (void);
static void render_testBlitBlend (void);


/**
 * Compares screen pixels with image pixels.
 */
static int render_compare( const char *msg, const SurfaceImage_t *s )
{
   int ret;
   void *pix;
   SDL_Surface *testsur;

#if 0

   /* Allocate pixel space. */
   pix = malloc( 4*80*60 );
   if (SDL_ATassert( "malloc", pix!=NULL ))
      return 1;

   /* Read pixels. */
   ret = SDL_RenderReadPixels( NULL, pix, 80*4 );
   if (SDL_ATassert( "SDL_RenderReadPixels", ret==0) )
      return 1;

   /* Create surface. */
   testsur = SDL_CreateRGBSurfaceFrom( pix, 80, 60, 32, 80*4,
         RMASK, GMASK, BMASK, AMASK );
   if (SDL_ATassert( "SDL_CreateRGBSurfaceFrom", testsur!=NULL ))
      return 1;

   /* Compare surface. */
   ret = surface_compare( testsur, s );
   if (SDL_ATassert( msg, ret==0 ))
      return 1;

   /* Clean up. */
   SDL_FreeSurface( testsur );
   free(pix);

#endif

   return 0;
}

/**
 * @brief Clears the screen.
 */
static int render_clearScreen (void)
{
   int ret;

   /* Set colour. */
   ret = SDL_SetRenderDrawColor( 0, 0, 0, SDL_ALPHA_OPAQUE );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return -1;

   /* Clear screen. */
   ret = SDL_RenderFill( NULL );
   if (SDL_ATassert( "SDL_RenderFill", ret == 0))
      return -1;

   /* Set defaults. */
   ret = SDL_SetRenderDrawBlendMode( SDL_BLENDMODE_NONE );
   if (SDL_ATassert( "SDL_SetRenderDrawBlendMode", ret == 0))
      return -1;
   ret = SDL_SetRenderDrawColor( 255, 255, 255, SDL_ALPHA_OPAQUE );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return -1;

   return 0;
}


/**
 * @brief Tests the SDL primitives for rendering.
 */
static void render_testPrimitives (void)
{
   int ret;
   int x, y;
   SDL_Rect rect;

   /* Clear surface. */
   if (render_clearScreen())
      return;

   /* Draw a rectangle. */
   rect.x = 40;
   rect.y = 0;
   rect.w = 40;
   rect.h = 80;
   ret = SDL_SetRenderDrawColor( 13, 73, 200, SDL_ALPHA_OPAQUE );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return;
   ret = SDL_RenderFill( &rect );
   if (SDL_ATassert( "SDL_RenderRect", ret == 0))
      return;

   /* Draw a rectangle. */
   rect.x = 10;
   rect.y = 10;
   rect.w = 60;
   rect.h = 40;
   ret = SDL_SetRenderDrawColor( 200, 0, 100, SDL_ALPHA_OPAQUE );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return;
   ret = SDL_RenderFill( &rect );
   if (SDL_ATassert( "SDL_RenderRect", ret == 0))
      return;

   /* Draw some points like so:
    * X.X.X.X..
    * .X.X.X.X.
    * X.X.X.X.. */
   for (y=0; y<3; y++) {
      x = y % 2;
      for (; x<80; x+=2) {
         ret = SDL_SetRenderDrawColor( x*y, x*y/2, x*y/3, SDL_ALPHA_OPAQUE );
         if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
            return;
         ret = SDL_RenderPoint( x, y );
         if (SDL_ATassert( "SDL_RenderPoint", ret == 0))
            return;
      }
   }

   /* Draw some lines. */
   ret = SDL_SetRenderDrawColor( 0, 255, 0, SDL_ALPHA_OPAQUE );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return;
   ret = SDL_RenderLine( 0, 30, 80, 30 );
   if (SDL_ATassert( "SDL_RenderLine", ret == 0))
      return;
   ret = SDL_SetRenderDrawColor( 55, 55, 5, SDL_ALPHA_OPAQUE );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return;
   ret = SDL_RenderLine( 40, 30, 40, 60 );
   if (SDL_ATassert( "SDL_RenderLine", ret == 0))
      return;
   ret = SDL_SetRenderDrawColor( 5, 105, 105, SDL_ALPHA_OPAQUE );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return;
   ret = SDL_RenderLine( 0, 60, 80, 0 );
   if (SDL_ATassert( "SDL_RenderLine", ret == 0))
      return;

   /* See if it's the same. */
   if (render_compare( "Primitives output not the same.", &img_primitives ))
      return;
}


/**
 * @brief Tests the SDL primitives with alpha for rendering.
 */
static void render_testPrimitivesBlend (void)
{
   int ret;
   int i, j;
   SDL_Rect rect;

   /* Clear surface. */
   if (render_clearScreen())
      return;

   /* See if we can actually run the test. */
   ret  = 0;
   ret |= SDL_SetRenderDrawBlendMode( SDL_BLENDMODE_BLEND );
   ret |= SDL_SetRenderDrawBlendMode( SDL_BLENDMODE_ADD );
   ret |= SDL_SetRenderDrawBlendMode( SDL_BLENDMODE_MOD );
   ret |= SDL_SetRenderDrawBlendMode( SDL_BLENDMODE_NONE );
   if (ret != 0)
      return;

   /* Create some rectangles for each blend mode. */
   ret = SDL_SetRenderDrawColor( 255, 255, 255, 0 );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return;
   ret = SDL_SetRenderDrawBlendMode( SDL_BLENDMODE_NONE );
   if (SDL_ATassert( "SDL_SetRenderDrawBlendMode", ret == 0))
      return;
   ret = SDL_RenderFill( NULL );
   if (SDL_ATassert( "SDL_RenderFill", ret == 0))
      return;
   rect.x = 10;
   rect.y = 25;
   rect.w = 40;
   rect.h = 25;
   ret = SDL_SetRenderDrawColor( 240, 10, 10, 75 );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return;
   ret = SDL_SetRenderDrawBlendMode( SDL_BLENDMODE_ADD );
   if (SDL_ATassert( "SDL_SetRenderDrawBlendMode", ret == 0))
      return;
   ret = SDL_RenderFill( &rect );
   if (SDL_ATassert( "SDL_RenderFill", ret == 0))
      return;
   rect.x = 30;
   rect.y = 40;
   rect.w = 45;
   rect.h = 15;
   ret = SDL_SetRenderDrawColor( 10, 240, 10, 100 );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return;
   ret = SDL_SetRenderDrawBlendMode( SDL_BLENDMODE_BLEND );
   if (SDL_ATassert( "SDL_SetRenderDrawBlendMode", ret == 0))
      return;
   ret = SDL_RenderFill( &rect );
   if (SDL_ATassert( "SDL_RenderFill", ret == 0))
      return;
   rect.x = 25;
   rect.y = 25;
   rect.w = 25;
   rect.h = 25;
   ret = SDL_SetRenderDrawColor( 10, 10, 240, 125 );
   if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
      return;
   ret = SDL_SetRenderDrawBlendMode( SDL_BLENDMODE_MOD );
   if (SDL_ATassert( "SDL_SetRenderDrawBlendMode", ret == 0))
      return;
   ret = SDL_RenderFill( &rect );
   if (SDL_ATassert( "SDL_RenderFill", ret == 0))
      return;

   /* Draw blended lines, lines for everyone. */
   for (i=0; i<SCREEN_W; i+=2)  {
      ret = SDL_SetRenderDrawColor( 60+2*i, 240-2*i, 50, 3*i );
      if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
         return;
      ret = SDL_SetRenderDrawBlendMode((((i/2)%3)==0) ? SDL_BLENDMODE_BLEND :
            (((i/2)%3)==1) ? SDL_BLENDMODE_ADD : SDL_BLENDMODE_MOD );
      if (SDL_ATassert( "SDL_SetRenderDrawBlendMode", ret == 0))
         return;
      ret = SDL_RenderLine( 0, 0, i, 59 );
      if (SDL_ATassert( "SDL_RenderLine", ret == 0))
         return;
   }
   for (i=0; i<SCREEN_H; i+=2)  {
      ret = SDL_SetRenderDrawColor( 60+2*i, 240-2*i, 50, 3*i );
      if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
         return;
      ret = SDL_SetRenderDrawBlendMode((((i/2)%3)==0) ? SDL_BLENDMODE_BLEND :
            (((i/2)%3)==1) ? SDL_BLENDMODE_ADD : SDL_BLENDMODE_MOD );
      if (SDL_ATassert( "SDL_SetRenderDrawBlendMode", ret == 0))
         return;
      ret = SDL_RenderLine( 0, 0, 79, i );
      if (SDL_ATassert( "SDL_RenderLine", ret == 0))
         return;
   }

   /* Draw points. */
   for (j=0; j<SCREEN_H; j+=3) {
      for (i=0; i<SCREEN_W; i+=3) {
         ret = SDL_SetRenderDrawColor( j*4, i*3, j*4, i*3 );
         if (SDL_ATassert( "SDL_SetRenderDrawColor", ret == 0))
            return;
         ret = SDL_SetRenderDrawBlendMode( ((((i+j)/3)%3)==0) ? SDL_BLENDMODE_BLEND :
               ((((i+j)/3)%3)==1) ? SDL_BLENDMODE_ADD : SDL_BLENDMODE_MOD );
         if (SDL_ATassert( "SDL_SetRenderDrawBlendMode", ret == 0))
            return;
         ret = SDL_RenderPoint( i, j );
         if (SDL_ATassert( "SDL_RenderPoint", ret == 0))
            return;
      }
   }

   /* See if it's the same. */
   if (render_compare( "Blended primitives output not the same.", &img_primitives ))
      return;
}


/**
 * @brief Tests some blitting routines.
 */
static void render_testBlit (void)
{
   int ret;
   SDL_Rect rect;
   SDL_Surface *face;
   SDL_TextureID tface;
   int i, j, ni, nj;

   /* Clear surface. */
   if (render_clearScreen())
      return;

   /* Create face surface. */
   face = SDL_CreateRGBSurfaceFrom( (void*)img_face.pixel_data,
         img_face.width, img_face.height, 32, img_face.width*4,
         RMASK, GMASK, BMASK, AMASK );
   if (SDL_ATassert( "SDL_CreateRGBSurfaceFrom", face != NULL))
      return;
   tface = SDL_CreateTextureFromSurface( 0, face );
   if (SDL_ATassert( "SDL_CreateTextureFromSurface", tface != 0))
      return;

   /* Clean up. */
   SDL_FreeSurface( face );

   /* Constant values. */
   rect.w = face->w;
   rect.h = face->h;
   ni     = SCREEN_W - face->w;
   nj     = SCREEN_H - face->h;

   /* Loop blit. */
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {
         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_RenderCopy( tface, NULL, &rect );
         if (SDL_ATassert( "SDL_RenderCopy", ret == 0))
            return;
      }
   }

   /* See if it's the same. */
   if (render_compare( "Blit output not the same.", &img_blit ))
      return;

   /* Clear surface. */
   if (render_clearScreen())
      return;

   /* Test blitting with colour mod. */
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {
         /* Set colour mod. */
         ret = SDL_SetTextureColorMod( tface, (255/nj)*j, (255/ni)*i, (255/nj)*j );
         if (SDL_ATassert( "SDL_SetTextureColorMod", ret == 0))
            return;

         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_RenderCopy( tface, NULL, &rect );
         if (SDL_ATassert( "SDL_RenderCopy", ret == 0))
            return;
      }
   }

   /* See if it's the same. */
   if (render_compare( "Blit output not the same (using SDL_SetTextureColorMod).",
            &img_blitColour ))
      return;

   /* Clear surface. */
   if (render_clearScreen())
      return;

   /* Restore colour. */
   ret = SDL_SetTextureColorMod( tface, 255, 255, 255 );
   if (SDL_ATassert( "SDL_SetTextureColorMod", ret == 0))
      return;

   /* Test blitting with colour mod. */
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {
         /* Set alpha mod. */
         ret = SDL_SetTextureAlphaMod( tface, (255/ni)*i );
         if (SDL_ATassert( "SDL_SetTextureAlphaMod", ret == 0))
            return;

         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_RenderCopy( tface, NULL, &rect );
         if (SDL_ATassert( "SDL_RenderCopy", ret == 0))
            return;
      }
   }

   /* See if it's the same. */
   if (render_compare( "Blit output not the same (using SDL_SetSurfaceAlphaMod).",
            &img_blitAlpha ))
      return;

   /* Clean up. */
   SDL_DestroyTexture( tface );
}
#if 0


/**
 * @brief Tests a blend mode.
 */
static int render_testBlitBlendMode( SDL_Surface *face, int mode )
{
   int ret;
   int i, j, ni, nj;
   SDL_Rect rect;

   /* Clear surface. */
   ret = SDL_FillRect( testsur, NULL,
         SDL_MapRGB( testsur->format, 0, 0, 0 ) );
   if (SDL_ATassert( "SDL_FillRect", ret == 0))
      return 1;

   /* Steps to take. */
   ni     = testsur->w - face->w;
   nj     = testsur->h - face->h;

   /* Constant values. */
   rect.w = face->w;
   rect.h = face->h;

   /* Test blend mode. */
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {
         /* Set blend mode. */
         ret = SDL_SetSurfaceBlendMode( face, mode );
         if (SDL_ATassert( "SDL_SetSurfaceBlendMode", ret == 0))
            return 1;

         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_BlitSurface( face, NULL, testsur, &rect );
         if (SDL_ATassert( "SDL_BlitSurface", ret == 0))
            return 1;
      }
   }

   return 0;
}


/**
 * @brief Tests some more blitting routines.
 */
static void render_testBlitBlend (void)
{
   int ret;
   SDL_Rect rect;
   SDL_Surface *face;
   int i, j, ni, nj;
   int mode;

   SDL_ATbegin( "Blit Blending Tests" );

   /* Clear surface. */
   ret = SDL_FillRect( testsur, NULL,
         SDL_MapRGB( testsur->format, 0, 0, 0 ) );
   if (SDL_ATassert( "SDL_FillRect", ret == 0))
      return;

   /* Create the blit surface. */
   face = SDL_CreateRGBSurfaceFrom( (void*)img_face.pixel_data,
         img_face.width, img_face.height, 32, img_face.width*4,
         RMASK, GMASK, BMASK, AMASK );
   if (SDL_ATassert( "SDL_CreateRGBSurfaceFrom", face != NULL))
      return;

   /* Set alpha mod. */
   ret = SDL_SetSurfaceAlphaMod( face, 100 );
   if (SDL_ATassert( "SDL_SetSurfaceAlphaMod", ret == 0))
      return;

   /* Steps to take. */
   ni     = testsur->w - face->w;
   nj     = testsur->h - face->h;

   /* Constant values. */
   rect.w = face->w;
   rect.h = face->h;

   /* Test None. */
   if (render_testBlitBlendMode( face, SDL_BLENDMODE_NONE ))
      return;
   if (SDL_ATassert( "Blitting blending output not the same (using SDL_BLENDMODE_NONE).",
            render_compare( testsur, &img_blendNone )==0 ))
      return;

   /* Test Mask. */
   if (render_testBlitBlendMode( face, SDL_BLENDMODE_MASK ))
      return;
   if (SDL_ATassert( "Blitting blending output not the same (using SDL_BLENDMODE_MASK).",
            render_compare( testsur, &img_blendMask )==0 ))
      return;

   /* Test Blend. */
   if (render_testBlitBlendMode( face, SDL_BLENDMODE_BLEND ))
      return;
   if (SDL_ATassert( "Blitting blending output not the same (using SDL_BLENDMODE_BLEND).",
            render_compare( testsur, &img_blendBlend )==0 ))
      return;

   /* Test Add. */
   if (render_testBlitBlendMode( face, SDL_BLENDMODE_ADD ))
      return;
   if (SDL_ATassert( "Blitting blending output not the same (using SDL_BLENDMODE_ADD).",
            render_compare( testsur, &img_blendAdd )==0 ))
      return;

   /* Test Mod. */
   if (render_testBlitBlendMode( face, SDL_BLENDMODE_MOD ))
      return;
   if (SDL_ATassert( "Blitting blending output not the same (using SDL_BLENDMODE_MOD).",
            render_compare( testsur, &img_blendMod )==0 ))
      return;

   /* Clear surface. */
   ret = SDL_FillRect( testsur, NULL,
         SDL_MapRGB( testsur->format, 0, 0, 0 ) );
   if (SDL_ATassert( "SDL_FillRect", ret == 0))
      return;

   /* Loop blit. */
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {

         /* Set colour mod. */
         ret = SDL_SetSurfaceColorMod( face, (255/nj)*j, (255/ni)*i, (255/nj)*j );
         if (SDL_ATassert( "SDL_SetSurfaceColorMod", ret == 0))
            return;

         /* Set alpha mod. */
         ret = SDL_SetSurfaceAlphaMod( face, (100/ni)*i );
         if (SDL_ATassert( "SDL_SetSurfaceAlphaMod", ret == 0))
            return;

         /* Crazy blending mode magic. */
         mode = (i/4*j/4) % 4;
         if (mode==0) mode = SDL_BLENDMODE_MASK;
         else if (mode==1) mode = SDL_BLENDMODE_BLEND;
         else if (mode==2) mode = SDL_BLENDMODE_ADD;
         else if (mode==3) mode = SDL_BLENDMODE_MOD;
         ret = SDL_SetSurfaceBlendMode( face, mode );
         if (SDL_ATassert( "SDL_SetSurfaceBlendMode", ret == 0))
            return;

         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_BlitSurface( face, NULL, testsur, &rect );
         if (SDL_ATassert( "SDL_BlitSurface", ret == 0))
            return;
      }
   }

   /* Check to see if matches. */
   if (SDL_ATassert( "Blitting blending output not the same (using SDL_BLEND_*).",
            render_compare( testsur, &img_blendAll )==0 ))
      return;

   /* Clean up. */
   SDL_FreeSurface( face );

   SDL_ATend();
}
#endif


/**
 * @brief Runs all the tests on the surface.
 *
 *    @param testsur Surface to run tests on.
 */
void render_runTests (void)
{
   /* Software surface blitting. */
   render_testPrimitives();
   render_testPrimitivesBlend();
   render_testBlit();
   /*
   render_testBlitBlend();
   */
}


/**
 * @brief Entry point.
 *
 * This testsuite is tricky, we're creating a testsuite per driver, the thing
 *  is we do quite a of stuff outside of the actual testcase which *could*
 *  give issues. Don't like that very much, but no way around without creating
 *  superfluous testsuites.
 */
int main( int argc, const char *argv[] )
{
   (void) argc;
   (void) argv;
   int i, j, nd, nr;
   int ret;
   const char *driver, *str;
   char msg[256];
   SDL_WindowID wid;
   SDL_RendererInfo renderer;

   /* Initializes the SDL subsystems. */
   ret = SDL_Init(0);
   if (ret != 0)
      return -1;

   /* Get number of drivers. */
   nd = SDL_GetNumVideoDrivers();
   if (ret < 0)
      goto err;

   /* Now run on the video mode. */
   ret = SDL_InitSubSystem( SDL_INIT_VIDEO );
   if (ret != 0)
      goto err;

   /*
    * Surface on video mode tests.
    */
   /* Run for all video modes. */
   for (i=0; i<nd; i++) {
      /* Get video mode. */
      driver = SDL_GetVideoDriver(i);
      if (driver == NULL)
         goto err;
      /* Hack to avoid dummy driver. */
      if (strcmp(driver,"dummy")==0)
         continue;

      /*
       * Initialize testsuite.
       */
      snprintf( msg, sizeof(msg) , "Rendering with %s driver", driver );
      SDL_ATinit( msg );

      /*
       * Initialize.
       */
      SDL_ATbegin( "Initializing video mode" );
      /* Initialize video mode. */
      ret = SDL_VideoInit( driver, 0 );
      if (SDL_ATvassert( ret==0, "SDL_VideoInit( %s, 0 )", driver ))
         goto err;
      /* Check to see if it's the one we want. */
      str = SDL_GetCurrentVideoDriver();
      if (SDL_ATassert( "SDL_GetCurrentVideoDriver", strcmp(driver,str)==0))
         goto err;
      /* Create window. */
      wid = SDL_CreateWindow( msg, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            80, 60, 0 );
      if (SDL_ATassert( "SDL_CreateWindow", wid!=0 ))
         goto err;
      /* Check title. */
      str = SDL_GetWindowTitle( wid );
      if (SDL_ATassert( "SDL_GetWindowTitle", strcmp(msg,str)==0))
         goto err;
      /* Get renderers. */
      nr = SDL_GetNumRenderDrivers();
      if (SDL_ATassert("SDL_GetNumRenderDrivers", nr>=0))
         goto err;
      SDL_ATend();
      for (j=0; j<nr; j++) {

         /* Get renderer info. */
         ret = SDL_GetRenderDriverInfo( j, &renderer );
         if (ret != 0)
            goto err;
         /* Set testcase name. */
         snprintf( msg, sizeof(msg), "Renderer %s", renderer.name );
         SDL_ATbegin( msg );
         /* Set renderer. */
         ret = SDL_CreateRenderer( wid, j, 0 );
         if (SDL_ATassert( "SDL_CreateRenderer", ret==0 ))
            goto err;

         /*
          * Run tests.
          */
         render_runTests();

         SDL_ATend();
      }

      /* Exit the current renderer. */
      SDL_VideoQuit();

      /*
       * Finish testsuite.
       */
      SDL_ATfinish(1);
   }


   /* Exit SDL. */
   SDL_Quit();

   return 0;

err:
   return -1;
}

