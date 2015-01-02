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
