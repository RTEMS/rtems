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

int clock_gettime(
  clockid_t        clock_id,
  struct timespec *tp
)
{
  ISR_Level      level;
  time_t         seconds;
  long           ticks;

  if ( !tp )
    set_errno_and_return_minus_one( EINVAL );

  switch ( clock_id ) {

    case CLOCK_REALTIME:
 
      _ISR_Disable( level );
        seconds = _TOD_Seconds_since_epoch;
        ticks   = _TOD_Current.ticks;
      _ISR_Enable( level );
 
      tp->tv_sec  = seconds + POSIX_TIME_SECONDS_1970_THROUGH_1988;
      tp->tv_nsec = ticks * _TOD_Microseconds_per_tick * 
                      TOD_NANOSECONDS_PER_MICROSECOND; 
      break;

#ifdef _POSIX_CPUTIME
    case CLOCK_PROCESS_CPUTIME:
      /* don't base this on _Watchdog_Ticks_since_boot--duration is too short*/
      return POSIX_NOT_IMPLEMENTED();
      break;
#endif

#ifdef _POSIX_THREAD_CPUTIME
    case CLOCK_THREAD_CPUTIME:
      return POSIX_NOT_IMPLEMENTED();
      break;
#endif
    default:
      set_errno_and_return_minus_one( EINVAL );

  }
  return 0;
}
