/*
 * _POSIX_Timer_TSR
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <pthread.h>
#include <signal.h>

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/posix/time.h>
#include <rtems/posix/ptimer.h>
#include <rtems/posix/timer.h>

/*
 *  This is the operation that is run when a timer expires
 */
void _POSIX_Timer_TSR(
  Objects_Id timer __attribute__((unused)),
  void *data)
{
  POSIX_Timer_Control *ptimer;
  bool                 activated;

  ptimer = (POSIX_Timer_Control *)data;

  /* Increment the number of expirations. */
  ptimer->overrun = ptimer->overrun + 1;

  /* The timer must be reprogrammed */
  if ( ( ptimer->timer_data.it_interval.tv_sec  != 0 ) ||
       ( ptimer->timer_data.it_interval.tv_nsec != 0 ) ) {
    activated = _POSIX_Timer_Insert_helper(
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
