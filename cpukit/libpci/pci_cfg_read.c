/*  Read current PCI configuration that bootloader or BIOS has already setup
 *  and initialize the PCI structures.
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <stdlib.h>
#include <rtems/bspIo.h>
#include <pci/cfg.h>
#include <pci/access.h>

#include "pci_internal.h"

/* PCI Library
 * (For debugging it might be good to use other functions or the driver's
 *  directly)
 */
#define PCI_CFG_R8(dev, args...) pci_cfg_r8(dev, args)
#define PCI_CFG_R16(dev, args...) pci_cfg_r16(dev, args)
#define PCI_CFG_R32(dev, args...) pci_cfg_r32(dev, args)
#define PCI_CFG_W8(dev, args...) pci_cfg_w8(dev, args)
#define PCI_CFG_W16(dev, args...) pci_cfg_w16(dev, args)
#define PCI_CFG_W32(dev, args...) pci_cfg_w32(dev, args)

#ifdef DEBUG
#define DBG(args...)	printk(args)
#else
#define DBG(args...)
#endif

/* The Host Bridge bus is initialized here */
extern struct pci_bus pci_hb;

static struct pci_dev *pci_dev_create(int isbus)
{
	void *ptr;
	int size;

	if (isbus)
		size = sizeof(struct pci_bus);
	else
		size = sizeof(struct pci_dev);

	ptr = calloc(1, size);
	if (!ptr)
		rtems_fatal_error_occurred(RTEMS_NO_MEMORY);
	return ptr;
}

/* Check if address is accessible from host */
static int pci_read_addressable(struct pci_dev *dev, struct pci_res *res)
{
	struct pci_bus *bus = dev->bus;
	int type = res->flags & PCI_RES_TYPE_MASK;
	struct pci_res *range0, *range1;

	if (type == PCI_BUS_IO && (bus->flags & PCI_BUS_IO) == 0)
		return 0;

	/* Assume that host bridge can access all */
	if (bus->pri == 0)
		return 1;

	range1 = NULL;
	switch (type) {
	case PCI_RES_IO:
		range0 = &bus->dev.resources[BRIDGE_RES_IO];
		break;
	case PCI_RES_MEM:
		range1 = &bus->dev.resources[BRIDGE_RES_MEM];
	default:
	case PCI_RES_MEMIO:
		range0 = &bus->dev.resources[BRIDGE_RES_MEMIO];
		break;
	}
	if ((res->start >= range0->start) && (res->end <= range0->end)) {
		return pci_read_addressable(&bus->dev, range0);
	} else if (range1 && (res->start >= range1->start) &&
			(res->end <= range1->end)) {
		return pci_read_addressable(&bus->dev, range1);
	}

	return 0;
}

