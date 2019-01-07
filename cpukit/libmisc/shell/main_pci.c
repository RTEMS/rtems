/*  LIBPCI Command Implementation
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pci.h>
#include <pci/cfg.h>
#include <pci/access.h>
#include <sys/endian.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static void usage(void);

struct shell_pci_modifier {
  char *name;
  int (*func)(int argc, char *argv[], struct shell_pci_modifier *mod);
  int data;
};

static unsigned long get_pciid_from_string(char *arg)
{
  unsigned long pciid;
  char *bus_str, *dev_str, *fun_str;
  unsigned long busno, devno, funno;

  dev_str = strstr(arg, ":");
  if (dev_str == NULL) {
    /* PCIID */
    pciid = strtoul(arg, NULL, 16);
    if (pciid == ULONG_MAX)
      return ~0;
  } else {
    /* bus:dev:fun */
    bus_str = arg;
    *dev_str = '\0';
    dev_str++;
    fun_str = strstr(dev_str, ":");
    if (fun_str == NULL)
      return ~0;
    *fun_str = '\0';
    fun_str++;

    busno = strtoul(bus_str, NULL, 16);
    if (busno == ULONG_MAX)
      return ~0;
    devno = strtoul(dev_str, NULL, 16);
    if (devno == ULONG_MAX)
      return ~0;
    funno = strtoul(fun_str, NULL, 16);
    if (funno == ULONG_MAX)
      return ~0;
    pciid = PCI_DEV(busno, devno, funno);
  }

  return pciid;
}

/* Print current PCI configuration that can be used in a static/peripheral PCI
 * configuration setup.
 */
static int shell_pci_pcfg(
	int argc,
	char *argv[],
	struct shell_pci_modifier *mod)
{
  if (argc != 2)
    return -1;

  pci_cfg_print();

  return 0;
}

static int shell_pci_ls(
	int argc,
	char *argv[],
	struct shell_pci_modifier *mod)
{
  unsigned long pciid;

  if (argc == 2) {
    /* List all devices */
    pci_print();
  } else if (argc > 3) {
    return -1;
  } else {
    pciid = get_pciid_from_string(argv[2]);
    if ((pciid & 0xffff0000) != 0)
      return -1;

    pci_print_dev((pci_dev_t)pciid);
  }
  return 0;
}

static int shell_pci_rX(
	unsigned long pciid,
	int offset,
	int size)
{
  uint8_t data8;
  uint16_t data16;
  uint32_t data32;
  int result;

  switch(size) {
    case 1:
      result = pci_cfg_r8(pciid, offset, &data8);
      if (result == PCISTS_OK)
        printf(" r08[0x%02x]: 0x%02x  DEC=%d\n", offset, data8, data8);
      break;

    case 2:
      result = pci_cfg_r16(pciid, offset, &data16);
      if (result == PCISTS_OK)
        printf(" r16[0x%02x]: 0x%04x  DEC=%d\n", offset, data16, data16);
      break;

    case 4:
      result = pci_cfg_r32(pciid, offset, &data32);
      if (result == PCISTS_OK)
        printf(" r32[0x%02x]: 0x%08" PRIx32 "  DEC=%" PRIu32 "\n",
               offset, data32, data32);
      break;

    default:
      return PCISTS_EINVAL;
  }
  return result;
}

static int shell_pci_wX(
	unsigned long pciid,
	int offset,
	uint32_t data,
	int size)
{
  uint8_t data8;
  uint16_t data16;
  int result;

  switch(size) {
    case 1:
      if (data > 0xff)
        return PCISTS_EINVAL;
      data8 = data & 0xff;
      result = pci_cfg_w8(pciid, offset, data8);
      if (result == PCISTS_OK)
        printf(" w08[0x%02x]: 0x%02x  DEC=%d\n", offset, data8, data8);
      break;

    case 2:
      if (data > 0xffff)
        return PCISTS_EINVAL;
      data16 = data & 0xffff;
      result = pci_cfg_w16(pciid, offset, data16);
      if (result == PCISTS_OK)
        printf(" w16[0x%02x]: 0x%04x  DEC=%d\n", offset, data16, data16);
      break;

    case 4:
      result = pci_cfg_w32(pciid, offset, data);
      if (result == PCISTS_OK)
        printf(" w32[0x%02x]: 0x%08" PRIx32 "  DEC=%" PRIu32 "\n",
               offset, data, data);
      break;

    default:
      return PCISTS_EINVAL;
  }
  return result;
}

static int shell_pci_read(
	int argc,
	char *argv[],
	struct shell_pci_modifier *mod)
{
  unsigned long pciid, offset;
  int result, size;

  if (argc != 4)
    return -1;

  pciid = get_pciid_from_string(argv[2]);
  if ((pciid & 0xffff0000) != 0)
    return -1;

  offset = strtoul(argv[3], NULL, 0);
  if (offset > 256)
    return -1;

