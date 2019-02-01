/**
 * @file
 *
 * @brief Set Scheduling Policy and Scheduling Parameters
 * @ingroup POSIXAPI
 */

/*
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

int sched_setscheduler(
  pid_t                     pid RTEMS_UNUSED,
  int                       policy RTEMS_UNUSED,
  const struct sched_param *param RTEMS_UNUSED
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
