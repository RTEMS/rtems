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

void _Watchdog_Remove_it(
  Watchdog_Header   *header,
  Watchdog_Control  *the_watchdog
)
{
  Chain_Node        *next;
  Watchdog_Interval  delta;
  const Chain_Node  *iterator_tail;
  Chain_Node        *iterator_node;

  _Assert(
    the_watchdog->state == WATCHDOG_ACTIVE
      || the_watchdog->state == WATCHDOG_REMOVE_IT
  );

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
      iterator->current = _Chain_Previous( &the_watchdog->Node );
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
    case WATCHDOG_REMOVE_IT:
      _Watchdog_Remove_it( header, the_watchdog );
      break;
  }

  _Watchdog_Release( header, &lock_context );
  return( previous_state );
}
