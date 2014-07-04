/**
 *  @file
 *
 *  @brief Scheduler EDF Allocate
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

#include <rtems/score/scheduleredfimpl.h>

void _Scheduler_EDF_Node_initialize(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_EDF_Node *node = _Scheduler_EDF_Thread_get_node( the_thread );

  (void) scheduler;

  _Scheduler_Node_do_initialize( &node->Base, the_thread );

  node->thread = the_thread;
  node->queue_state = SCHEDULER_EDF_QUEUE_STATE_NEVER_HAS_BEEN;
}
