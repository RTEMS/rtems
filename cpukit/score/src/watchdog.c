/*
 *  Watchdog Handler
 *
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  _Watchdog_Handler_initialization
 *
 *  This routine initializes the watchdog handler.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 */

void _Watchdog_Handler_initialization( void )
{
  _Watchdog_Sync_count = 0;
  _Watchdog_Sync_level = 0;
  _Watchdog_Ticks_since_boot = 0;

  _Chain_Initialize_empty( &_Watchdog_Ticks_chain );
  _Chain_Initialize_empty( &_Watchdog_Seconds_chain );
}

/*PAGE
 *
 *  _Watchdog_Remove
 *
 *  The routine removes a watchdog from a delta chain and updates
 *  the delta counters of the remaining watchdogs.
 */

Watchdog_States _Watchdog_Remove(
  Watchdog_Control *the_watchdog
)
{
  ISR_Level                level;
  Watchdog_States          previous_state;
  Watchdog_Control *next_watchdog;

  _ISR_Disable( level );
  previous_state = the_watchdog->state;
  switch ( previous_state ) {
    case WATCHDOG_INACTIVE:
      break;

    case WATCHDOG_BEING_INSERTED:  
   
      /*
       *  It is not actually on the chain so just change the state and
       *  the Insert operation we interrupted will be aborted.
       */
      the_watchdog->state = WATCHDOG_INACTIVE;
      break;

    case WATCHDOG_ACTIVE:
    case WATCHDOG_REMOVE_IT:

      the_watchdog->state = WATCHDOG_INACTIVE;
      next_watchdog = _Watchdog_Next( the_watchdog );

      if ( _Watchdog_Next(next_watchdog) )
        next_watchdog->delta_interval += the_watchdog->delta_interval;

      if ( _Watchdog_Sync_count )
        _Watchdog_Sync_level = _ISR_Nest_level;

      _Chain_Extract_unprotected( &the_watchdog->Node );
      break;
  }
  the_watchdog->stop_time = _Watchdog_Ticks_since_boot;

  _ISR_Enable( level );
  return( previous_state );
}

/*PAGE
 *
 *  _Watchdog_Adjust
 *
 *  This routine adjusts the delta chain backward or forward in response
 *  to a time change.
 *
 *  Input parameters:
 *    header    - pointer to the delta chain to be adjusted
 *    direction - forward or backward adjustment to delta chain
 *    units     - units to adjust
 *
 *  Output parameters:
 */

void _Watchdog_Adjust(
  Chain_Control               *header,
  Watchdog_Adjust_directions   direction,
  Watchdog_Interval            units
)
{
  if ( !_Chain_Is_empty( header ) ) {
    switch ( direction ) {
      case WATCHDOG_BACKWARD:
        _Watchdog_First( header )->delta_interval += units;
        break;
      case WATCHDOG_FORWARD:
        while ( units ) {
          if ( units < _Watchdog_First( header )->delta_interval ) {
            _Watchdog_First( header )->delta_interval -= units;
            break;
          } else {
            units -= _Watchdog_First( header )->delta_interval;
            _Watchdog_First( header )->delta_interval = 1;
            _Watchdog_Tickle( header );
            if ( _Chain_Is_empty( header ) )
              break;
          }
        }
        break;
    }
  }
}

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

  _Watchdog_Sync_count++;
restart:
  delta_interval = the_watchdog->initial;

  _ISR_Disable( level );

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
       _ISR_Enable( level );
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
