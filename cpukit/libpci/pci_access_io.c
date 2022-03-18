/* SPDX-License-Identifier: BSD-2-Clause */

/*  PCI Access Library
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
