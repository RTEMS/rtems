/*
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
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/posix/time.h>

/************************************/
/* These includes are now necessary */
/************************************/

#include <unistd.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/timer.h>
#include <rtems/rtems/clock.h>
#include <rtems/posix/psignal.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>

#include <rtems/seterr.h>
#include <rtems/posix/timer.h>

boolean _Watchdog_Insert_ticks_helper(
  Watchdog_Control               *timer,
  Watchdog_Interval               ticks,
  Objects_Id                      id,
  Watchdog_Service_routine_entry  TSR,
  void                           *arg
)
{
  ISR_Level            level;

  (void) _Watchdog_Remove( timer );
  _ISR_Disable( level );

    /*
     *  Check to see if the watchdog has just been inserted by a
     *  higher priority interrupt.  If so, abandon this insert.
     */
    if ( timer->state != WATCHDOG_INACTIVE ) {
      _ISR_Enable( level );
      return FALSE;
    }

    /*
     *  OK.  Now we now the timer was not rescheduled by an interrupt
     *  so we can atomically initialize it as in use.
     */
    _Watchdog_Initialize( timer, TSR, id, arg );
    _Watchdog_Insert_ticks( timer, ticks );
  _ISR_Enable( level );
  return TRUE;
}

/* #define DEBUG_MESSAGES */

/* ***************************************************************************
 * _POSIX_Timer_TSR
 *
 *  Description: This is the operation that is ran when a timer expires
 * ***************************************************************************/


void _POSIX_Timer_TSR(Objects_Id timer, void *data)
{
  POSIX_Timer_Control *ptimer;
  boolean              activated;

  ptimer = (POSIX_Timer_Control *)data;

  /* Increment the number of expirations. */
  ptimer->overrun = ptimer->overrun + 1;
  /* The timer must be reprogrammed */
  if ( ( ptimer->timer_data.it_interval.tv_sec  != 0 ) ||
       ( ptimer->timer_data.it_interval.tv_nsec != 0 ) ) {
#if 0
    status = rtems_timer_fire_after(
         ptimer->timer_id, ptimer->ticks, _POSIX_Timer_TSR, ptimer );
#endif
    activated = _Watchdog_Insert_ticks_helper(
      &ptimer->Timer,
      ptimer->ticks,
      ptimer->Object.id,
      _POSIX_Timer_TSR,
      ptimer
    );
    if ( !activated )
      return;

    /* Store the time when the timer was started again */
    _TOD_Get( &ptimer->time );

    /* The state really did not change but just to be safe */
    ptimer->state = POSIX_TIMER_STATE_CREATE_RUN;
  } else {
   /* Indicates that the timer is stopped */
   ptimer->state = POSIX_TIMER_STATE_CREATE_STOP;
  }

  /*
   * The sending of the signal to the process running the handling function
   * specified for that signal is simulated
   */

  if ( pthread_kill ( ptimer->thread_id, ptimer->inf.sigev_signo ) ) {
    /* XXX error handling */
  }

  /* After the signal handler returns, the count of expirations of the
   * timer must be set to 0.
   */
  ptimer->overrun = 0;
}

/* *********************************************************************
 *  14.2.2 Create a Per-Process Timer, P1003.1b-1993, p. 264
 * ********************************************************************/

/* **************
 * timer_create
 * **************/

int timer_create(
  clockid_t        clock_id,
  struct sigevent *evp,
  timer_t         *timerid
)
{
  POSIX_Timer_Control *ptimer;

  if ( clock_id != CLOCK_REALTIME )
    rtems_set_errno_and_return_minus_one( EINVAL );

 /*
  *  The data of the structure evp are checked in order to verify if they
  *  are coherent.
  */

  if (evp != NULL) {
    /* The structure has data */
    if ( ( evp->sigev_notify != SIGEV_NONE ) &&
         ( evp->sigev_notify != SIGEV_SIGNAL ) ) {
       /* The value of the field sigev_notify is not valid */
       rtems_set_errno_and_return_minus_one( EINVAL );
     }

     if ( !evp->sigev_signo )
       rtems_set_errno_and_return_minus_one( EINVAL );

     if ( !is_valid_signo(evp->sigev_signo) )
       rtems_set_errno_and_return_minus_one( EINVAL );
  }

  _Thread_Disable_dispatch();         /* to prevent deletion */

  /*
   *  Allocate a timer
   */
  ptimer = _POSIX_Timer_Allocate();
  if ( !ptimer ) {
    _Thread_Enable_dispatch();
    rtems_set_errno_and_return_minus_one( EAGAIN );
  }

  /* The data of the created timer are stored to use them later */

  ptimer->state     = POSIX_TIMER_STATE_CREATE_NEW;
  ptimer->thread_id = _Thread_Executing->Object.id;

  if ( evp != NULL ) {
    ptimer->inf.sigev_notify = evp->sigev_notify;
    ptimer->inf.sigev_signo  = evp->sigev_signo;
    ptimer->inf.sigev_value  = evp->sigev_value;
  }

  ptimer->overrun  = 0;
  ptimer->timer_data.it_value.tv_sec     = 0;
  ptimer->timer_data.it_value.tv_nsec    = 0;
  ptimer->timer_data.it_interval.tv_sec  = 0;
  ptimer->timer_data.it_interval.tv_nsec = 0;

  _Watchdog_Initialize( &ptimer->Timer, NULL, 0, NULL );
  _Objects_Open(&_POSIX_Timer_Information, &ptimer->Object, (Objects_Name) 0);

  *timerid  = ptimer->Object.id;
  _Thread_Enable_dispatch();
  return 0;
}

