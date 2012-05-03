/*
 *  Thread Queue Handler - Enqueue By Priority
 *
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*
 *  Support the user forcing the unrolling to be disabled.
 */
#if __RTEMS_DO_NOT_UNROLL_THREADQ_ENQUEUE_PRIORITY__
  #undef CPU_UNROLL_ENQUEUE_PRIORITY
  #define CPU_UNROLL_ENQUEUE_PRIORITY FALSE
#endif

/*
 *  _Thread_queue_Enqueue_priority
 *
 *  This routine places a blocked thread on a priority thread queue.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *    thread           - thread to insert
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    forward less than
 *    forward equal
 */

Thread_blocking_operation_States _Thread_queue_Enqueue_priority (
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  ISR_Level            *level_p
)
{
  Priority_Control     search_priority;
  Thread_Control      *search_thread;
  ISR_Level            level;
  Chain_Control       *header;
  uint32_t             header_index;
  Chain_Node          *the_node;
  Chain_Node          *next_node;
  Chain_Node          *previous_node;
  Chain_Node          *search_node;
  Priority_Control     priority;
  States_Control       block_state;

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
  search_thread = (Thread_Control *) _Chain_First( header );
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

  if ( the_thread_queue->sync_state !=
       THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED )
    goto synchronize;

  the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;

  if ( priority == search_priority )
    goto equal_priority;

  search_node   = (Chain_Node *) search_thread;
  previous_node = search_node->previous;
  the_node      = (Chain_Node *) the_thread;

  the_node->next         = search_node;
  the_node->previous     = previous_node;
  previous_node->next    = the_node;
  search_node->previous  = the_node;
  the_thread->Wait.queue = the_thread_queue;
  _ISR_Enable( level );
  return THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;

restart_reverse_search:
  search_priority     = PRIORITY_MAXIMUM + 1;

  _ISR_Disable( level );
  search_thread = (Thread_Control *) _Chain_Last( header );
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

  if ( the_thread_queue->sync_state !=
       THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED )
    goto synchronize;

  the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;

  if ( priority == search_priority )
    goto equal_priority;

  search_node = (Chain_Node *) search_thread;
  next_node   = search_node->next;
  the_node    = (Chain_Node *) the_thread;

  the_node->next          = next_node;
  the_node->previous      = search_node;
  search_node->next       = the_node;
  next_node->previous    = the_node;
  the_thread->Wait.queue = the_thread_queue;
  _ISR_Enable( level );
  return THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;

equal_priority:               /* add at end of priority group */
  search_node   = _Chain_Tail( &search_thread->Wait.Block2n );
  previous_node = search_node->previous;
  the_node      = (Chain_Node *) the_thread;

  the_node->next         = search_node;
  the_node->previous     = previous_node;
  previous_node->next    = the_node;
  search_node->previous  = the_node;
  the_thread->Wait.queue = the_thread_queue;
  _ISR_Enable( level );
  return THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;

synchronize:
  /*
   *  An interrupt completed the thread's blocking request.
   *  For example, the blocking thread could have been given
   *  the mutex by an ISR or timed out.
   *
   *  WARNING! Returning with interrupts disabled!
   */
  *level_p = level;
  return the_thread_queue->sync_state;
}
