/*
 *  Thread Queue Handler
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
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*PAGE
 *
 *  _Thread_queue_Initialize
 *
 *  This routine initializes the specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue      - pointer to a threadq header
 *    the_class             - class of the object to which this belongs
 *    discipline            - queueing discipline
 *    state                 - state of waiting threads
 *    proxy_extract_callout - MP specific callout
 *    timeout_status        - return on a timeout
 *
 *  Output parameters: NONE
 */

void _Thread_queue_Initialize(
  Thread_queue_Control         *the_thread_queue,
  Objects_Classes               the_class,
  Thread_queue_Disciplines      the_discipline,
  States_Control                state,
  Thread_queue_Extract_callout  proxy_extract_callout,
  unsigned32                    timeout_status
)
{
  unsigned32 index;

  _Thread_queue_Extract_table[ the_class ] = proxy_extract_callout;

  the_thread_queue->state          = state;
  the_thread_queue->discipline     = the_discipline;
  the_thread_queue->timeout_status = timeout_status;
  the_thread_queue->sync_state     = THREAD_QUEUE_SYNCHRONIZED;

  switch ( the_discipline ) {
    case THREAD_QUEUE_DISCIPLINE_FIFO:
      _Chain_Initialize_empty( &the_thread_queue->Queues.Fifo );
      break;
    case THREAD_QUEUE_DISCIPLINE_PRIORITY:
      for( index=0 ;
           index < TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS ;
           index++)
        _Chain_Initialize_empty( &the_thread_queue->Queues.Priority[index] );
      break;
  }

}

/*PAGE
 *
 *  _Thread_queue_Enqueue
 *
 *  This routine blocks a thread, places it on a thread, and optionally
 *  starts a timeout timer.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *    timeout          - interval to wait
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Thread_queue_Enqueue(
  Thread_queue_Control *the_thread_queue,
  Watchdog_Interval     timeout
)
{
  Thread_Control *the_thread;

  the_thread = _Thread_Executing;

  if ( _Thread_MP_Is_receive( the_thread ) && the_thread->receive_packet )
    the_thread = _Thread_MP_Allocate_proxy( the_thread_queue->state );
  else
    _Thread_Set_state( the_thread, the_thread_queue->state );

  if ( timeout ) {
    _Watchdog_Initialize(
       &the_thread->Timer,
       _Thread_queue_Timeout,
       the_thread->Object.id,
       NULL
    );

    _Watchdog_Insert_ticks( &the_thread->Timer, timeout );
  }

  switch( the_thread_queue->discipline ) {
    case THREAD_QUEUE_DISCIPLINE_FIFO:
      _Thread_queue_Enqueue_fifo( the_thread_queue, the_thread, timeout );
      break;
    case THREAD_QUEUE_DISCIPLINE_PRIORITY:
      _Thread_queue_Enqueue_priority( the_thread_queue, the_thread, timeout );
      break;
  }
}

/*PAGE
 *
 *  _Thread_queue_Dequeue
 *
 *  This routine removes a thread from the specified threadq.  If the
 *  threadq discipline is FIFO, it unblocks a thread, and cancels its
 *  timeout timer.  Priority discipline is processed elsewhere.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *
 *  Output parameters:
 *    returns - thread dequeued or NULL
 *
 *  INTERRUPT LATENCY:
 *    check sync
 */

Thread_Control *_Thread_queue_Dequeue(
  Thread_queue_Control *the_thread_queue
)
{
  Thread_Control *the_thread;

  switch ( the_thread_queue->discipline ) {
    case THREAD_QUEUE_DISCIPLINE_FIFO:
      the_thread = _Thread_queue_Dequeue_fifo( the_thread_queue );
      break;
    case THREAD_QUEUE_DISCIPLINE_PRIORITY:
      the_thread = _Thread_queue_Dequeue_priority( the_thread_queue );
      break;
    default:              /* this is only to prevent warnings */
      the_thread = NULL;
      break;
  }

  return( the_thread );
}

/*PAGE
 *
 *  _Thread_queue_Extract_with_proxy
 *
 *  This routine extracts the_thread from the_thread_queue
 *  and insures that if there is a proxy for this task on 
 *  another node, it is also dealt with.
 *
 *  XXX
 */
 
boolean _Thread_queue_Extract_with_proxy(
  Thread_Control       *the_thread
)
{
  States_Control                state;
  Objects_Classes               the_class;
  Thread_queue_Extract_callout  proxy_extract_callout;

  state = the_thread->current_state;

  if ( _States_Is_waiting_on_thread_queue( state ) ) {
    if ( _States_Is_waiting_for_rpc_reply( state ) &&
         _States_Is_locally_blocked( state ) ) {

      the_class = _Objects_Get_class( the_thread->Wait.id );

      proxy_extract_callout = _Thread_queue_Extract_table[ the_class ];

      if ( proxy_extract_callout )
        (*proxy_extract_callout)( the_thread );
    }
    _Thread_queue_Extract( the_thread->Wait.queue, the_thread );

    return TRUE;
  }
  return FALSE;
}