/*
 *  14.2.3 Delete a Per_process Timer, P1003.1b-1993, p. 266
 */

int timer_delete(
  timer_t timerid
)
{
 /*
  * IDEA: This function must probably stop the timer first and then delete it
  *
  *       It will have to do a call to rtems_timer_cancel and then another
  *       call to rtems_timer_delete.
  *       The call to rtems_timer_delete will be probably unnecessary,
  *       because rtems_timer_delete stops the timer before deleting it.
  */
  POSIX_Timer_Control *ptimer;
  Objects_Locations    location;

  ptimer = _POSIX_Timer_Get( timerid, &location );
  switch ( location ) {
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      rtems_set_errno_and_return_minus_one( EINVAL );
#endif

    case OBJECTS_ERROR:
      rtems_set_errno_and_return_minus_one( EINVAL );

    case OBJECTS_LOCAL:
      _Objects_Close( &_POSIX_Timer_Information, &ptimer->Object );
      ptimer->state = POSIX_TIMER_STATE_FREE;
      (void) _Watchdog_Remove( &ptimer->Timer );
      _POSIX_Timer_Free( ptimer );
      _Thread_Enable_dispatch();
      return 0;
  }
  return -1;   /* unreached - only to remove warnings */
}

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */

/* **************
 * timer_settime
 * **************/


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

  if ( value == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

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
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return -1;
     rtems_set_errno_and_return_minus_one( EINVAL );
#endif

    case OBJECTS_ERROR:
      return -1;

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

           activated = _Watchdog_Insert_ticks_helper(
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
      rtems_set_errno_and_return_minus_one( EINVAL );
  }
  return -1;   /* unreached - only to remove warnings */
}

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */

/* **************
 * timer_gettime
 * **************/

int timer_gettime(
  timer_t            timerid,
  struct itimerspec *value
)
{

 /*
  * IDEA:  This function does not use functions of RTEMS to the handle
  *        of timers. It uses some functions for managing the time.
  *
  *        A possible form to do this is the following:
  *
  *          - When a timer is initialized, the value of the time in
  *            that moment is stored.
  *          - When this function is called, it returns the difference
  *            between the current time and the initialization time.
  */

  POSIX_Timer_Control *ptimer;
  Objects_Locations    location;
  struct timespec      current_time;

  /* Reads the current time */
  _TOD_Get( &current_time );

  ptimer = _POSIX_Timer_Get( timerid, &location );
  switch ( location ) {
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      rtems_set_errno_and_return_minus_one( EINVAL );
#endif

    case OBJECTS_ERROR:
      rtems_set_errno_and_return_minus_one( EINVAL );

    case OBJECTS_LOCAL:

      /* Calculates the time left before the timer finishes */

      _Timespec_Subtract(
        &ptimer->timer_data.it_value,
        &current_time,
        &value->it_value
      );

      value->it_interval  = ptimer->timer_data.it_interval;

      _Thread_Enable_dispatch();
      return 0;
  }
  return -1;   /* unreached - only to remove warnings */
}

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */

/*
 * timer_getoverrun
 *
 * The expiration of a timer must increase by one a counter.
 * After the signal handler associated to the timer finishes
 * its execution, _POSIX_Timer_TSR will have to set this counter to 0.
 */

int timer_getoverrun(
  timer_t   timerid
)
{
  int                  overrun;
  POSIX_Timer_Control *ptimer;
  Objects_Locations    location;

  ptimer = _POSIX_Timer_Get( timerid, &location );
  switch ( location ) {
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      rtems_set_errno_and_return_minus_one( EINVAL );
#endif

    case OBJECTS_ERROR:
      rtems_set_errno_and_return_minus_one( EINVAL );

    case OBJECTS_LOCAL:
      overrun = ptimer->overrun;
      ptimer->overrun = 0;
      _Thread_Enable_dispatch();
      return overrun;
  }
  return -1;   /* unreached - only to remove warnings */
}
