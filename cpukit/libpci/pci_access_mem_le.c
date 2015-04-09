/*  PCI Access Library
 *  Registers-over-Memory Space - Generic Little endian PCI bus definitions
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci.h>

uint16_t pci_mem_le_ld_le16(uint16_t *adr)
{
	return ld_le16(adr);
}

uint16_t pci_mem_le_ld_be16(uint16_t *adr)
{
	return ld_be16(adr);
}

uint32_t pci_mem_le_ld_le32(uint32_t *adr)
{
	return ld_le32(adr);
}

uint32_t pci_mem_le_ld_be32(uint32_t *adr)
{
	return ld_be32(adr);
}

void pci_mem_le_st_le16(uint16_t *adr, uint16_t data)
{
	st_le16(adr, data);
}

void pci_mem_le_st_be16(uint16_t *adr, uint16_t data)
{
	st_be16(adr, data);
}

void pci_mem_le_st_le32(uint32_t *adr, uint32_t data)
{
	st_le32(adr, data);
}

void pci_mem_le_st_be32(uint32_t *adr, uint32_t data)
{
	st_be32(adr, data);
}

struct pci_memreg_ops pci_mem_le_ops = {
	.ld8    = pci_mem_ld8,
	.st8    = pci_mem_st8,

	.ld_le16 = pci_mem_le_ld_le16,
	.st_le16 = pci_mem_le_st_le16,	
	.ld_be16 = pci_mem_le_ld_be16,
	.st_be16 = pci_mem_le_st_be16,
	.ld_le32 = pci_mem_le_ld_le32,
	.st_le32 = pci_mem_le_st_le32,
	.ld_be32 = pci_mem_le_ld_be32,
	.st_be32 = pci_mem_le_st_be32,
};
