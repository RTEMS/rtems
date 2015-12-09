/*
 *  bsp_predriver_hook().
 *  Optionally initializes I2C and VME.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Modified to support the MCP750.
 *  Modifications Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 */

#include <string.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#ifndef BSP_HAS_NO_VME
#include <bsp/VME.h>
#endif

#ifdef SHOW_MORE_INIT_SETTINGS
#include <rtems/bspIo.h>
#endif

/*
 *  bsp_predriver_hook
 *
 *  Description:
 *      BSP predriver hook.  Called just before drivers are initialized.
 *
 *  NOTES:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 */

void bsp_predriver_hook(void)
{
#ifdef BSP_PREDRIVER_I2C_INIT
  /* Some drivers (RTC) may need i2c */
  BSP_i2c_initialize();
#endif

  /* Note that VME support may be omitted also by
   * providing a no-op  BSP_vme_config routine
   */
#ifndef BSP_HAS_NO_VME
  /*
   * Initialize VME bridge - needs working PCI
   * and IRQ subsystems...
   *
   * NOTE: vmeUniverse driver now uses shared interrupts.
   *       this requires malloc/free which are not available
   *       from bspstart()...
   */
#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Going to initialize VME bridge\n");
#endif
  BSP_vme_config();
#endif

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Leaving bsp_predriver_hook\n");
#endif
}
