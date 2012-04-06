/*  Installs the BSP pre-driver hook
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

/*
 *  bsp_predriver_hook
 *
 *  BSP predriver hook. Called just before drivers are initialized.
 *  Is used to initialize shared interrupt handling.
 */
void bsp_predriver_hook( void )
{
  /* Initialize shared interrupt handling, must be done after IRQ
   * controller has been found and initialized.
   */
  BSP_shared_interrupt_init();
}
