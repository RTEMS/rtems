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

int _POSIX_Priority_Get_maximum( const Scheduler_Control *scheduler )
{
  if ( scheduler->maximum_priority < INT_MAX ) {
    return (int) scheduler->maximum_priority - 1;
  } else {
    return INT_MAX;
  }
}

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

  return core_priority;
}
