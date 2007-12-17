/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
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
  boolean              activated;

  if ( !value )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /* First, it verifies if the structure "value" is correct */
  if ( ( value->it_value.tv_nsec > TOD_NANOSECONDS_PER_SECOND ) ||
       ( value->it_value.tv_nsec < 0 ) ) {
    /* The number of nanoseconds is not correct */
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  
  /* XXX check for seconds in the past */

  if ( flags != TIMER_ABSTIME && flags != POSIX_TIMER_RELATIVE ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* If the function reaches this point, then it will be necessary to do
   * something with the structure of times of the timer: to stop, start
   * or start it again
   */

  ptimer = _POSIX_Timer_Get( timerid, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      /* First, it verifies if the timer must be stopped */
      if ( value->it_value.tv_sec == 0 && value->it_value.tv_nsec == 0 ) {
         /* Stop the timer */
         (void) _Watchdog_Remove( &ptimer->Timer );
         /* The old data of the timer are returned */
         if ( ovalue )
           *ovalue = ptimer->timer_data;
         /* The new data are set */
         ptimer->timer_data = *value;
         /* Indicates that the timer is created and stopped */
         ptimer->state = POSIX_TIMER_STATE_CREATE_STOP;
         /* Returns with success */
        _Thread_Enable_dispatch();
        return 0;
       }

       /* absolute or relative? */
       switch (flags) {
         case TIMER_ABSTIME:

           /* The fire time is absolute: use "rtems_time_fire_when" */
           /* First, it converts from struct itimerspec to rtems_time_of_day */

           _Watchdog_Initialize(
             &ptimer->Timer, _POSIX_Timer_TSR, ptimer->Object.id, ptimer );

           _Watchdog_Insert_seconds(
              &ptimer->Timer,
              value->it_value.tv_sec - _TOD_Seconds_since_epoch
           );

           /* Returns the old ones in "ovalue" */
           if ( ovalue )
             *ovalue = ptimer->timer_data;
           ptimer->timer_data = *value;

           /* Indicate that the time is running */
           ptimer->state = POSIX_TIMER_STATE_CREATE_RUN;

           /* Stores the time in which the timer was started again */
           _TOD_Get( &ptimer->time );
           _Thread_Enable_dispatch();
           return 0;
           break;

         /* The fire time is relative: use "rtems_time_fire_after" */
         case POSIX_TIMER_RELATIVE:
           /* First, convert from seconds and nanoseconds to ticks */
           ptimer->ticks = _Timespec_To_ticks( &value->it_value );

           activated = _POSIX_Timer_Insert_helper(
             &ptimer->Timer,
             ptimer->ticks,
             ptimer->Object.id,
             _POSIX_Timer_TSR,
             ptimer
           );
           if ( !activated )
             return 0;

           /* The timer has been started and is running */
           /* return the old ones in "ovalue" */
           if ( ovalue )
             *ovalue = ptimer->timer_data;
           ptimer->timer_data = *value;

           /* Indicate that the time is running */
           ptimer->state = POSIX_TIMER_STATE_CREATE_RUN;
           _TOD_Get( &ptimer->time );
            _Thread_Enable_dispatch();
           return 0;
      }
      _Thread_Enable_dispatch();
      break;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