/*PAGE
 *
 *  _Thread_queue_Extract
 *
 *  This routine removes a specific thread from the specified threadq,
 *  deletes any timeout, and unblocks the thread.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to a threadq header
 *    the_thread       - pointer to a thread control block
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY: NONE
 */

void _Thread_queue_Extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  switch ( the_thread_queue->discipline ) {
    case THREAD_QUEUE_DISCIPLINE_FIFO:
      _Thread_queue_Extract_fifo( the_thread_queue, the_thread );
      break;
    case THREAD_QUEUE_DISCIPLINE_PRIORITY:
      _Thread_queue_Extract_priority( the_thread_queue, the_thread );
      break;
   }
}

/*PAGE
 *
 *  _Thread_queue_Flush
 *
 *  This kernel routine flushes the given thread queue.
 *
 *  Input parameters:
 *    the_thread_queue       - pointer to threadq to be flushed
 *    remote_extract_callout - pointer to routine which extracts a remote thread
 *    status                 - status to return to the thread
 *
 *  Output parameters:  NONE
 */

void _Thread_queue_Flush(
  Thread_queue_Control       *the_thread_queue,
  Thread_queue_Flush_callout  remote_extract_callout,
  unsigned32                  status
)
{
  Thread_Control *the_thread;

  while ( (the_thread = _Thread_queue_Dequeue( the_thread_queue )) ) {
    if ( _Objects_Is_local_id( the_thread->Object.id ) )
      the_thread->Wait.return_code = status;
    else
      ( *remote_extract_callout )( the_thread );
  }
}

/*PAGE
 *
 *  _Thread_queue_First
 *
 *  This routines returns a pointer to the first thread on the
 *  specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to thread queue
 *
 *  Output parameters:
 *    returns - first thread or NULL
 */

Thread_Control *_Thread_queue_First(
  Thread_queue_Control *the_thread_queue
)
{
  Thread_Control *the_thread;

  switch ( the_thread_queue->discipline ) {
    case THREAD_QUEUE_DISCIPLINE_FIFO:
      the_thread = _Thread_queue_First_fifo( the_thread_queue );
      break;
    case THREAD_QUEUE_DISCIPLINE_PRIORITY:
      the_thread = _Thread_queue_First_priority( the_thread_queue );
      break;
    default:              /* this is only to prevent warnings */
      the_thread = NULL;
      break;
  }

  return the_thread;
}

/*PAGE
 *
 *  _Thread_queue_Timeout
 *
 *  This routine processes a thread which timeouts while waiting on
 *  a thread queue. It is called by the watchdog handler.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters: NONE
 */

void _Thread_queue_Timeout(
  Objects_Id  id,
  void       *ignored
)
{
  Thread_Control       *the_thread;
  Thread_queue_Control *the_thread_queue;
  Objects_Locations     location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:  /* impossible */
      break;
    case OBJECTS_LOCAL:
      the_thread_queue = the_thread->Wait.queue;

      /*
       *  If the_thread_queue is not synchronized, then it is either
       *  "nothing happened", "timeout", or "satisfied".   If the_thread
       *  is the executing thread, then it is in the process of blocking
       *  and it is the thread which is responsible for the synchronization
       *  process.
       *
       *  If it is not satisfied, then it is "nothing happened" and
       *  this is the "timeout" transition.  After a request is satisfied,
       *  a timeout is not allowed to occur.
       */

      if ( the_thread_queue->sync_state != THREAD_QUEUE_SYNCHRONIZED &&
           _Thread_Is_executing( the_thread ) ) {
        if ( the_thread_queue->sync_state != THREAD_QUEUE_SATISFIED )
          the_thread_queue->sync_state = THREAD_QUEUE_TIMEOUT;
      } else {
        the_thread->Wait.return_code = the_thread->Wait.queue->timeout_status;
        _Thread_queue_Extract( the_thread->Wait.queue, the_thread );
      }
      _Thread_Unnest_dispatch();
      break;
  }
}

