/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>

/*
 *  _Thread_Restart
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
boolean _Thread_Restart(
  Thread_Control      *the_thread,
  void                *pointer_argument,
  unsigned32           numeric_argument
)
{
  if ( !_States_Is_dormant( the_thread->current_state ) ) {
 
    _Thread_Set_transient( the_thread );
    the_thread->resource_count = 0;
    the_thread->is_preemptible   = the_thread->Start.is_preemptible;
    the_thread->budget_algorithm = the_thread->Start.budget_algorithm;
    the_thread->budget_callout   = the_thread->Start.budget_callout;

    the_thread->Start.pointer_argument = pointer_argument;
    the_thread->Start.numeric_argument = numeric_argument;
 
    if ( !_Thread_queue_Extract_with_proxy( the_thread ) ) {
 
      if ( _Watchdog_Is_active( &the_thread->Timer ) )
        (void) _Watchdog_Remove( &the_thread->Timer );
    }

    if ( the_thread->current_priority != the_thread->Start.initial_priority ) {
      the_thread->real_priority = the_thread->Start.initial_priority;
      _Thread_Set_priority( the_thread, the_thread->Start.initial_priority );
    }
 
    _Thread_Load_environment( the_thread );
 
    _Thread_Ready( the_thread );
 
    _User_extensions_Thread_restart( the_thread );
 
    if ( _Thread_Is_executing ( the_thread ) )
      _Thread_Restart_self();
 
    return TRUE;
  }
 
  return FALSE;
}
