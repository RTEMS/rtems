/* SPDX-License-Identifier: BSD-2-Clause */

/*  PCI Access Library
 *  Registers-over-Memory Space - Generic Big endian PCI bus definitions
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <pci.h>

uint16_t pci_mem_be_ld_le16(uint16_t *adr)
{
	return ld_be16(adr);
}

uint16_t pci_mem_be_ld_be16(uint16_t *adr)
{
	return ld_le16(adr);
}

uint32_t pci_mem_be_ld_le32(uint32_t *adr)
{
	return ld_be32(adr);
}

uint32_t pci_mem_be_ld_be32(uint32_t *adr)
{
	return ld_le32(adr);
}

void pci_mem_be_st_le16(uint16_t *adr, uint16_t data)
{
	st_be16(adr, data);
}

void pci_mem_be_st_be16(uint16_t *adr, uint16_t data)
{
	st_le16(adr, data);
}

void pci_mem_be_st_le32(uint32_t *adr, uint32_t data)
{
	st_be32(adr, data);
}

void pci_mem_be_st_be32(uint32_t *adr, uint32_t data)
{
	st_le32(adr, data);
}

struct pci_memreg_ops pci_mem_be_ops = {
	.ld8    = pci_mem_ld8,
	.st8    = pci_mem_st8,

	.ld_le16 = pci_mem_be_ld_le16,
	.st_le16 = pci_mem_be_st_le16,
	.ld_be16 = pci_mem_be_ld_be16,
	.st_be16 = pci_mem_be_st_be16,

	.ld_le32 = pci_mem_be_ld_le32,
	.st_le32 = pci_mem_be_st_le32,
	.ld_be32 = pci_mem_be_ld_be32,
	.st_be32 = pci_mem_be_st_be32,
};
