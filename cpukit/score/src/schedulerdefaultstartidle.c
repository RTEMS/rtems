/*
 * Copyright (c) 2013 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerimpl.h>

void _Scheduler_default_Start_idle(
  Thread_Control  *thread,
  Per_CPU_Control *processor
)
{
  (void) processor;
  _Scheduler_Unblock( thread );
}
