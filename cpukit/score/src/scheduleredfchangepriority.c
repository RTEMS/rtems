/**
 *  @file
 *
 *  @brief Scheduler EDF Extract
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

Priority_Control _Scheduler_EDF_Map_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return SCHEDULER_EDF_PRIO_MSB | priority;
}

Priority_Control _Scheduler_EDF_Unmap_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return priority & ~SCHEDULER_EDF_PRIO_MSB;
}

Scheduler_Void_or_thread _Scheduler_EDF_Change_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority,
  bool                     prepend_it
)
{
  Scheduler_EDF_Context *context;
  Scheduler_EDF_Node    *node;

  context = _Scheduler_EDF_Get_context( scheduler );
  node = _Scheduler_EDF_Thread_get_node( the_thread );

  if ( ( new_priority & SCHEDULER_EDF_PRIO_MSB ) != 0 ) {
    node->background_priority = new_priority;
  }

  node->current_priority = new_priority;

  _Scheduler_EDF_Extract( context, node );

  if ( prepend_it ) {
    _Scheduler_EDF_Enqueue_first( context, node, new_priority );
  } else {
    _Scheduler_EDF_Enqueue( context, node, new_priority );
  }

  _Scheduler_EDF_Schedule_body( scheduler, the_thread, false );

  SCHEDULER_RETURN_VOID_OR_NULL;
}
