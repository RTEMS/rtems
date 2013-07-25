/**
 * @file
 *
 * @brief Schedule Alarm
 * @ingroup POSIXAPI
 */

/*
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

#include <signal.h>
#include <unistd.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/time.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdogimpl.h>

static void _POSIX_signals_Ualarm_TSR( Objects_Id id, void *argument );

static Watchdog_Control _POSIX_signals_Ualarm_timer = WATCHDOG_INITIALIZER(
  _POSIX_signals_Ualarm_TSR,
  0,
  NULL
);

/*
 *  _POSIX_signals_Ualarm_TSR
 */

static void _POSIX_signals_Ualarm_TSR(
  Objects_Id      id __attribute__((unused)),
  void           *argument __attribute__((unused))
)
{
  /*
   * Send a SIGALRM but if there is a problem, ignore it.
   * It's OK, there isn't a way this should fail.
   */
  (void) kill( getpid(), SIGALRM );

  /*
   * If the reset interval is non-zero, reschedule ourselves.
   */
  _Watchdog_Reset( &_POSIX_signals_Ualarm_timer );
}

useconds_t ualarm(
  useconds_t useconds,
  useconds_t interval
)
{
  useconds_t        remaining = 0;
  Watchdog_Control *the_timer;
  Watchdog_Interval ticks;
  Watchdog_States   state;
  struct timespec   tp;

  the_timer = &_POSIX_signals_Ualarm_timer;

  _Thread_Disable_dispatch();

  state = _Watchdog_Remove( the_timer );
  if ( (state == WATCHDOG_ACTIVE) || (state == WATCHDOG_REMOVE_IT) ) {
    /*
     *  The stop_time and start_time fields are snapshots of ticks since
     *  boot.  Since alarm() is dealing in seconds, we must account for
     *  this.
     */

    ticks = the_timer->initial;
    ticks -= (the_timer->stop_time - the_timer->start_time);
    /* remaining is now in ticks */

    _Timespec_From_ticks( ticks, &tp );
    remaining  = tp.tv_sec * TOD_MICROSECONDS_PER_SECOND;
    remaining += tp.tv_nsec / 1000;
  }

  /*
   *  If useconds is non-zero, then the caller wants to schedule
   *  the alarm repeatedly at that interval.  If the interval is
   *  less than a single clock tick, then fudge it to a clock tick.
   */
  if ( useconds ) {
    Watchdog_Interval ticks;

    tp.tv_sec = useconds / TOD_MICROSECONDS_PER_SECOND;
    tp.tv_nsec = (useconds % TOD_MICROSECONDS_PER_SECOND) * 1000;
    ticks = _Timespec_To_ticks( &tp );
    if ( ticks == 0 )
      ticks = 1;

    _Watchdog_Insert_ticks( the_timer, _Timespec_To_ticks( &tp ) );
  }

  _Thread_Enable_dispatch();

  return remaining;
}
