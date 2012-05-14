/*
 *  Schedule Simple Handler / Ready Queue Enqueue First
 *
 *  COPYRIGHT (c) 2011.
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
#include <rtems/score/thread.h>
#include <rtems/score/schedulersimple.h>

void _Scheduler_simple_Ready_queue_enqueue_first(
  Thread_Control    *the_thread
)
{
  Chain_Control    *ready;
  Chain_Node       *the_node;
  Thread_Control   *current;

  ready    = (Chain_Control *)_Scheduler.information;
  current  = (Thread_Control *)ready;

  /*
   * Do NOT need to check for end of chain because there is always
   * at least one task on the ready chain -- the IDLE task.  It can
   * never block, should never attempt to obtain a semaphore or mutex,
   * and thus will always be there.
   */
  for ( the_node = _Chain_First(ready) ; ; the_node = the_node->next ) {
    current = (Thread_Control *) the_node;

    /* break when AT HEAD OF (or PAST) our priority */
    if ( the_thread->current_priority <= current->current_priority ) {
      current = (Thread_Control *)current->Object.Node.previous;
      break;
    }
  }

  /* enqueue */
  _Chain_Insert_unprotected( (Chain_Node *)current, &the_thread->Object.Node );
}
