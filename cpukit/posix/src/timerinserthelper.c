/**
 * @file
 *
 * @brief Helper Routine for POSIX TIMERS 
 * @ingroup POSIXAPI
 */

/*
 *  Helper routine for POSIX timers
 *
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/isr.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/posix/timerimpl.h>
#include <rtems/posix/ptimer.h>

bool _POSIX_Timer_Insert_helper(
  Watchdog_Control               *timer,
  Watchdog_Interval               ticks,
  Objects_Id                      id,
  Watchdog_Service_routine_entry  TSR,
  void                           *arg
)
{
  ISR_lock_Context  lock_context;
  Watchdog_Header  *header;

  _Watchdog_Remove_ticks( timer );

  header = &_Watchdog_Ticks_header;
  _Watchdog_Acquire( header, &lock_context );

    /*
     *  Check to see if the watchdog has just been inserted by a
     *  higher priority interrupt.  If so, abandon this insert.
     */
    if ( timer->state != WATCHDOG_INACTIVE ) {
      _Watchdog_Release( header, &lock_context );
      return false;
    }

    /*
     *  OK.  Now we now the timer was not rescheduled by an interrupt
     *  so we can atomically initialize it as in use.
     */
    _Watchdog_Initialize( timer, TSR, id, arg );
    timer->initial = ticks;
    _Watchdog_Insert_locked( header, timer, &lock_context );
  _Watchdog_Release( header, &lock_context );
  return true;
}
