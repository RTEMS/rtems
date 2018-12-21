/* Driver Manager Information printing Interface Implementation
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 *  These functions print stuff about the driver manager, what devices were
 *  found and were united with a driver, the Bus topology, memory taken, etc.
 *
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drvmgr/drvmgr.h>
#include "drvmgr_internal.h"

typedef void (*fun_ptr)(void);

static int print_dev_found(struct drvmgr_dev *dev, void *arg)
{
	char **pparg = arg;

	if (pparg && *pparg) {
		printf(*pparg);
		*pparg = NULL;
	}

	printf(" DEV %p  %s on bus %p\n", dev,
		dev->name ? dev->name : "NO_NAME", dev->parent);

	return 0; /* Continue to next device */
}

void drvmgr_print_devs(unsigned int options)
{
	struct drvmgr *mgr = &drvmgr;
	char *parg;

	/* Print Drivers */
	if (options & PRINT_DEVS_ASSIGNED) {
		parg = " --- DEVICES ASSIGNED TO DRIVER ---\n";
		drvmgr_for_each_listdev(&mgr->devices[DRVMGR_LEVEL_MAX],
				DEV_STATE_UNITED, 0, print_dev_found, &parg);
		if (parg != NULL)
			printf("\n NO DEVICES WERE ASSIGNED A DRIVER\n");
	}

	if (options & PRINT_DEVS_UNASSIGNED) {
		parg = "\n --- DEVICES WITHOUT DRIVER ---\n";
		drvmgr_for_each_listdev(&mgr->devices_inactive, 0,
			DEV_STATE_UNITED, print_dev_found, &parg);
		if (parg != NULL)
			printf("\n NO DEVICES WERE WITHOUT DRIVER\n");
	}

	if (options & PRINT_DEVS_FAILED) {
		parg = "\n --- DEVICES FAILED TO INITIALIZE ---\n";
		drvmgr_for_each_listdev(&mgr->devices_inactive,
			DEV_STATE_INIT_FAILED, 0, print_dev_found, &parg);
		if (parg != NULL)
			printf("\n NO DEVICES FAILED TO INITIALIZE\n");
	}

	if (options & PRINT_DEVS_IGNORED) {
		parg = "\n --- DEVICES IGNORED ---\n";
		drvmgr_for_each_listdev(&mgr->devices_inactive,
			DEV_STATE_IGNORED, 0, print_dev_found, &parg);
		if (parg != NULL)
			printf("\n NO DEVICES WERE IGNORED\n");
	}

	printf("\n\n");
}

static intptr_t drvmgr_topo_func(struct drvmgr_dev *dev, void *arg)
{
	char prefix[32];
	int depth = dev->parent->depth;

	if (depth > 30)
		return 0; /* depth more than 30 not supported */
	memset(prefix, ' ', depth + 1);
	prefix[depth + 1] = '\0';

	printf(" %s|-> DEV  %p  %s\n", prefix, dev,
		dev->name ? dev->name :  "NO_NAME");
	return 0;
}

void drvmgr_print_topo(void)
{
	/* Print Bus topology */
	printf(" --- BUS TOPOLOGY ---\n");
	drvmgr_for_each_dev(drvmgr_topo_func, NULL, DRVMGR_FED_DF);
	printf("\n\n");
}

/* Print the memory usage */
void drvmgr_print_mem(void)
{
	struct drvmgr *mgr = &drvmgr;
	struct drvmgr_bus *bus;
	struct drvmgr_dev *dev;
	struct drvmgr_drv *drv;

	struct drvmgr_bus_res *node;
	struct drvmgr_drv_res *res;
	struct drvmgr_key *key;

	unsigned int busmem = 0;
	unsigned int devmem = 0;
	unsigned int drvmem = 0;
	unsigned int resmem = 0;
	unsigned int devprivmem = 0;

	DRVMGR_LOCK_READ();

	bus = BUS_LIST_HEAD(&mgr->buses[DRVMGR_LEVEL_MAX]);
	while (bus) {
		busmem += sizeof(struct drvmgr_bus);

		/* Get size of resources on this bus */
		node = bus->reslist;
		while (node) {
			resmem += sizeof(struct drvmgr_bus_res);

			res = node->resource;
			while (res->keys) {
				resmem += sizeof(struct drvmgr_drv_res);

				key = res->keys;
				while (key->key_type != DRVMGR_KT_NONE) {
					resmem += sizeof
						(struct drvmgr_key);
					key++;
				}
				resmem += sizeof(struct drvmgr_key);
				res++;
			}

			node = node->next;
		}

		bus = bus->next;
	}

	drv = DRV_LIST_HEAD(&mgr->drivers);
	while (drv) {
		drvmem += sizeof(struct drvmgr_drv);
		drv = drv->next;
	}

	dev = DEV_LIST_HEAD(&mgr->devices[DRVMGR_LEVEL_MAX]);
	while (dev) {
		devmem += sizeof(struct drvmgr_dev);
		if (dev->drv && dev->drv->dev_priv_size > 0)
			devprivmem += dev->drv->dev_priv_size;
		dev = dev->next;
	}

	DRVMGR_UNLOCK();

	printf(" --- MEMORY USAGE ---\n");
	printf(" BUS:          %d bytes\n", busmem);
	printf(" DRV:          %d bytes\n", drvmem);
	printf(" DEV:          %d bytes\n", devmem);
	printf(" DEV private:  %d bytes\n", devprivmem);
	printf(" RES:          %d bytes\n", resmem);
	printf(" TOTAL:        %d bytes\n",
			busmem + drvmem + devmem + devprivmem + resmem);
	printf("\n\n");
}

