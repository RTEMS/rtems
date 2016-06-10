/*
 * Copyright (c) 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduler.h>

Priority_Control _Scheduler_default_Map_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return priority;
}
