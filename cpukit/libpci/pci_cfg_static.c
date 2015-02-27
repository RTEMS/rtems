/*  PCI (Static) Configuration Library
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
 
/*
 * The Host Bridge bus must be declared by user. It contains the static
 * configuration used to setup the devices/functions.
 */

/* Configure headers */
#define PCI_CFG_STATIC_LIB

#include <stdlib.h>
#include <pci.h>
#include <pci/access.h>
#include <pci/cfg.h>

#define PCI_CFG_R8(dev, args...) pci_cfg_r8(dev, args)
#define PCI_CFG_R16(dev, args...) pci_cfg_r16(dev, args)
#define PCI_CFG_R32(dev, args...) pci_cfg_r32(dev, args)
#define PCI_CFG_W8(dev, args...) pci_cfg_w8(dev, args)
#define PCI_CFG_W16(dev, args...) pci_cfg_w16(dev, args)
#define PCI_CFG_W32(dev, args...) pci_cfg_w32(dev, args)

/* Number of buses */
extern int pci_bus_cnt;

/* Enumrate one bus if device is a bridge, and all it's subordinate buses */
static int pci_init_dev(struct pci_dev *dev, void *unused)
{
	uint32_t tmp;
	uint16_t tmp16, cmd;
	struct pci_bus *bridge;
	int maxbars, i, romofs;
	pci_dev_t pcidev = dev->busdevfun;
	struct pci_res *res;

	/* Init Device */

	/* Set command to reset values, it disables bus
	 * mastering and address responses.
	 */
	PCI_CFG_W16(pcidev, PCI_COMMAND, 0);

	/* Clear any already set status bits */
	PCI_CFG_W16(pcidev, PCI_STATUS, 0xf900);

	/* Set latency timer to 64 */
	PCI_CFG_W8(pcidev, PCI_LATENCY_TIMER, 64);

	/* Set System IRQ of PIN */
	PCI_CFG_W8(pcidev, PCI_INTERRUPT_LINE, dev->sysirq);

	cmd = dev->command;

	if ((dev->flags & PCI_DEV_BRIDGE) == 0) {
		/* Disable Cardbus CIS Pointer */
		PCI_CFG_W32(pcidev, PCI_CARDBUS_CIS, 0);

		romofs = PCI_ROM_ADDRESS;
		maxbars = 6;
	} else {
		/* Init Bridge */

		/* Configure bridge (no support for 64-bit) */
		PCI_CFG_W32(pcidev, PCI_PREF_BASE_UPPER32, 0);
		PCI_CFG_W32(pcidev, PCI_PREF_LIMIT_UPPER32, 0);

		bridge = (struct pci_bus *)dev;
		tmp = (64 << 24) | (bridge->sord << 16) |
			(bridge->num << 8) | bridge->pri;
		PCI_CFG_W32(pcidev, PCI_PRIMARY_BUS, tmp);

		/*** Setup I/O Bridge Window ***/
		res = &dev->resources[BRIDGE_RES_IO];
		if (res->size > 0) {
			tmp16 = ((res->end-1) & 0x0000f000) |
				((res->start & 0x0000f000) >> 8);
			tmp = ((res->end-1) & 0xffff0000) | (res->start >> 16);
			cmd |= PCI_COMMAND_IO;
		} else {
			tmp16 = 0x00ff;
			tmp = 0;
		}
		/* I/O Limit and Base */
		PCI_CFG_W16(pcidev, PCI_IO_BASE, tmp16);
		PCI_CFG_W32(pcidev, PCI_IO_BASE_UPPER16, tmp);

		/*** Setup MEMIO Bridge Window ***/
		res = &dev->resources[BRIDGE_RES_MEMIO];
		if (res->size > 0) {
			tmp = ((res->end-1) & 0xffff0000) |
				(res->start >> 16);
			cmd |= PCI_COMMAND_MEMORY;
		} else {
			tmp = 0x0000ffff;
		}
		/* MEMIO Limit and Base */
		PCI_CFG_W32(pcidev, PCI_MEMORY_BASE, tmp);

		/*** Setup MEM Bridge Window ***/
		res = &dev->resources[BRIDGE_RES_MEM];
		if (res->size > 0) {
			tmp = ((res->end-1) & 0xffff0000) |
				(res->start >> 16);
			cmd |= PCI_COMMAND_MEMORY;
		} else {
			tmp = 0x0000ffff;
		}
		/* MEM Limit and Base */
		PCI_CFG_W32(pcidev, PCI_PREF_MEMORY_BASE, tmp);
		/* 64-bit space not supported */
		PCI_CFG_W32(pcidev, PCI_PREF_BASE_UPPER32, 0);
		PCI_CFG_W32(pcidev, PCI_PREF_LIMIT_UPPER32, 0);

		cmd |= PCI_COMMAND_MASTER;
		romofs = PCI_ROM_ADDRESS1;
		maxbars = 2;
	}

	/* Init BARs */
	for (i = 0; i < maxbars; i++) {
		res = &dev->resources[i];
		if (res->flags & PCI_RES_TYPE_MASK) {
			PCI_CFG_W32(pcidev, PCI_BASE_ADDRESS_0 + 4*i,
								res->start);
			if ((res->flags & PCI_RES_TYPE_MASK) == PCI_RES_IO)
				cmd |= PCI_COMMAND_IO;
			else
				cmd |= PCI_COMMAND_MEMORY;
		}
	}
	res = &dev->resources[DEV_RES_ROM];
	if (res->flags & PCI_RES_TYPE_MASK) {
		PCI_CFG_W32(pcidev, romofs, res->start|PCI_ROM_ADDRESS_ENABLE);
		cmd |= PCI_COMMAND_MEMORY;
	}
	PCI_CFG_W16(pcidev, PCI_COMMAND, cmd);

	return 0;
}

/* Assume that user has defined static setup array in pci_hb */
int pci_config_static(void)
{
	pci_bus_cnt = pci_hb.sord + 1;
	pci_system_type = PCI_SYSTEM_HOST;

	/* Init all PCI devices according to depth-first search algorithm */
	return pci_for_each_dev(pci_init_dev, NULL);
}
