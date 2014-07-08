/**
 *  @file
 *
 *  @brief Scheduler EDF Update
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

void _Scheduler_EDF_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority
)
{
  Scheduler_EDF_Node *node = _Scheduler_EDF_Thread_get_node( the_thread );

  (void) scheduler;
  (void) new_priority;

  if (node->queue_state == SCHEDULER_EDF_QUEUE_STATE_NEVER_HAS_BEEN) {
    /* Shifts the priority to the region of background tasks. */
    the_thread->Start.initial_priority |= (SCHEDULER_EDF_PRIO_MSB);
    the_thread->real_priority    = the_thread->Start.initial_priority;
    the_thread->current_priority = the_thread->Start.initial_priority;
    node->queue_state = SCHEDULER_EDF_QUEUE_STATE_NOT_PRESENTLY;
  }
}