static void pci_read_bar(struct pci_dev *dev, int bar)
{
	uint32_t orig, size, mask;
	struct pci_res *res = &dev->resources[bar];
	pci_dev_t pcidev = dev->busdevfun;
	int ofs;
#ifdef DEBUG
	char *str;
#define DBG_SET_STR(str, val) str = (val)
#else
#define DBG_SET_STR(str, val)
#endif

	DBG("Bus: %x, Slot: %x, function: %x, bar%d\n",
		PCI_DEV_EXPAND(pcidev), bar);

	res->bar = bar;
	if (bar == DEV_RES_ROM) {
		if (dev->flags & PCI_DEV_BRIDGE)
			ofs = PCIR_BIOS_1;
		else
			ofs = PCIR_BIOS;
	} else {
		ofs = PCIR_BAR(0) + (bar << 2);
	}

	PCI_CFG_R32(pcidev, ofs, &orig);
	PCI_CFG_W32(pcidev, ofs, 0xffffffff);
	PCI_CFG_R32(pcidev, ofs, &size);
	PCI_CFG_W32(pcidev, ofs, orig);

	if (size == 0 || size == 0xffffffff)
		return;
	if (bar == DEV_RES_ROM) {
		mask = PCIM_BIOS_ADDR_MASK;
		DBG_SET_STR(str, "ROM");
		if (dev->bus->flags & PCI_BUS_MEM)
			res->flags = PCI_RES_MEM;
		else
			res->flags = PCI_RES_MEMIO;
	} else if (((size & 0x1) == 0) && (size & 0x6)) {
		/* unsupported Memory type */
		return;
	} else {
		mask = ~0xf;
		if (size & 0x1) {
			/* I/O */
			mask = ~0x3;
			res->flags = PCI_RES_IO;
			DBG_SET_STR(str, "I/O");
			if (size & 0xffff0000)
				res->flags |= PCI_RES_IO32;
			/* Limit size of I/O space to 256 byte */
			size |= 0xffffff00;
			if ((dev->bus->flags & PCI_BUS_IO) == 0) {
				res->flags |= PCI_RES_FAIL;
				dev->flags |= PCI_DEV_RES_FAIL;
			}
		} else {
			/* Memory */
			if (size & 0x8) {
				/* Prefetchable */
				res->flags = PCI_RES_MEM;
				DBG_SET_STR(str, "MEM");
			} else {
				res->flags = PCI_RES_MEMIO;
				DBG_SET_STR(str, "MEMIO");
			}
		}
	}
	res->start = orig & mask;
	size &= mask;
	res->size = ~size + 1;
	res->boundary = res->size;
	res->end = res->start +  res->size;

	DBG("Bus: %x, Slot: %x, function: %x, %s bar%d size: %x\n",
		PCI_DEV_EXPAND(pcidev), str, bar, res->size);

	/* Check if BAR is addressable by host */
	if (pci_read_addressable(dev, res) == 0) {
		/* No matching bridge window contains this BAR */
		res->flags |= PCI_RES_FAIL;
		dev->flags |= PCI_DEV_RES_FAIL;
	}
}

