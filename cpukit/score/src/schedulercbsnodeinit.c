/*
 *  @file
 *
 *  @brief Scheduler CBS Allocate
 *  @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulercbsimpl.h>

void _Scheduler_CBS_Node_initialize(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_CBS_Node *node = _Scheduler_CBS_Thread_get_node( the_thread );

  (void) scheduler;

  _Scheduler_Node_do_initialize( &node->Base.Base, the_thread );

  node->Base.thread = the_thread;
  node->Base.queue_state = SCHEDULER_EDF_QUEUE_STATE_NEVER_HAS_BEEN;
  node->cbs_server = NULL;
}
