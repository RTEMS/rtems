/*
 *  3.4.1 Schedule Alarm, P1003.1b-1993, p. 79
 *
 *  COPYRIGHT (c) 1989-1999.
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

#include <pthread.h>
/* #include <errno.h> */

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/psignal.h>

unsigned int alarm(
  unsigned int seconds
)
{
  unsigned int      remaining = 0;
  Watchdog_Control *the_timer;

  the_timer = &_POSIX_signals_Alarm_timer;

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

      remaining = the_timer->initial -
       ((the_timer->stop_time - the_timer->start_time) / _TOD_Ticks_per_second);
      break;
  }

  _Watchdog_Insert_seconds( the_timer, seconds );

  return remaining;
}

