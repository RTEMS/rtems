/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <umon/monlib.h>

/* XXX eventually add lock/unlock methods */

void rtems_umon_connect(void)
{
  void *moncomptr;

  moncomptr = rtems_bsp_get_umon_monptr();
  monConnect(
    (int(*)())(*(unsigned long *)moncomptr),      /* monitor base */
    (void *)0,                                    /* lock */
    (void *)0                                     /* unlock */
  );
}
