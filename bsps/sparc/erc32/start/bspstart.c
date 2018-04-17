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
#include <rtems/sysinit.h>

/*
 * Called just before drivers are initialized.  Is used to initialize shared
 * interrupt handling.
 */
static void erc32_pre_driver_hook( void )
{
  bsp_spurious_initialize();

  /* Initialize shared interrupt handling, must be done after IRQ
   * controller has been found and initialized.
   */
  BSP_shared_interrupt_init();
}

RTEMS_SYSINIT_ITEM(
  erc32_pre_driver_hook,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

void bsp_start(void)
{
  /* Nothing to do */
}
