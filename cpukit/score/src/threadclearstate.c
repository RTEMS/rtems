/*
 *  Thread Handler / Thread Clear State
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>

/*
 *  INTERRUPT LATENCY:
 *    priority map
 *    select heir
 */
void _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
)
{
  ISR_Level       level;
  States_Control  current_state;

  _ISR_Disable( level );
    current_state = the_thread->current_state;

    if ( current_state & state ) {
      current_state =
      the_thread->current_state = _States_Clear( state, current_state );

      if ( _States_Is_ready( current_state ) ) {
        _Scheduler_Unblock( the_thread );
      }
  }
  _ISR_Enable( level );
}
