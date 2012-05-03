/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ptf.h"
#include "bridges.h"
#include "devices.h"

void add_device(device_desc **dl, struct ptf *dev)
{
    device_desc *eds;

    for(eds = *dl; eds; eds = eds->next)
    {
        if(eds->ptf == dev)
        {
            eds->slaves++;
            return;
        };
    };

    eds = (device_desc *)malloc(sizeof(device_desc));
    eds->slaves = 1;
    eds->ptf = dev;
    eds->next = *dl;
    *dl = eds;
}

void check_and_add_device(struct ptf_item *pi, void *arg)
{
    struct ptf *module = pi->item[pi->level-3];
    struct ptf *sysinfo  = pi->item[pi->level-2];
    char *master_name  = pi->item[pi->level]->value;

    struct { char *dm; char *im; device_desc **dl; bus_bridge_pair *bridges; } *dinfo = arg;

    if(is_bridged(dinfo->dm, master_name, dinfo->bridges) ||
       is_bridged(dinfo->im, master_name, dinfo->bridges))
    {
       struct ptf *ni = ptf_alloc_item(item, "N2G_Selected", "1");
       if(ni != NULL)
       {
         ni->next = sysinfo->sub;
         sysinfo->sub = ni;
       };
       add_device(dinfo->dl, module);
    };
}

void set_dev_cfgname(struct ptf_item *pi, void *arg)
{
  device_desc *dev = arg;
  dev->cfgname = pi->item[pi->level]->name;
}


device_desc *find_devices(
  struct ptf *ptf,
  struct ptf *cfg,
  struct ptf *cpu,
  bus_bridge_pair *bridges)
{
    struct ptf system     = { section, "SYSTEM", 0, 0, 0 };
    struct ptf module     = { section, "MODULE", 0, 0, 0 };
    struct ptf slave      = { section, "SLAVE",  0, 0, 0 };
    struct ptf syb        = { section, "SYSTEM_BUILDER_INFO", 0, 0, 0 };
    struct ptf maby       = { section, "MASTERED_BY", 0, 0, 0 };
    struct ptf_item brdg  = { 5, &system, &module, &slave, &syb, &maby };

    struct ptf modules    = { section, "MODULES", 0, 0, 0 };
    struct ptf named      = { item, 0, 0, 0, 0};
    struct ptf_item devcf = { 2, &modules, &named };

    struct { char *dm; char *im; device_desc **dl; bus_bridge_pair *bridges; } dinfo;

    device_desc *found, *reverse;

    found = NULL;

    add_device(&found, cpu); /* The CPU is "self-connected", add it */

    dinfo.dl = &found;
    dinfo.bridges = bridges;
    dinfo.dm = (char *)malloc(strlen(cpu->value)+13);
    dinfo.im = (char *)malloc(strlen(cpu->value)+20);

    strcpy(dinfo.im, cpu->value);
    strcat(dinfo.im, "/");
    strcpy(dinfo.dm, dinfo.im);
    strcat(dinfo.dm, "data_master");
    strcat(dinfo.im, "instruction_master");

    /* "Available" is any MODULE with a SLAVE section that is MASTERED_BY
        either instr_master or data_master of selected CPU, either directly
        or through a bridge. See code above for more info about bridges.
     */

    ptf_match(ptf, &brdg, check_and_add_device, &dinfo);

    free(dinfo.dm);
    free(dinfo.im);

    /* Reverse the linked list */

    reverse = NULL;
    while(found)
    {
      device_desc *tmp = found;
      found = found->next;

      tmp->next = reverse;
      reverse = tmp;

      named.value = tmp->ptf->value;
      tmp->cfgname = NULL;
      ptf_match(cfg, &devcf, set_dev_cfgname, tmp);
      if(tmp->cfgname == NULL) tmp->cfgname = ptf_defused_name(tmp->ptf->value);
     };

    return reverse;
}

