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
 *  14.2.5 High Resolution Sleep, P1003.1b-1993, p. 269
 */

int nanosleep(
  const struct timespec  *rqtp,
  struct timespec        *rmtp
)
{
  Watchdog_Interval  ticks;
  struct timespec   *the_rqtp;

  if ( !rqtp )
    rtems_set_errno_and_return_minus_one( EINVAL );

  the_rqtp = (struct timespec *)rqtp;

  /*
   *  Return EAGAIN if the delay interval is negative.  
   *
   *  NOTE:  This behavior is beyond the POSIX specification.  
   *         FSU pthreads shares this behavior.
   */

  if ( the_rqtp->tv_sec < 0 )
    the_rqtp->tv_sec = 0;

  if ( /* the_rqtp->tv_sec < 0 || */ the_rqtp->tv_nsec < 0 )
    rtems_set_errno_and_return_minus_one( EAGAIN );

  if ( the_rqtp->tv_nsec >= TOD_NANOSECONDS_PER_SECOND )
    rtems_set_errno_and_return_minus_one( EINVAL );
 
  ticks = _POSIX_Timespec_to_interval( the_rqtp );

  /*
   *  This behavior is also beyond the POSIX specification but is
   *  consistent with the RTEMS api and yields desirable behavior.
   */

  if ( !ticks ) {
    _Thread_Disable_dispatch();
      _Thread_Yield_processor();
    _Thread_Enable_dispatch();
    if ( rmtp ) {
       rmtp->tv_sec = 0; 
       rmtp->tv_nsec = 0; 
    }
    return 0;
  }
  
  _Thread_Disable_dispatch();
    _Thread_Set_state(
      _Thread_Executing,
      STATES_DELAYING | STATES_INTERRUPTIBLE_BY_SIGNAL
    );
    _Watchdog_Initialize(
      &_Thread_Executing->Timer,
      _Thread_Delay_ended,
      _Thread_Executing->Object.id,
      NULL
    );
    _Watchdog_Insert_ticks( &_Thread_Executing->Timer, ticks );
  _Thread_Enable_dispatch();

  /* calculate time remaining */

  if ( rmtp ) {
    ticks -= 
      _Thread_Executing->Timer.stop_time - _Thread_Executing->Timer.start_time;

    _POSIX_Interval_to_timespec( ticks, rmtp );

    /*
     *  If there is time remaining, then we were interrupted by a signal.
     */

    if ( ticks )
      rtems_set_errno_and_return_minus_one( EINTR );
  }

  return 0;
}
