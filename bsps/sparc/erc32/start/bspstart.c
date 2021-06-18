/*  Installs the BSP pre-driver hook
 */

/*
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <rtems/sysinit.h>

/*
 * Initialize shared interrupt handling, must be done after IRQ controller has
 * been found and initialized.
 */
RTEMS_SYSINIT_ITEM(
  bsp_interrupt_initialize,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

void bsp_start(void)
{
  /* Nothing to do */
}
