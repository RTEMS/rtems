/*  PCI (Auto) configuration Library. Setup PCI configuration space and IRQ.
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
#include <string.h>

/* Configure headers */
#define PCI_CFG_AUTO_LIB

#include <pci.h>
#include <pci/access.h>
#include <pci/cfg.h>

#include "pci_internal.h"

/* #define DEBUG */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

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

int pci_config_auto_initialized = 0;

/* Configuration setup */
struct pci_auto_setup pci_auto_cfg;

/* Insert BAR into the sorted resources list. The BARs are sorted on the
 * BAR size/alignment need.
 */
static void pci_res_insert(struct pci_res **root, struct pci_res *res)
{
	struct pci_res *curr, *last;
	unsigned long curr_size_resulting_boundary, size_resulting_boundary;
	unsigned long boundary, size;

	res->start = 0;
	res->end = 0;
	boundary = res->boundary;
	size = res->size;

	/* Insert the resources depending on the boundary needs
	 * Normally the boundary=size of the BAR, however when
	 * PCI bridges are involved the bridge's boundary may be
	 * smaller than the size due to the fact that a bridge
	 * may have different-sized BARs behind, the largest BAR
	 * (also the BAR with the largest boundary) will decide
	 * the alignment need.
	 */
	last = NULL;
	curr = *root;

	/* Order List after boundary, the boundary is maintained
	 * when the size is on an equal boundary, normally it is
	 * but may not be with bridges. So in second hand it is
	 * sorted after resulting boundary - the boundary after
	 * the resource.
	 */
	while (curr && (curr->boundary >= boundary)) {
		if (curr->boundary == boundary) {
			/* Find Resulting boundary of size */
			size_resulting_boundary = 1;
			while ((size & size_resulting_boundary) == 0)
				size_resulting_boundary =
					size_resulting_boundary << 1;

			/* Find Resulting boundary of curr->size */
			curr_size_resulting_boundary = 1;
			while ((curr->size & curr_size_resulting_boundary) == 0)
				curr_size_resulting_boundary =
					curr_size_resulting_boundary << 1;

			if (size_resulting_boundary >=
			    curr_size_resulting_boundary)
				break;
		}
		last = curr;
		curr = curr->next;
	}

	if (last == NULL) {
		/* Insert first in list */
		res->next = *root;
		*root = res;
	} else {
		last->next = res;
		res->next = curr;
	}
}

#ifdef DEBUG
void pci_res_list_print(struct pci_res *root)
{
	if (root == NULL)
		return;

	printf("RESOURCE LIST:\n");
	while (root) {
		printf(" SIZE: 0x%08x, BOUNDARY: 0x%08x\n", root->size,
								root->boundary);
		root = root->next;
	}
}
#endif

/* Reorder a size/alignment ordered resources list. The idea is to
 * avoid unused due to alignment/size restriction.
 *
 * NOTE: The first element is always untouched.
 * NOTE: If less than three elements in list, nothing will be done
 *
 * Normally a BAR has the same alignment requirements as the size of the
 * BAR. However, when bridges are involved the alignment need may be smaller
 * than the size, because a bridge resource consist or multiple BARs.
 * For example, say that a bridge with a 256Mb and a 16Mb BAR is found, then
 * the alignment is required to be 256Mb but the size 256+16Mb.
 *
 * In order to minimize dead space on the bus, the boundary ordered list
 * is reordered, example:
 *  BUS0
 *  |	         BUS1
 *  |------------|
 *  |            |-- BAR0: SIZE=256Mb, ALIGNMENT=256MB
 *  |            |-- BAR1: SIZE=16Mb, ALIGNMENT=16MB
 *  |            |
 *  |            |
 *  |            |
 *  |            |          BUS2 (BAR_BRIDGE1: SIZE=256+16, ALIGNEMENT=256)
 *  |            |----------|
 *  |            |          |-- BAR2: SIZE=256Mb, ALIGNMENT=256Mb
 *  |            |          |-- BAR3: SIZE=16Mb, ALIGNMENT=16MB
 *
 * A alignment/boundary ordered list of BUS1 will look like:
 *	- BAR_BRIDGE1
 *	- BAR0		  (ALIGMENT NEED 256Mb)
 *	- BAR1
 *
 * However, Between BAR_BRIDGE1 and BAR0 will be a unused hole of 256-16Mb.
 * We can put BAR1 before BAR0 to avoid the problem.
 */
