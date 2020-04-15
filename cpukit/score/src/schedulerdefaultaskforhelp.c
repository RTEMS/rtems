/*
 * Copyright (c) 2014, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduler.h>

bool _Scheduler_default_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  (void) scheduler;
  (void) the_thread;
  (void) node;

  return false;
}

void _Scheduler_default_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  (void) scheduler;
  (void) the_thread;
  (void) node;
}

void _Scheduler_default_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
)
{
  (void) scheduler;
  (void) the_thread;
  (void) node;
  (void) next_state;
}
