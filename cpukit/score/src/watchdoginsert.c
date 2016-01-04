/**
 * @file 
 *
 * @brief Watchdog Insert
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

static void _Watchdog_Insert_fixup(
  Watchdog_Header   *header,
  Watchdog_Control  *the_watchdog,
  Watchdog_Interval  delta,
  Watchdog_Control  *next_watchdog,
  Watchdog_Interval  delta_next
)
{
  const Chain_Node *iterator_tail;
  Chain_Node       *iterator_node;

  next_watchdog->delta_interval = delta_next - delta;

  iterator_node = _Chain_First( &header->Iterators );
  iterator_tail = _Chain_Immutable_tail( &header->Iterators );

  while ( iterator_node != iterator_tail ) {
    Watchdog_Iterator *iterator;

    iterator = (Watchdog_Iterator *) iterator_node;

    if ( iterator->current == &next_watchdog->Node ) {
      iterator->current = &the_watchdog->Node;
    }

    iterator_node = _Chain_Next( iterator_node );
  }
}

void _Watchdog_Insert_locked(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog,
  ISR_lock_Context *lock_context
)
{
  if ( the_watchdog->state == WATCHDOG_INACTIVE ) {
    Watchdog_Iterator  iterator;
    Chain_Node        *current;
    Chain_Node        *next;
    Watchdog_Interval  delta;

    the_watchdog->state = WATCHDOG_BEING_INSERTED;

    _Chain_Append_unprotected( &header->Iterators, &iterator.Node );

    delta = the_watchdog->initial;
    current = _Chain_Head( &header->Watchdogs );

    while (
      ( next = _Chain_Next( current ) ) != _Chain_Tail( &header->Watchdogs )
    ) {
      Watchdog_Control  *next_watchdog;
      Watchdog_Interval  delta_next;

      next_watchdog = (Watchdog_Control *) next;
      delta_next = next_watchdog->delta_interval;

      if ( delta < delta_next ) {
        _Watchdog_Insert_fixup(
          header,
          the_watchdog,
          delta,
          next_watchdog,
          delta_next
        );
        break;
      }

      iterator.delta_interval = delta - delta_next;
      iterator.current = next;

      _Watchdog_Flash( header, lock_context );

      if ( the_watchdog->state != WATCHDOG_BEING_INSERTED ) {
        goto abort_insert;
      }

      delta = iterator.delta_interval;
      current = iterator.current;
    }

    the_watchdog->delta_interval = delta;
    the_watchdog->start_time = _Watchdog_Ticks_since_boot;
    _Watchdog_Activate( the_watchdog );
    _Chain_Insert_unprotected( current, &the_watchdog->Node );

abort_insert:

    _Chain_Extract_unprotected( &iterator.Node );
  }
}

void _Watchdog_Insert(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog
)
{
  ISR_lock_Context lock_context;

  _Watchdog_Acquire( header, &lock_context );
  _Watchdog_Insert_locked( header, the_watchdog, &lock_context );
  _Watchdog_Release( header, &lock_context );
}