static void pci_res_reorder(struct pci_res *root)
{
	struct pci_res *curr, *last, *curr2, *last2;
	unsigned int start, start_next, hole_size, hole_boundary;

	if (root == NULL)
		return;

	/* Make up a start address with the boundary of the
	 * First element.
	 */
	start = root->boundary + root->size;
	last = root;
	curr = root->next;
	while (curr) {

		/* Find start address of resource */
		start_next = (start + (curr->boundary - 1)) &
					~(curr->boundary - 1);

		/* Find hole size, the unsed space in between last resource
		 * and next */
		hole_size = start_next - start;

		/* Find Boundary of START */
		hole_boundary = 1;
		while ((start & hole_boundary) == 0)
			hole_boundary = hole_boundary<<1;

		/* Detect dead hole */
		if (hole_size > 0) {
			/* Step through list and try to find a resource that
			 * can fit into hole. Take into account hole start
			 * boundary and hole size.
			 */
			last2 = curr;
			curr2 = curr->next;
			while (curr2) {
				if ((curr2->boundary <= hole_boundary) &&
					 (curr2->size <= hole_size)) {
					/* Found matching resource. Move it
					 * first in the hole. Then rescan, now
					 * that the hole has changed in
					 * size/boundary.
					 */
					last2->next = curr2->next;
					curr2->next = curr;
					last->next = curr2;

					/* New Start address */
					start_next = (start +
						     (curr2->boundary - 1)) &
						     ~(curr2->boundary - 1);
					/* Since we inserted the resource before
					 * curr we need to re-evaluate curr one
					 * more, more resources may fit into the
					 * shrunken hole.
					 */
					curr = curr2;
					break;
				}
				last2 = curr2;
				curr2 = curr2->next;
			}
		}

		/* No hole or nothing fit into hole. */
		start = start_next;

		last = curr;
		curr = curr->next;
	}
}

/* Find the total size required in PCI address space needed by a resource list*/
static unsigned int pci_res_size(struct pci_res *root)
{
	struct pci_res *curr;
	unsigned int size;

	/* Get total size of all resources */
	size = 0;
	curr = root;
	while (curr) {
		size = (size + (curr->boundary - 1)) & ~(curr->boundary - 1);
		size += curr->size;
		curr = curr->next;
	}

	return size;
}

#if 0 /* not used for now */
/* Free a device and secondary bus if device is a bridge */
static void pci_dev_free(struct pci_dev *dev)
{
	struct pci_dev *subdev;
	struct pci_bus *bus;

	if (dev->flags & PCI_DEV_BRIDGE) {
		bus = (struct pci_bus *)dev;
		for (subdev = bus->devs; subdev ; subdev = subdev->next)
			pci_dev_free(dev);
	}

	free(dev);
}
#endif

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

static void pci_find_devs(struct pci_bus *bus)
{
	uint32_t id, tmp;
	uint8_t header;
	int slot, func, fail;
	struct pci_dev *dev, **listptr;
	struct pci_bus *bridge;
	pci_dev_t pcidev;

	DBG("Scanning bus %d\n", bus->num);

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

			/* Set command to reset values, it disables bus
			 * mastering and address responses.
			 */
			PCI_CFG_W16(pcidev, PCIR_COMMAND, 0);

			/* Clear any already set status bits */
			PCI_CFG_W16(pcidev, PCIR_STATUS, 0xf900);

			/* Set latency timer to 64 */
			PCI_CFG_W8(pcidev, PCIR_LATTIMER, 64);

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
				dev->subvendor = 0;
				dev->subdevice = 0;
				bridge = (struct pci_bus *)dev;
				bridge->num = bus->sord + 1;
				bridge->pri = bus->num;
				bridge->sord = bus->sord + 1;

				/* Configure bridge (no support for 64-bit) */
				PCI_CFG_W32(pcidev, 0x28, 0);
				PCI_CFG_W32(pcidev, 0x2C, 0);
				tmp = (64 << 24) | (0xff << 16) |
				      (bridge->num << 8) | bridge->pri;
				PCI_CFG_W32(pcidev, PCIR_PRIBUS_1, tmp);

				/* Scan Secondary Bus */
				pci_find_devs(bridge);

				/* sord might have been updated */
				PCI_CFG_W8(pcidev, 0x1a, bridge->sord);
				bus->sord = bridge->sord;

				DBG("PCI-PCI BRIDGE: Primary %x, Secondary %x, "
				    "Subordinate %x\n",
				    bridge->pri, bridge->num, bridge->sord);
			} else {
				/* Disable Cardbus CIS Pointer */
				PCI_CFG_W32(pcidev, PCIR_CIS, 0);

				/* Devices have subsytem device and vendor ID */
				PCI_CFG_R16(pcidev, PCIR_SUBVEND_0,
							&dev->subvendor);
				PCI_CFG_R16(pcidev, PCIR_SUBDEV_0,
							&dev->subdevice);
			}

			/* Stop if not a multi-function device */
			if (func == 0) {
				pci_cfg_r8(pcidev, PCIR_HDRTYPE, &header);
				if ((header & PCIM_MFDEV) == 0)
					break;
			}
		}
	}
}