/*PAGE
 *
 *  _Thread_queue_Enqueue_fifo
 *
 *  This routine blocks a thread, places it on a thread, and optionally
 *  starts a timeout timer.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *    the_thread       - pointer to the thread to block
 *    timeout          - interval to wait
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Thread_queue_Enqueue_fifo (
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  Watchdog_Interval    timeout
)
{
  ISR_Level            level;
  Thread_queue_States  sync_state;

  _ISR_Disable( level );

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
      _Chain_Append_unprotected(
        &the_thread_queue->Queues.Fifo,
        &the_thread->Object.Node
      );
      _ISR_Enable( level );
      return;

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

  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );

}

/*PAGE
 *
 *  _Thread_queue_Dequeue_fifo
 *
 *  This routine removes a thread from the specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *
 *  Output parameters:
 *    returns - thread dequeued or NULL
 *
 *  INTERRUPT LATENCY:
 *    check sync
 *    FIFO
 */

Thread_Control *_Thread_queue_Dequeue_fifo(
  Thread_queue_Control *the_thread_queue
)
{
  ISR_Level              level;
  Thread_Control *the_thread;

  _ISR_Disable( level );
  if ( !_Chain_Is_empty( &the_thread_queue->Queues.Fifo ) ) {

    the_thread = (Thread_Control *)
       _Chain_Get_first_unprotected( &the_thread_queue->Queues.Fifo );

    if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
      _ISR_Enable( level );
      _Thread_Unblock( the_thread );
    } else {
      _Watchdog_Deactivate( &the_thread->Timer );
      _ISR_Enable( level );
      (void) _Watchdog_Remove( &the_thread->Timer );
      _Thread_Unblock( the_thread );
    }

    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      _Thread_MP_Free_proxy( the_thread );

    return the_thread;
  } 

  switch ( the_thread_queue->sync_state ) {
    case THREAD_QUEUE_SYNCHRONIZED:
    case THREAD_QUEUE_SATISFIED:
      _ISR_Enable( level );
      return NULL;

    case THREAD_QUEUE_NOTHING_HAPPENED:
    case THREAD_QUEUE_TIMEOUT:
      the_thread_queue->sync_state = THREAD_QUEUE_SATISFIED;
      _ISR_Enable( level );
      return _Thread_Executing;
  }
  return NULL;                /* this is only to prevent warnings */
}

/*PAGE
 *
 *  _Thread_queue_Extract_fifo
 *
 *  This routine removes a specific thread from the specified threadq,
 *  deletes any timeout, and unblocks the thread.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to a threadq header
 *    the_thread       - pointer to the thread to block
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    EXTRACT_FIFO
 */

void _Thread_queue_Extract_fifo(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  ISR_Level level;

  _ISR_Disable( level );

  if ( !_States_Is_waiting_on_thread_queue( the_thread->current_state ) ) {
    _ISR_Enable( level );
    return;
  }

  _Chain_Extract_unprotected( &the_thread->Object.Node );

  if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
    _ISR_Enable( level );
  } else {
    _Watchdog_Deactivate( &the_thread->Timer );
    _ISR_Enable( level );
    (void) _Watchdog_Remove( &the_thread->Timer );
  }

  _Thread_Unblock( the_thread );

  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
 
}

/*PAGE
 *
 *  _Thread_queue_First_fifo
 *
 *  This routines returns a pointer to the first thread on the
 *  specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *
 *  Output parameters:
 *    returns - first thread or NULL
 */

Thread_Control *_Thread_queue_First_fifo(
  Thread_queue_Control *the_thread_queue
)
{
  if ( !_Chain_Is_empty( &the_thread_queue->Queues.Fifo ) )
    return (Thread_Control *) the_thread_queue->Queues.Fifo.first;

  return NULL;
}

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
 
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
}

/*PAGE
 *
 *  _Thread_queue_Dequeue_priority
 *
 *  This routine removes a thread from the specified PRIORITY based
 *  threadq, unblocks it, and cancels its timeout timer.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to thread queue
 *
 *  Output parameters:
 *    returns - thread dequeued or NULL
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

Thread_Control *_Thread_queue_Dequeue_priority(
  Thread_queue_Control *the_thread_queue
)
{
  unsigned32      index;
  ISR_Level       level;
  Thread_Control *the_thread = NULL;  /* just to remove warnings */
  Thread_Control *new_first_thread;
  Chain_Node     *new_first_node;
  Chain_Node     *new_second_node;
  Chain_Node     *last_node;
  Chain_Node     *next_node;
  Chain_Node     *previous_node;

  _ISR_Disable( level );
  for( index=0 ;
       index < TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS ;
       index++ ) {
    if ( !_Chain_Is_empty( &the_thread_queue->Queues.Priority[ index ] ) ) {
      the_thread = (Thread_Control *)
                    the_thread_queue->Queues.Priority[ index ].first;
      goto dequeue;
    }
  }

  switch ( the_thread_queue->sync_state ) {
    case THREAD_QUEUE_SYNCHRONIZED:
    case THREAD_QUEUE_SATISFIED:
      _ISR_Enable( level );
      return NULL;

    case THREAD_QUEUE_NOTHING_HAPPENED:
    case THREAD_QUEUE_TIMEOUT:
      the_thread_queue->sync_state = THREAD_QUEUE_SATISFIED;
      _ISR_Enable( level );
      return _Thread_Executing;
  }

