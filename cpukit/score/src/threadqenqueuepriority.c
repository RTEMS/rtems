/*
 *  Thread Queue Handler
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
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*PAGE
 *
 *  _Thread_queue_Enqueue_priority
 *
 *  This routine blocks a thread, places it on a thread, and optionally
 *  starts a timeout timer.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *    thread           - thread to insert
 *    timeout          - timeout interval in ticks
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    forward less than
 *    forward equal
 */

void _Thread_queue_Enqueue_priority(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  Watchdog_Interval     timeout
)
{
  Priority_Control     search_priority;
  Thread_Control      *search_thread;
  ISR_Level            level;
  Chain_Control       *header;
  unsigned32           header_index;
  Chain_Node          *the_node;
  Chain_Node          *next_node;
  Chain_Node          *previous_node;
  Chain_Node          *search_node;
  Priority_Control     priority;
  States_Control       block_state;
  Thread_queue_States  sync_state;

  _Chain_Initialize_empty( &the_thread->Wait.Block2n );

  priority     = the_thread->current_priority;
  header_index = _Thread_queue_Header_number( priority );
  header       = &the_thread_queue->Queues.Priority[ header_index ];
  block_state  = the_thread_queue->state;

  if ( _Thread_queue_Is_reverse_search( priority ) )
    goto restart_reverse_search;

restart_forward_search:
  search_priority = PRIORITY_MINIMUM - 1;
  _ISR_Disable( level );
  search_thread = (Thread_Control *) header->first;
  while ( !_Chain_Is_tail( header, (Chain_Node *)search_thread ) ) {
    search_priority = search_thread->current_priority;
    if ( priority <= search_priority )
      break;

#if ( CPU_UNROLL_ENQUEUE_PRIORITY == TRUE )
    search_thread = (Thread_Control *) search_thread->Object.Node.next;
    if ( _Chain_Is_tail( header, (Chain_Node *)search_thread ) )
      break;
    search_priority = search_thread->current_priority;
    if ( priority <= search_priority )
      break;
#endif
    _ISR_Flash( level );
    if ( !_States_Are_set( search_thread->current_state, block_state) ) {
      _ISR_Enable( level );
      goto restart_forward_search;
    }
    search_thread =
       (Thread_Control *)search_thread->Object.Node.next;
  }

  if ( the_thread_queue->sync_state != THREAD_QUEUE_NOTHING_HAPPENED )
    goto synchronize;

  the_thread_queue->sync_state = THREAD_QUEUE_SYNCHRONIZED;

  if ( priority == search_priority )
    goto equal_priority;

  search_node   = (Chain_Node *) search_thread;
  previous_node = search_node->previous;
  the_node      = (Chain_Node *) the_thread;

  the_node->next        = search_node;
  the_node->previous    = previous_node;
  previous_node->next   = the_node;
  search_node->previous = the_node;
  _ISR_Enable( level );
  return;

restart_reverse_search:
  search_priority     = PRIORITY_MAXIMUM + 1;

  _ISR_Disable( level );
  search_thread = (Thread_Control *) header->last;
  while ( !_Chain_Is_head( header, (Chain_Node *)search_thread ) ) {
    search_priority = search_thread->current_priority;
    if ( priority >= search_priority )
      break;
#if ( CPU_UNROLL_ENQUEUE_PRIORITY == TRUE )
    search_thread = (Thread_Control *) search_thread->Object.Node.previous;
    if ( _Chain_Is_head( header, (Chain_Node *)search_thread ) )
      break;
    search_priority = search_thread->current_priority;
    if ( priority >= search_priority )
      break;
#endif
    _ISR_Flash( level );
    if ( !_States_Are_set( search_thread->current_state, block_state) ) {
      _ISR_Enable( level );
      goto restart_reverse_search;
    }
    search_thread = (Thread_Control *)
                         search_thread->Object.Node.previous;
  }

  if ( the_thread_queue->sync_state != THREAD_QUEUE_NOTHING_HAPPENED )
    goto synchronize;

  the_thread_queue->sync_state = THREAD_QUEUE_SYNCHRONIZED;

  if ( priority == search_priority )
    goto equal_priority;

  search_node = (Chain_Node *) search_thread;
  next_node   = search_node->next;
  the_node    = (Chain_Node *) the_thread;

  the_node->next      = next_node;
  the_node->previous  = search_node;
  search_node->next   = the_node;
  next_node->previous = the_node;
  _ISR_Enable( level );
  return;

equal_priority:               /* add at end of priority group */
  search_node   = _Chain_Tail( &search_thread->Wait.Block2n );
  previous_node = search_node->previous;
  the_node      = (Chain_Node *) the_thread;

  the_node->next        = search_node;
  the_node->previous    = previous_node;
  previous_node->next   = the_node;
  search_node->previous = the_node;
  _ISR_Enable( level );
  return;

synchronize:

  sync_state = the_thread_queue->sync_state;
  the_thread_queue->sync_state = THREAD_QUEUE_SYNCHRONIZED;

  switch ( sync_state ) {
    case THREAD_QUEUE_SYNCHRONIZED:
      /*
       *  This should never happen.  It indicates that someone did not
       *  enter a thread queue critical section.
       */
      break;
 
    case THREAD_QUEUE_NOTHING_HAPPENED:
      /*
       *  This should never happen.  All of this was dealt with above.
       */
      break;
 
    case THREAD_QUEUE_TIMEOUT:
      the_thread->Wait.return_code = the_thread->Wait.queue->timeout_status;
      _ISR_Enable( level );
      break;
 
    case THREAD_QUEUE_SATISFIED:
      if ( _Watchdog_Is_active( &the_thread->Timer ) ) {
        _Watchdog_Deactivate( &the_thread->Timer );
        _ISR_Enable( level );
        (void) _Watchdog_Remove( &the_thread->Timer );
      } else
        _ISR_Enable( level );
      break;
  }
 
  /*
   *  Global objects with thread queue's should not be operated on from an
   *  ISR.  But the sync code still must allow short timeouts to be processed
   *  correctly.
   */
 
  _Thread_Unblock( the_thread );
 
#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
#endif
}

