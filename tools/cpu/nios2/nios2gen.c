/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ptf.h"
#include "bridges.h"
#include "clocks.h"
#include "devices.h"
#include "output.h"

/********************************************************/

void store_ptf_parent(struct ptf_item *pi, void *arg)
{
  struct ptf *p = pi->item[pi->level-1];
  *(struct ptf **)arg = p;
}

/********************************************************/

void store_ptf_ptr(struct ptf_item *pi, void *arg)
{
  struct ptf *p = pi->item[pi->level];
  *(struct ptf **)arg = p;
}

/********************************************************/

void printf_ptf_value(struct ptf_item *pi, void *arg)
{
  struct ptf *p = pi->item[pi->level];
  printf(*(char **)arg, p->value);
}

/********************************************************/

void read_include_file(struct ptf_item *pi, void *arg)
{
    struct ptf *inc, *next;
    struct ptf *p = pi->item[pi->level];

    inc = ptf_parse_file(p->value);

    if(inc == NULL)
    {
        fprintf(stderr, "Warning: couldn't parse included '%s'.\n", p->value);
        return;
    };

    printf("Successfully read included PTF file %s.\n", p->value);

    next = p->next;
    for(p->next = inc; p->next != NULL; p = p->next);
    p->next = next;
}

void usage()
{
        fprintf(stderr, 
"Please specify the name of a nios2gen PTF file that describes where to\n"
"find the system description PTF from SOPC Builder on the command line.\n");
}

/********************************************************/

int main(int argc, char *argv[])
{
    struct ptf *sopc, *cfg, *p, *cpu;
    struct ptf_item pi;
    device_desc *devices;
    bus_bridge_pair *bridges;
    clock_desc *clocks;

    if (argc<2)
    {
        usage();
        return -1;
    };

    cfg = ptf_parse_file(argv[1]);
    if(cfg == NULL)
    {
        fprintf(stderr, "Couldn't parse '%s'.\n", argv[1]);
        return -1;
    };

    printf("Successfully read config PTF file %s.\n", argv[1]);

    /********************************************************/

    {
      struct ptf include_item = { item, "INCLUDE", 0, 0, 0 };
      struct ptf_item inc_file_spec = { 1, &include_item };
      ptf_match(cfg, &inc_file_spec, read_include_file, NULL);
    }

    /********************************************************/
    
    {
      struct ptf *p;
      struct ptf sopc_ptf_item = { item, "SOPC_PTF", 0, 0, 0 };
      struct ptf_item sopc_ptf_spec = { 1, &sopc_ptf_item };
      ptf_match(cfg, &sopc_ptf_spec, store_ptf_ptr, &p);

      if(p == NULL)
      {
          fprintf(stderr, "Missing 'SOPC_PTF' filename in %s!\n", argv[1]);
          return -1;
      };

      sopc = ptf_parse_file(p->value);
      if(sopc == NULL)
      {
          fprintf(stderr, "Could not parse SOPC_PTF '%s'.\n", p->value);
          return -1;
      };

      printf("Successfully read SOPC PTF file %s.\n", p->value);
    };

    /********************************************************/
    /* Find CPU */

    printf("Looking for usable CPUs...\n");

    {
      struct ptf modules         = { section, "MODULES", 0, 0, 0 };
      struct ptf cpu_def         = { item, "CPU", 0, 0, 0 };
      struct ptf_item cpu_spec   = { 2, &modules, &cpu_def };

      ptf_match(cfg, &cpu_spec, store_ptf_ptr, &cpu);
    };

    {
      int cpu_count;
      struct ptf system          = { section, "SYSTEM", 0, 0, 0 };
      struct ptf module          = { section, "MODULE", 0, 0, 0 };
      struct ptf nios2_cpu_class = { item, "class", "altera_nios2", 0, 0 };
      struct ptf_item class_spec = { 3, &system, &module, &nios2_cpu_class };

      if(cpu) if(cpu->value) class_spec.item[1]->value = cpu->value;

      cpu_count = ptf_match(sopc, &class_spec, store_ptf_parent, &cpu);

      if(cpu_count > 1)
      {
        printf("There is more than one CPU. Please specify the one\n");
        printf("you want to use with this BSP in your config file.\n");
        printf("The available CPUs are named as follows:\n");
        ptf_match(sopc, &class_spec, printf_ptf_value, "  %s\n");
        return -1;  
      };

      if(cpu_count == 0)
      {
        printf("There is no NIOS2 cpu in the system.\n");
        return -1;  
      }
    };

    printf("Using NIOS II CPU '%s'.\n", cpu->value);
    printf("Only modules mastered by this CPU are considered now.\n");

    /********************************************************/
    /* Find clocks */

    printf("Looking for clock definitions...\n");

    clocks = find_clocks(sopc, cfg);

    if(clocks)
    {
      clock_desc *cs;
      for(cs = clocks; cs; cs = cs->next)
      {
        printf("Found clock: %s (%lu Hz)\n", cs->name, cs->freq);
      };
    }
    else
    {
      printf("No clocks present.\n");
    };

    /********************************************************/
    /* Find Bridges */

    printf("Looking for bus bridges...\n");

    bridges = find_bridges(sopc);

    if(bridges)
    {
      bus_bridge_pair *bbp;
      for(bbp = bridges; bbp; bbp=bbp->next)
      {
        printf("Found bridge: %s\n", bbp->mastered_by);
        printf("               \\_%s\n", bbp->bridges_to);
      };
    }
    else
    {
      printf("No bridges present.\n");
    };

    /********************************************************/
    /* Find other devices available to the selected CPU */

    devices = find_devices(sopc, cfg, cpu, bridges);

    fwrite_header_file(stdout, cfg, devices, clocks);

    // ptf_printf(stdout, ptf, "");
    // ptf_printf(stdout, cfg, "");

    return 0;
}

/* vi:ts=4:
 */


