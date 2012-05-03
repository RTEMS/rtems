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
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/watchdog.h>

/*
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

  /*
   * For some reason, on rare occasions the_watchdog->delta_interval
   * of the head of the watchdog chain is 0.  Before this test was
   * added, on these occasions an event (which usually was supposed
   * to have a timeout of 1 tick would have a delta_interval of 0, which
   * would be decremented to 0xFFFFFFFF by the unprotected
   * "the_watchdog->delta_interval--;" operation.
   * This would mean the event would not timeout, and also the chain would
   * be blocked, because a timeout with a very high number would be at the
   * head, rather than at the end.
   * The test "if (the_watchdog->delta_interval != 0)"
   * here prevents this from occuring.
   *
   * We were not able to categorically identify the situation that causes
   * this, but proved it to be true empirically.  So this check causes
   * correct behaviour in this circumstance.
   *
   * The belief is that a race condition exists whereby an event at the head
   * of the chain is removed (by a pending ISR or higher priority task)
   * during the _ISR_Flash( level ); in _Watchdog_Insert, but the watchdog
   * to be inserted has already had its delta_interval adjusted to 0, and
   * so is added to the head of the chain with a delta_interval of 0.
   *
   * Steven Johnson - 12/2005 (gcc-3.2.3 -O3 on powerpc)
   */
  if (the_watchdog->delta_interval != 0) {
    the_watchdog->delta_interval--;
    if ( the_watchdog->delta_interval != 0 )
      goto leave;
  }

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
