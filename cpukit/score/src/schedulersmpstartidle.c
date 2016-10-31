/*
 * Copyright (c) 2013, 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulersmpimpl.h>

void _Scheduler_SMP_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context     *context;
  Scheduler_SMP_Context *self;
  Scheduler_SMP_Node    *node;

  context = _Scheduler_Get_context( scheduler );
  self = _Scheduler_SMP_Get_self( context );
  node = _Scheduler_SMP_Thread_get_node( idle );

  _Scheduler_Thread_change_state( idle, THREAD_SCHEDULER_SCHEDULED );
  node->state = SCHEDULER_SMP_NODE_SCHEDULED;

  _Thread_Set_CPU( idle, cpu );
  _Chain_Append_unprotected( &self->Scheduled, &node->Base.Node );
  _Scheduler_SMP_Release_idle_thread( &self->Base, idle );
}
