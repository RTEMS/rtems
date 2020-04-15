/*
 *  @file
 *
 *  @brief Scheduler CBS Allocate
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

#include <rtems/score/schedulercbsimpl.h>

void _Scheduler_CBS_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_CBS_Node *the_node;

  _Scheduler_EDF_Node_initialize( scheduler, node, the_thread, priority );

  the_node = _Scheduler_CBS_Node_downcast( node );
  the_node->cbs_server = NULL;
  the_node->deadline_node = NULL;
}
