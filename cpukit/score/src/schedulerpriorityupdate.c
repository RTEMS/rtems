/**
 * @file
 *
 * @brief Update Scheduler Priority 
 * @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

void _Scheduler_priority_Update(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_priority_Context *context =
    _Scheduler_priority_Get_context( scheduler );
  Scheduler_priority_Node *node = _Scheduler_priority_Node_get( the_thread );

  _Scheduler_priority_Ready_queue_update(
    the_thread,
    &node->Ready_queue,
    &context->Bit_map,
    &context->Ready[ 0 ]
  );
}
