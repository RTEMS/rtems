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
 * _Thread_Suspend
 *
 * This kernel routine sets the SUSPEND state in the THREAD.  The
 * THREAD chain and suspend count are adjusted if necessary.
 *
 * Input parameters:
 *   the_thread   - pointer to thread control block
 *
 * Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select map
 */

void _Thread_Suspend(
  Thread_Control   *the_thread
)
{
  ISR_Level      level;
  Chain_Control *ready;

  ready = the_thread->ready;
  _ISR_Disable( level );
  the_thread->suspend_count++;
  if ( !_States_Is_ready( the_thread->current_state ) ) {
    the_thread->current_state =
       _States_Set( STATES_SUSPENDED, the_thread->current_state );
    _ISR_Enable( level );
    return;
  }

  the_thread->current_state = STATES_SUSPENDED;

  if ( _Chain_Has_only_one_node( ready ) ) {

    _Chain_Initialize_empty( ready );
    _Priority_Remove_from_bit_map( &the_thread->Priority_map );

  } else
    _Chain_Extract_unprotected( &the_thread->Object.Node );

  _ISR_Flash( level );

  if ( _Thread_Is_heir( the_thread ) )
     _Thread_Calculate_heir();

  if ( _Thread_Is_executing( the_thread ) )
    _Context_Switch_necessary = TRUE;

  _ISR_Enable( level );
}
