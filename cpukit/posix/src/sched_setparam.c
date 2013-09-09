/**
 * @file
 *
 * @brief Sets Scheduling Parameters Associated with Scheduling Policies 
 * @ingroup POSIXAPI
 */

/*
 *  13.3.1 Set Scheduling Parameters, P1003.1b-1993, p. 252
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sched.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/seterr.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/time.h>

int sched_setparam(
  pid_t                     pid __attribute__((unused)),
  const struct sched_param *param __attribute__((unused))
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
