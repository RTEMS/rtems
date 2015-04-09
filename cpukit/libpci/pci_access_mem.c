/*  PCI Access Library
 *  Registers-over-Memory Space - Generic Big/Little endian PCI bus definitions
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci.h>

uint8_t pci_mem_ld8(uint8_t *adr)
{
	return *adr;
}

void pci_mem_st8(uint8_t *adr, uint8_t data)
{
	*adr = data;
}
