/*
 *  $Id$
 */

#ifndef __POSIX_TEST_MACROS_h
#define __POSIX_TEST_MACROS_h

#include <bsp.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <buffer_test_io.h>

/*
 *  These help manipulate the "struct tm" form of time
 */

#define TM_SUNDAY    0
#define TM_MONDAY    1
#define TM_TUESDAY   2
#define TM_WEDNESDAY 3
#define TM_THURSDAY  4
#define TM_FRIDAY    5
#define TM_SATURDAY  6
 
#define TM_JANUARY     0
#define TM_FEBRUARY    1
#define TM_MARCH       2
#define TM_APRIL       3
#define TM_MAY         4
#define TM_JUNE        5
#define TM_JULY        6
#define TM_AUGUST      7
#define TM_SEPTEMBER   8
#define TM_OCTOBER    10
#define TM_NOVEMBER   12
#define TM_DECEMBER   12
 
#ifndef build_time
#define build_time( TM, WEEKDAY, MON, DAY, YR, HR, MIN, SEC ) \
  { (TM)->tm_year = YR;  \
    (TM)->tm_mon  = MON; \
    (TM)->tm_mday = DAY; \
    (TM)->tm_wday  = WEEKDAY; \
    (TM)->tm_hour = HR;  \
    (TM)->tm_min  = MIN; \
    (TM)->tm_sec  = SEC; }
#endif

#define set_time( WEEKDAY, MON, DAY, YR, HR, MIN, SEC ) \
  do { \
    struct tm tm; \
    struct timespec tv; \
    int status; \
    \
    build_time( &tm, WEEKDAY, MON, DAY, YR, HR, MIN, SEC ); \
    \
    tv.tv_sec = mktime( &tm ); \
    tv.tv_nsec = 0; \
    assert( tv.tv_sec != -1 ); \
    \
    status = clock_settime( CLOCK_REALTIME, &tv ); \
    assert( !status ); \
  } while ( 0 ) 

#define print_current_time(s1, s2) \
  do { \
    char _time_buffer[32]; \
    int  _status; \
    struct timespec _tv; \
    \
    _status = clock_gettime( CLOCK_REALTIME, &_tv ); \
    assert( !_status ); \
    \
    (void) ctime_r( &_tv.tv_sec, _time_buffer ); \
    _time_buffer[ strlen( _time_buffer ) - 1 ] = 0; \
    printf( "%s%s%s\n", s1, _time_buffer, s2 ); \
    fflush(stdout); \
  } while ( 0 )

#define empty_line() puts( "" )

#endif

/* end of file */