static void pci_find_bar(struct pci_dev *dev, int bar)
{
	uint32_t size, disable, mask;
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
		disable = 0; /* ROM BARs have a unique enable bit per BAR */
	} else {
		ofs = PCIR_BAR(0) + (bar << 2);
		disable = pci_invalid_address;
	}

	PCI_CFG_W32(pcidev, ofs, 0xffffffff);
	PCI_CFG_R32(pcidev, ofs, &size);
	PCI_CFG_W32(pcidev, ofs, disable);

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
		PCI_CFG_W32(pcidev, ofs, 0);
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
			/* Memory. We convert Prefetchable Memory BARs to Memory
			 * BARs in case the Bridge does not support prefetchable
			 * memory.
			 */
			if ((size & 0x8) && (dev->bus->flags & PCI_BUS_MEM)) {
				/* Prefetchable and Bus supports it */
				res->flags = PCI_RES_MEM;
				DBG_SET_STR(str, "MEM");
			} else {
				res->flags = PCI_RES_MEMIO;
				DBG_SET_STR(str, "MEMIO");
			}
		}
	}
	size &= mask;
	res->size = ~size + 1;
	res->boundary = ~size + 1;

	DBG("Bus: %x, Slot: %x, function: %x, %s bar%d size: %x\n",
		PCI_DEV_EXPAND(pcidev), str, bar, res->size);
}

static int pci_find_res_dev(struct pci_dev *dev, void *unused)
{
	struct pci_bus *bridge;
	uint32_t tmp;
	uint16_t tmp16;
	pci_dev_t pcidev = dev->busdevfun;
	int i, maxbars;

	if (dev->flags & PCI_DEV_BRIDGE) {
		/* PCI-PCI Bridge */
		bridge = (struct pci_bus *)dev;

		/* Only 2 Bridge BARs */
		maxbars = 2;

		/* Probe Bridge Spaces (MEMIO space always implemented), the
		 * probe disables all space-decoding at the same time
		 */
		PCI_CFG_W32(pcidev, 0x30, 0);
		PCI_CFG_W16(pcidev, 0x1c, 0x00f0);
		PCI_CFG_R16(pcidev, 0x1c, &tmp16);
		if (tmp16 != 0) {
			bridge->flags |= PCI_BUS_IO;
			if (tmp16 & 0x1)
				bridge->flags |= PCI_BUS_IO32;
		}

		PCI_CFG_W32(pcidev, 0x24, 0x0000ffff);
		PCI_CFG_R32(pcidev, 0x24, &tmp);
		if (tmp != 0)
			bridge->flags |= PCI_BUS_MEM;

		PCI_CFG_W32(pcidev, 0x20, 0x0000ffff);
		bridge->flags |= PCI_BUS_MEMIO;
	} else {
		/* Normal PCI Device as max 6 BARs */
		maxbars = 6;
	}

	/* Probe BARs */
	for (i = 0; i < maxbars; i++)
		pci_find_bar(dev, i);
	pci_find_bar(dev, DEV_RES_ROM);

	return 0;
}

static int pci_add_res_dev(struct pci_dev *dev, void *arg);

static void pci_add_res_bus(struct pci_bus *bus, int type)
{
	int tindex = type - 1;

	/* Clear old resources */
	bus->busres[tindex] = NULL;

	/* Add resources of devices behind bridge if bridge supports
	 * resource type. If MEM space not supported by bridge, they are
	 * converted to MEMIO in the process.
	 */
	if (!((type == PCI_BUS_IO) && ((bus->flags & PCI_BUS_IO) == 0))) {
		pci_for_each_child(bus, pci_add_res_dev, (void *)type, 0);

		/* Reorder Bus resources to fit more optimally (avoid dead
		 * PCI space). Currently they are sorted by boundary and size.
		 *
		 * This is especially important when multiple buses (bridges)
		 * are present.
		 */
		pci_res_reorder(bus->busres[tindex]);
	}
}

