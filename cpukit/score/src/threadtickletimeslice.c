/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

/*PAGE
 *
 *  _Thread_Tickle_timeslice
 *
 *  This scheduler routine determines if timeslicing is enabled
 *  for the currently executing thread and, if so, updates the
 *  timeslice count and checks for timeslice expiration.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Thread_Tickle_timeslice( void )
{
  Thread_Control *executing;

  executing = _Thread_Executing;

  /*
   *  Increment the number of ticks this thread has been executing
   */

  executing->ticks_executed++;

  /*
   *  If the thread is not preemptible or is not ready, then 
   *  just return.
   */

  if ( !executing->is_preemptible )
    return;

  if ( !_States_Is_ready( executing->current_state ) )
    return;

  /*
   *  The cpu budget algorithm determines what happens next.
   */

  switch ( executing->budget_algorithm ) {
    case THREAD_CPU_BUDGET_ALGORITHM_NONE:
      break;

    case THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE:
    case THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE:
      if ( --executing->cpu_time_budget == 0 ) {
        _Thread_Reset_timeslice();
        executing->cpu_time_budget = _Thread_Ticks_per_timeslice;
      }
      break;

    case THREAD_CPU_BUDGET_ALGORITHM_CALLOUT:
      if ( --executing->cpu_time_budget == 0 )
        (*executing->budget_callout)( executing );
      break;
  }
}
