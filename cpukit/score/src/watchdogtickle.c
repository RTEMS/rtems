/*
 *  Watchdog Handler
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  _Watchdog_Tickle
 *
 *  This routine decrements the delta counter in response to a tick.  The
 *  delta chain is updated accordingly.
 *
 *  Input parameters:
 *    header - pointer to the delta chain to be tickled
 *
 *  Output parameters: NONE
 */

void _Watchdog_Tickle(
  Chain_Control *header
)
{
  Watchdog_Control *the_watchdog;

  if ( _Chain_Is_empty( header ) )
    return;

  the_watchdog = _Watchdog_First( header );
  the_watchdog->delta_interval--;
  if ( the_watchdog->delta_interval != 0 )
    return;

  do {
     switch( _Watchdog_Remove( the_watchdog ) ) {
       case WATCHDOG_ACTIVE:
         (*the_watchdog->routine)(
           the_watchdog->id,
           the_watchdog->user_data
         );
         break;

       case WATCHDOG_INACTIVE:
         /*
          *  This state indicates that the watchdog is not on any chain.
          *  Thus, it is NOT on a chain being tickled.  This case should 
          *  never occur.
          */
         break;

       case WATCHDOG_BEING_INSERTED:
         /*
          *  This state indicates that the watchdog is in the process of
          *  BEING inserted on the chain.  Thus, it can NOT be on a chain
          *  being tickled.  This case should never occur.
          */
         break;

       case WATCHDOG_REMOVE_IT:
         break;
     }
     the_watchdog = _Watchdog_First( header );
   } while ( !_Chain_Is_empty( header ) &&
             (the_watchdog->delta_interval == 0) );
}
