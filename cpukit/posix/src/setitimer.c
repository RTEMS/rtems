/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <rtems/seterr.h>

int setitimer(
  int                     which,
  const struct itimerval *value,
  struct itimerval       *ovalue
)
{
  switch ( which ) {
    case ITIMER_REAL:  break;
    case ITIMER_VIRTUAL: break;
    case ITIMER_PROF: break;
  }
  rtems_set_errno_and_return_minus_one( ENOSYS );
  /* return -1; */
}

