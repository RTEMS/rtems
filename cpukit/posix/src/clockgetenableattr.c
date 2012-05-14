/*
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

#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/seterr.h>
#include <rtems/posix/time.h>

/*
 *  20.1.5 CPU-time Clock Attribute Access, P1003.4b/D8, p. 58
 */

int clock_getenable_attr(
  clockid_t    clock_id,
  int         *attr
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
