/**
 * @file
 *
 * BSP specific helpers
 */

/*
 *  COPYRIGHT (c) 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __BSPIMPL_h
#define __BSPIMPL_h

#include <rtems/pci.h>

/*
 * PCI Support Methods
 */
const pci_config_access_functions *pci_bios_initialize(void);
const pci_config_access_functions *pci_io_initialize(void);

/*
 * Helper to parse boot command line arguments related to the console driver
 */
void pc386_parse_console_arguments(void);

/*
 * Helper to parse boot command line arguments related to gdb
 */
void pc386_parse_gdb_arguments(void);

/*
 * Dynamically probe for Legacy UARTS
 */
void legacy_uart_probe(void);

/*
 * Dynamically probe for PCI UARTS
 */
void pci_uart_probe(void);

#endif
