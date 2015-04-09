/*  PCI Access Library
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci.h>
#include <pci/access.h>

/* Read a 8-bit register over PCI I/O Space */
uint8_t pci_io_r8(uint32_t adr)
{
	return pci_access_ops.io.read8((uint8_t *)adr);
}

/* Read a 16-bit I/O Register */
uint16_t pci_io_r16(uint32_t adr)
{
	return pci_access_ops.io.read16((uint16_t *)adr);
}

/* Read a 32-bit I/O Register */
uint32_t pci_io_r32(uint32_t adr)
{
	return pci_access_ops.io.read32((uint32_t *)adr);
}

/* Write a 8-bit I/O Register */
void pci_io_w8(uint32_t adr, uint8_t data)
{
	pci_access_ops.io.write8((uint8_t *)adr, data);
}

/* Write a 16-bit I/O Register */
void pci_io_w16(uint32_t adr, uint16_t data)
{
	pci_access_ops.io.write16((uint16_t *)adr, data);
}

/* Write a 32-bit I/O Register */
void pci_io_w32(uint32_t adr, uint32_t data)
{
	pci_access_ops.io.write32((uint32_t *)adr, data);
}
