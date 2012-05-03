/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/sigset.h>
#include <rtems/seterr.h>

int sigfillset(
  sigset_t   *set
)
{
  if ( !set )
    rtems_set_errno_and_return_minus_one( EINVAL );

  *set = SIGNAL_ALL_MASK;
  return 0;
}
