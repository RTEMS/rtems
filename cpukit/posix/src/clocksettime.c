/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/seterr.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_settime(
  clockid_t              clock_id,
  const struct timespec *tp
)
{
  struct tm         split_time;
  TOD_Control       tod;
  Watchdog_Interval seconds;

  assert( tp );

  switch ( clock_id ) {
 
    case CLOCK_REALTIME:
      (void) gmtime_r( &tp->tv_sec, &split_time );
 
      /*
       *  Convert the tm structure format to that used by the TOD Handler
       *
       *  NOTE: TOD Handler does not honor leap seconds.
       */

      tod.year   = split_time.tm_year + 1900;  /* RHS is years since 1900 */
      tod.month  = split_time.tm_mon + 1;      /* RHS uses 0-11 */
      tod.day    = split_time.tm_mday;
      tod.hour   = split_time.tm_hour;
      tod.minute = split_time.tm_min;
      tod.second = split_time.tm_sec;  /* RHS allows 0-61 for leap seconds */

      tod.ticks  = (tp->tv_nsec / TOD_NANOSECONDS_PER_MICROSECOND) /
                      _TOD_Microseconds_per_tick;

      if ( !_TOD_Validate( &tod ) )
        rtems_set_errno_and_return_minus_one( EINVAL );
 
      /*
       *  We can't use the tp->tv_sec field because it is based on 
       *  a different EPOCH.
       */

      seconds = _TOD_To_seconds( &tod );
      _Thread_Disable_dispatch();
        _TOD_Set( &tod, seconds );
      _Thread_Enable_dispatch();
      break;
 
#ifdef _POSIX_CPUTIME
    case CLOCK_PROCESS_CPUTIME:
      return POSIX_NOT_IMPLEMENTED();
      break;
#endif
 
#ifdef _POSIX_THREAD_CPUTIME
    case CLOCK_THREAD_CPUTIME:
      return POSIX_NOT_IMPLEMENTED();
      break;
#endif
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
 
  }
  return 0;
}
