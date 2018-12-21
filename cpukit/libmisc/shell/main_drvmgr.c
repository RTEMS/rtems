/*
 * DRVMGR Command Implementation
 *
 * COPYRIGHT (c) 2010.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
#include <drvmgr/drvmgr.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static void usage(void);

static void *get_obj_adr(char *arg)
{
  unsigned long obj_adr;

  obj_adr = strtoul(arg, NULL, 16);
  if (obj_adr == ULONG_MAX || obj_adr == 0) {
    puts(" Not a valid ID");
    return NULL;
  }

  return (void *)obj_adr;
}

/* General info, root bus, number of devices etc. */
static void show_drvmgr_info(void)
{
  drvmgr_summary();
  drvmgr_print_devs(PRINT_DEVS_ALL);
}

static int shell_drvmgr_topo(int argc, char *argv[])
{
  drvmgr_print_topo();
  return 0;
}

static int shell_drvmgr_short(int argc, char *argv[])
{
  void *obj;

  if (argc < 2)
    return -1;
  if (argc < 3) {
    /* All Devices */
    drvmgr_info_drvs(0);
    drvmgr_info_buses(0);
    drvmgr_info_devs(OPTION_DEV_GENINFO);
    return 0;
  }

  /* Get ID from string */
  obj = get_obj_adr(argv[2]);
  if (!obj)
    return -3;

  drvmgr_info(obj, OPTION_DEV_GENINFO);

  return 0;
}

static int shell_drvmgr_info(int argc, char *argv[])
{
  void *obj;

  if (argc < 2)
    return -1;
  if (argc < 3) {
    /* All Drivers, Buses and Devices */
    drvmgr_info_drvs(OPTION_INFO_ALL);
    drvmgr_info_buses(OPTION_INFO_ALL);
    drvmgr_info_devs(OPTION_INFO_ALL);
    return 0;
  }

  /* Get ID from string */
  obj = get_obj_adr(argv[2]);
  if (!obj)
    return -3;

  drvmgr_info(obj, OPTION_INFO_ALL);

  return 0;
}

static int shell_drvmgr_remove(int argc, char *argv[])
{
  puts(" Not implemented");
  return 0;
}

static int shell_drvmgr_parent(int argc, char *argv[])
{
  void *obj;
  int obj_type;
  struct drvmgr_dev *dev;
  struct drvmgr_bus *bus;

  /* Get ID from string */
  if (argc < 3)
    return -2;
  obj = get_obj_adr(argv[2]);
  if (!obj)
    return -3;

  obj_type = *(int *)obj;
  if (obj_type == DRVMGR_OBJ_BUS) {
    bus = obj;
    if (!bus->dev) {
      puts(" bus has no bridge device");
    } else if(!bus->dev->parent) {
      puts(" bridge device has no parent");
    } else {
      dev = bus->dev;
      printf(" BUSID=%p\n", dev->parent);
    }
  } else if (obj_type == DRVMGR_OBJ_DEV) {
    dev = obj;
    if (!dev->parent) {
      puts(" device has no parent bus");
    } else {
      printf(" BUSID=%p\n", dev->parent);
    }
  } else {
    puts(" ID is not a device or bus");
    return 1;
  }

  return 0;
}

static void shell_drvmgr_print_key_array(struct drvmgr_key *keys)
{
  struct drvmgr_key *key;
  static char *type_strs[4] = {"UNKNOWN","INTEGER","STRING ","POINTER"};
  enum drvmgr_kt type;
  union drvmgr_key_value *val;

  if (keys == NULL) {
    printf("  DEV HAS NO KEYS\n");
    return;
  }

  key = &keys[0];
  while (key->key_type != DRVMGR_KT_NONE) {
    if (key->key_type > DRVMGR_KT_POINTER)
      type = DRVMGR_KT_NONE;
    else
      type = key->key_type;
    printf("  NAME=%-14s TYPE=%s  VALUE=", key->key_name, type_strs[type]);
    val = &key->key_value;
    switch (type) {
      default:
      case DRVMGR_KT_NONE:
      case DRVMGR_KT_INT:
        printf("0x%x (%d)\n", val->i, val->i);
        break;
      case DRVMGR_KT_STRING:
        printf("%s\n", val->str);
        break;
      case DRVMGR_KT_POINTER:
        printf("%p\n", val->ptr);
        break;
    }
    key++;
  }
}

