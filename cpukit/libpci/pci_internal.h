/* Private libpci declarations
 *
 * COPYRIGHT (c) 2015 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/* Number of buses */
extern int pci_bus_cnt;

/* Allocate a PCI device for a standard device or a bridge device */
struct pci_dev *pci_dev_create(int isbus);
