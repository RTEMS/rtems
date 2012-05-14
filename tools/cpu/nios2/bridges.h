/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __NIOS2GEN_BRIDGES_H
#define __NIOS2GEN_BRIDGES_H 1

typedef struct bus_bridge_pair
{
  char *mastered_by;
  char *bridges_to;
  struct bus_bridge_pair *next;
}
bus_bridge_pair;

bus_bridge_pair *find_bridges(struct ptf *p);

int is_bridged(
  char *cpu_master,
  char *dev_master,
  bus_bridge_pair *bridges);

#endif