  size = mod->data;
  result = shell_pci_rX(pciid, offset, size);
  switch (result) {
    default:
    case PCISTS_OK:
      break;

    case PCISTS_ERR:
    case PCISTS_EINVAL:
      puts(" Bad input argument\n");
      return PCISTS_EINVAL;

    case PCISTS_MSTABRT:
      puts(" Master abort while reading configuration space");
      return PCISTS_MSTABRT;
  }

  return 0;
}

static int shell_pci_write(
	int argc,
	char *argv[],
	struct shell_pci_modifier *mod)
{
  unsigned long pciid, offset;
  int result, size;
  uint32_t data;

  if (argc != 5)
    return -1;

  pciid = get_pciid_from_string(argv[2]);
  if ((pciid & 0xffff0000) != 0)
    return -1;

  offset = strtoul(argv[3], NULL, 0);
  if (offset > 256)
    return -1;

  data = strtoul(argv[4], NULL, 0);
  if (data == ULONG_MAX && errno == ERANGE)
    return -1;

  size = mod->data;
  result = shell_pci_wX(pciid, offset, data, size);
  switch (result) {
    default:
    case PCISTS_OK:
      break;

    case PCISTS_ERR:
    case PCISTS_EINVAL:
      puts(" Bad input argument\n");
      return PCISTS_EINVAL;

    case PCISTS_MSTABRT:
      puts(" Master abort while reading configuration space");
      return PCISTS_MSTABRT;
  }
  return 0;
}

static int shell_pci_pciid(
	int argc,
	char *argv[],
	struct shell_pci_modifier *mod)
{
  unsigned long pciid;

  if (argc != 3)
    return -1;

  pciid = get_pciid_from_string(argv[2]);
  if ((pciid & 0xffff0000) != 0)
    return -1;

  printf(" PCIID: 0x%lx [%lx:%lx:%lx]\n", pciid, PCI_DEV_EXPAND(pciid));
  return 0;
}

static int shell_pci_getdev(
	int argc,
	char *argv[],
	struct shell_pci_modifier *mod)
{
  unsigned long pciid;
  struct pci_dev *dev;

  if (argc != 3)
    return -1;

  pciid = get_pciid_from_string(argv[2]);
  if ((pciid & 0xffff0000) != 0)
    return -1;

  if (pci_get_dev(pciid, &dev)) {
    printf(" GETDEV: no device on [%lx:%lx:%lx]\n", PCI_DEV_EXPAND(pciid));
    return 0;
  }

  printf(" PCI RAM DEVICE: %p\n", dev);
  return 0;
}

static int shell_pci_infodev(
	int argc,
	char *argv[],
	struct shell_pci_modifier *mod)
{
  unsigned long arg;
  struct pci_dev *dev;
  struct pci_bus *bus;
  struct pci_res *res;
  char *type_str, *str1, *res_types[3] = {" IO16", "MEMIO", "MEM"};
  int i, res_avail;

  if (argc != 3)
    return -1;

  arg = strtoul(argv[2], NULL, 0);
  if (arg == ULONG_MAX && errno == ERANGE)
    return -1;

  dev = (struct pci_dev *)arg;
  if (!dev) {
    printf(" INFODEV: invalid device\n");
    return 0;
  }

  if (dev->flags & PCI_DEV_BRIDGE) {
    type_str = "PCI-to-PCI BRIDGE";
    if (!dev->bus)
      type_str = "PCI HOST BRIDGE";
  } else
    type_str = "PCI DEVICE";
  printf(" %s at [%x:%x:%x]\n", type_str, PCI_DEV_EXPAND(dev->busdevfun));

  bus = (struct pci_bus *)dev;
  if (bus) {
    printf(" PRIMARY:       BUS 0x%x\n", bus->pri);
    printf(" SECONDARY:     BUS 0x%x\n", bus->num);
    printf(" SUB ORDINATE:  BUS 0x%x\n", bus->sord);
  }

  printf(" PCIID:         0x%04x\n", dev->busdevfun);
  bus = dev->bus;
  if (!bus) {
    printf(" AT BUS:        0x%x via Host Bridge\n", bus->num);
  } else {
    printf(" AT BUS:        0x%x via Bridge at [%x:%x:%x]\n", bus->num, 
           PCI_DEV_EXPAND(bus->dev.busdevfun));
  }
  printf(" VENDOR:        0x%04x\n", dev->vendor);
  printf(" DEVICE:        0x%04x\n", dev->device);
  printf(" SUB VENDOR:    0x%04x\n", dev->subvendor);
  printf(" SUB DEVICE:    0x%04x\n", dev->subdevice);
  printf(" CLASS:         0x%06" PRIx32 "\n", dev->classrev >> 8);
  printf(" REVISION:      0x%02" PRIx32 "\n", dev->classrev & 0xff);
  printf(" IRQ:           %d\n", dev->sysirq);

