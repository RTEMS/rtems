/* PCI Read Configuration Library. Read current config that bootloader/BIOS
 * has setup.
 *
 * COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */


#ifndef __PCI_CFG_READ_H__
#define __PCI_CFG_READ_H__

/* Build PCI device tree in "struct pci_bus pci_hb" according to current setup
 * in hardware. Devices/buses are created by reading the resource assignments
 * that the BIOS/bootloader has already setup for us.
 */
extern int pci_config_read(void);

#endif
