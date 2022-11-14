/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Old AMBA scanning Interface provided for backwards compability
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
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

#include <grlib/ambapp.h>

struct ambapp_dev_find_match_arg {
  int      index;
  int      count;
  int      type;
  void      *dev;
};

/* AMBA PP find routines */
static int ambapp_dev_find_match(struct ambapp_dev *dev, int index, void *arg)
{
  struct ambapp_dev_find_match_arg *p = arg;

  if (p->index == 0) {
    /* Found controller, stop */
    if (p->type == DEV_APB_SLV) {
      *(struct ambapp_apb_info *)p->dev = *DEV_TO_APB(dev);
      p->dev = ((struct ambapp_apb_info *)p->dev)+1;
    } else {
      *(struct ambapp_ahb_info *)p->dev = *DEV_TO_AHB(dev);
      p->dev = ((struct ambapp_ahb_info *)p->dev)+1;
    }
    p->count--;
    if (p->count < 1)
      return 1;
  } else {
    p->index--;
  }
  return 0;
}

int ambapp_find_apbslvs_next(struct ambapp_bus *abus, int vendor, int device, struct ambapp_apb_info *dev, int index, int maxno)
{
  struct ambapp_dev_find_match_arg arg;

  arg.index = index;
  arg.count = maxno;
  arg.type = DEV_APB_SLV; /* APB */
  arg.dev = dev;

  ambapp_for_each(abus, (OPTIONS_ALL|OPTIONS_APB_SLVS), vendor, device,
                  ambapp_dev_find_match, &arg);

  return maxno - arg.count;
}

int ambapp_find_apbslv(struct ambapp_bus *abus, int vendor, int device, struct ambapp_apb_info *dev)
{
  return ambapp_find_apbslvs_next(abus, vendor, device, dev, 0, 1);
}

int ambapp_find_apbslv_next(struct ambapp_bus *abus, int vendor, int device, struct ambapp_apb_info *dev, int index)
{
  return ambapp_find_apbslvs_next(abus, vendor, device, dev, index, 1);
}

int ambapp_find_apbslvs(struct ambapp_bus *abus, int vendor, int device, struct ambapp_apb_info *dev, int maxno)
{
  return ambapp_find_apbslvs_next(abus, vendor, device, dev, 0, maxno);
}

int ambapp_get_number_apbslv_devices(struct ambapp_bus *abus, int vendor, int device)
{
  return ambapp_dev_count(abus, (OPTIONS_ALL|OPTIONS_APB_SLVS), vendor, device);
}

int ambapp_find_ahbslvs_next(struct ambapp_bus *abus, int vendor, int device, struct ambapp_ahb_info *dev, int index, int maxno)
{
  struct ambapp_dev_find_match_arg arg;

  arg.index = index;
  arg.count = maxno;
  arg.type = DEV_AHB_SLV; /* AHB SLV */
  arg.dev = dev;

  ambapp_for_each(abus, (OPTIONS_ALL|OPTIONS_AHB_SLVS), vendor, device,
                  ambapp_dev_find_match, &arg);

  return maxno - arg.count;
}

int ambapp_find_ahbslv_next(struct ambapp_bus *abus, int vendor, int device, struct ambapp_ahb_info *dev, int index)
{
  return ambapp_find_ahbslvs_next(abus, vendor, device, dev, index, 1);
}

int ambapp_find_ahbslv(struct ambapp_bus *abus, int vendor, int device, struct ambapp_ahb_info *dev)
{
  return ambapp_find_ahbslvs_next(abus, vendor, device, dev, 0, 1);
}

int ambapp_find_ahbslvs(struct ambapp_bus *abus, int vendor, int device, struct ambapp_ahb_info *dev, int maxno)
{
  return ambapp_find_ahbslvs_next(abus, vendor, device, dev, 0, maxno);
}

int ambapp_get_number_ahbslv_devices(struct ambapp_bus *abus, int vendor, int device)
{
  return ambapp_dev_count(abus, (OPTIONS_ALL|OPTIONS_AHB_SLVS), vendor, device);
}
