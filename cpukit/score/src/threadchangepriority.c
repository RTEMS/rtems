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
  boolean           prepend_it
)
{
  ISR_Level level;
  /* boolean   do_prepend = FALSE; */

  /*
   *  If this is a case where prepending the task to its priority is
   *  potentially desired, then we need to consider whether to do it.
   *  This usually occurs when a task lowers its priority implcitly as
   *  the result of losing inherited priority.  Normal explicit priority
   *  change calls (e.g. rtems_task_set_priority) should always do an
   *  append not a prepend.
   */
 
  /*
   *  Techically, the prepend should conditional on the thread lowering
   *  its priority but that does allow cxd2004 of the acvc 2.0.1 to 
   *  pass with rtems 4.0.0.  This should change when gnat redoes its
   *  priority scheme.
   */
/*
  if ( prepend_it &&
       _Thread_Is_executing( the_thread ) && 
       new_priority >= the_thread->current_priority )
    prepend_it = TRUE;
*/
                  
  _Thread_Set_transient( the_thread );

  if ( the_thread->current_priority != new_priority )
    _Thread_Set_priority( the_thread, new_priority );

  _ISR_Disable( level );

  the_thread->current_state =
    _States_Clear( STATES_TRANSIENT, the_thread->current_state );

  if ( ! _States_Is_ready( the_thread->current_state ) ) {
    /*
     *  XXX If a task is to be reordered while blocked on a priority
     *  XXX priority ordered thread queue, then this is where that
     *  XXX should occur.
     */
    _ISR_Enable( level );
    return;
  }

  _Priority_Add_to_bit_map( &the_thread->Priority_map );
  if ( prepend_it )
    _Chain_Prepend_unprotected( the_thread->ready, &the_thread->Object.Node );
  else
    _Chain_Append_unprotected( the_thread->ready, &the_thread->Object.Node );

  _ISR_Flash( level );

  _Thread_Calculate_heir();

  if ( !_Thread_Is_executing_also_the_heir() &&
       _Thread_Executing->is_preemptible )
    _Context_Switch_necessary = TRUE;
  _ISR_Enable( level );
}