static int pci_add_res_dev(struct pci_dev *dev, void *arg)
{
	int tindex, type = (int)arg;
	struct pci_bus *bridge;
	struct pci_res *res, *first_busres;
	int i;
	uint32_t bbound;

	/* Type index in Bus resource */
	tindex = type - 1;

	if (dev->flags & PCI_DEV_BRIDGE) {
		/* PCI-PCI Bridge. Add all sub-bus resources first */
		bridge = (struct pci_bus *)dev;

		/* Add all child device's resources to this type */
		pci_add_res_bus(bridge, type);

		/* Propagate the resources from child bus to BAR on
		 * this bus, by adding a "fake" BAR per type.
		 */
		res = &bridge->dev.resources[BUS_RES_START + tindex];
		res->bar = BUS_RES_START + tindex;
		res->start = 0;
		res->end = 0;
		res->flags = 0; /* mark BAR resource not available */
		first_busres = bridge->busres[tindex];
		if (first_busres) {
			res->flags = type;
			res->size = pci_res_size(first_busres);
			res->boundary = first_busres->boundary;
			if (type == PCI_RES_IO) {
				bbound = 0x1000; /* Bridge I/O min 4KB */
			} else {
				bbound = 0x100000; /* Bridge MEM min 1MB */

				/* Convert MEM to MEMIO if not supported by
				 * this bridge
				 */
				if ((bridge->flags & PCI_BUS_MEM) == 0)
					res->flags = PCI_RES_MEMIO;
			}
			/* Fulfil minimum bridge boundary */
			if (res->boundary < bbound)
				res->boundary = bbound;
			/* Make sure that size is atleast bridge boundary */
			if (res->size > bbound && (res->size & (bbound-1)))
				res->size = (res->size | (bbound-1)) + 1;
		}
	}

	/* Normal PCI Device as max 6 BARs and a ROM Bar.
	 * Insert BARs into the sorted resource list.
	 */
	for (i = 0; i < DEV_RES_CNT; i++) {
		res = &dev->resources[i];
		if ((res->flags & PCI_RES_TYPE_MASK) != type)
			continue;
		pci_res_insert(&dev->bus->busres[tindex], res);
	}

	return 0;
}

/* Function assumes that base is properly aligned to the requirement of the
 * largest BAR in the system.
 */
static uint32_t pci_alloc_res(struct pci_bus *bus, int type,
			    uint32_t start, uint32_t end)
{
	struct pci_dev *dev;
	struct pci_res *res, **prev_next;
	unsigned long starttmp;
	struct pci_bus *bridge;
	int removed, sec_type;

	/* The resources are sorted on their size (size and alignment is the
	 * same)
	 */
	prev_next = &bus->busres[type - 1];
	while ((res = *prev_next) != NULL) {

		dev = RES2DEV(res);
		removed = 0;

		/* Align start to this reource's need, only needed after
		 * a bridge resource has been allocated.
		 */
		starttmp = (start + (res->boundary-1)) & ~(res->boundary-1);

		if ((starttmp + res->size - 1) > end) {
			/* Not enough memory available for this resource */
			printk("PCI[%x:%x:%x]: DEV BAR%d (%d): no resource "
			       "assigned\n",
			       PCI_DEV_EXPAND(dev->busdevfun),
			       res->bar, res->flags & PCI_RES_TYPE_MASK);
			res->start = res->end = 0;

			/* If this resources is a bridge window to the
			 * secondary bus, the secondary resources are not
			 * changed which has the following effect:
			 *  I/O    :  Will never be assigned
			 *  MEMIO  :  Will never be assigned
			 *  MEM    :  Will stay marked as MEM, but bridge window
			 *            is changed into MEMIO, when the window is
			 *            assigned a MEMIO address the secondary
			 *            resources will also be assigned.
			 */

			if (type == PCI_RES_MEM) {
				/* Try prefetchable as non-prefetchable mem */
				res->flags &= ~PCI_RES_MEM_PREFETCH;
				/* Remove resource from MEM list, ideally we
				 * should regenerate this list in order to fit
				 * the comming BARs more optimially...
				 */
				*prev_next = res->next;
				/* We should not update prev_next here since
				 * we just removed the resource from the list
				 */
				removed = 1;
			} else {
				res->flags |= PCI_RES_FAIL;
				dev->flags |= PCI_DEV_RES_FAIL;
			}
		} else {
			start = starttmp;

			res->start = start;
			res->end = start + res->size;

			/* "Virtual BAR" on a bridge? A bridge resource need all
			 * its child devices resources allocated
			 */
			if ((res->bar != DEV_RES_ROM) &&
			    (dev->flags & PCI_DEV_BRIDGE) &&
			    (res->bar >= BUS_RES_START)) {
				bridge = (struct pci_bus *)dev;
				/* If MEM bar was changed into a MEMIO the
				 * secondary MEM resources are still set to MEM,
				 */
				if (type == PCI_BUS_MEMIO &&
				    res->bar == BRIDGE_RES_MEM)
					sec_type = PCI_RES_MEM;
				else
					sec_type = type;

				pci_alloc_res(bridge, sec_type, res->start,
						res->end);
			}

			start += res->size;
		}
		if (removed == 0)
			prev_next = &res->next;
	}

	return start;
}

