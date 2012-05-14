/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/posix/priority.h>

bool _POSIX_Priority_Is_valid(
  int priority
)
{
  return ((priority >= POSIX_SCHEDULER_MINIMUM_PRIORITY) &&
          (priority <= POSIX_SCHEDULER_MAXIMUM_PRIORITY));

}

