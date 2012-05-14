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
 *  _Watchdog_Insert
 *
 *  This routine inserts a watchdog timer on to the appropriate delta
 *  chain while updating the delta interval counters.
 */

void _Watchdog_Insert(
  Chain_Control         *header,
  Watchdog_Control      *the_watchdog
)
{
  ISR_Level          level;
  Watchdog_Control  *after;
  uint32_t           insert_isr_nest_level;
  Watchdog_Interval  delta_interval;


  insert_isr_nest_level   = _ISR_Nest_level;

  _ISR_Disable( level );

  /*
   *  Check to see if the watchdog has just been inserted by a
   *  higher priority interrupt.  If so, abandon this insert.
   */

  if ( the_watchdog->state != WATCHDOG_INACTIVE ) {
    _ISR_Enable( level );
    return;
  }

  the_watchdog->state = WATCHDOG_BEING_INSERTED;
  _Watchdog_Sync_count++;

restart:
  delta_interval = the_watchdog->initial;

  for ( after = _Watchdog_First( header ) ;
        ;
        after = _Watchdog_Next( after ) ) {

     if ( delta_interval == 0 || !_Watchdog_Next( after ) )
       break;

     if ( delta_interval < after->delta_interval ) {
       after->delta_interval -= delta_interval;
       break;
     }

     delta_interval -= after->delta_interval;

     _ISR_Flash( level );

     if ( the_watchdog->state != WATCHDOG_BEING_INSERTED ) {
       goto exit_insert;
     }

     if ( _Watchdog_Sync_level > insert_isr_nest_level ) {
       _Watchdog_Sync_level = insert_isr_nest_level;
       goto restart;
     }
  }

  _Watchdog_Activate( the_watchdog );

  the_watchdog->delta_interval = delta_interval;

  _Chain_Insert_unprotected( after->Node.previous, &the_watchdog->Node );

  the_watchdog->start_time = _Watchdog_Ticks_since_boot;

exit_insert:
  _Watchdog_Sync_level = insert_isr_nest_level;
  _Watchdog_Sync_count--;
  _ISR_Enable( level );
}
