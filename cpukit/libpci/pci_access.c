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

/* Access Routines valid after a PCI-Access-Driver has registered */
struct pci_access_drv pci_access_ops;

/* Read a 8-bit register over configuration space */
int pci_cfg_r8(pci_dev_t dev, int ofs, uint8_t *data)
{
	return pci_access_ops.cfg.read8(dev, ofs, data);
}

/* Read a 16-bit register over configuration space */
int pci_cfg_r16(pci_dev_t dev, int ofs, uint16_t *data)
{
	return pci_access_ops.cfg.read16(dev, ofs, data);
}

/* Read a 32-bit register over configuration space */
int pci_cfg_r32(pci_dev_t dev, int ofs, uint32_t *data)
{
	return pci_access_ops.cfg.read32(dev, ofs, data);
}

/* Write a 8-bit register over configuration space */
int pci_cfg_w8(pci_dev_t dev, int ofs, uint8_t data)
{
	return pci_access_ops.cfg.write8(dev, ofs, data);
}

/* Write a 16-bit register over configuration space */
int pci_cfg_w16(pci_dev_t dev, int ofs, uint16_t data)
{
	return pci_access_ops.cfg.write16(dev, ofs, data);
}

/* Write a 32-bit register over configuration space */
int pci_cfg_w32(pci_dev_t dev, int ofs, uint32_t data)
{
	return pci_access_ops.cfg.write32(dev, ofs, data);
}

void pci_modify_cmdsts(pci_dev_t dev, uint32_t mask, uint32_t val)
{
	uint32_t data;

	pci_cfg_r32(dev, PCIR_COMMAND, &data);
	data &= ~mask;
	data |= val;
	pci_cfg_w32(dev, PCIR_COMMAND, data);
}

/* Register a driver for handling access to PCI */
int pci_access_drv_register(struct pci_access_drv *drv)
{
	if (pci_access_ops.cfg.read8)
		return -1; /* Already registered a driver.. */

	pci_access_ops = *drv;

	return 0;
}
