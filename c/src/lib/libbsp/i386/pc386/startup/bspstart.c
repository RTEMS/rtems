/*-------------------------------------------------------------------------+
| This file contains the PC386 BSP startup package. It includes application,
| board, and monitor specific initialization and configuration. The generic CPU
| dependent initialization has been performed before this routine is invoked.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   bspstart.c,v 1.8 1996/05/28 13:12:40 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-2008.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in the file LICENSE in this distribution or at
| *  http://www.rtems.org/license/LICENSE.
| **************************************************************************
+--------------------------------------------------------------------------*/

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/pci.h>
#include <libcpu/cpuModel.h>

/*
 * PCI Bus Configuration
 */
rtems_pci_config_t BSP_pci_configuration = {
  (volatile unsigned char*)0,
  (volatile unsigned char*)0,
  NULL
};

/*
 * Helper to initialize the PCI Bus
 */
static void bsp_pci_initialize_helper(void)
{
#if (BSP_IS_EDISON == 0)
  const pci_config_access_functions *pci_accessors;

  pci_accessors = pci_bios_initialize();
  if (pci_accessors != NULL) {
    printk("PCI bus: using PCI BIOS interface\n");
    BSP_pci_configuration.pci_functions = pci_accessors;
    return;
  }

  pci_accessors = pci_io_initialize();
  if (pci_accessors != NULL) {
    printk("PCI bus: using PCI I/O interface\n");
    BSP_pci_configuration.pci_functions = pci_accessors;
    return;
  }

  printk("PCI bus: could not initialize PCI BIOS interface\n");
#endif
}

/*-------------------------------------------------------------------------+
|         Function: bsp_start
|      Description: Called before main is invoked.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
static void bsp_start_default( void )
{
  /*
   * Turn off watchdog
   */
#if (BSP_IS_EDISON == 1)
  volatile uint32_t *edison_wd = (volatile uint32_t *)0xff009000;
  *edison_wd = 0x11f8;
#endif

  /*
   * Calibrate variable for 1ms-loop (see timer.c)
   */
#if (BSP_IS_EDISON == 0)
  Calibrate_loop_1ms();
#endif

  /*
   * Init rtems interrupt management
   */
  rtems_irq_mngt_init();

  /*
   * Init rtems exceptions management
   */
  rtems_exception_init_mngt();

  /*
   * init PCI Bios interface...
   */
  bsp_pci_initialize_helper();

  /*
   * Figure out where printk() and console IO is to be directed.
   * Do this after the PCI bus is initialized so we have a chance
   * for those devices to be added to the set in the console driver.
   * In general, Do it as early as possible so printk() has a chance
   * to work early on devices found via PCI probe.
   */
  pc386_parse_console_arguments();

#if (BSP_IS_EDISON == 0)
  Clock_driver_install_handler();
#endif

#if BSP_ENABLE_IDE
  bsp_ide_cmdline_init();
#endif

} /* bsp_start_default */

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */
void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