/* Print the memory usage */
void drvmgr_summary(void)
{
	struct drvmgr *mgr = &drvmgr;
	struct drvmgr_bus *bus;
	struct drvmgr_dev *dev;
	struct drvmgr_drv *drv;
	int i, buscnt = 0, devcnt = 0, drvcnt = 0;

	printf(" --- SUMMARY ---\n");

	drv = DRV_LIST_HEAD(&mgr->drivers);
	while (drv) {
		drvcnt++;
		drv = drv->next;
	}
	printf(" NUMBER OF DRIVERS:               %d\n", drvcnt);

	DRVMGR_LOCK_READ();

	for (i = 0; i <= DRVMGR_LEVEL_MAX; i++) {
		buscnt = 0;
		bus = BUS_LIST_HEAD(&mgr->buses[i]);
		while (bus) {
			buscnt++;
			bus = bus->next;
		}
		if (buscnt > 0) {
			printf(" NUMBER OF BUSES IN LEVEL[%d]:     %d\n",
				i, buscnt);
		}
	}

	for (i = 0; i <= DRVMGR_LEVEL_MAX; i++) {
		devcnt = 0;
		dev = DEV_LIST_HEAD(&mgr->devices[i]);
		while (dev) {
			devcnt++;
			dev = dev->next;
		}
		if (devcnt > 0) {
			printf(" NUMBER OF DEVS IN LEVEL[%d]:      %d\n",
				i, devcnt);
		}
	}

	DRVMGR_UNLOCK();

	printf("\n\n");
}

static void print_info(void *p, char *str)
{
	printf("  ");
	puts(str);
}

void drvmgr_info_dev(struct drvmgr_dev *dev, unsigned int options)
{
	if (!dev)
		return;

	printf(" -- DEVICE %p --\n", dev);
	if (options & OPTION_DEV_GENINFO) {
		printf("  PARENT BUS:  %p\n", dev->parent);
		printf("  NAME:        %s\n", dev->name ? dev->name : "NO_NAME");
		printf("  STATE:       0x%08x\n", dev->state);
		if (dev->bus)
			printf("  BRIDGE TO:   %p\n", dev->bus);
		printf("  INIT LEVEL:  %d\n", dev->level);
		printf("  ERROR:       %d\n", dev->error);
		printf("  MINOR BUS:   %d\n", dev->minor_bus);
		if (dev->drv) {
			printf("  MINOR DRV:   %d\n", dev->minor_drv);
			printf("  DRIVER:      %p (%s)\n", dev->drv,
				dev->drv->name ? dev->drv->name : "NO_NAME");
			printf("  PRIVATE:     %p\n", dev->priv);
		}
	}

	if (options & OPTION_DEV_BUSINFO) {
		printf("  --- DEVICE INFO FROM BUS DRIVER ---\n");
		if (!dev->parent)
			printf("  !! device has no parent bus !!\n");
		else if (dev->parent->ops->get_info_dev)
			dev->parent->ops->get_info_dev(dev, print_info, NULL);
		else
			printf("  Bus doesn't implement get_info_dev func\n");
	}

	if (options & OPTION_DEV_DRVINFO) {
		if (dev->drv) {
			printf("  --- DEVICE INFO FROM DEVICE DRIVER ---\n");
			if (dev->drv->ops->info)
				dev->drv->ops->info(dev, print_info, NULL, 0, 0);
			else
				printf("  Driver doesn't implement info func\n");
		}
	}
}

static void drvmgr_info_bus_map(struct drvmgr_map_entry *map)
{
	if (map == NULL)
		printf("    Addresses mapped 1:1\n");
	else if (map == DRVMGR_TRANSLATE_NO_BRIDGE)
		printf("    No bridge in this direction\n");
	else {
		while (map->size != 0) {
			printf("    0x%08lx-0x%08lx => 0x%08lx-0x%08lx  %s\n",
				(unsigned long)map->from_adr,
				(unsigned long)(map->from_adr + map->size - 1),
				(unsigned long)map->to_adr,
				(unsigned long)(map->to_adr + map->size - 1),
				map->name ? map->name : "no label");
			map++;
		}
	}
}

