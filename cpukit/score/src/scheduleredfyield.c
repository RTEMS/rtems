/**
 * @file
 *
 * @brief Scheduler EDF Yield
 *
 * @ingroup ScoreScheduler
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

Scheduler_Void_or_thread _Scheduler_EDF_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_EDF_Context *context;
  Scheduler_EDF_Node    *node;

  context = _Scheduler_EDF_Get_context( scheduler );
  node = _Scheduler_EDF_Thread_get_node( the_thread );

  _Scheduler_EDF_Extract( context, node );
  _Scheduler_EDF_Enqueue( context, node, node->current_priority );
  _Scheduler_EDF_Schedule_body( scheduler, the_thread, true );

  SCHEDULER_RETURN_VOID_OR_NULL;
}
