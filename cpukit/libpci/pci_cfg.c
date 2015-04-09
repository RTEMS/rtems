/*  PCI Configuration Library
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci/cfg.h>

/* Number of buses. This is set from respective library */
int pci_bus_cnt = 0;

/* PCI Address assigned to BARs which failed to fit into the PCI Window or
 * is disabled by any other cause.
 */
uint32_t pci_invalid_address = 0;

/* PCI System type. Configuration Library setup this */
enum pci_system_type pci_system_type = PCI_SYSTEM_NONE;

/* PCI Endianness.
 *
 * Host driver or BSP must override this be writing here if bus is defined
 * as non-standard big-endian.
 */
int pci_endian = PCI_LITTLE_ENDIAN;

/* Configure PCI devices and bridges, and setup the RAM data structures
 * describing the PCI devices currently present in the system
 */
int pci_config_init(void)
{
	if (pci_config_lib_init)
		return pci_config_lib_init();
	else
		return 0;
}

void pci_config_register(void *config)
{
	if (pci_config_lib_register)
		pci_config_lib_register(config);
}

/* Return the number of PCI busses available in the system, note that
 * there are always one bus (bus0) after the PCI library has been
 * initialized and a driver has been registered.
 */
int pci_bus_count(void)
{
	return pci_bus_cnt;
}
