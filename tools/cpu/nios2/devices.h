/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __NIOS2GEN_DEVICES_H
#define __NIOS2GEN_DEVICES_H 1

#include "ptf.h"
#include "bridges.h"

typedef struct dev_descr
{
  int slaves;
  char *cfgname;
  struct ptf *ptf;
  struct dev_descr *next;
}
device_desc;

device_desc *find_devices(
  struct ptf *ptf,
  struct ptf *cfg,
  struct ptf *cpu,
  bus_bridge_pair *bridges);

#endif