static void pci_set_bar(struct pci_dev *dev, int residx)
{
	uint32_t tmp;
	uint16_t tmp16;
	pci_dev_t pcidev;
	struct pci_res *res;
	int is_bridge, ofs;

	res = &dev->resources[residx];
	pcidev = dev->busdevfun;

	if ((res->flags == 0) || (res->flags & PCI_RES_FAIL))
		return;

	is_bridge = dev->flags & PCI_DEV_BRIDGE;

	if (res->bar == DEV_RES_ROM) {
		/* ROM: 32-bit prefetchable memory BAR */
		if (is_bridge)
			ofs = PCIR_BIOS_1;
		else
			ofs = PCIR_BIOS;
		PCI_CFG_W32(pcidev, ofs, res->start | PCIM_BIOS_ENABLE);
		DBG("PCI[%x:%x:%x]: ROM BAR: 0x%x-0x%x\n",
			PCI_DEV_EXPAND(pcidev), res->start, res->end);
	} else if (is_bridge && (res->bar == BRIDGE_RES_IO)) {
		/* PCI Bridge I/O BAR */
		DBG("PCI[%x:%x:%x]: BAR 1C: 0x%x-0x%x\n",
			PCI_DEV_EXPAND(pcidev), res->start, res->end);

		/* Limit and Base */
		tmp16 = ((res->end-1) & 0x0000f000) |
			((res->start & 0x0000f000) >> 8);
		tmp = ((res->end-1) & 0xffff0000) | (res->start >> 16);

		DBG("PCI[%x:%x:%x]: BRIDGE BAR 0x%x: 0x%08x [0x30: 0x%x]\n",
			PCI_DEV_EXPAND(pcidev), 0x1C, tmp, tmp2);
		PCI_CFG_W16(pcidev, 0x1C, tmp16);
		PCI_CFG_W32(pcidev, 0x30, tmp);
	} else if (is_bridge && (res->bar >= BRIDGE_RES_MEMIO)) {
		/* PCI Bridge MEM and MEMIO Space */

		/* Limit and Base */
		tmp = ((res->end-1) & 0xfff00000) | (res->start >> 16);

		DBG("PCI[%x:%x:%x]: BRIDGE BAR 0x%x: 0x%08x\n",
			PCI_DEV_EXPAND(pcidev),
			0x20 + (res->bar-BRIDGE_RES_MEMIO)*4, tmp);
		PCI_CFG_W32(pcidev, 0x20+(res->bar-BRIDGE_RES_MEMIO)*4, tmp);
	} else {
		/* PCI Device */
		DBG("PCI[%x:%x:%x]: DEV BAR%d: 0x%08x\n",
			PCI_DEV_EXPAND(pcidev), res->bar, res->start);
		ofs = PCIR_BAR(0) + res->bar*4;
		PCI_CFG_W32(pcidev, ofs, res->start);
	}

	/* Enable Memory or I/O responses */
	if ((res->flags & PCI_RES_TYPE_MASK) == PCI_RES_IO)
		pci_io_enable(pcidev);
	else
		pci_mem_enable(pcidev);

	/* Enable Master if bridge */
	if (is_bridge)
		pci_master_enable(pcidev);
}