static void shell_drvmgr_print_res_array(struct drvmgr_drv_res *resources)
{
  struct drvmgr_drv_res *res = &resources[0];
  struct drvmgr_drv *drv;
  char *drv_name;

  while (res->drv_id) {
    /* Find Driver in order to print name of driver */
    drv = drvmgr_drv_by_id(res->drv_id);
    if (drv && drv->name)
      drv_name = drv->name;
    else
      drv_name = "UNKNOWN";
    printf(" RESOURCES FOR DEVICE[%02d] DRIVER[0x%" PRIu64 " (%s)]\n",
            res->minor_bus, res->drv_id, drv_name);
    shell_drvmgr_print_key_array(res->keys);
    res++;
  }
}

static int shell_drvmgr_res(int argc, char *argv[])
{
  void *obj;
  int obj_type;
  struct drvmgr_dev *dev;
  struct drvmgr_bus *bus;
  struct drvmgr_key *keys;
  struct drvmgr_bus_res *lst;
  int i;

  /* Get ID from string */
  if (argc < 3)
    return -2;
  obj = get_obj_adr(argv[2]);
  if (!obj)
    return -3;

  obj_type = *(int *)obj;
  if (obj_type == DRVMGR_OBJ_BUS) {
    bus = obj;
    lst = bus->reslist;
    if (lst == NULL) {
      puts(" BUS does not have resources\n");
      return 0;
    }
    i = 0;
    while (lst) {
      printf(" -- RESOURCES ARRAY %d --\n", i);
      shell_drvmgr_print_res_array(lst->resource);
      puts("");
      i++;
      lst = lst->next;
    }
  } else if (obj_type == DRVMGR_OBJ_DEV) {
    dev = obj;
    if (dev->drv == NULL) {
      puts(" DEVICE has no driver ==> resources not available\n");
      return 0;
    }
    drvmgr_keys_get(dev, &keys);
    if (keys == NULL) {
      puts(" DEVICE does not have resources\n");
      return 0;
    }
    shell_drvmgr_print_key_array(keys);
  } else {
    puts(" ID is not a device or bus");
    return 1;
  }

  return 0;
}

static int shell_drvmgr_buses(int argc, char *argv[])
{
  drvmgr_info_buses(OPTION_INFO_ALL);
  return 0;
}

static int shell_drvmgr_devs(int argc, char *argv[])
{
  drvmgr_info_devs(OPTION_INFO_ALL);
  return 0;
}

static int shell_drvmgr_drvs(int argc, char *argv[])
{
  drvmgr_info_drvs(OPTION_INFO_ALL);
  return 0;
}

static int shell_drvmgr_mem(int argc, char *argv[])
{
  drvmgr_print_mem();
  return 0;
}

static int shell_drvmgr_translate(int argc, char *argv[])
{
  int rc, rev, up, obj_type;
  void *obj, *dst;
  unsigned long src, tmp;

  if (argc != 5)
    return -1;

  obj = get_obj_adr(argv[2]);
  if (!obj)
    return -3;

  obj_type = *(int *)obj;
  if (obj_type != DRVMGR_OBJ_DEV) {
    puts(" ID is not a device\n");
    return 0;
  }

  tmp = strtoul(argv[3], NULL, 0);
  if (tmp > 3) {
    puts(" Not a valid option OPT, only [0..3] is valid");
    return 0;
  }
  rev = tmp & DRVMGR_TR_REVERSE;
  up = tmp & DRVMGR_TR_PATH;

  src = strtoul(argv[4], NULL, 0);
  if (src == ULONG_MAX && errno == ERANGE) {
    puts(" Not a valid source address");
    return 0;
  }

  rc = drvmgr_translate((struct drvmgr_dev *)obj, up | rev, (void *)src, &dst);
  if (rc == 0)
    printf(" Address %p could not be translated\n", (void *)src);
  else if (rc == 0xffffffff)
    printf(" %p => %p  (no translation required)\n", (void *)src, dst);
  else
    printf(" %p => %p  (map size 0x%x)\n", (void *)src, dst, rc);

  return 0;
}

