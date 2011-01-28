/*
 *  Thread Handler
 *
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

/*PAGE
 *
 *  _Thread_Resume
 *
 *  This kernel routine clears the SUSPEND state if the suspend_count
 *  drops below one.  If the force parameter is set the suspend_count
 *  is forced back to zero. The thread ready chain is adjusted if
 *  necessary and the Heir thread is set accordingly.
 *
 *  Input parameters:
 *    the_thread - pointer to thread control block
 *    force      - force the suspend count back to 0
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    priority map
 *    select heir
 */


void _Thread_Resume(
  Thread_Control   *the_thread,
  bool              force
)
{

  ISR_Level       level;
  States_Control  current_state;

  _ISR_Disable( level );

  current_state = the_thread->current_state;
  if ( current_state & STATES_SUSPENDED ) {
    current_state =
    the_thread->current_state = _States_Clear(STATES_SUSPENDED, current_state);

    if ( _States_Is_ready( current_state ) ) {
      _Scheduler_Unblock( &_Scheduler, the_thread );
    }
  }

  _ISR_Enable( level );
}
