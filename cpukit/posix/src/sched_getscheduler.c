/**
 * @file
 *
 * @brief Function Returns Scheduling Policy of the process specified by pid
 * @ingroup POSIXAPI
 */

/*
 *  13.3.4 Get Scheduling Policy, P1003.1b-1993, p. 256
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sched.h>
#include <errno.h>

#include <rtems/score/thread.h>
#include <rtems/seterr.h>
#include <rtems/posix/priorityimpl.h>

int sched_getscheduler(
  pid_t                     pid RTEMS_UNUSED
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
