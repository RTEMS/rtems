/**
 * @file
 *
 * This include file contains macros which are useful in the RTEMS
 * test suites.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __TMACROS_h
#define __TMACROS_h

#include <inttypes.h>
#include <rtems/inttypes.h>
#include <bsp.h>    /* includes <rtems.h> */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtems/error.h>
#include <rtems/test.h>
#include <rtems/score/threaddispatch.h>

#include <buffer_test_io.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FOREVER 1                  /* infinite loop */

#ifdef CONFIGURE_INIT
#define TEST_EXTERN
#else
#define TEST_EXTERN extern
#endif

/*
 *  Check that that the dispatch disable level is proper for the
 *  mode/state of the test.  Normally it should be 0 when in task space.
 */
#define check_dispatch_disable_level( _expect ) \
  do { \
    if ( (_expect) != -1 \
           && (((!_Thread_Dispatch_is_enabled()) == false && (_expect) != 0) \
             || ((!_Thread_Dispatch_is_enabled()) && (_expect) == 0)) \
    ) { \
      printf( \
        "\n_Thread_Dispatch_disable_level is (%i)" \
           " not %d detected at %s:%d\n", \
         !_Thread_Dispatch_is_enabled(), (_expect), __FILE__, __LINE__ ); \
      rtems_test_exit( 1 ); \
    } \
  } while ( 0 )

/*
 *  Check that that the allocator mutex is not owned by the executing thread.
 */
#include <rtems/score/apimutex.h>
#define check_if_allocator_mutex_is_not_owned() \
  do { \
    if ( _RTEMS_Allocator_is_owner() ) { \
      printf( \
        "\nRTEMS Allocator Mutex is owned by executing thread " \
          "and should not be.\n" \
        "Detected at %s:%d\n", \
        __FILE__, \
        __LINE__ \
      ); \
      rtems_test_exit( 1 ); \
    } \
  } while ( 0 )

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
      rtems_test_exit( _stat ); \
    } \
  } while ( 0 )

#define fatal_directive_status_with_level( _stat, _desired, _msg, _level ) \
  do { \
    check_dispatch_disable_level( _level ); \
    check_if_allocator_mutex_is_not_owned(); \
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
    check_if_allocator_mutex_is_not_owned(); \
    printf( "\n%s FAILED -- expected (%d - %s) got (%ld %d - %s)\n", \
	    (_msg), _desired, strerror(_desired), \
            statx, errno, strerror(errno) ); \
    rtems_test_exit( _stat ); \
  }

#define fatal_posix_service_status( _stat, _desired, _msg ) \
  fatal_posix_service_status_with_level( _stat, _desired, _msg, 0 )

#define fatal_posix_service_status_with_level( _stat, _desired, _msg, _level ) \
  do { \
    check_dispatch_disable_level( _level ); \
    check_if_allocator_mutex_is_not_owned(); \
    if ( (_stat) != (_desired) ) { \
      printf( "\n%s FAILED -- expected (%d - %s) got (%d - %s)\n", \
              (_msg), _desired, strerror(_desired), _stat, strerror(_stat) ); \
      printf( "\n FAILED -- errno (%d - %s)\n", \
              errno, strerror(errno) ); \
      rtems_test_exit( _stat ); \
    } \
  } while ( 0 )

/*
 * This macro evaluates the semaphore id returned.
 */
#define fatal_posix_sem( _ptr, _msg ) \
  if ( (_ptr != SEM_FAILED) ) { \
    check_dispatch_disable_level( 0 ); \
    printf( "\n%s FAILED -- expected (-1) got (%p - %d/%s)\n", \
	    (_msg), _ptr, errno, strerror(errno) ); \
    rtems_test_exit( -1 ); \
  }

/*
 * This macro evaluates the message queue id returned.
 */
#define fatal_posix_mqd( _ptr, _msg ) \
  if ( (_ptr != (mqd_t) -1) ) { \
    check_dispatch_disable_level( 0 ); \
    printf( "\n%s FAILED -- expected (-1) got (%" PRId32 " - %d/%s)\n", \
	    (_msg), _ptr, errno, strerror(errno) ); \
    rtems_test_exit( -1 ); \
  }

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
  } while ( 0 )

#define new_line  puts( "" )

#define puts_nocr printf

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

/**
 * This assists in clearly disabling warnings on GCC in certain very
 * specific cases.
 *
 * + -Wnon-null - If a method is declared as never having a NULL first
 *   parameter. We need to explicitly disable this compiler warning to make
 *   the code warning free.
 */
#ifdef __GNUC__
  #define COMPILER_DIAGNOSTIC_SETTINGS_PUSH _Pragma("GCC diagnostic push")
  #define COMPILER_DIAGNOSTIC_SETTINGS_POP _Pragma("GCC diagnostic pop")
  #define COMPILER_DIAGNOSTIC_SETTINGS_DISABLE_NONNULL \
    _Pragma("GCC diagnostic ignored \"-Wnonnull\"")
#else
  #define COMPILER_DIAGNOSTIC_SETTINGS_PUSH
  #define COMPILER_DIAGNOSTIC_SETTINGS_POP
  #define COMPILER_DIAGNOSTIC_SETTINGS_DISABLE_NONNULL
#endif

#ifdef __cplusplus
}
#endif

#endif
