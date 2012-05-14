/*  tmacros.h
 *
 *  This include file contains macros which are useful in the RTEMS
 *  test suites.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __TMACROS_h
#define __TMACROS_h

#include <inttypes.h>
#include <bsp.h>    /* includes <rtems.h> */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtems/error.h>
#include <rtems/score/thread.h> /*  _Thread_Dispatch_disable_level */
#include <rtems/score/thread.inl> /*  _Thread_Dispatch_disable_level */

#ifdef __cplusplus
extern "C" {
#endif


#define FOREVER 1                  /* infinite loop */

#ifdef CONFIGURE_INIT
#define TEST_EXTERN
#else
#define TEST_EXTERN extern
#endif

#include <buffer_test_io.h>

/*
 *  Check that that the dispatch disable level is proper for the
 *  mode/state of the test.  Normally it should be 0 when in task space.
 *
 *  This test is only valid when in a non smp system.  In an smp system
 *  another cpu may be accessing the core at any point when this core
 *  does not have it locked.
 */
#if defined SMPTEST
 #define check_dispatch_disable_level( _expect ) 
#else
 #define check_dispatch_disable_level( _expect ) \
  do { \
    if ( (_expect) != -1 \
           && ((_Thread_Dispatch_in_critical_section() == false && (_expect) != 0) \
             || (_Thread_Dispatch_in_critical_section() && (_expect) == 0)) \
    ) { \
      printk( \
        "\n_Thread_Dispatch_disable_level is (%" PRId32 \
           ") not %d detected at %s:%d\n", \
         _Thread_Dispatch_in_critical_section(), (_expect), __FILE__, __LINE__ ); \
      FLUSH_OUTPUT(); \
      rtems_test_exit( 1 ); \
    } \
  } while ( 0 )
#endif

/*
 *  These macros properly report errors within the Classic API
 */
#define directive_failed( _dirstat, _failmsg )  \
 fatal_directive_status( _dirstat, RTEMS_SUCCESSFUL, _failmsg )

#define directive_failed_with_level( _dirstat, _failmsg, _level )  \
 fatal_directive_status_with_level( \
      _dirstat, RTEMS_SUCCESSFUL, _failmsg, _level )

#define fatal_directive_status( _stat, _desired, _msg ) \
  fatal_directive_status_with_level( _stat, _desired, _msg, 0 )

#define fatal_directive_check_status_only( _stat, _desired, _msg ) \
  do { \
    if ( (_stat) != (_desired) ) { \
      printf( "\n%s FAILED -- expected (%s) got (%s)\n", \
              (_msg), rtems_status_text(_desired), rtems_status_text(_stat) ); \
      FLUSH_OUTPUT(); \
      rtems_test_exit( _stat ); \
    } \
  } while ( 0 )

#define fatal_directive_status_with_level( _stat, _desired, _msg, _level ) \
  do { \
    check_dispatch_disable_level( _level ); \
    fatal_directive_check_status_only( _stat, _desired, _msg ); \
  } while ( 0 )

/*
 *  These macros properly report errors from the POSIX API
 */

#define posix_service_failed( _dirstat, _failmsg )  \
 fatal_posix_service_status( _dirstat, 0, _failmsg )

#define posix_service_failed_with_level( _dirstat, _failmsg, _level )  \
 fatal_posix_service_status_with_level( _dirstat, 0, _failmsg, _level )

#define fatal_posix_service_status_errno( _stat, _desired, _msg ) \
  if ( (_stat != -1) && (errno) != (_desired) ) { \
    long statx = _stat; \
    check_dispatch_disable_level( 0 ); \
    printf( "\n%s FAILED -- expected (%d - %s) got (%ld %d - %s)\n", \
	    (_msg), _desired, strerror(_desired), \
            statx, errno, strerror(errno) ); \
    FLUSH_OUTPUT(); \
    rtems_test_exit( _stat ); \
  }

#define fatal_posix_service_status( _stat, _desired, _msg ) \
  fatal_posix_service_status_with_level( _stat, _desired, _msg, 0 )

#define fatal_posix_service_status_with_level( _stat, _desired, _msg, _level ) \
  do { \
    check_dispatch_disable_level( _level ); \
    if ( (_stat) != (_desired) ) { \
      printf( "\n%s FAILED -- expected (%d - %s) got (%d - %s)\n", \
              (_msg), _desired, strerror(_desired), _stat, strerror(_stat) ); \
      printf( "\n FAILED -- errno (%d - %s)\n", \
              errno, strerror(errno) ); \
      FLUSH_OUTPUT(); \
      rtems_test_exit( _stat ); \
    } \
  } while ( 0 )

/*
 *  Generic integer version of the error reporting
 */

#define int_service_failed( _dirstat, _failmsg )  \
 fatal_int_service_status( _dirstat, RTEMS_SUCCESSFUL, _failmsg )

#define int_service_failed_with_level( _dirstat, _failmsg, _level )  \
 fatal_int_service_status_with_level( \
      _dirstat, RTEMS_SUCCESSFUL, _failmsg, _level )

#define fatal_int_service_status( _stat, _desired, _msg ) \
  fatal_int_service_status_with_level( _stat, _desired, _msg, 0 )

#define fatal_int_service_status_with_level( _stat, _desired, _msg, _level ) \
  do { \
    check_dispatch_disable_level( _level ); \
    if ( (_stat) != (_desired) ) { \
      printf( "\n%s FAILED -- expected (%d) got (%d)\n", \
              (_msg), (_desired), (_stat) ); \
      FLUSH_OUTPUT(); \
      rtems_test_exit( _stat ); \
    } \
  } while ( 0 )


/*
 *  Print the time
 */

#define sprint_time(_str, _s1, _tb, _s2) \
  do { \
    sprintf( (_str), "%s%02d:%02d:%02d   %02d/%02d/%04d%s", \
       _s1, (_tb)->hour, (_tb)->minute, (_tb)->second, \
       (_tb)->month, (_tb)->day, (_tb)->year, _s2 ); \
  } while ( 0 )

#define print_time(_s1, _tb, _s2) \
  do { \
    printf( "%s%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 "   %02" PRIu32 "/%02" PRIu32 "/%04" PRIu32 "%s", \
       _s1, (_tb)->hour, (_tb)->minute, (_tb)->second, \
       (_tb)->month, (_tb)->day, (_tb)->year, _s2 ); \
  } while ( 0 )

#define put_dot( _c ) \
  do { \
    putchar( _c ); \
    FLUSH_OUTPUT(); \
  } while ( 0 )

#define new_line  puts( "" )

#define puts_nocr printf

#ifdef RTEMS_TEST_NO_PAUSE
#define rtems_test_pause() \
    do { \
      printf( "<pause>\n" ); \
      FLUSH_OUTPUT(); \
  } while ( 0 )

#define rtems_test_pause_and_screen_number( _screen ) \
  do { \
    printf( "<pause - screen %d>\n", (_screen) ); \
    FLUSH_OUTPUT(); \
  } while ( 0 )
#else
#define rtems_test_pause() \
  do { \
    char buffer[ 80 ]; \
    printf( "<pause>" ); \
    FLUSH_OUTPUT(); \
    gets( buffer ); \
    puts( "" ); \
  } while ( 0 )

#define rtems_test_pause_and_screen_number( _screen ) \
  do { \
    char buffer[ 80 ]; \
    printf( "<pause - screen %d>", (_screen) ); \
    FLUSH_OUTPUT(); \
    gets( buffer ); \
    puts( "" ); \
  } while ( 0 )
#endif

#define put_name( name, crlf ) \
{ int c0, c1, c2, c3; \
  c0 = (name >> 24) & 0xff; \
  c1 = (name >> 16) & 0xff; \
  c2 = (name >> 8) & 0xff; \
  c3 = name & 0xff; \
  putchar( (isprint(c0)) ? c0 : '*' ); \
  if ( c1 ) putchar( (isprint(c1)) ? c1 : '*' ); \
  if ( c2 ) putchar( (isprint(c2)) ? c2 : '*' ); \
  if ( c3 ) putchar( (isprint(c3)) ? c3 : '*' ); \
  if ( crlf ) \
    putchar( '\n' ); \
}

#ifndef build_time
#define build_time( TB, MON, DAY, YR, HR, MIN, SEC, TK ) \
  { (TB)->year   = YR;  \
    (TB)->month  = MON; \
    (TB)->day    = DAY; \
    (TB)->hour   = HR;  \
    (TB)->minute = MIN; \
    (TB)->second = SEC; \
    (TB)->ticks  = TK; }
#endif

#define task_number( tid ) \
  ( rtems_object_id_get_index( tid ) - \
      rtems_configuration_get_rtems_api_configuration()-> \
        number_of_initialization_tasks )

#define rtems_test_assert(__exp) \
  do { \
    if (!(__exp)) { \
      printf( "%s: %d %s\n", __FILE__, __LINE__, #__exp ); \
      rtems_test_exit(0); \
    } \
  } while (0)

/*
 * Various inttypes.h-stype macros to assist printing
 * certain system types on different targets.
 */

#if defined(RTEMS_USE_16_BIT_OBJECT)
#define PRIxrtems_id PRIx16
#else
#define PRIxrtems_id PRIx32
#endif

/* c.f. cpukit/score/include/rtems/score/priority.h */
#define PRIdPriority_Control PRId32
#define PRIxPriority_Control PRIx32
/* rtems_task_priority is a typedef to Priority_Control */
#define PRIdrtems_task_priority PRIdPriority_Control
#define PRIxrtems_task_priority PRIxPriority_Control

/* c.f. cpukit/score/include/rtems/score/watchdog.h */
#define PRIdWatchdog_Interval PRIu32
/* rtems_interval is a typedef to Watchdog_Interval */
#define PRIdrtems_interval    PRIdWatchdog_Interval

/* c.f. cpukit/score/include/rtems/score/thread.h */
#define PRIdThread_Entry_numeric_type PRIuPTR
/* rtems_task_argument is a typedef to Thread_Entry_numeric_type */
#define PRIdrtems_task_argument PRIdThread_Entry_numeric_type

/* rtems_event_set is a typedef to unit32_t */
#define PRIxrtems_event_set PRIx32

/* HACK: newlib defines pthread_t as a typedef to __uint32_t */
/* HACK: There is no portable way to print pthread_t's */
#define PRIxpthread_t PRIx32

/* rtems_signal_set is a typedef to uint32_t */
#define PRIxrtems_signal_set PRIx32

/* newlib's ino_t is a typedef to "unsigned long" */
#define PRIxino_t "lx"

#ifdef __cplusplus
}
#endif

#endif
