/*
 * Common code for automated test suite.
 *
 * Written by Edgar Simo "bobbens"
 *
 * Released under Public Domain.
 */


/**
 * @file SDL_at.h
 *
 * @brief Handles automatic testing functionality.
 *
 * You create a testsuite and then run multiple testcases within that suite.
 *  Syntax is similar to OpenGL. An example would be:
 *
 * @code
 * int f;
 * SDL_ATinit( "My testsuite" );
 *
 * SDL_ATbegin( "My first testcase" );
 * if (!SDL_ATassert( "Trying '1+1=2'.", (1+1)==2))
 *    SDL_ATend();
 *
 * SDL_ATbegin( "My second testcase" );
 * if (!SDL_ATassert( "Trying '4/2=2'.", (4/2)==2))
 *    SDL_ATend();
 *
 * f = SDL_ATend();
 * @endcode
 *
 * @author Edgar Simo "bobbens"
 */


#ifndef _SDL_AT_H
#  define _SDL_AT_H


/*
 * Suite level actions.
 */
/**
 * @brief Starts the testsuite.
 *
 *    @param suite Name of the suite to start testing.
 */
void SDL_ATinit( const char *suite );
/**
 * @brief Finishes the testsuite printing out global results if verbose.
 *
 *    @param verbose Displays global results.
 */
int SDL_ATfinish( int verbose );


/*
 * Testcase level actions.
 */
/**
 * @brief Begins a testcase.
 *
 *    @param testcase Name of the testcase to begin.
 */
void SDL_ATbegin( const char *testcase );
/**
 * @brief Checks a condition in the testcase.
 *
 * Will automatically call SDL_ATend if the condition isn't met.
 *
 *    @param msg Message to display for failure.
 *    @param condition Condition to make sure is true.
 *    @return Returns 1 if the condition isn't met.
 */
int SDL_ATassert( const char *msg, int condition );
/**
 * @brief Ends a testcase.
 */
void SDL_ATend (void);


/*
 * Misc functions.
 */
/**
 * @brief Prints some text.
 *
 *    @param msg printf formatted string to display.
 *    @return Number of character printed.
 */
int SDL_ATprint( const char *msg, ... );


#endif /* _SDL_AT_H */


