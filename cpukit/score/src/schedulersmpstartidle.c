/**
 * @file
 *
 * @brief SMP Scheduler Start Idle Operation
 *
 * @ingroup ScoreSchedulerSMP
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulersmpimpl.h>
#include <rtems/score/chainimpl.h>

void _Scheduler_SMP_Start_idle(
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  thread->is_scheduled = true;
  thread->cpu = cpu;
  _Chain_Append_unprotected( &self->scheduled, &thread->Object.Node );
}
