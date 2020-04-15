/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

#if defined(RTEMS_SMP)
void _Thread_Scheduler_process_requests( Thread_Control *the_thread )
{
  ISR_lock_Context  lock_context;
  Scheduler_Node   *scheduler_node;

  _Thread_Scheduler_acquire_critical( the_thread, &lock_context );

  scheduler_node = the_thread->Scheduler.requests;

  if ( scheduler_node != NULL ) {
    Scheduler_Node *next;
    Scheduler_Node *remove;

    the_thread->Scheduler.requests = NULL;
    remove = NULL;

    do {
      Scheduler_Node_request request;

      request = scheduler_node->Thread.request;
      scheduler_node->Thread.request = SCHEDULER_NODE_REQUEST_NOT_PENDING;

      next = scheduler_node->Thread.next_request;
#if defined(RTEMS_DEBUG)
      scheduler_node->Thread.next_request = NULL;
#endif

      if ( request == SCHEDULER_NODE_REQUEST_ADD ) {
        ++the_thread->Scheduler.helping_nodes;
        _Chain_Append_unprotected(
          &the_thread->Scheduler.Scheduler_nodes,
          &scheduler_node->Thread.Scheduler_node.Chain
        );
      } else if ( request == SCHEDULER_NODE_REQUEST_REMOVE ) {
        --the_thread->Scheduler.helping_nodes;
        _Chain_Extract_unprotected(
          &scheduler_node->Thread.Scheduler_node.Chain
        );
        scheduler_node->Thread.Scheduler_node.next = remove;
        remove = scheduler_node;
      } else {
        _Assert( request == SCHEDULER_NODE_REQUEST_NOTHING );
      }

      scheduler_node = next;
    } while ( scheduler_node != NULL );

    _Thread_Scheduler_release_critical( the_thread, &lock_context );

    scheduler_node = remove;

    while ( scheduler_node != NULL ) {
      const Scheduler_Control *scheduler;
      ISR_lock_Context         lock_context;

      next = scheduler_node->Thread.Scheduler_node.next;
#if defined(RTEMS_DEBUG)
      scheduler_node->Thread.Scheduler_node.next = NULL;
#endif

      scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

      _Scheduler_Acquire_critical( scheduler, &lock_context );
      ( *scheduler->Operations.withdraw_node )(
        scheduler,
        the_thread,
        scheduler_node,
        THREAD_SCHEDULER_READY
      );
      _Scheduler_Release_critical( scheduler, &lock_context );

      scheduler_node = next;
    }
  } else {
    _Thread_Scheduler_release_critical( the_thread, &lock_context );
  }
}
#endif
