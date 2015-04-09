/* PCI Peripheral Configuration Library
 *
 * COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __PCI_CFG_PERIPHERAL_H__
#define __PCI_CFG_PERIPHERAL_H__

/* The user must provide a PCI configuration using the "struct pci_bus pci_hb"
 * structure. Nothing else than setting pci_system_type and pci_bus_cnt is done
 * by the peripheral library.
 */
extern int pci_config_peripheral(void);

#endif
