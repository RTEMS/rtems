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
  boolean           force
)
{

  ISR_Level       level;
  States_Control  current_state;

  _ISR_Disable( level );
  
  if ( force == TRUE )
    the_thread->suspend_count = 0;
  else
    the_thread->suspend_count--;

  if ( the_thread->suspend_count > 0 ) {
    _ISR_Enable( level );
    return;
  }

  current_state = the_thread->current_state;
  if ( current_state & STATES_SUSPENDED ) {
    current_state = 
    the_thread->current_state = _States_Clear(STATES_SUSPENDED, current_state);

    if ( _States_Is_ready( current_state ) ) {

      _Priority_Add_to_bit_map( &the_thread->Priority_map );

      _Chain_Append_unprotected(the_thread->ready, &the_thread->Object.Node);

      _ISR_Flash( level );

      if ( the_thread->current_priority < _Thread_Heir->current_priority ) {
        _Thread_Heir = the_thread;
        if ( _Thread_Executing->is_preemptible ||
             the_thread->current_priority == 0 )
          _Context_Switch_necessary = TRUE;
      }
    }
  }

  _ISR_Enable( level );
}