dequeue:
  new_first_node   = the_thread->Wait.Block2n.first;
  new_first_thread = (Thread_Control *) new_first_node;
  next_node        = the_thread->Object.Node.next;
  previous_node    = the_thread->Object.Node.previous;

  if ( !_Chain_Is_empty( &the_thread->Wait.Block2n ) ) {
    last_node       = the_thread->Wait.Block2n.last;
    new_second_node = new_first_node->next;

    previous_node->next      = new_first_node;
    next_node->previous      = new_first_node;
    new_first_node->next     = next_node;
    new_first_node->previous = previous_node;

    if ( !_Chain_Has_only_one_node( &the_thread->Wait.Block2n ) ) {
                                                /* > two threads on 2-n */
      new_second_node->previous =
                _Chain_Head( &new_first_thread->Wait.Block2n );

      new_first_thread->Wait.Block2n.first = new_second_node;
      new_first_thread->Wait.Block2n.last  = last_node;

      last_node->next = _Chain_Tail( &new_first_thread->Wait.Block2n );
    }
  } else {
    previous_node->next = next_node;
    next_node->previous = previous_node;
  }

  if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
    _ISR_Enable( level );
    _Thread_Unblock( the_thread );
  } else {
    _Watchdog_Deactivate( &the_thread->Timer );
    _ISR_Enable( level );
    (void) _Watchdog_Remove( &the_thread->Timer );
    _Thread_Unblock( the_thread );
  }

  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
  return( the_thread );
}

/*PAGE
 *
 *  _Thread_queue_Extract_priority
 *
 *  This routine removes a specific thread from the specified threadq,
 *  deletes any timeout, and unblocks the thread.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to a threadq header
 *    the_thread       - pointer to a thread control block
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    EXTRACT_PRIORITY
 */

void _Thread_queue_Extract_priority(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  ISR_Level              level;
  Chain_Node     *the_node;
  Chain_Node     *next_node;
  Chain_Node     *previous_node;
  Thread_Control *new_first_thread;
  Chain_Node     *new_first_node;
  Chain_Node     *new_second_node;
  Chain_Node     *last_node;

  the_node = (Chain_Node *) the_thread;
  _ISR_Disable( level );
  if ( _States_Is_waiting_on_thread_queue( the_thread->current_state ) ) {
    next_node     = the_node->next;
    previous_node = the_node->previous;

    if ( !_Chain_Is_empty( &the_thread->Wait.Block2n ) ) {
      new_first_node   = the_thread->Wait.Block2n.first;
      new_first_thread = (Thread_Control *) new_first_node;
      last_node        = the_thread->Wait.Block2n.last;
      new_second_node  = new_first_node->next;

      previous_node->next      = new_first_node;
      next_node->previous      = new_first_node;
      new_first_node->next     = next_node;
      new_first_node->previous = previous_node;

      if ( !_Chain_Has_only_one_node( &the_thread->Wait.Block2n ) ) {
                                          /* > two threads on 2-n */
        new_second_node->previous =
                  _Chain_Head( &new_first_thread->Wait.Block2n );
        new_first_thread->Wait.Block2n.first = new_second_node;

        new_first_thread->Wait.Block2n.last = last_node;
        last_node->next = _Chain_Tail( &new_first_thread->Wait.Block2n );
      }
    } else {
      previous_node->next = next_node;
      next_node->previous = previous_node;
    }

    if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
      _ISR_Enable( level );
      _Thread_Unblock( the_thread );
    } else {
      _Watchdog_Deactivate( &the_thread->Timer );
      _ISR_Enable( level );
      (void) _Watchdog_Remove( &the_thread->Timer );
      _Thread_Unblock( the_thread );
    }

    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      _Thread_MP_Free_proxy( the_thread );
  }
  else
    _ISR_Enable( level );
}

/*PAGE
 *
 *  _Thread_queue_First_priority
 *
 *  This routines returns a pointer to the first thread on the
 *  specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to thread queue
 *
 *  Output parameters:
 *    returns - first thread or NULL
 */

Thread_Control *_Thread_queue_First_priority (
  Thread_queue_Control *the_thread_queue
)
{
  unsigned32 index;

  for( index=0 ;
       index < TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS ;
       index++ ) {
    if ( !_Chain_Is_empty( &the_thread_queue->Queues.Priority[ index ] ) )
      return (Thread_Control *)
        the_thread_queue->Queues.Priority[ index ].first;
  }
  return NULL;
}
