/**
 *  @file
 *
 *  @brief Scheduler EDF Extract
 *  @ingroup RTEMSScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduleredfimpl.h>

void _Scheduler_EDF_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_EDF_Context *context;
  Scheduler_EDF_Node    *the_node;
  Priority_Control       priority;
  Priority_Control       insert_priority;

  if ( !_Thread_Is_ready( the_thread ) ) {
    /* Nothing to do */
    return;
  }

  the_node = _Scheduler_EDF_Node_downcast( node );
  insert_priority = _Scheduler_Node_get_priority( &the_node->Base );
  priority = SCHEDULER_PRIORITY_PURIFY( insert_priority );

  if ( priority == the_node->priority ) {
    /* Nothing to do */
    return;
  }

  the_node->priority = priority;
  context = _Scheduler_EDF_Get_context( scheduler );

  _Scheduler_EDF_Extract( context, the_node );
  _Scheduler_EDF_Enqueue( context, the_node, insert_priority );
  _Scheduler_EDF_Schedule_body( scheduler, the_thread, false );
}
