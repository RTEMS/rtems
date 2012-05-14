/*
 *  COPYRIGHT (c) 1989-20089
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <rtems/seterr.h>

int getitimer(
  int               which,
  struct itimerval *value
)
{
  if ( !value )
    rtems_set_errno_and_return_minus_one( EFAULT );

  switch ( which ) {
    case ITIMER_REAL:
    case ITIMER_VIRTUAL:
    case ITIMER_PROF:
      rtems_set_errno_and_return_minus_one( ENOSYS );
    default:
      break;
  }
  rtems_set_errno_and_return_minus_one( EINVAL );
}

