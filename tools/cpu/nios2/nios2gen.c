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
#include "clocks.h"
#include "devices.h"
#include "memory.h"
#include "output.h"
#include "linkcmds.h"

#define NIOS2GEN_PACKAGE PACKAGE
#define NIOS2GEN_VERSION VERSION

#include "getopt.h"

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

/********************************************************/

void version(FILE *f)
{
fprintf(f,
  "nios2gen (" __DATE__ ")\n"
  "  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de\n"
  "\n"
  "  The license and distribution terms for this file may be\n"
  "  found in the file LICENSE in this distribution or at\n"
  "  http://www.rtems.com/license/LICENSE.\n"
  , VERSION
);
}

void usage (FILE *f, char *errmsg)
{
  char *info = "Purpose:\n  Create RTEMS nios2 BSP configuration from Altera SOPC description\n";
  if(errmsg) info=errmsg;
  version(f);
  fprintf(f,"\n%s\nUsage: nios2gen [OPTIONS] <SOPC PTF> [CONFIG PTF] [CONFIG PTF]...\n", info);
  fprintf(f,"   -h         --help               Print help and exit\n");
  fprintf(f,"   -V         --version            Print version and exit\n");
  fprintf(f,"   -bFILENAME --bspheader=FILENAME Output BSP configuration header file (default='sopc.h')\n");
  fprintf(f,"   -B         --no-bspheader       Do not output BSP configuration header file\n");
  fprintf(f,"   -lFILENAME --linkcmds=FILENAME  Output linker script (default='linkcmds')\n");
  fprintf(f,"   -L         --no-linkcmds        Do not output linker script\n");
  fprintf(f,"   -pFILENAME --parsed=FILENAME    Output PTF contents as long list (default: don't)\n");
  fprintf(f,"   -P         --no-parsed          Do not output PTF contents as long list\n");
  fprintf(f,"   -q         --quiet              Do not print progress info to stdout\n\n");
  fprintf(f,"Using \"-\" as the FILENAME means standard output (stdout).\n");
}

/********************************************************/

