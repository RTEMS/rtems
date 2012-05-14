/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/********************************************************/
/* Find bus bridges */

/* This part of the program builds a list with pairs of bus
   master port names (each is "device name/master port name").
   It is then possible to find if a given master is actually
   available under a different master port name through bridges.
 */

/* Typical example with external SRAM that is slave of
   tristate_bridge_0/tristate_master, and
   tristate_bridge_0 itself is slave of cpu0/data_master, the
   bridge information would be stored as this bus_bridge_pair:
      mastered_by = "cpu0/data_master" and
      bridges_to = "tristate_bridge_0/tristate_master".
   That allows to deduce that SRAM is actually mastered by
   cpu0/data_master. If there were any address or bus width
   translations, it should be noted in the bridges list... For
   now we simply assume that bridges never translate anything.
 */

#include <string.h>
#include <stdlib.h>

#include "ptf.h"
#include "bridges.h"

int is_bridged(
  char *cpu_master,
  char *dev_master,
  bus_bridge_pair *bridges)
{
  char *curr_master;
  bus_bridge_pair *bbp;

  if(strcmp(cpu_master, dev_master) == 0) return 1; /* cpu directly masters dev */

  for(bbp = bridges; bbp != NULL; bbp=bbp->next)
  {
    if(strcmp(cpu_master, bbp->mastered_by) == 0 &&
       is_bridged(bbp->bridges_to, dev_master, bridges))
    {
      return 1; /* cpu masters dev via bridge */
    }
  };

  return 0;
}

void add_bridge_master(struct ptf_item *pi, void *arg)
{
    struct { char *bt; bus_bridge_pair **bridges; } *binfo = arg;
    bus_bridge_pair *new_pair;

    if(binfo->bridges == 0) return;

    new_pair = (bus_bridge_pair *)malloc(sizeof(bus_bridge_pair));
    if(new_pair == NULL) return;

    new_pair->bridges_to = binfo->bt;
    new_pair->mastered_by = pi->item[pi->level]->value;
    new_pair->next = *(binfo->bridges);
    *(binfo->bridges) = new_pair;
}

void add_bridge_dest(struct ptf_item *pi, void *arg)
{
    struct ptf maby_section = { section, "MASTERED_BY", 0, 0, 0 };
    struct ptf_item maby = { 1, &maby_section };

    char *bridge_name = pi->item[1]->value;
    char *bridge_dest = pi->item[pi->level]->value;
    struct { char *bt; bus_bridge_pair **bridges; } binfo;

    binfo.bridges = arg;
    binfo.bt = (char*)malloc(strlen(bridge_name)+strlen(bridge_dest) + 2);
    strcpy(binfo.bt, bridge_name);
    strcat(binfo.bt, "/");
    strcat(binfo.bt, bridge_dest);

    ptf_match(pi->item[pi->level-1]->sub, &maby, add_bridge_master, &binfo);

    /* binfo.bt is NOT freed here */
}

bus_bridge_pair *find_bridges(struct ptf *p)
{
    bus_bridge_pair *bridges = 0;

    struct ptf system     = { section, "SYSTEM", 0, 0, 0 };
    struct ptf module     = { section, "MODULE", 0, 0, 0 };
    struct ptf slave      = { section, "SLAVE",  0, 0, 0 };
    struct ptf syb        = { section, "SYSTEM_BUILDER_INFO", 0, 0, 0 };
    struct ptf to         = { item,    "Bridges_To", 0, 0, 0 };
    struct ptf_item brdg  = { 5, &system, &module, &slave, &syb, &to };

    ptf_match(p, &brdg, add_bridge_dest, &bridges);

    return bridges;
}




