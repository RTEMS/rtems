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
 *  _Thread_Rotate_Ready_Queue
 *
 *  This kernel routine will rotate the ready queue.
 *  remove the running THREAD from the ready chain
 *  and place it immediatly at the rear of this chain.  Reset timeslice
 *  and yield the processor functions both use this routine, therefore if
 *  reset is TRUE and this is the only thread on the chain then the
 *  timeslice counter is reset.  The heir THREAD will be updated if the
 *  running is also the currently the heir.
 *
 *  Input parameters:  
 *      Priority of the queue we wish to modify.
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select heir
 */

void _Thread_Rotate_Ready_Queue( 
  Priority_Control  priority
)
{
  ISR_Level       level;
  Thread_Control *executing;
  Chain_Control  *ready;
  Chain_Node     *node;

  ready     = &_Thread_Ready_chain[ priority ];
  executing = _Thread_Executing;

  if ( ready == executing->ready ) {
    _Thread_Yield_processor();
    return;
  }

  _ISR_Disable( level );

  if ( !_Chain_Is_empty( ready ) ) {
    if (!_Chain_Has_only_one_node( ready ) ) {
      node = _Chain_Get_first_unprotected( ready );
      _Chain_Append_unprotected( ready, node );
    }
  }

  _ISR_Flash( level );

  if ( _Thread_Heir->ready == ready )
    _Thread_Heir = (Thread_Control *) ready->first;

  if ( executing != _Thread_Heir )
    _Context_Switch_necessary = TRUE;

  _ISR_Enable( level );
}






