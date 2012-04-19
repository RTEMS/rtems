/*
 *  AMBA Plug & Play routines: device information printing.
 *
 *  COPYRIGHT (c) 2009.
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <ambapp.h>

extern char *ambapp_device_id2str(int vendor, int id);
extern char *ambapp_vendor_id2str(int vendor);

struct ambapp_dev_print_arg {
  int show_depth;
};

static char *unknown = "unknown";

int ambapp_dev_print(struct ambapp_dev *dev, int index, void *arg)
{
  char *dev_str, *ven_str, *type_str;
  struct ambapp_dev_print_arg *p = arg;
  char dp[32];
  int i=0;
  unsigned int basereg;

  if (p->show_depth) {
    for (i=0; i<ambapp_depth(dev)*2; i+=2) {
      dp[i] = ' ';
      dp[i+1] = ' ';
    }
  }
  dp[i] = '\0';

  ven_str = ambapp_vendor_id2str(dev->vendor);
  if (!ven_str) {
    ven_str = unknown;
    dev_str = unknown;
  } else {
    dev_str = ambapp_device_id2str(dev->vendor, dev->device);
    if (!dev_str)
      dev_str = unknown;
  }
  if (dev->dev_type == DEV_APB_SLV) {
    /* APB */
    basereg = DEV_TO_APB(dev)->start;
    type_str = "apb";
  } else {
    /* AHB */
    basereg = DEV_TO_AHB(dev)->start[0];
    type_str = "ahb";
  }
  printf("%s |-> 0x%x:0x%x:0x%x: %s_%s, %s: 0x%x, 0x%x (OWNER: 0x%x)\n",
         dp, index, dev->vendor, dev->device, ven_str, dev_str, type_str,
         basereg, (unsigned int)dev, (unsigned int)dev->owner);

  return 0;
}

void ambapp_print(struct ambapp_bus *abus, int show_depth)
{
  struct ambapp_dev_print_arg arg;
  arg.show_depth = show_depth;
  ambapp_for_each(abus, (OPTIONS_ALL_DEVS|OPTIONS_ALL|OPTIONS_DEPTH_FIRST), -1,
                  -1, ambapp_dev_print, &arg);
}
