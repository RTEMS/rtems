/* PCI (Static) Configuration Library. PCI Configuration C code console
 * printout routines that can be used to build a static PCI configuration.
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <pci/cfg.h>

int pci_cfg_print_bus(struct pci_bus *bus);

static void get_bus_name(struct pci_bus *bus, char *buf)
{
	if (bus->num == 0)
		strcpy(buf, "pci_hb");
	else
		sprintf(buf, "bus%d", bus->num);
}

static void get_device_name(struct pci_dev *dev, char *buf)
{
	char busname[64];

	if (dev->flags & PCI_DEV_BRIDGE) {
		get_bus_name((struct pci_bus *)dev, busname);
		sprintf(buf, "%s.dev", busname);
	} else {
		sprintf(buf, "dev_%x_%x_%x", PCI_DEV_EXPAND(dev->busdevfun));
	}
}

static void pci_cfg_print_resources(struct pci_res *resources, char *prefix)
{
	struct pci_res *res;
	int i;

	for (i = 0; i < DEV_RES_CNT; i++) {
		res = &resources[i];
		if (((res->flags & PCI_RES_TYPE_MASK) == 0) ||
		    ((res->flags & PCI_RES_FAIL) == PCI_RES_FAIL)) {
			printf("%sPCIRES_EMPTY,\n", prefix);
			continue;
		}
		printf("%s{\n", prefix);
		printf("%s\t.next = NULL,\n", prefix);
		printf("%s\t.size = 0x%08" PRIx32 ",\n", prefix, res->size);
		printf("%s\t.boundary = 0x%08" PRIx32 ",\n", prefix, res->boundary);
		printf("%s\t.flags = 0x%x,\n", prefix, res->flags);
		printf("%s\t.bar = %d,\n", prefix, i);
		printf("%s\t.start = 0x%08" PRIx32 ",\n", prefix, res->start);
		printf("%s\t.end = 0x%08" PRIx32 ",\n", prefix, res->end);
		printf("%s},\n", prefix);
	}
}

static void pci_cfg_print_device(struct pci_dev *dev, char *prefix)
{
	char name[32];
	char buf[8];
	printf("%s.resources = {\n", prefix);
	strcpy(buf, prefix);
	strcat(buf, "\t");
	pci_cfg_print_resources(dev->resources, buf);
	printf("%s},\n", prefix);
	if (dev->next == NULL) {
		printf("%s.next = NULL,\n", prefix);
	} else {
		get_device_name(dev->next, name);
		printf("%s.next = &%s,\n", prefix, name);
	}
	if (!dev->bus) { /* Host Bridge? */
		printf("%s.bus = NULL,\n", prefix);
	} else {
		get_bus_name(dev->bus, name);
		printf("%s.bus = &%s,\n", prefix, name);
	}

	printf("%s.busdevfun = 0x%04x,\n", prefix, dev->busdevfun);
	printf("%s.flags = 0x%x,\n", prefix, dev->flags);
	printf("%s.sysirq = %d,\n", prefix, dev->sysirq);
	printf("%s.vendor = 0x%04x,\n", prefix, dev->vendor);
	printf("%s.device = 0x%04x,\n", prefix, dev->device);
	printf("%s.subvendor = 0x%04x,\n", prefix, dev->subvendor);
	printf("%s.subdevice = 0x%04x,\n", prefix, dev->subdevice);
	printf("%s.classrev = 0x%08" PRIx32 ",\n", prefix, dev->classrev);
	printf("%s.command = 0,\n", prefix);
}

static int pci_cfg_print_dev(struct pci_dev *dev, void *unused)
{
	if (dev->flags & PCI_DEV_BRIDGE) {
		pci_cfg_print_bus((struct pci_bus *)dev);
	} else {
		printf("\n\n/* PCI DEV at [%x:%x:%x] */\n",
			PCI_DEV_EXPAND(dev->busdevfun));
		printf("static struct pci_dev dev_%x_%x_%x = {\n",
			PCI_DEV_EXPAND(dev->busdevfun));
		pci_cfg_print_device(dev, "\t");
		printf("};\n");
	}
	return 0;
}

int pci_cfg_print_bus(struct pci_bus *bus)
{
	char name[32];

	/* Print Bus */
	printf("\n\n/* PCI BUS %d - Bridge at [%x:%x:%x] */\n\n",
		bus->num, PCI_DEV_EXPAND(bus->dev.busdevfun));
	get_bus_name(bus, name);
	printf("%sstruct pci_bus %s = {\n",
		bus->num == 0 ? "" : "static ", name);
	printf("\t.dev = {\n");
	pci_cfg_print_device(&bus->dev, "\t\t");
	printf("\t},\n");
	if (bus->devs == NULL) {
		printf("\t.devs = NULL,\n");
	} else {
		get_device_name(bus->devs, name);
		printf("\t.devs = &%s,\n", name);
	}
	printf("\t.flags = 0x%x,\n", bus->flags);
	printf("\t.num = %d,\n", bus->num);
	printf("\t.pri = %d,\n", bus->pri);
	printf("\t.sord = %d,\n", bus->sord);
	printf("};\n");

	/* Print all child devices */
	pci_for_each_child(bus, pci_cfg_print_dev, NULL, 0);

	return 0;
}

static int pci_cfg_print_forw_dev(struct pci_dev *dev, void *unused)
{
	if ((dev->flags & PCI_DEV_BRIDGE) == 0) {
		printf("static struct pci_dev dev_%x_%x_%x;\n",
			PCI_DEV_EXPAND(dev->busdevfun));
	}
	return 0;
}

void pci_cfg_print(void)
{
	int i;

	printf("\n\n/*** PCI Configuration ***/\n\n");
	printf("#include <stdlib.h>\n");
	printf("#define PCI_CFG_STATIC_LIB\n");
	printf("#include <pci/cfg.h>\n\n");
	printf("#define PCIRES_EMPTY {0}\n\n");

	/* Forward declaration for all devices / buses */
	printf("/* FORWARD BUS DECLARATIONS */\n");
	for (i = 0; i < pci_bus_count(); i++) {
		if (i == 0)
			printf("struct pci_bus pci_hb;\n");
		else
			printf("static struct pci_bus bus%d;\n", i);
	}
	printf("\n/* FORWARD DEVICE DECLARATIONS */\n");
	pci_for_each_dev(pci_cfg_print_forw_dev, NULL);

	pci_cfg_print_bus(&pci_hb);
}
