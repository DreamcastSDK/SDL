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
 * if (!SDL_ATassert( (1+1)==2, "Trying '1+1=2'."))
 *    return;
 *
 * SDL_ATbegin( "My second testcase" );
 * if (!SDL_ATassert( (4/2)==2, "Trying '4/2=2'."))
 *    return;
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
 *    @param condition Condition to make sure is true.
 *    @param msg Message to display for failure.
 *    @return Returns 1 if the condition isn't met.
 */
int SDL_ATassert( const char *msg, int condition );
/**
 * @brief Checks a condition in the testcase.
 *
 * Will automatically call SDL_ATend if the condition isn't met.
 *
 *    @param condition Condition to make sure is true.
 *    @param msg Message to display for failure with printf style formatting.
 *    @return Returns 1 if the condition isn't met.
 */
int SDL_ATvassert( int condition, const char *msg, ... );
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