static void pci_read_devs(struct pci_bus *bus)
{
	uint32_t id, tmp;
	uint16_t tmp16;
	uint8_t header;
	int slot, func, fail, i, maxbars, max_sord;
	struct pci_dev *dev, **listptr;
	struct pci_bus *bridge;
	pci_dev_t pcidev;
	struct pci_res *res;

	DBG("Scanning bus %d\n", bus->num);

	max_sord = bus->num;
	listptr = &bus->devs;
	for (slot = 0; slot <= PCI_SLOTMAX; slot++) {

		/* Slot address */
		pcidev = PCI_DEV(bus->num, slot, 0);

		for (func = 0; func <= PCI_FUNCMAX; func++, pcidev++) {

			fail = PCI_CFG_R32(pcidev, PCIR_VENDOR, &id);
			if (fail || id == 0xffffffff || id == 0) {
				/*
				 * This slot is empty
				 */
				if (func == 0)
					break;
				else
					continue;
			}

			DBG("Found PCIDEV 0x%x at (bus %x, slot %x, func %x)\n",
							id, bus, slot, func);

			PCI_CFG_R32(pcidev, PCIR_REVID, &tmp);
			tmp >>= 16;
			dev = pci_dev_create(tmp == PCID_PCI2PCI_BRIDGE);
			*listptr = dev;
			listptr = &dev->next;

			dev->busdevfun = pcidev;
			dev->bus = bus;
			PCI_CFG_R16(pcidev, PCIR_VENDOR, &dev->vendor);
			PCI_CFG_R16(pcidev, PCIR_DEVICE, &dev->device);
			PCI_CFG_R32(pcidev, PCIR_REVID, &dev->classrev);

			if (tmp == PCID_PCI2PCI_BRIDGE) {
				DBG("Found PCI-PCI Bridge 0x%x at "
				    "(bus %x, slot %x, func %x)\n",
				    id, bus, slot, func);
				dev->flags = PCI_DEV_BRIDGE;
				bridge = (struct pci_bus *)dev;

				PCI_CFG_R32(pcidev, PCIR_PRIBUS_1, &tmp);
				bridge->pri = tmp & 0xff;
				bridge->num = (tmp >> 8) & 0xff;
				bridge->sord = (tmp >> 16) & 0xff;
				if (bridge->sord > max_sord)
					max_sord = bridge->sord;

				DBG("    Primary %x, Secondary %x, "
				    "Subordinate %x\n",
				    bridge->pri, bridge->num, bridge->sord);

				/*** Probe Bridge Spaces ***/

				/* MEMIO Window - always implemented */
				bridge->flags = PCI_BUS_MEMIO;
				res = &bridge->dev.resources[BRIDGE_RES_MEMIO];
				res->flags = PCI_RES_MEMIO;
				res->bar = BRIDGE_RES_MEMIO;
				PCI_CFG_R32(pcidev, 0x20, &tmp);
				res->start = (tmp & 0xfff0) << 16;
				res->end = 1 + ((tmp & 0xfff00000) | 0xfffff);
				if (res->end <= res->start) {
					/* Window disabled */
					res->end = res->start = 0;
				}
				res->size = res->end - res->start;

				/* I/O Window - optional */
				res = &bridge->dev.resources[BRIDGE_RES_IO];
				res->bar = BRIDGE_RES_IO;
				PCI_CFG_R32(pcidev, 0x30, &tmp);
				PCI_CFG_R16(pcidev, 0x1c, &tmp16);
				if (tmp != 0 || tmp16 != 0) {
					bridge->flags |= PCI_BUS_IO;
					res->flags = PCI_RES_IO;
					if (tmp16 & 0x1) {
						bridge->flags |= PCI_BUS_IO32;
						res->flags |= PCI_RES_IO32;
					}

					res->start = (tmp & 0xffff) << 16 |
							(tmp16 & 0xf0) << 8;
					res->end = 1 + ((tmp & 0xffff0000) |
							(tmp16 & 0xf000) |
							0xfff);
					if (res->end <= res->start) {
						/* Window disabled */
						res->end = res->start = 0;
					}
					res->size = res->end - res->start;
				}

				/* MEM Window - optional */
				res = &bridge->dev.resources[BRIDGE_RES_MEM];
				res->bar = BRIDGE_RES_MEM;
				PCI_CFG_R32(pcidev, 0x24, &tmp);
				if (tmp != 0) {
					bridge->flags |= PCI_BUS_MEM;
					res->flags = PCI_RES_MEM;
					res->start = (tmp & 0xfff0) << 16;
					res->end = 1 + ((tmp & 0xfff00000) |
							0xfffff);
					if (res->end <= res->start) {
						/* Window disabled */
						res->end = res->start = 0;
					}
					res->size = res->end - res->start;
				}

				/* Scan Secondary Bus */
				pci_read_devs(bridge);

				/* Only 2 BARs for Bridges */
				maxbars = 2;
			} else {
				/* Devices have subsytem device and vendor ID */
				PCI_CFG_R16(pcidev, PCIR_SUBVEND_0,
							&dev->subvendor);
				PCI_CFG_R16(pcidev, PCIR_SUBDEV_0,
							&dev->subdevice);

				/* Normal PCI Device has max 6 BARs */
				maxbars = 6;
			}

			/* Probe BARs */
			for (i = 0; i < maxbars; i++)
				pci_read_bar(dev, i);
			pci_read_bar(dev, DEV_RES_ROM);

			/* Get System Interrupt/Vector for device.
			 * 0 means no-IRQ
			 */
			PCI_CFG_R8(pcidev, PCIR_INTLINE, &dev->sysirq);

			/* Stop if not a multi-function device */
			if (func == 0) {
				pci_cfg_r8(pcidev, PCIR_HDRTYPE, &header);
				if ((header & PCIM_MFDEV) == 0)
					break;
			}
		}
	}

	if (bus->num == 0)
		bus->sord = max_sord;
}

int pci_config_read(void)
{
	pci_system_type = PCI_SYSTEM_HOST;

	/* Find all devices and buses */
	pci_hb.flags = PCI_BUS_IO|PCI_BUS_MEMIO|PCI_BUS_MEM;
	pci_hb.dev.flags = PCI_DEV_BRIDGE;
	pci_read_devs(&pci_hb);
	pci_bus_cnt = pci_hb.sord + 1;
	if (pci_hb.devs == NULL)
		return 0;

	return 0;
}
