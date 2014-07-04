/*
 * Copyright (c) 2013-2014 embedded brains GmbH
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
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );
  Scheduler_SMP_Node *node = _Scheduler_SMP_Thread_get_node( thread );

  node->state = SCHEDULER_SMP_NODE_SCHEDULED;

  _Thread_Set_CPU( thread, cpu );
  _Chain_Append_unprotected( &self->Scheduled, &node->Base.Node );
  _Chain_Prepend_unprotected( &self->Idle_threads, &thread->Object.Node );
}
