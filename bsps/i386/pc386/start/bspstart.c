/**
 * @file
 *
 * This file contains the PC386 BSP startup package. It includes application,
 * board, and monitor specific initialization and configuration.
 */

/*
 * This file was initially written by Joel Sherrill as part of the go32 BSP.
 * It was subsequently adapted as part of the pc386 BSP by developers from
 * the NavIST Group in 1997.
 *
 * Copyright (c) 2016.
 * Chris Johns <chrisj@rtems.org>
 *
 * COPYRIGHT (c) 1989-2008, 2016.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 * (C) Copyright 1997 -
 * - NavIST Group - Real-Time Distributed Systems and Industrial Automation
 * Instituto Superior Tecnico * Lisboa * PORTUGAL (http://pandora.ist.utl.pt)
 *
 * NavISY Disclaimer:
 * This file is provided "AS IS" without warranty of any kind, either
 * expressed or implied.
 */

#include <bsp.h>
#include <bsp/bspimpl.h>
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
  /*
   * Calibrate variable for 1ms-loop (see timer.c)
   */
  Calibrate_loop_1ms();

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
   * Probe for legacy UARTs.
   */
  legacy_uart_probe();

  /*
   * Probe for UARTs on PCI.
   */
  pci_uart_probe();

  /*
   * Parse the GDB arguments and flag a serial port as not valid. This stops
   * the console from claming the port.
   */
#if BSP_GDB_STUB
  pc386_parse_gdb_arguments();
#endif

  /*
   * Figure out where printk() and console IO is to be directed.  Do this after
   * the legacy and PCI bus probes so we have a chance for those devices to be
   * added to the set in the console driver.  In general, do it as early as
   * possible so printk() has a chance to work early on devices found via PCI
   * probe.
   */
  pc386_parse_console_arguments();

  Clock_driver_install_handler();

#if BSP_ENABLE_IDE
  bsp_ide_cmdline_init();
#endif
} /* bsp_start_default */

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */
void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
