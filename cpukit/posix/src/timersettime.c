/**
 * @file
 *
 * @brief Function Arms or Disarms the Timer Identified by timerid 
 * @ingroup POSIXAPI
 */

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <errno.h>

#include <rtems/posix/ptimer.h>
#include <rtems/posix/timerimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/seterr.h>

static void _POSIX_Timer_Insert(
  POSIX_Timer_Control *ptimer,
  Per_CPU_Control     *cpu,
  Watchdog_Interval    ticks
)
{
  /* The state really did not change but just to be safe */
  ptimer->state = POSIX_TIMER_STATE_CREATE_RUN;

  /* Store the time when the timer was started again */
  _TOD_Get( &ptimer->time );

  _Watchdog_Insert(
    &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
    &ptimer->Timer,
    cpu->Watchdog.ticks + ticks
  );
}

/*
 *  This is the operation that is run when a timer expires
 */
void _POSIX_Timer_TSR( Watchdog_Control *the_watchdog )
{
  POSIX_Timer_Control *ptimer;
  ISR_lock_Context     lock_context;
  Per_CPU_Control     *cpu;

  ptimer = RTEMS_CONTAINER_OF( the_watchdog, POSIX_Timer_Control, Timer );
  _ISR_lock_ISR_disable( &lock_context );
  cpu = _POSIX_Timer_Acquire_critical( ptimer, &lock_context );

  /* Increment the number of expirations. */
  ptimer->overrun = ptimer->overrun + 1;

  /* The timer must be reprogrammed */
  if ( ( ptimer->timer_data.it_interval.tv_sec  != 0 ) ||
       ( ptimer->timer_data.it_interval.tv_nsec != 0 ) ) {
    _POSIX_Timer_Insert( ptimer, cpu, ptimer->ticks );
  } else {
   /* Indicates that the timer is stopped */
   ptimer->state = POSIX_TIMER_STATE_CREATE_STOP;
  }

  _POSIX_Timer_Release( cpu, &lock_context );

  /*
   * The sending of the signal to the process running the handling function
   * specified for that signal is simulated
   */

  if ( pthread_kill ( ptimer->thread_id, ptimer->inf.sigev_signo ) ) {
    _Assert( FALSE );
    /*
     * TODO: What if an error happens at run-time? This should never
     *       occur because the timer should be canceled if the thread
     *       is deleted. This method is being invoked from the Clock
     *       Tick ISR so even if we decide to take action on an error,
     *       we don't have many options. We shouldn't shut the system down.
     */
  }

  /* After the signal handler returns, the count of expirations of the
   * timer must be set to 0.
   */
  ptimer->overrun = 0;
}

int timer_settime(
  timer_t                  timerid,
  int                      flags,
  const struct itimerspec *__restrict value,
  struct itimerspec       *__restrict ovalue
)
{
  POSIX_Timer_Control *ptimer;
  ISR_lock_Context     lock_context;
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

  ptimer = _POSIX_Timer_Get( timerid, &lock_context );
  if ( ptimer != NULL ) {
    Per_CPU_Control *cpu;

    cpu = _POSIX_Timer_Acquire_critical( ptimer, &lock_context );

    /* Stop the timer */
    _Watchdog_Remove(
      &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
      &ptimer->Timer
    );

    /* First, it verifies if the timer must be stopped */
    if ( normalize.it_value.tv_sec == 0 && normalize.it_value.tv_nsec == 0 ) {
      /* The old data of the timer are returned */
      if ( ovalue )
        *ovalue = ptimer->timer_data;
      /* The new data are set */
      ptimer->timer_data = normalize;
      /* Indicates that the timer is created and stopped */
      ptimer->state = POSIX_TIMER_STATE_CREATE_STOP;
      /* Returns with success */
      _POSIX_Timer_Release( cpu, &lock_context );
      return 0;
    }

    /* Convert from seconds and nanoseconds to ticks */
    ptimer->ticks  = _Timespec_To_ticks( &value->it_interval );
    initial_period = _Timespec_To_ticks( &normalize.it_value );

    _POSIX_Timer_Insert( ptimer, cpu, initial_period );

    /*
     * The timer has been started and is running.  So we return the
     * old ones in "ovalue"
     */
    if ( ovalue )
      *ovalue = ptimer->timer_data;
    ptimer->timer_data = normalize;
    _POSIX_Timer_Release( cpu, &lock_context );
    return 0;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
