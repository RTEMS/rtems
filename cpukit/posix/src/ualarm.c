/*
 *  XXX 3.4.1 Schedule Alarm, P1003.1b-1993, p. 79
 *
 *  COPYRIGHT (c) 1989-2003.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
/* #include <errno.h> */

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/time.h>

Watchdog_Control _POSIX_signals_Ualarm_timer;

/*PAGE
 *
 *  _POSIX_signals_Ualarm_TSR
 */
 
void _POSIX_signals_Ualarm_TSR(
  Objects_Id      id,
  void           *argument
)
{
  int status;

  status = kill( getpid(), SIGALRM );
  /* XXX can't print from an ISR, should this be fatal? */
}

useconds_t ualarm(
  useconds_t useconds,
  useconds_t interval
)
{
  useconds_t        remaining = 0;
  Watchdog_Control *the_timer;
  Watchdog_Interval ticks;
  struct timespec   tp;

  the_timer = &_POSIX_signals_Ualarm_timer;

  /*
   *  Initialize the timer used to implement alarm().
   */

  if ( !the_timer->routine ) {
    _Watchdog_Initialize( the_timer, _POSIX_signals_Ualarm_TSR, 0, NULL );
  } else {
    switch ( _Watchdog_Remove( the_timer ) ) {
      case WATCHDOG_INACTIVE:
      case WATCHDOG_BEING_INSERTED:
        break;

      case WATCHDOG_ACTIVE:
      case WATCHDOG_REMOVE_IT:
        /*
         *  The stop_time and start_time fields are snapshots of ticks since
         *  boot.  Since alarm() is dealing in seconds, we must account for
         *  this.
         */

	
        ticks = the_timer->initial -
         ((the_timer->stop_time - the_timer->start_time) /
	   _TOD_Ticks_per_second);

        _POSIX_Interval_to_timespec( ticks, &tp );
        remaining  = tp.tv_sec * TOD_MICROSECONDS_PER_SECOND;
        remaining += tp.tv_nsec / 1000;
        break;
    }
  }

  tp.tv_sec = useconds / TOD_MICROSECONDS_PER_SECOND;
  tp.tv_nsec = (useconds % TOD_MICROSECONDS_PER_SECOND) * 1000;
  ticks = _POSIX_Timespec_to_interval( &tp );
  _Watchdog_Insert_ticks( the_timer, ticks );

  return remaining;
}

