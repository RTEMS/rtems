/**
 * @file
 *
 * @brief Scheduler EDF Yield
 *
 * @ingroup RTEMSScoreScheduler
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

void _Scheduler_EDF_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_EDF_Context *context;
  Scheduler_EDF_Node    *the_node;

  context = _Scheduler_EDF_Get_context( scheduler );
  the_node = _Scheduler_EDF_Node_downcast( node );

  _Scheduler_EDF_Extract( context, the_node );
  _Scheduler_EDF_Enqueue( context, the_node, the_node->priority );
  _Scheduler_EDF_Schedule_body( scheduler, the_thread, true );
}
