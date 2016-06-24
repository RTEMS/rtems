/**
 *  @file
 *
 *  @brief POSIX Is Priority Valid
 *  @ingroup POSIX_PRIORITY
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/priorityimpl.h>
#include <rtems/score/schedulerimpl.h>

Priority_Control _POSIX_Priority_To_core(
  const Scheduler_Control *scheduler,
  int                      posix_priority,
  bool                    *valid
)
{
  Priority_Control core_posix_priority;
  Priority_Control core_priority;

  core_posix_priority = (Priority_Control) posix_priority;
  core_priority = scheduler->maximum_priority - core_posix_priority;

  *valid = ( posix_priority >= POSIX_SCHEDULER_MINIMUM_PRIORITY
    && core_posix_priority < scheduler->maximum_priority );

  return _Scheduler_Map_priority( scheduler, core_priority );
}

int _POSIX_Priority_From_core(
  const Scheduler_Control *scheduler,
  Priority_Control         core_priority
)
{
  core_priority = _Scheduler_Unmap_priority( scheduler, core_priority );

  return (int) ( scheduler->maximum_priority - core_priority );
}
