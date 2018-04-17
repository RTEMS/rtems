/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/sysinit.h>

extern void Spurious_Initialize(void);

/*
 * Must be called after the _ISR_Vector_Table allocation
 */
RTEMS_SYSINIT_ITEM(
  Spurious_Initialize,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

void bsp_start(void)
{
  /* Nothing to do */
}
