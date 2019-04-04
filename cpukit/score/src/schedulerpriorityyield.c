/**
 *  @file
 *
 *  @brief Scheduler Priority Yield
 *  @ingroup RTEMSScoreScheduler
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

void _Scheduler_priority_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_priority_Node *the_node;
  Chain_Control           *ready_chain;

  the_node = _Scheduler_priority_Node_downcast( node );
  ready_chain = the_node->Ready_queue.ready_chain;

  if ( !_Chain_Has_only_one_node( ready_chain ) ) {
    _Chain_Extract_unprotected( &the_thread->Object.Node );
    _Chain_Append_unprotected( ready_chain, &the_thread->Object.Node );
  }

  _Scheduler_priority_Schedule_body( scheduler, the_thread, true );
}
