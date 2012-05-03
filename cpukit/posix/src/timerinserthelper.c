/*
 *  Helper routine for POSIX timers
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

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/isr.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/timer.h>
#include <rtems/posix/ptimer.h>

bool _POSIX_Timer_Insert_helper(
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
      return false;
    }

    /*
     *  OK.  Now we now the timer was not rescheduled by an interrupt
     *  so we can atomically initialize it as in use.
     */
    _Watchdog_Initialize( timer, TSR, id, arg );
    _Watchdog_Insert_ticks( timer, ticks );
  _ISR_Enable( level );
  return true;
}
