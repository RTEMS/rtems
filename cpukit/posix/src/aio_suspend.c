/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Suspends Process until Asynchronous I/O Operation completes
 */

/*
 *  6.7.7 Wait for Asynchronous I/O Request, P1003.1b-1993, p. 164
 *
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

#include <aio.h>
#include <errno.h>

#include <rtems/score/basedefs.h>
#include <rtems/seterr.h>

int aio_suspend(
  const struct aiocb  * const list[] RTEMS_UNUSED,
  int                     nent RTEMS_UNUSED,
  const struct timespec  *timeout RTEMS_UNUSED
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
