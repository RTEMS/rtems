/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Set Scheduling Parameters
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sched.h>
#include <errno.h>

#include <rtems/score/basedefs.h>
#include <rtems/seterr.h>

/**
 * 13.3.2 Set Scheduling Parameters, P1003.1b-1993, p. 253
 */
int sched_getparam(
  pid_t                     pid RTEMS_UNUSED,
  struct sched_param       *param RTEMS_UNUSED
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