int main(int argc, char *argv[])
{
    struct ptf *sopc, *cfg, *p, *cpu;
    struct ptf_item pi;
    device_desc *devices;
    bus_bridge_pair *bridges;
    clock_desc *clocks;
    memory_desc *memory;

    int verbose = 1;
    int output_order = 127;
    char *parsed_filename = NULL;
    int output_parsed = 0;
    char *bspheader_filename = "sopc.h";
    int output_bspheader = 1;
    char *linkcmds_filename = "linkcmds";
    int output_linkcmds = 2;

    optarg = 0;
    optind = 1;
    opterr = 1;
    optopt = '?';

    if(argc > 126)
    {
      usage(stderr,"Too many commandline arguments!\n");
      return -1;
    };

    while(1)
    {
      int c, long_index = 0;
      static char *optstring = "hVBb:Ll:Pp:q";

      static struct option long_options[] =
      {
        { "help",           0, NULL, 'h' },
        { "version",        0, NULL, 'V' },
        { "no-bspheader",   0, NULL, 'B' },
        { "bspheader",      1, NULL, 'b' },
        { "no-linkcmds",    0, NULL, 'L' },
        { "linkcmds",       1, NULL, 'l' },
        { "no-parsed",      0, NULL, 'P' },
        { "parsed",         1, NULL, 'p' },
        { "quiet",          0, NULL, 'q' },
        { NULL, 0, NULL, 0 }
      };

      c = getopt_long (argc, argv, optstring, long_options, &long_index);

      if(c == -1) break; /* Exit from while(1) loop */

      switch(c)
      {
      case 'q': /* Be quiet */
        verbose = 0;
        break;

      case 'h': /* Print help and exit */
        usage(stdout,NULL);
        return 0;

      case 'V': /* Print version and exit */
        version(stdout);
        return 0;

      case 'B': /* Do not output BSP configuration header file */
        output_bspheader = 0;
        break;

      case 'b': /* Output BSP configuration header file */
        bspheader_filename = optarg;
        output_bspheader = output_order;
        output_order--;
        break;

      case 'L': /* Do not output linker script */
        output_linkcmds = 0;
        break;

      case 'l': /* Output linker script */
        linkcmds_filename = optarg;
        output_linkcmds = output_order;
        output_order--;
        break;

      case 'P': /* Do not output PTF contents */
        output_parsed = 0;
        break;

      case 'p': /* Output PTF contents as long list */
        parsed_filename = optarg;
        output_parsed = output_order;
        output_order--;
        break;

      case 0:
      case '?':
        return -1;

      default:
        fprintf(stderr, "%s: unknown option: %c\n", NIOS2GEN_PACKAGE, c);
      };
    };

    if(optind >= argc)
    {
        usage(stderr,"No PTF specified!\n");
        return -1;
    };

    /********************************************************/

    sopc = ptf_parse_file(argv[optind]);
    if(sopc == NULL)
    {
        fprintf(stderr, "Could not parse system description PTF '%s'.\n", argv[optind]);
        return -1;
    };

    if(verbose) printf("Successfully read SOPC PTF file %s.\n", argv[optind]);

    /********************************************************/

    cfg = NULL;

    for(optind++;optind<argc;optind++)
    {
      struct ptf *morecfg = ptf_parse_file(argv[optind]);

      if(morecfg == NULL)
      {
        fprintf(stderr, "Couldn't parse '%s'.\n", argv[optind]);
        return -1;
      };

      if(verbose) printf("Successfully read config PTF file %s.\n", argv[optind]);

      cfg = ptf_concat(cfg, morecfg);
    };

    /********************************************************/
    /* Pull in include files specified in the configs; */
    /* Only one level is read; included files are not */
    /* checked for further INCLUDEs */

    {
      struct ptf include_item = { item, "INCLUDE", 0, 0, 0 };
      struct ptf_item inc_file_spec = { 1, &include_item };
      ptf_match(cfg, &inc_file_spec, read_include_file, NULL);
    }

    /********************************************************/
    /* Find CPU */

    if(verbose) printf("Looking for usable CPUs...\n");

    /* Check if a CPU has been specified in the config PTF */
    {
      struct ptf modules         = { section, "MODULES", 0, 0, 0 };
      struct ptf cpu_def         = { item, "CPU", 0, 0, 0 };
      struct ptf_item cpu_spec   = { 2, &modules, &cpu_def };

      ptf_match(cfg, &cpu_spec, store_ptf_ptr, &cpu);
    };

    /* Look for CPUs in system description PTF */
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
        fprintf(stderr, "There is more than one CPU. Please specify the one\n");
        fprintf(stderr, "you want to use with this BSP in your config file.\n");
        fprintf(stderr, "The available CPUs are named as follows:\n");
        ptf_match(sopc, &class_spec, printf_ptf_value, "  %s\n");
        return -1;
      };

      if(cpu_count == 0)
      {
        fprintf(stderr, "There is no NIOS2 cpu in the system.\n");
        return -1;
      }
    };

    if(verbose)
    {
      printf("Using NIOS II CPU '%s'.\n", cpu->value);
      printf("Only modules mastered by this CPU are considered now.\n");
    };

    /********************************************************/
    /* Find Bridges */

    if(verbose) printf("Looking for bus bridges...\n");

    bridges = find_bridges(sopc);

    if(verbose)
    {
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
    };

    /********************************************************/
    /* Find clocks */

    if(verbose) printf("Looking for clock definitions...\n");

    clocks = find_clocks(sopc, cfg);

    if(verbose)
    {
      if(clocks)
      {
        clock_desc *cs;
        for(cs = clocks; cs; cs = cs->next)
        {
          printf("Found clock \"%s\" (%lu Hz), naming it %s\n", cs->name, cs->freq, cs->cfgname);
        };
      }
      else
      {
        printf("No clocks present.\n");
      };
    };

    /********************************************************/
    /* Find other devices available to the selected CPU */

    if(verbose) printf("Looking for devices...\n");

    devices = find_devices(sopc, cfg, cpu, bridges);

    if(verbose)
    {
      if(devices)
      {
        device_desc *dd;
        for(dd = devices; dd; dd=dd->next)
        {
          printf("Found device \"%s\", naming it %s\n", dd->ptf->value, dd->cfgname);
        };
      }
      else
      {
        printf("No devices present.\n");
      };
    };

    /********************************************************/
    /* Find out which devices are actually memory */

    if(verbose) printf("Looking for memory...\n");

    memory = find_memory(devices);

    if(verbose)
    {
      if(memory)
      {
        memory_desc *md;
        for(md = memory; md; md=md->next)
        {
          printf("Found memory in \"%s\", base=0x%08X, size=%lu bytes\n",
                          md->dev->cfgname,
                          md->base, md->size);
        };
      }
      else
      {
        printf("None of the devices seems to provide memory?!\n");
      };
    };


    /********************************************************/
    /* Output files in the order they were specified
       on the command line */

    {
      int i;
      for(i=0;i<3;i++)
      {
        if(output_bspheader>0
           && output_bspheader>=output_linkcmds
           && output_bspheader>=output_parsed)
        {
          output_bspheader = 0;
          if(bspheader_filename == NULL || (bspheader_filename[0]=='-' && bspheader_filename[1]==0))
          {
            fwrite_header_file(stdout, cfg, devices, clocks);
          }
          else
          {
            FILE *f = fopen(bspheader_filename, "w");
            if(!f)
            {
              perror(bspheader_filename);
              return -1;
            }
            else
            {
              fwrite_header_file(f, cfg, devices, clocks);
              fclose(f);
            }
          }
        };
        if(output_linkcmds>0
           && output_linkcmds>=output_bspheader
           && output_linkcmds>=output_parsed)
        {
          output_linkcmds = 0;
          if(linkcmds_filename == NULL || (linkcmds_filename[0]=='-' && linkcmds_filename[1]==0))
          {
            fwrite_linkcmds_file(stdout, cfg, cpu, devices, memory);
          }
          else
          {
            FILE *f = fopen(linkcmds_filename, "w");
            if(!f)
            {
              perror(linkcmds_filename);
              return -1;
            }
            else
            {
              fwrite_linkcmds_file(f, cfg, cpu, devices, memory);
              fclose(f);
            }
          }
        };
        if(output_parsed>0
           && output_parsed>=output_linkcmds
           && output_parsed>=output_bspheader)
        {
          output_parsed = 0;
          if(parsed_filename == NULL || (parsed_filename[0]=='-' && parsed_filename[1]==0))
          {
            ptf_printf(stdout, sopc, "");
          }
          else
          {
            FILE *f = fopen(parsed_filename, "w");
            if(!f)
            {
              perror(parsed_filename);
              return -1;
            }
            else
            {
              ptf_printf(f, sopc, "");
              fclose(f);
            }
          }
        };
      }
    };

    if(verbose) printf("Done.\n");

    return 0;
}

/* vi:ts=4:
 */


