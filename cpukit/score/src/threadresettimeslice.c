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
 *  _Thread_Reset_timeslice
 *
 *  This routine will remove the running thread from the ready chain
 *  and place it immediately at the rear of this chain and then the
 *  timeslice counter is reset.  The heir THREAD will be updated if
 *  the running is also the currently the heir.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select heir
 */

void _Thread_Reset_timeslice( void )
{
  ISR_Level       level;
  Thread_Control *executing;
  Chain_Control  *ready;

  executing = _Thread_Executing;
  ready     = executing->ready;
  _ISR_Disable( level );
    if ( _Chain_Has_only_one_node( ready ) ) {
      _ISR_Enable( level );
      return;
    }
    _Chain_Extract_unprotected( &executing->Object.Node );
    _Chain_Append_unprotected( ready, &executing->Object.Node );

  _ISR_Flash( level );

    if ( _Thread_Is_heir( executing ) )
      _Thread_Heir = (Thread_Control *) ready->first;

    _Context_Switch_necessary = TRUE;

  _ISR_Enable( level );
}
