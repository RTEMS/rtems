/*
 *  Watchdog Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
  ISR_Level level;
  Watchdog_Control *the_watchdog;
  Watchdog_States  watchdog_state;

  /*
   * See the comment in watchdoginsert.c and watchdogadjust.c
   * about why it's safe not to declare header a pointer to 
   * volatile data - till, 2003/7
   */

  _ISR_Disable( level );

  if ( _Chain_Is_empty( header ) )
    goto leave;

  the_watchdog = _Watchdog_First( header );
  the_watchdog->delta_interval--;
  if ( the_watchdog->delta_interval != 0 )
    goto leave;

  do {
	 watchdog_state = _Watchdog_Remove( the_watchdog );

	 _ISR_Enable( level );

     switch( watchdog_state ) {
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

	 _ISR_Disable( level );

     the_watchdog = _Watchdog_First( header );
   } while ( !_Chain_Is_empty( header ) &&
             (the_watchdog->delta_interval == 0) );

leave:
   _ISR_Enable(level);
}
