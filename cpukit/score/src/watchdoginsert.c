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
  unsigned32         insert_isr_nest_level;
  Watchdog_Interval  delta_interval;
  

  insert_isr_nest_level   = _ISR_Nest_level;
  the_watchdog->state = WATCHDOG_BEING_INSERTED;

  _ISR_Disable( level );

  _Watchdog_Sync_count++;

restart:
  delta_interval = the_watchdog->initial;

  /*
   * We CANT use _Watchdog_First() here, because a TICK interrupt
   * could modify the chain during the _ISR_Flash() below. Hence,
   * the header is pointing to volatile data. The _Watchdog_First()
   * INLINE routine (but not the macro - note the subtle difference)
   * casts away the 'volatile'...
   *
   * Also, this is only necessary because we call no other routine
   * from this piece of code, hence the compiler thinks it's safe to
   * cache *header!!
   *
   *  Till Straumann, 7/2003 (gcc-3.2.2 -O4 on powerpc)
   *
   */
  for ( after = (Watchdog_Control *) ((volatile Chain_Control *)header)->first ;
        ;
        after = _Watchdog_Next( after ) ) {

     if ( delta_interval == 0 || !_Watchdog_Next( after ) )
       break;

     if ( delta_interval < after->delta_interval ) {
       after->delta_interval -= delta_interval;
       break;
     }

     delta_interval -= after->delta_interval;

     /*
      *  If you experience problems comment out the _ISR_Flash line.  
      *  3.2.0 was the first release with this critical section redesigned.
      *  Under certain circumstances, the PREVIOUS critical section algorithm
      *  used around this flash point allowed interrupts to execute
      *  which violated the design assumptions.  The critical section 
      *  mechanism used here WAS redesigned to address this.
      */

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