static const char drvmgr_usage_str[] =
 " usage:\n"
 "  drvmgr buses         List bus specfic information on all buses\n"
 "  drvmgr devs          List general and driver specfic information\n"
 "                       about all devices\n"
 "  drvmgr drvs          List driver specfic information on all drivers\n"
 "  drvmgr info [ID]     List general and driver specfic information\n"
 "                       about all devices or one device, bus or driver\n"
 "  drvmgr mem           Dynamically memory usage\n"
 "  drvmgr parent ID     Short info about parent bus of a device\n"
 "  drvmgr remove ID     Remove a device or a bus\n"
 "  drvmgr res ID        List Resources of a device or bus\n"
 "  drvmgr short [ID]    Short info about all devices/buses or one\n"
 "                       device/bus\n"
 "  drvmgr topo          Show bus topology with all devices\n"
 "  drvmgr tr ID OPT ADR Translate ADR down(0)/up(1) -streams (OPT bit 1) in\n"
 "                       std(0)/reverse(1) (OPT bit 0) direction for device\n"
 "  drvmgr --help\n";

static void usage(void)
{
  puts(drvmgr_usage_str);
}

static int shell_drvmgr_usage(int argc, char *argv[])
{
  usage();
  return 0;
}

struct shell_drvmgr_modifier {
  char *name;
  int (*func)(int argc, char *argv[]);
};

#define MODIFIER_NUM 12
static struct shell_drvmgr_modifier shell_drvmgr_modifiers[MODIFIER_NUM] =
{
  {"buses", shell_drvmgr_buses},
  {"devs", shell_drvmgr_devs},
  {"drvs", shell_drvmgr_drvs},
  {"info", shell_drvmgr_info},
  {"mem", shell_drvmgr_mem},
  {"parent", shell_drvmgr_parent},
  {"remove", shell_drvmgr_remove},
  {"res", shell_drvmgr_res},
  {"short", shell_drvmgr_short},
  {"topo", shell_drvmgr_topo},
  {"tr", shell_drvmgr_translate},
  {"--help", shell_drvmgr_usage},
};

static struct shell_drvmgr_modifier *shell_drvmgr_find_modifier(char *name)
{
  struct shell_drvmgr_modifier *mod;
  int i;

  if (name == NULL)
    return NULL;

  for (i=0, mod=&shell_drvmgr_modifiers[0]; i<MODIFIER_NUM; i++, mod++) {
    if (strcmp(name, mod->name) == 0)
      return mod;
  }

  return NULL;
}

static int rtems_shell_main_drvmgr(
  int   argc,
  char *argv[]
)
{
  struct shell_drvmgr_modifier *mod;
  int rc;

  if (argc < 2) {
    show_drvmgr_info();
    rc = 0;
  } else if ((mod=shell_drvmgr_find_modifier(argv[1])) != NULL) {
    rc = mod->func(argc, argv);
  } else {
    rc = -1;
  }

  if (rc < 0) {
    printf(" invalid argument\n");
    usage();
  }

  return rc;
}

rtems_shell_cmd_t rtems_shell_DRVMGR_Command = {
  "drvmgr",                      /* name */
  drvmgr_usage_str,              /* usage */
  "system",                      /* topic */
  rtems_shell_main_drvmgr,       /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
