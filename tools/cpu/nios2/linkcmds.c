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
#include "devices.h"
#include "output.h" /* is_not_connected, fwrite_value, etc */
#include "memory.h"
#include "linkcmds.h"

typedef struct
{
  FILE *file;
  struct ptf *cfg, *cpu;
  device_desc *devices;
  memory_desc *memory;
}
lcmd_desc;

void fwrite_lcmds_section(struct ptf_item *pi, void *arg)
{
  lcmd_desc *li = (lcmd_desc *)arg;
  struct ptf *p;
  struct ptf_item lpi;
  char *location = NULL;
  char *section_name = pi->item[1]->value;

  if(section_name == 0)
  {
    fprintf(stderr, "Found a LINKER/SECTION without name, ignoring it.\n");
    return;
  };

  p = ptf_find(pi->item[1]->sub, &lpi, item, "LOCATION", 0);
  if(p)
  {
    location = p->value;
  }
  else
  {
    if(strcmp(section_name, "entry") == 0)
    {
      p = ptf_find(li->cpu, &lpi, item, "reset_slave", 0);
    }
    else if(strcmp(section_name, "exceptions") == 0)
    {
      p = ptf_find(li->cpu, &lpi, item, "exc_slave", 0);
    };
    if(p) location = p->value;
    /* TODO: This doesn't work yet, parse full slave address, translate into our naming */
  }

  if(location == 0)
  {
    fprintf(stderr, "No LOCATION configured for section '%s'!\n", pi->item[1]->value);
    return;
  };

  fprintf(li->file, "    .%s :\n    {\n", pi->item[1]->value);
  fprintf(li->file, pi->item[2]->value);
  fprintf(li->file, "    } > %s\n\n", location);
}

void fwrite_linkcmds_file(FILE *file, struct ptf *cfg, struct ptf *cpu, device_desc *devices, memory_desc *memory)
{
  struct ptf *p;
  struct ptf_item pi;
  memory_desc *tmd;
  lcmd_desc linfo;

  struct ptf ptlink = { section, "LINKCMDS", 0, 0, 0 };
  struct ptf ptleadtext = { item,    "LEADTEXT", 0, 0, 0 };
  struct ptf ptepilog = { item,    "EPILOG", 0, 0, 0 };
  struct ptf_item malihead = { 2, &ptlink, &ptleadtext };
  struct ptf_item maliepil = { 2, &ptlink, &ptepilog };

  struct ptf ptsect = { section, "SECTION", 0, 0, 0 };
  struct ptf ptcmds = { item, "COMMANDS", 0, 0, 0 };
  struct ptf ptstabs = { item, "STABS", 0, 0, 0 };
  struct ptf_item malisect = { 3, &ptlink, &ptsect, &ptcmds };
  struct ptf_item malistabs = { 2, &ptlink, &ptstabs };

  linfo.cfg     = cfg;
  linfo.cpu     = cpu;
  linfo.file    = file;
  linfo.devices = devices;
  linfo.memory  = memory;

  ptf_match(cfg, &malihead, fwrite_value, file);

  fprintf(file, "MEMORY\n{\n");
  for(tmd = linfo.memory; tmd; tmd = tmd->next)
  {
    fprintf(file, "    %s : ORIGIN = 0x%08X, LENGTH = 0x%08X\n", tmd->dev->cfgname, tmd->base, tmd->size);
  }
  fprintf(file, "}\n\nSECTIONS\n{\n");

  ptf_match(cfg, &malisect, fwrite_lcmds_section, &linfo);
  ptf_match(cfg, &malistabs, fwrite_value, file);

  for(tmd = linfo.memory; tmd; tmd = tmd->next)
  {
    fprintf(file, "    %s : ORIGIN = 0x%08X, LENGTH = 0x%08X\n", tmd->dev->cfgname, tmd->base, tmd->size);
  }


  fprintf(file, "}\n\n");

  ptf_match(cfg, &maliepil, fwrite_value, file);
}


