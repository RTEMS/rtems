/*
 * Copyright (c) 2018 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduler.h>
#include <rtems/score/interr.h>
#include <rtems/score/smpimpl.h>

void _Scheduler_default_Pin_or_unpin(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  struct Per_CPU_Control  *cpu
)
{
  (void) scheduler;
  (void) the_thread;
  (void) node;
  (void) cpu;

  if ( _SMP_Get_processor_maximum() > 1 ) {
    _Terminate(
      RTEMS_FATAL_SOURCE_SMP,
      SMP_FATAL_SCHEDULER_PIN_OR_UNPIN_NOT_SUPPORTED
    );
  }
}
