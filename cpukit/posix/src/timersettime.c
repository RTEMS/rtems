/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/posix/time.h>
#include <rtems/posix/ptimer.h>
#include <rtems/posix/timer.h>

int timer_settime(
  timer_t                  timerid,
  int                      flags,
  const struct itimerspec *value,
  struct itimerspec       *ovalue
)
{
  POSIX_Timer_Control *ptimer;
  Objects_Locations    location;
  bool                 activated;
  uint32_t             initial_period;
  struct itimerspec    normalize;

  if ( !value )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /* 
   * First, it verifies if the structure "value" is correct   
   * if the number of nanoseconds is not correct return EINVAL
   */
  if ( !_Timespec_Is_valid( &(value->it_value) ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  if ( !_Timespec_Is_valid( &(value->it_interval) ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( flags != TIMER_ABSTIME && flags != POSIX_TIMER_RELATIVE ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  normalize = *value;

  /* Convert absolute to relative time */
  if (flags == TIMER_ABSTIME) {
    struct timespec now;
    _TOD_Get( &now );
    /* Check for seconds in the past */
    if ( _Timespec_Greater_than( &now, &normalize.it_value ) )
      rtems_set_errno_and_return_minus_one( EINVAL );
    _Timespec_Subtract( &now, &normalize.it_value, &normalize.it_value );
  }

  /* If the function reaches this point, then it will be necessary to do
   * something with the structure of times of the timer: to stop, start
   * or start it again
   */

  ptimer = _POSIX_Timer_Get( timerid, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      /* First, it verifies if the timer must be stopped */
      if ( normalize.it_value.tv_sec == 0 && normalize.it_value.tv_nsec == 0 ) {
         /* Stop the timer */
         (void) _Watchdog_Remove( &ptimer->Timer );
         /* The old data of the timer are returned */
         if ( ovalue )
           *ovalue = ptimer->timer_data;
         /* The new data are set */
         ptimer->timer_data = normalize;
         /* Indicates that the timer is created and stopped */
         ptimer->state = POSIX_TIMER_STATE_CREATE_STOP;
         /* Returns with success */
        _Thread_Enable_dispatch();
        return 0;
       }

       /* Convert from seconds and nanoseconds to ticks */
       ptimer->ticks  = _Timespec_To_ticks( &value->it_interval );
       initial_period = _Timespec_To_ticks( &normalize.it_value );


       activated = _POSIX_Timer_Insert_helper(
         &ptimer->Timer,
         initial_period,
         ptimer->Object.id,
         _POSIX_Timer_TSR,
         ptimer
       );
       if ( !activated ) {
         _Thread_Enable_dispatch();
         return 0;
       }

       /*
        * The timer has been started and is running.  So we return the
        * old ones in "ovalue"
        */
       if ( ovalue )
         *ovalue = ptimer->timer_data;
       ptimer->timer_data = normalize;

       /* Indicate that the time is running */
       ptimer->state = POSIX_TIMER_STATE_CREATE_RUN;
       _TOD_Get( &ptimer->time );
       _Thread_Enable_dispatch();
       return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
