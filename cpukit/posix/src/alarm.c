/*
 *  3.4.1 Schedule Alarm, P1003.1b-1993, p. 79
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */


#include <pthread.h>
#include <errno.h>

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
      remaining = the_timer->initial -
                  (the_timer->stop_time - the_timer->start_time);
      break;
  }

  _Watchdog_Insert_seconds( the_timer, seconds );

  return remaining;
}

