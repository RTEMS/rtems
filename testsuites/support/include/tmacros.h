/*  tmacros.h
 *
 *  This include file contains macros which are useful in the RTEMS
 *  test suites.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __TMACROS_h
#define __TMACROS_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bsp.h>    /* includes <rtems.h> */

#include <stdio.h>
#include <stdlib.h>

#define FOREVER 1                  /* infinite loop */

#ifdef TEST_INIT
#define TEST_EXTERN 
#define CONFIGURE_INIT
#else
#define TEST_EXTERN extern
#endif

#define directive_failed( dirstat, failmsg )  \
    fatal_directive_status( dirstat, RTEMS_SUCCESSFUL, failmsg )

#define fatal_directive_status( stat, desired, msg ) \
     do { \
       if ( (stat) != (desired) ) { \
         printf( "\n%s FAILED -- expected (%d) got (%d)\n", \
                 (msg), (desired), (stat) ); \
         fflush(stdout); \
         exit( stat ); \
       } \
     } while ( 0 ) 

#define sprint_time(str,s1,tb,s2) \
  do { \
    sprintf( (str), "%s%02d:%02d:%02d   %02d/%02d/%04d%s", \
       s1, (tb)->hour, (tb)->minute, (tb)->second, \
       (tb)->month, (tb)->day, (tb)->year, s2 ); \
  } while ( 0 )

#define print_time(s1,tb,s2) \
  do { \
    printf( "%s%02d:%02d:%02d   %02d/%02d/%04d%s", \
       s1, (tb)->hour, (tb)->minute, (tb)->second, \
       (tb)->month, (tb)->day, (tb)->year, s2 ); \
    fflush(stdout); \
  } while ( 0 )

#define put_dot( c ) putchar( c ); fflush( stdout )

#define new_line  puts( "" )

#define puts_nocr printf

#ifdef RTEMS_TEST_NO_PAUSE
#define rtems_test_pause() \
    do { \
      printf( "<pause>\n" ); fflush( stdout ); \
  } while ( 0 )

#define rtems_test_pause_and_screen_number( _screen ) \
  do { \
    printf( "<pause - screen %d>\n", (_screen) ); fflush( stdout ); \
  } while ( 0 )
#else
#define rtems_test_pause() \
  do { \
    char buffer[ 80 ]; \
    printf( "<pause>" ); fflush( stdout ); \
    gets( buffer ); \
    puts( "" ); \
  } while ( 0 )

#define rtems_test_pause_and_screen_number( _screen ) \
  do { \
    char buffer[ 80 ]; \
    printf( "<pause - screen %d>", (_screen) ); fflush( stdout ); \
    gets( buffer ); \
    puts( "" ); \
  } while ( 0 )
#endif

#define put_name( name, crlf ) \
{ rtems_unsigned32 c0, c1, c2, c3; \
  c0 = (name >> 24) & 0xff; \
  c1 = (name >> 16) & 0xff; \
  c2 = (name >> 8) & 0xff; \
  c3 = name & 0xff; \
  putchar( (char)c0 ); \
  if ( c1 ) putchar( (char)c1 ); \
  if ( c2 ) putchar( (char)c2 ); \
  if ( c3 ) putchar( (char)c3 ); \
  if ( crlf ) \
    putchar( '\n' ); \
}

#define build_time( TB, MON, DAY, YR, HR, MIN, SEC, TK ) \
  { (TB)->year   = YR;  \
    (TB)->month  = MON; \
    (TB)->day    = DAY; \
    (TB)->hour   = HR;  \
    (TB)->minute = MIN; \
    (TB)->second = SEC; \
    (TB)->ticks  = TK; }

#define task_number( tid ) \
  ( rtems_get_index( tid ) - \
     rtems_configuration_get_rtems_api_configuration()->number_of_initialization_tasks )

static inline rtems_unsigned32 get_ticks_per_second( void )
{
  rtems_interval ticks_per_second;
  (void) rtems_clock_get( RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second );
  return ticks_per_second;
}

#define TICKS_PER_SECOND get_ticks_per_second()

#ifdef __cplusplus
}
#endif

#endif