void drvmgr_info_bus(struct drvmgr_bus *bus, unsigned int options)
{
	struct drvmgr_dev *dev;

	/* Print Driver */
	printf("-- BUS %p --\n", bus);
	printf("  BUS TYPE:    %d\n", bus->bus_type);
	printf("  DEVICE:      %p (%s)\n", bus->dev,
		bus->dev->name ? bus->dev->name : "NO_NAME");
	printf("  OPS:         %p\n", bus->ops);
	printf("  CHILDREN:    %d devices\n", bus->dev_cnt);
	printf("  LEVEL:       %d\n", bus->level);
	printf("  STATE:       0x%08x\n", bus->state);
	printf("  ERROR:       %d\n", bus->error);

	/* Print address mappings up- (to parent) and down- (from parent to
	 * this bus) stream the bridge of this bus
	 */
	printf("  DOWN STREAMS BRIDGE MAPPINGS  (from parent to this bus)\n");
	drvmgr_info_bus_map(bus->maps_down);
	printf("  UP STREAMS BRIDGE MAPPINGS    (from this bus to parent)\n");
	drvmgr_info_bus_map(bus->maps_up);

	/* Print Devices on this bus? */
	if (options & OPTION_BUS_DEVS) {
		printf("  CHILDREN:\n");
		DRVMGR_LOCK_READ();
		dev = bus->children;
		while (dev) {
			printf("   |- DEV[%02d]: %p  %s\n", dev->minor_bus,
				dev, dev->name ? dev->name : "NO_NAME");
			dev = dev->next_in_bus;
		}
		DRVMGR_UNLOCK();
	}
}

void drvmgr_info_drv(struct drvmgr_drv *drv, unsigned int options)
{
	struct drvmgr_dev *dev;
	int i;

	/* Print Driver */
	printf(" -- DRIVER %p --\n", drv);
	printf("  DRIVER ID:   0x%" PRIx64 "\n", drv->drv_id);
	printf("  NAME:        %s\n", drv->name ? drv->name : "NO_NAME");
	printf("  BUS TYPE:    %d\n", drv->bus_type);
	printf("  OPERATIONS:\n");
	for (i = 0; i < DRVMGR_LEVEL_MAX; i++)
		printf("   init[%d]:    %p\n", i + 1, drv->ops->init[i]);
	printf("   remove:     %p\n", drv->ops->remove);
	printf("   info:       %p\n", drv->ops->info);
	printf("  NO. DEVICES: %d\n", drv->dev_cnt);

	/* Print devices united with this driver? */
	if (options & OPTION_DRV_DEVS) {
		DRVMGR_LOCK_READ();
		dev = drv->dev;
		while (dev) {
			printf("  DEV[%02d]:     %p  %s\n", dev->minor_drv,
				dev, dev->name ? dev->name : "NO_NAME");
			dev = dev->next_in_drv;
		}
		DRVMGR_UNLOCK();
	}
}

void (*info_obj[3])(void *obj, unsigned int) = {
	/* DRVMGR_OBJ_DRV */ (void (*)(void *, unsigned int))drvmgr_info_drv,
	/* DRVMGR_OBJ_BUS */ (void (*)(void *, unsigned int))drvmgr_info_bus,
	/* DRVMGR_OBJ_DEV */ (void (*)(void *, unsigned int))drvmgr_info_dev,
};

/* Get information about a device/bus/driver */
void drvmgr_info(void *id, unsigned int options)
{
	int obj_type;
	void (*func)(void *, unsigned int);

	if (!id)
		return;
	obj_type = *(int *)id;
	if ((obj_type < DRVMGR_OBJ_DRV) || (obj_type > DRVMGR_OBJ_DEV))
		return;
	func = info_obj[obj_type - 1];
	func(id, options);
}

void drvmgr_info_devs_on_bus(struct drvmgr_bus *bus, unsigned int options)
{
	struct drvmgr_dev *dev;

	/* Print All Devices on Bus */
	printf("\n\n  -= All Devices on BUS %p =-\n\n", bus);
	dev = bus->children;
	while (dev) {
		drvmgr_info_dev(dev, options);
		puts("");
		dev = dev->next_in_bus;
	}

	if ((options & OPTION_RECURSIVE) == 0)
		return;

	/* This device provides a bus, print the bus */
	dev = bus->children;
	while (dev) {
		if (dev->bus)
			drvmgr_info_devs_on_bus(dev->bus, options);
		dev = dev->next_in_bus;
	}
}

void drvmgr_info_devs(unsigned int options)
{
	struct drvmgr *mgr = &drvmgr;
	struct drvmgr_dev *dev;

	/* Print device information of all devices and their child devices */
	dev = &mgr->root_dev;
	drvmgr_info_devs_on_bus(dev->bus, options);
	printf("\n\n");
}

void drvmgr_info_drvs(unsigned int options)
{
	struct drvmgr *mgr = &drvmgr;
	struct drvmgr_drv *drv;

	drv = DRV_LIST_HEAD(&mgr->drivers);
	while (drv) {
		drvmgr_info_drv(drv, options);
		puts("\n");
		drv = drv->next;
	}
}

void drvmgr_info_buses(unsigned int options)
{
	struct drvmgr *mgr = &drvmgr;
	struct drvmgr_bus *bus;

	bus = BUS_LIST_HEAD(&mgr->buses[DRVMGR_LEVEL_MAX]);
	while (bus) {
		drvmgr_info_bus(bus, options);
		puts("\n");
		bus = bus->next;
	}
}