  res_avail = 0;
  for (i = 0; i < DEV_RES_CNT; i++) {
    res = &dev->resources[i];

    if ((res->flags & PCI_RES_TYPE_MASK) == 0)
      continue;

    str1 = res_types[(res->flags & PCI_RES_TYPE_MASK) - 1];
    if (res->flags & PCI_RES_IO32)
      str1 = " IO32";

    if (res_avail == 0) {
      puts(" RESOURCES:");
      res_avail = 1;
    }

    if (res->flags & PCI_RES_FAIL) {
      printf("  %s[%d]:  NOT ASSIGNED", str1, i);
      continue;
    }

    printf("  %s[%d]:  %08" PRIx32 "-%08" PRIx32 "\n",
           str1, i, res->start, res->end - 1);
  }

  if (res_avail == 0)
    puts(" NO CONFIGURED RESOURCES AVAILABLE");

  return 0;
}

static int pci_summary(void)
{
	char *str;
	char *cfglib_strs[5] = {"NONE", "AUTO", "STATIC", "READ", "PERIPHERAL"};

	if (pci_system_type == PCI_SYSTEM_HOST)
		str = "HOST";
	else if (pci_system_type == PCI_SYSTEM_PERIPHERAL)
		str = "PERIPHERAL";
	else
		str = "UNKNOWN / UNINITIALIZED";
	printf(" SYSTEM:            %s\n", str);

	if (pci_config_lib_type > PCI_CONFIG_LIB_PERIPHERAL) {
		puts(" Bad configuration library");
		return 1;
	}
	printf(" CFG LIBRARY:       %s\n", cfglib_strs[pci_config_lib_type]);
	printf(" NO. PCI BUSES:     %d buses\n", pci_bus_count());
	printf(" PCI ENDIAN:        %s\n", pci_endian ? "Big" : "Little");
#if BYTE_ORDER == LITTLE_ENDIAN
	puts(" MACHINE ENDIAN:    Little");
#else
	puts(" MACHINE ENDIAN:    Big");
#endif

	return 0;
}

static const char pci_usage_str[] =
 " usage:\n"
 "  pci ls [bus:dev:fun|PCIID]         List one or all devices\n"
 "  pci r{8|16|32} bus:dev:fun OFS     Configuration space read\n"
 "  pci r{8|16|32} PCIID OFS           Configuration space read\n"
 "                                     access by PCIID\n"
 "  pci w{8|16|32} bus:dev:fun OFS D   Configuration space write\n"
 "  pci w{8|16|32} PCIID OFS D         Configuration space write\n"
 "                                     access by PCIID\n"
 "  pci pciid bus:dev:fun              Print PCIID for bus:dev:fun\n"
 "  pci pciid PCIID                    Print bus:dev:fun for PCIID\n"
 "  pci pcfg                           Print current PCI config for\n"
 "                                     static configuration library\n"
 "  pci getdev {PCIID|bus:dev:fun}     Get PCI Device from RAM tree\n"
 "  pci infodev DEV_ADR                Info about a PCI RAM Device\n"
 "  pci --help\n";

static void usage(void)
{
  puts(pci_usage_str);
}

static int shell_pci_usage(
	int argc,
	char *argv[],
	struct shell_pci_modifier *mod)
{
  usage();
  return 0;
}

#define MODIFIER_NUM 12
static struct shell_pci_modifier shell_pci_modifiers[MODIFIER_NUM] =
{
  {"ls", shell_pci_ls, 0},
  {"r8", shell_pci_read, 1},
  {"r16", shell_pci_read, 2},
  {"r32", shell_pci_read, 4},
  {"w8", shell_pci_write, 1},
  {"w16", shell_pci_write, 2},
  {"w32", shell_pci_write, 4},
  {"pciid", shell_pci_pciid, 0},
  {"pcfg", shell_pci_pcfg, 0},
  {"getdev", shell_pci_getdev, 0},
  {"infodev", shell_pci_infodev, 0},
  {"--help", shell_pci_usage},
};

static struct shell_pci_modifier *shell_pci_find_modifier(char *name)
{
  struct shell_pci_modifier *mod;
  int i;

  if (name == NULL)
    return NULL;

  for (i=0, mod=&shell_pci_modifiers[0]; i<MODIFIER_NUM; i++, mod++) {
    if (strcmp(name, mod->name) == 0)
      return mod;
  }

  return NULL;
}

static int rtems_shell_main_pci(
  int   argc,
  char *argv[]
)
{
  struct shell_pci_modifier *mod;
  int rc;

  if (argc < 2) {
    /* without arguments */
    pci_summary();
    rc = 0;
  } else if ((mod=shell_pci_find_modifier(argv[1])) != NULL) {
    rc = mod->func(argc, argv, mod);
  } else {
    rc = -1;
  }

  if (rc < 0) {
    printf(" invalid argument\n");
    usage();
  }

  return rc;
}

rtems_shell_cmd_t rtems_shell_PCI_Command = {
  "pci",                         /* name */
  pci_usage_str,                 /* usage */
  "system",                      /* topic */
  rtems_shell_main_pci,          /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
