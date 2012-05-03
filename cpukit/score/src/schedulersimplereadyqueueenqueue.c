/*
 *  Schedule Simple Handler / Ready Queue Enqueue
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
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/schedulersimple.h>

void _Scheduler_simple_Ready_queue_enqueue(
  Thread_Control    *the_thread
)
{
  Chain_Control    *ready;
  Chain_Node       *the_node;
  Thread_Control   *current;

  ready    = (Chain_Control *)_Scheduler.information;
  the_node = _Chain_First( ready );
  current  = (Thread_Control *)ready;

  for ( ; !_Chain_Is_tail( ready, the_node ) ; the_node = the_node->next ) {
    current = (Thread_Control *) the_node;

    /* break when AT END OR PAST our priority */
    if ( the_thread->current_priority < current->current_priority ) {
      current = (Thread_Control *)current->Object.Node.previous;
      break;
    }
  }

  /* enqueue */
  _Chain_Insert_unprotected( (Chain_Node *)current, &the_thread->Object.Node );
}
