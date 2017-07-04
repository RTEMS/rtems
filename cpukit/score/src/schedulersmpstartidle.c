/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.
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
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Do_start_idle(
    context,
    idle,
    cpu,
    _Scheduler_SMP_Do_nothing_register_idle
  );
}
