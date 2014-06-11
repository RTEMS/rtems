/**
 *  @file
 *
 *  @brief Scheduler Priority Yield
 *  @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/threadimpl.h>

Scheduler_Void_or_thread _Scheduler_priority_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_priority_Node *node = _Scheduler_priority_Thread_get_node( the_thread );
  Chain_Control *ready_chain = node->Ready_queue.ready_chain;

  (void) scheduler;

  if ( !_Chain_Has_only_one_node( ready_chain ) ) {
    _Chain_Extract_unprotected( &the_thread->Object.Node );
    _Chain_Append_unprotected( ready_chain, &the_thread->Object.Node );

    if ( _Thread_Is_heir( the_thread ) ) {
      _Thread_Heir = (Thread_Control *) _Chain_First( ready_chain );
    }

    _Thread_Dispatch_necessary = true;
  } else if ( !_Thread_Is_heir( the_thread ) ) {
    _Thread_Dispatch_necessary = true;
  }

  SCHEDULER_RETURN_VOID_OR_NULL;
}
