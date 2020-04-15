/*
 * Copyright (c) 2016, 2017 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerimpl.h>

Priority_Control _Scheduler_default_Map_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return SCHEDULER_PRIORITY_MAP( priority );
}

Priority_Control _Scheduler_default_Unmap_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return SCHEDULER_PRIORITY_UNMAP( priority );
}
