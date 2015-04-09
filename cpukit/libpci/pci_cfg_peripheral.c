/*  PCI (Peripheral) Configuration Library
 *
 *  COPYRIGHT (c) 2011 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 *
 *  The Host Bridge bus must be declared by user. It contain the static
 *  configuration used to setup the devices/functions.
 */

/* Configure headers */
#define PCI_CFG_PERIPHERAL_LIB

#include <pci/cfg.h>

#include "pci_internal.h"

/* Assume that user has defined static setup array in pci_hb */
int pci_config_peripheral(void)
{
	pci_bus_cnt = pci_hb.sord + 1;
	pci_system_type = PCI_SYSTEM_PERIPHERAL;

	return 0;
}
