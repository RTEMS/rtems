/*
 *  @file
 *
 *  @brief Initialize Scheduler Priority
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

void _Scheduler_priority_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_priority_Context *context =
    _Scheduler_priority_Get_context( scheduler );

  _Priority_bit_map_Initialize( &context->Bit_map );
  _Scheduler_priority_Ready_queue_initialize(
    &context->Ready[ 0 ],
    scheduler->maximum_priority
  );
}

void _Scheduler_priority_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_priority_Context *context;
  Scheduler_priority_Node    *the_node;

  _Scheduler_Node_do_initialize( scheduler, node, the_thread, priority );

  context = _Scheduler_priority_Get_context( scheduler );
  the_node = _Scheduler_priority_Node_downcast( node );
  _Scheduler_priority_Ready_queue_update(
    &the_node->Ready_queue,
    SCHEDULER_PRIORITY_UNMAP( priority ),
    &context->Bit_map,
    &context->Ready[ 0 ]
  );
}
