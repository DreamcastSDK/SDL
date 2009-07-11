/*
 * Common code for automated test suite.
 *
 * Written by Edgar Simo "bobbens"
 *
 * Released under Public Domain.
 */


#include "SDL_at.h"

#include <stdio.h>
#include <stdarg.h>


/*
 * Internal usage SDL_AT variables.
 */
static const char *at_suite_msg = NULL; /**< Testsuite message. */
static const char *at_test_msg = NULL; /**< Testcase message. */
static int at_success = 0; /**< Number of successful testcases. */
static int at_failure = 0; /**< Number of failed testcases. */


/**
 * @brief Cleans up the automated testsuite state.
 */
static void SDL_ATcleanup (void)
{
   at_suite_msg = NULL;
   at_test_msg = NULL;
   at_success = 0;
   at_failure = 0;
}


/**
 * @brief Begin testsuite.
 */
void SDL_ATinit( const char *suite )
{
   /* Do not open twice. */
   if (at_suite_msg) {
      SDL_ATprint( "AT suite '%s' not closed before opening suite '%s'\n",
            at_suite_msg, suite );
   }
   /* Must have a name. */
   if (suite == NULL) {
      SDL_ATprint( "AT testsuite does not have a name.\n");
   }
   SDL_ATcleanup();
   at_suite_msg = suite;
}


/**
 * @brief Finish testsuite.
 */
int SDL_ATfinish( int verbose )
{
   int failed;

   /* Make sure initialized. */
   if (at_suite_msg == NULL) {
      SDL_ATprint("Ended testcase without initializing.\n");
      return 1;
   }

   /* Finished without closing testcase. */
   if (at_test_msg) {
      SDL_ATprint( "AT suite '%s' finished without closing testcase '%s'\n",
            at_suite_msg, at_test_msg );
   }

   /* Display message if verbose on failed. */
   failed = at_failure;
   if (verbose) {
      if (at_failure > 0) {
         SDL_ATprint( "%s : Failed %d out of %d testcases!\n",
               at_suite_msg, at_failure, at_failure+at_success );
      }
      else {
         SDL_ATprint( "%s : All tests successful (%d)\n",
               at_suite_msg, at_success );
      }
   }

   /* Clean up. */
   SDL_ATcleanup();

   /* Return failed. */
   return failed;
}


/**
 * @brief Begin testcase.
 */
void SDL_ATbegin( const char *testcase )
{
   /* Do not open twice. */
   if (at_test_msg) {
      SDL_ATprint( "AT testcase '%s' not closed before opening testcase '%s'\n",
            at_test_msg, testcase );
   }
   /* Must have a name. */
   if (testcase == NULL) {
      SDL_ATprint( "AT testcase does not have a name.\n");
   }
   at_test_msg = testcase;
}


/**
 * @brief Ends the testcase with a succes or failure.
 */
static void SDL_ATendWith( int success )
{
   /* Make sure initialized. */
   if (at_test_msg == NULL) {
      SDL_ATprint("Ended testcase without initializing.\n");
      return;
   }

   /* Mark as success or failure. */
   if (success)
      at_success++;
   else
      at_failure++;

   /* Clean up. */
   at_test_msg = NULL;
}


/**
 * @brief Testcase test.
 */
int SDL_ATassert( const char *msg, int condition )
{
   /* Condition failed. */
   if (!condition) {
      /* Print. */
      SDL_ATprint( "%s [%s] : %s\n", at_suite_msg, at_test_msg, msg );
      /* End. */
      SDL_ATendWith(0);
   }
   return !condition;
}


/**
 * @brief Testcase test.
 */
int SDL_ATvassert( int condition, const char *msg, ... )
{
   va_list args;
   char buf[256];

   /* Condition failed. */
   if (!condition) {
      /* Get message. */
      va_start( args, msg );
      vsnprintf( buf, sizeof(buf), msg, args );
      va_end( args );
      /* Print. */
      SDL_ATprint( "%s [%s] : %s\n", at_suite_msg, at_test_msg, buf );
      /* End. */
      SDL_ATendWith(0);
   }
   return !condition;
}


/**
 * @brief End testcase.
 */
void SDL_ATend (void)
{
   SDL_ATendWith(1);
}


/**
 * @brief Displays a message.
 */
int SDL_ATprint( const char *msg, ... )
{
   va_list ap;
   int ret;

   /* Make sure there is something to print. */
   if (msg == NULL)
      return 0;
   else {
      va_start(ap, msg);
      ret = vprintf(msg, ap);
      va_end(ap);
   }

   return ret;
}


