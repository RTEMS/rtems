/**
 * @file
 *
 * @brief Remove Watchdog from List
 * @ingroup ScoreWatchdog
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/watchdogimpl.h>
#include <rtems/score/assert.h>

static void _Watchdog_Remove_it(
  Watchdog_Header   *header,
  Watchdog_Control  *the_watchdog
)
{
  Chain_Node        *next;
  Watchdog_Interval  delta;
  const Chain_Node  *iterator_tail;
  Chain_Node        *iterator_node;

  _Assert( the_watchdog->state == WATCHDOG_ACTIVE );

  the_watchdog->state = WATCHDOG_INACTIVE;
  the_watchdog->stop_time = _Watchdog_Ticks_since_boot;

  next = _Chain_Next( &the_watchdog->Node );
  delta = the_watchdog->delta_interval;

  if ( next != _Chain_Tail( &header->Watchdogs ) ) {
    Watchdog_Control *next_watchdog;

    next_watchdog = (Watchdog_Control *) next;
    next_watchdog->delta_interval += delta;
  }

  _Chain_Extract_unprotected( &the_watchdog->Node );

  iterator_node = _Chain_First( &header->Iterators );
  iterator_tail = _Chain_Immutable_tail( &header->Iterators );

  while ( iterator_node != iterator_tail ) {
    Watchdog_Iterator *iterator;

    iterator = (Watchdog_Iterator *) iterator_node;

    if ( iterator->current == next ) {
      iterator->delta_interval += delta;
    }

    if ( iterator->current == &the_watchdog->Node ) {
      Chain_Node *previous = _Chain_Previous( &the_watchdog->Node );

      iterator->current = previous;

      if ( previous != _Chain_Head( &header->Watchdogs ) ) {
        Watchdog_Control *previous_watchdog;

        previous_watchdog = (Watchdog_Control *) previous;
        iterator->delta_interval += previous_watchdog->delta_interval;
      }
    }

    iterator_node = _Chain_Next( iterator_node );
  }
}

Watchdog_States _Watchdog_Remove(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog
)
{
  ISR_lock_Context  lock_context;
  Watchdog_States   previous_state;
  Watchdog_Interval now;

  _Watchdog_Acquire( header, &lock_context );
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
      now = _Watchdog_Ticks_since_boot;
      the_watchdog->start_time = now;
      the_watchdog->stop_time = now;
      break;

    case WATCHDOG_ACTIVE:
      _Watchdog_Remove_it( header, the_watchdog );
      break;
  }

  _Watchdog_Release( header, &lock_context );
  return( previous_state );
}

void _Watchdog_Tickle(
  Watchdog_Header *header
)
{
  ISR_lock_Context lock_context;

  _Watchdog_Acquire( header, &lock_context );

  if ( !_Watchdog_Is_empty( header ) ) {
    Watchdog_Control  *first;
    Watchdog_Interval  delta;

    first = _Watchdog_First( header );
    delta = first->delta_interval;

    /*
     * Although it is forbidden to insert watchdogs with a delta interval of
     * zero it is possible to observe watchdogs with a delta interval of zero
     * at this point.  For example lets have a watchdog chain of one watchdog
     * with a delta interval of one and insert a new one with an initial value
     * of one.  At the start of the insert procedure it will advance one step
     * and reduce its delta interval by one yielding zero.  Now a tick happens.
     * This will remove the watchdog on the chain and update the insert
     * iterator.  Now the insert operation continues and will insert the new
     * watchdog with a delta interval of zero.
     */
    if ( delta > 0 ) {
      --delta;
      first->delta_interval = delta;
    }

    while ( delta == 0 ) {
      bool                            run;
      Watchdog_Service_routine_entry  routine;
      Objects_Id                      id;
      void                           *user_data;

      run = ( first->state == WATCHDOG_ACTIVE );

      _Watchdog_Remove_it( header, first );

      routine = first->routine;
      id = first->id;
      user_data = first->user_data;

      _Watchdog_Release( header, &lock_context );

      if ( run ) {
        (*routine)( id, user_data );
      }

      _Watchdog_Acquire( header, &lock_context );

      if ( _Watchdog_Is_empty( header ) ) {
        break;
      }

      first = _Watchdog_First( header );
      delta = first->delta_interval;
    }
  }

  _Watchdog_Release( header, &lock_context );
}
