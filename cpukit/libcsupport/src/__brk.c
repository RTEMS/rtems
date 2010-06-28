/*
 *  RTEMS "Broken" __brk Implementation
 *
 *  NOTE: sbrk() is provided by each BSP.
 *
 *  COPYRIGHT (c) 1989-1999.
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

#if defined(RTEMS_NEWLIB) && !defined(HAVE___BRK)

#include <errno.h>
#include <rtems/seterr.h>

int __brk(
  const void *endds __attribute__((unused))
)
{
  rtems_set_errno_and_return_minus_one( EINVAL );
}
#endif
