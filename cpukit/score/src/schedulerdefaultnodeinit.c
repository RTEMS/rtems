/**
 * @file
 *
 * @brief Scheduler Default Node Initialization Operation
 *
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerimpl.h>

void _Scheduler_default_Node_initialize(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_Node *node = _Scheduler_Thread_get_own_node( the_thread );

  (void) scheduler;

  _Scheduler_Node_do_initialize( node, the_thread );
}