static int pci_set_res_dev(struct pci_dev *dev, void *unused)
{
	int i, maxbars;

	if (dev->flags & PCI_DEV_BRIDGE)
		maxbars = 2 + 3; /* 2 BARs + 3 Bridge-Windows "Virtual BARs" */
	else
		maxbars = 6; /* Normal PCI Device as max 6 BARs. */

	/* Set BAR resources with previous allocated values */
	for (i = 0; i < maxbars; i++)
		pci_set_bar(dev, i);
	pci_set_bar(dev, DEV_RES_ROM);

	return 0;
}

/* Route IRQ through PCI-PCI Bridges */
static int pci_route_irq(pci_dev_t dev, int irq_pin)
{
	int slot_grp;

	if (PCI_DEV_BUS(dev) == 0)
		return irq_pin;

	slot_grp = PCI_DEV_SLOT(dev) & 0x3;

	return (((irq_pin - 1) + slot_grp) & 0x3) + 1;
}

/* Put assigned system IRQ into PCI interrupt line information field.
 * This is to make it possible for drivers to read system IRQ / Vector from
 * configuration space later on.
 *
 * 1. Get Interrupt PIN
 * 2. Route PIN to host bridge
 * 3. Get System interrupt number assignment for PIN
 * 4. Set Interrupt LINE
 */
static int pci_set_irq_dev(struct pci_dev *dev, void *cfg)
{
	struct pci_auto_setup *autocfg = cfg;
	uint8_t irq_pin, irq_line, *psysirq;
	pci_dev_t pcidev;

	psysirq = &dev->sysirq;
	pcidev = dev->busdevfun;
	PCI_CFG_R8(pcidev, PCIR_INTPIN, &irq_pin);

	/* perform IRQ routing until we reach host bridge */
	while (dev->bus && irq_pin != 0) {
		irq_pin = autocfg->irq_route(dev->busdevfun, irq_pin);
		dev = &dev->bus->dev;
	}

	/* Get IRQ from PIN on PCI bus0 */
	if (irq_pin != 0 && autocfg->irq_map)
		irq_line = autocfg->irq_map(dev->busdevfun, irq_pin);
	else
		irq_line = 0;

	*psysirq = irq_line;

	/* Set System Interrupt/Vector for device. 0 means no-IRQ */
	PCI_CFG_W8(pcidev, PCIR_INTLINE, irq_line);

	return 0;
}

/* This routine assumes that PCI access library has been successfully
 * initialized. All information about the PCI bus needed is found in
 * the pci_auto_cfg structure passed on by pci_config_register().
 *
 * The PCI buses are enumerated as bridges are found, PCI devices are
 * setup with BARs and IRQs, etc.
 */
