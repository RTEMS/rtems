/* Driver Manager Driver Translate Interface Implementation
 *
 * COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 * Used by device drivers. The functions rely on that the parent bus driver
 * has implemented the neccessary operations correctly.
 *
 * The translate functions are used to translate addresses between buses
 * for DMA cores located on a "remote" bus, or for memory-mapped obtaining
 * an address that can be used to access an remote bus.
 *
 * For example, PCI I/O might be memory-mapped at the PCI Host bridge,
 * say address 0xfff10000-0xfff1ffff is mapped to the PCI I/O address
 * of 0x00000000-0x0000ffff. The PCI Host bridge driver may then set up
 * a map so that a driver that get PCI address 0x100 can translate that
 * into 0xfff10100.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drvmgr/drvmgr.h>
#include "drvmgr_internal.h"

unsigned int drvmgr_translate_bus(
	struct drvmgr_bus *from,
	struct drvmgr_bus *to,
	int reverse,
	void *src_address,
	void **dst_address)
{
	struct drvmgr_bus *path[16];
	int dir, levels, i;
	void *dst, *from_adr, *to_adr;
	struct drvmgr_map_entry *map;
	struct drvmgr_bus *bus;
	unsigned int sz;
	struct drvmgr_bus *bus_bot, *bus_top;

	dst = src_address;
	sz = 0xffffffff;

	if (from == to) /* no need translating addresses when on same bus */
		goto out;

	/* Always find translation path from remote bus towards root bus. All
	 * buses have root bus has parent at some level
	 */
	if (from->depth > to->depth) {
		bus_bot = from;
		bus_top = to;
		dir = 0;
	} else {
		bus_bot = to;
		bus_top = from;
		dir = 1;
	}
	levels = bus_bot->depth - bus_top->depth;
	if (levels >= 16)
		return 0; /* Does not support such a big depth */
	i = 0;
	while ((bus_bot != NULL) && bus_bot != bus_top) {
		if (dir)
			path[(levels - 1) - i] = bus_bot;
		else
			path[i] = bus_bot;
		i++;
		bus_bot = bus_bot->dev->parent;
	}
	if (bus_bot == NULL)
		return 0; /* from -> to is not linearly connected */

	for (i = 0; i < levels; i++) {
		bus = path[i];

		if ((dir && reverse) || (!dir && !reverse))
			map = bus->maps_up;
		else
			map = bus->maps_down;

		if (map == NULL)
			continue; /* No translation needed - 1:1 mapping */

		if (map == DRVMGR_TRANSLATE_NO_BRIDGE) {
			sz = 0;
			break; /* No bridge interface in this direction */
		}

		while (map->size != 0) {
			if (reverse) {
				/* Opposite direction */
				from_adr = map->to_adr;
				to_adr = map->from_adr;
			} else {
				from_adr = map->from_adr;
				to_adr = map->to_adr;
			}

			if ((dst >= from_adr) &&
			    (dst <= (from_adr + (map->size - 1)))) {
				if (((from_adr + (map->size - 1)) - dst) < sz)
					sz = (from_adr + (map->size - 1)) - dst;
				dst = (dst - from_adr) + to_adr;
				break;
			}
			map++;
		}
		/* quit if no matching translation information */
		if (map->size == 0) {
			sz = 0;
			break;
		}
	}

out:
	if (dst_address)
		*dst_address = dst;

	return sz;
}

unsigned int drvmgr_translate(
	struct drvmgr_dev *dev,
	unsigned int options,
	void *src_address,
	void **dst_address)
{
	struct drvmgr_bus *to, *from;
	int rev = 0;

	rev = (~options) & 1;
	if ((options == CPUMEM_TO_DMA) || (options == DMAMEM_FROM_CPU)) {
		from = drvmgr.root_dev.bus;
		to = dev->parent;
	} else { /* CPUMEM_FROM_DMA || DMAMEM_TO_CPU */
		from = dev->parent;
		to = drvmgr.root_dev.bus;
	}

	return drvmgr_translate_bus(from, to, rev, src_address, dst_address);
}
