/*
 *  Thread Queue Handler
 *
 *
 *  COPYRIGHT (c) 1989-2008.
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
 *  _Thread_queue_Extract_priority
 *
 *  This routine removes a specific thread from the specified threadq,
 *  deletes any timeout, and unblocks the thread.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to a threadq header
 *    the_thread       - pointer to a thread control block
 *    requeuing        - true if requeuing and should not alter timeout or state
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    EXTRACT_PRIORITY
 */

void _Thread_queue_Extract_priority_helper(
  Thread_queue_Control *the_thread_queue __attribute__((unused)),
  Thread_Control       *the_thread,
  bool                  requeuing
)
{
  ISR_Level       level;
  Chain_Node     *head;
  Chain_Node     *tail;
  Chain_Node     *the_node;
  Chain_Node     *next_node;
  Chain_Node     *previous_node;
  Thread_Control *new_first_thread;
  Chain_Node     *new_first_node;
  Chain_Node     *new_second_node;
  Chain_Node     *last_node;

  the_node = (Chain_Node *) the_thread;
  _ISR_Disable( level );
  if ( !_States_Is_waiting_on_thread_queue( the_thread->current_state ) ) {
    _ISR_Enable( level );
    return;
  }

  /*
   *  The thread was actually waiting on a thread queue so let's remove it.
   */

  next_node     = the_node->next;
  previous_node = the_node->previous;

  if ( !_Chain_Is_empty( &the_thread->Wait.Block2n ) ) {
    new_first_node   = _Chain_First( &the_thread->Wait.Block2n );
    new_first_thread = (Thread_Control *) new_first_node;
    last_node        = _Chain_Last( &the_thread->Wait.Block2n );
    new_second_node  = new_first_node->next;

    previous_node->next      = new_first_node;
    next_node->previous      = new_first_node;
    new_first_node->next     = next_node;
    new_first_node->previous = previous_node;

    if ( !_Chain_Has_only_one_node( &the_thread->Wait.Block2n ) ) {
                                        /* > two threads on 2-n */
      head = _Chain_Head( &new_first_thread->Wait.Block2n );
      tail = _Chain_Tail( &new_first_thread->Wait.Block2n );

      new_second_node->previous = head;
      head->next = new_second_node;
      tail->previous = last_node;
      last_node->next = tail;
    }
  } else {
    previous_node->next = next_node;
    next_node->previous = previous_node;
  }

  /*
   *  If we are not supposed to touch timers or the thread's state, return.
   */

  if ( requeuing ) {
    _ISR_Enable( level );
    return;
  }

  if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
    _ISR_Enable( level );
  } else {
    _Watchdog_Deactivate( &the_thread->Timer );
    _ISR_Enable( level );
    (void) _Watchdog_Remove( &the_thread->Timer );
  }
  _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
#endif
}