int pci_config_auto(void)
{
	uint32_t end;
	uint32_t startmemio, startmem, startio;
	struct pci_auto_setup *autocfg = &pci_auto_cfg;
#ifdef DEBUG
	uint32_t endmemio, endmem, endio;
	uint32_t start;
#endif

	if (pci_config_auto_initialized == 0)
		return -1; /* no config given to library */

#ifdef DEBUG
	DBG("\n--- PCI MEMORY AVAILABLE ---\n");
	if (autocfg->mem_size) {
		start = autocfg->mem_start;
		end = autocfg->mem_start + autocfg->mem_size - 1;
		DBG(" MEM AVAIL [0x%08x-0x%08x]\n", start, end);
	} else {
		/* One big memory space */
		DBG(" MEM share the space with MEMIO\n");
	}
	/* no-prefetchable memory space need separate memory space.
	 * For example PCI controller maps this region non-cachable.
	 */
	start = autocfg->memio_start;
	end = autocfg->memio_start + autocfg->memio_size - 1;
	DBG(" MEMIO AVAIL [0x%08x-0x%08x]\n", start, end);
	if (autocfg->io_size) {
		start = autocfg->io_start;
		end = autocfg->io_start + autocfg->io_size - 1;
		DBG(" I/O AVAIL [0x%08x-0x%08x]\n", start, end);
	} else {
		DBG(" I/O Space not available\n");
	}
#endif

	/* Init Host-Bridge */
	memset(&pci_hb, 0, sizeof(pci_hb));
	pci_hb.dev.flags = PCI_DEV_BRIDGE;
	if (autocfg->memio_size <= 0)
		return -1;
	pci_hb.flags = PCI_BUS_MEMIO;
	if (autocfg->mem_size)
		pci_hb.flags |= PCI_BUS_MEM;
	if (autocfg->io_size)
		pci_hb.flags |= PCI_BUS_IO;

	/* Find all PCI devices/functions on all buses. The buses will be
	 * enumrated (assigned a unique PCI Bus ID 0..255).
	 */
	DBG("\n--- PCI SCANNING ---\n");
	pci_find_devs(&pci_hb);
	pci_bus_cnt = pci_hb.sord + 1;
	if (pci_hb.devs == NULL)
		return 0;

	pci_system_type = PCI_SYSTEM_HOST;

	/* Find all resources (MEM/MEMIO/IO BARs) of all devices/functions
	 * on all buses.
	 *
	 * Device resources behind bridges which does not support prefetchable
	 * memory are already marked as non-prefetchable memory.
	 * Devices which as I/O resources behind a bridge that do not support
	 * I/O space are marked DISABLED.
	 *
	 * All BARs and Bridge Spaces are disabled after this. Only the ones
	 * that are allocated an address are initilized later on.
	 */
	DBG("\n\n--- PCI RESOURCES ---\n");
	pci_for_each_dev(pci_find_res_dev, 0);

	/* Add all device's resources to bus and sort them to fit in the PCI
	 * Window. The device resources are propagated upwards through bridges
	 * by adding a "virtual" BAR (boundary != BAR size).
	 *
	 * We wait with MEMIO (non-prefetchable memory) resources to after MEM
	 * resources have been allocated, so that MEM resources can be changed
	 * into MEMIO resources if not enough space.
	 */
	pci_add_res_bus(&pci_hb, PCI_RES_IO);
	pci_add_res_bus(&pci_hb, PCI_RES_MEM);

	/* Start assigning found resource according to the sorted order. */

	/* Allocate resources to I/O areas */
	if (pci_hb.busres[BUS_RES_IO]) {
		startio = autocfg->io_start;
		end = startio + autocfg->io_size;
#ifdef DEBUG
		endio =
#endif
			pci_alloc_res(&pci_hb, PCI_RES_IO, startio, end);
	}

	/* Allocate resources to prefetchable memory */
	if (pci_hb.busres[BUS_RES_MEM]) {
		startmem = autocfg->mem_start;
		end = startmem + autocfg->mem_size;
#ifdef DEBUG
		endmem =
#endif
			pci_alloc_res(&pci_hb, PCI_RES_MEM, startmem, end);
	}

	/* Add non-prefetchable memory resources and not fitting prefetchable
	 * memory resources.
	 *
	 * Some prefetchable memory resources may not have fitted into PCI
	 * window. Prefetchable memory can be mapped into non-prefetchable
	 * memory window. The failing BARs have been marked as MEMIO instead.
	 */
	pci_add_res_bus(&pci_hb, PCI_RES_MEMIO);

	/* Allocate resources to non-prefetchable memory */
	if (pci_hb.busres[BUS_RES_MEMIO]) {
		startmemio = autocfg->memio_start;
		end = startmemio + autocfg->memio_size;
#ifdef DEBUG
		endmemio =
#endif
			pci_alloc_res(&pci_hb, PCI_RES_MEMIO, startmemio, end);
	}

	DBG("\n--- PCI ALLOCATED SPACE RANGES ---\n");
	DBG(" MEM NON-PREFETCHABLE: [0x%08x-0x%08x]\n", startmemio, endmemio);
	DBG(" MEM PREFETCHABLE:	    [0x%08x-0x%08x]\n", startmem, endmem);
	DBG(" I/O:                  [0x%08x-0x%08x]\n", startio, endio);

	/* Set all allocated BARs and Bridge Windows */
	pci_for_each_dev(pci_set_res_dev, NULL);

	/* Initialize IRQs of all devices. According to the PCI-PCI bridge
	 * specification the IRQs are routed differently depending on slot
	 * number. Drivers can override the default routing if a motherboard
	 * requires it.
	 */
	if ((autocfg->options & CFGOPT_NOSETUP_IRQ) == 0) {
		if (autocfg->irq_route == NULL) /* use standard irq routing */
			autocfg->irq_route = pci_route_irq;
		pci_for_each_dev(pci_set_irq_dev, autocfg);
	}

	DBG("PCI resource allocation done\n");

	return 0;
}

void pci_config_auto_register(void *config)
{
	pci_config_auto_initialized = 1;
	memcpy(&pci_auto_cfg, config, sizeof(struct pci_auto_setup));
}
