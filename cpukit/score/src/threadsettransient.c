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
 *  _Thread_Set_transient
 *
 *  This kernel routine places the requested thread in the transient state
 *  which will remove it from the ready queue, if necessary.  No
 *  rescheduling is necessary because it is assumed that the transient
 *  state will be cleared before dispatching is enabled.
 *
 *  Input parameters:
 *    the_thread - pointer to thread control block
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Thread_Set_transient(
  Thread_Control *the_thread
)
{
  ISR_Level             level;
  unsigned32            old_state;
  Chain_Control *ready;

  ready = the_thread->ready;
  _ISR_Disable( level );

  old_state = the_thread->current_state;
  the_thread->current_state = _States_Set( STATES_TRANSIENT, old_state );

  if ( _States_Is_ready( old_state ) ) {
    if ( _Chain_Has_only_one_node( ready ) ) {

      _Chain_Initialize_empty( ready );
      _Priority_Remove_from_bit_map( &the_thread->Priority_map );

    } else
      _Chain_Extract_unprotected( &the_thread->Object.Node );
  }

  _ISR_Enable( level );

}
