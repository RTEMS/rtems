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

#include <rtems/itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/itron/time.h>

/*
 *  dly_tsk - Delay Task
 */

ER dly_tsk(
  DLYTIME dlytim
)
{
  Watchdog_Interval ticks;

  ticks = TOD_MILLISECONDS_TO_TICKS(dlytim);

  _Thread_Disable_dispatch();
    if ( ticks == 0 ) {
      _Thread_Yield_processor();
    } else {
      _Thread_Set_state( _Thread_Executing, STATES_DELAYING );
      _Watchdog_Initialize(
        &_Thread_Executing->Timer,
        _Thread_Delay_ended,
        _Thread_Executing->Object.id,
        NULL
      );
      _Watchdog_Insert_ticks( &_Thread_Executing->Timer, ticks );
    }
  _Thread_Enable_dispatch();
  return E_OK;
}
