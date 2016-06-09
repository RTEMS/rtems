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
  Scheduler_EDF_Node *node;

  node = _Scheduler_EDF_Thread_get_node( the_thread );

  if ( ( new_priority & SCHEDULER_EDF_PRIO_MSB ) != 0 ) {
    node->background_priority = new_priority;
  }

  node->current_priority = new_priority;
}
