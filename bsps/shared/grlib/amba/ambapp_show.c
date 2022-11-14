/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  AMBA Plug & Play routines: device information printing.
 *
 *  COPYRIGHT (c) 2009.
 *  Aeroflex Gaisler.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <grlib/ambapp.h>

struct ambapp_dev_print_arg {
  int show_depth;
};

static char *unknown = "unknown";

static int ambapp_dev_print(struct ambapp_dev *dev, int index, void *arg)
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
