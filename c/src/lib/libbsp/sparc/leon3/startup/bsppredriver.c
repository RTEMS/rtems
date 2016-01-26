/**
 * @file
 * @ingroup sparc_leon3
 * @brief Installs the BSP pre-driver hook
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

#include <drvmgr/drvmgr.h>

static void leon3_interrupt_common_init( void )
{
  /* Initialize shared interrupt handling, must be done after IRQ
   * controller has been found and initialized.
   */
  BSP_shared_interrupt_init();
}

/*
 *  bsp_predriver_hook
 *
 *  BSP predriver hook. Called just before drivers are initialized.
 *  Is used to initialize shared interrupt handling.
 */
void bsp_predriver_hook( void )
{
  bsp_spurious_initialize();

#ifndef RTEMS_DRVMGR_STARTUP
  leon3_interrupt_common_init();
#endif
}

#ifdef RTEMS_DRVMGR_STARTUP
/*
 *  bsp_driver_level_hook
 *
 *  BSP driver level hook. Called just after drivers have reached initialization
 *  level 'level' (1,2,3,4). See exinit.c for meaning of the every level.
 *
 *  Initializes the BSP Interrupt layer
 *  After Level 1 we can trust that interrupt controller and system
 *  clock timer drivers now have been initialized.
 */
void bsp_driver_level_hook( int level )
{
  /* Initialize shared interrupt handling, must be done after IRQ
   * controller has been found and initialized.
   */
  if (level != 1)
  	return;

  leon3_interrupt_common_init();
}
#endif
