/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
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
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>

/*PAGE
 *
 *  _Thread_Change_priority
 *
 *  This kernel routine changes the priority of the thread.  The
 *  thread chain is adjusted if necessary.
 *
 *  Input parameters:
 *    the_thread   - pointer to thread control block
 *    new_priority - ultimate priority
 *    prepend_it   - TRUE if the thread should be prepended to the chain
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select heir
 */

void _Thread_Change_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority,
  bool              prepend_it
)
{
  ISR_Level      level;
  States_Control state, original_state;

  /*
   *  If this is a case where prepending the task to its priority is
   *  potentially desired, then we need to consider whether to do it.
   *  This usually occurs when a task lowers its priority implcitly as
   *  the result of losing inherited priority.  Normal explicit priority
   *  change calls (e.g. rtems_task_set_priority) should always do an
   *  append not a prepend.
   */
/*
  if ( prepend_it &&
       _Thread_Is_executing( the_thread ) &&
       new_priority >= the_thread->current_priority )
    prepend_it = TRUE;
*/

  /*
   * Save original state
   */
  original_state = the_thread->current_state;

  /*
   * Set a transient state for the thread so it is pulled off the Ready chains.
   * This will prevent it from being scheduled no matter what happens in an
   * ISR.
   */
  _Thread_Set_transient( the_thread );

  /*
   *  Do not bother recomputing all the priority related information if
   *  we are not REALLY changing priority.
   */
 if ( the_thread->current_priority != new_priority )
    _Thread_Set_priority( the_thread, new_priority );

  _ISR_Disable( level );

  /*
   *  If the thread has more than STATES_TRANSIENT set, then it is blocked,
   *  If it is blocked on a thread queue, then we need to requeue it.
   */
  state = the_thread->current_state;
  if ( state != STATES_TRANSIENT ) {
    /* Only clear the transient state if it wasn't set already */
    if ( ! _States_Is_transient( original_state ) )
      the_thread->current_state = _States_Clear( STATES_TRANSIENT, state );
    _ISR_Enable( level );
    if ( _States_Is_waiting_on_thread_queue( state ) ) {
      _Thread_queue_Requeue( the_thread->Wait.queue, the_thread );
    }
    return;
  }

  /* Only clear the transient state if it wasn't set already */
  if ( ! _States_Is_transient( original_state ) ) {
    /*
     *  Interrupts are STILL disabled.
     *  We now know the thread will be in the READY state when we remove
     *  the TRANSIENT state.  So we have to place it on the appropriate
     *  Ready Queue with interrupts off.
     */
    the_thread->current_state = _States_Clear( STATES_TRANSIENT, state );

    _Priority_Add_to_bit_map( &the_thread->Priority_map );
    if ( prepend_it )
      _Chain_Prepend_unprotected( the_thread->ready, &the_thread->Object.Node );
    else
      _Chain_Append_unprotected( the_thread->ready, &the_thread->Object.Node );
  }

  _ISR_Flash( level );

  /*
   *  We altered the set of thread priorities.  So let's figure out
   *  who is the heir and if we need to switch to them.
   */
  _Thread_Calculate_heir();

  if ( !_Thread_Is_executing_also_the_heir() &&
       _Thread_Executing->is_preemptible )
    _Context_Switch_necessary = TRUE;
  _ISR_Enable( level );
}
