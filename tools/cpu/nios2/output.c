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
#include "output.h"

typedef struct
{
  FILE *file;
  device_desc *dev;
  char *def_name;
  char *orig_value;
  clock_desc *clocks;
  device_desc *devices;
}
out_desc;

int is_not_connected(struct ptf_item *pi)
{
  if(pi->item[0] == NULL) return 0;
  if(pi->item[0]->name == NULL) return 0;

  if(strcmp(pi->item[0]->name, "SLAVE") == 0)
  {
    struct ptf *t;
    struct ptf_item ti;
    t = ptf_find(pi->item[0]->sub, &ti, item, "N2G_Selected", "1");

    if(t == NULL) return 1;
  };

  return 0;
}

void fwrite_devhead_def(struct ptf_item *pi, void *arg)
{
  out_desc *dinfo = arg;

  if(pi != NULL) if(is_not_connected(pi)) return;

  fprintf(dinfo->file, "#define %s_%s %s\n",
    dinfo->dev->cfgname, dinfo->def_name, dinfo->orig_value);
}

void fwrite_devhead_line(struct ptf_item *pi, void *arg)
{
  out_desc *dinfo = arg;

  if(is_not_connected(pi)) return;

  if(strncmp(dinfo->orig_value, "N2G_", 4)==0)
  {
    if(strncmp(dinfo->orig_value, "N2G_CLOCKREF_", 13)==0)
    {
      clock_desc *c;
      for(c = dinfo->clocks; c; c=c->next)
      {
        if(strcmp(c->name, pi->item[pi->level]->value) == 0)
        {
          fprintf(dinfo->file, "#define %s_%s %s\n",
            dinfo->dev->cfgname, dinfo->orig_value + 13, c->cfgname);
          break;
        };
      };
    }
    else if(strncmp(dinfo->orig_value, "N2G_DEVICEREF_", 14)==0)
    {
      device_desc *d;
      for(d = dinfo->devices; d; d=d->next)
      {
        if(strcmp(d->ptf->value, pi->item[pi->level]->value) == 0)
        {
          fprintf(dinfo->file, "#define %s_%s %s\n",
            dinfo->dev->cfgname, dinfo->orig_value + 14, d->cfgname);
          break;
        };
      };
    }
  }
  else
  {
    fprintf(dinfo->file, "#define %s_%s %s\n",
      dinfo->dev->cfgname, dinfo->orig_value,
      pi->item[pi->level]->value);
  };
}

void fwrite_device_header(struct ptf_item *pi, void *arg)
{
  struct ptf *f;
  struct ptf_item fi;
  out_desc *dinfo = arg;

  /* This is called for every matching CLASS section in the
     configuration. The following loop iterates through all
     items in the CLASS section regardless of their nesting level */

  f = ptf_find(pi->item[pi->level]->sub, &fi, item, 0, 0);

  while(f != NULL)
  {
    dinfo->orig_value = f->value;
    if(f->name && strncmp(f->name, "N2G_DEFINE_", 11)==0)
    {
      dinfo->def_name = f->name + 11;
      if(fi.level >= 2)
      {
        fi.level--; /* match only the enclosing section */
        ptf_match(dinfo->dev->ptf->sub, &fi, fwrite_devhead_def, dinfo);
        fi.level++;
      }
      else
      {
        fwrite_devhead_def( 0, dinfo );
      };
    }
    else
    {
      f->value = 0; /* Match ANY value */
      ptf_match(dinfo->dev->ptf->sub, &fi, fwrite_devhead_line, dinfo);
      f->value = dinfo->orig_value;
    };
    f = ptf_next(&fi, item, 0, 0);
  };
}

void fwrite_value(struct ptf_item *pi, void *arg)
{
  FILE *file = arg;
  fputs(pi->item[pi->level]->value, file);
}

void fwrite_header_file( FILE *file, struct ptf *cfg, device_desc *devices, clock_desc *clocks)
{
  struct ptf *p;
  struct ptf_item pi;

  struct ptf aclass = { section, "CLASS", 0, 0, 0 };
  struct ptf_item matchaclass = { 1, &aclass };

  struct ptf bspsect = { section, "BSPHEADER", 0, 0, 0 };
  struct ptf leadtext = { item, "LEADTEXT", 0, 0, 0 };
  struct ptf_item matchleadtext = { 2, &bspsect, &leadtext };

  struct ptf epilog = { item, "EPILOG", 0, 0, 0 };
  struct ptf_item matchepilog = { 2, &bspsect, &epilog };

  out_desc dinfo;

  dinfo.file    = file;
  dinfo.clocks  = clocks;
  dinfo.devices = devices;

  ptf_match(cfg, &matchleadtext, fwrite_value, file);

  if(clocks)
  {
    clock_desc *cs;
    for(cs = clocks; cs; cs = cs->next)
    {
      fprintf(file, "#define %s_FREQ %luu\n", cs->cfgname, cs->freq);
    };
  };

  if(devices)
  {
    for(dinfo.dev = devices; dinfo.dev; dinfo.dev=dinfo.dev->next)
    {
      /* fprintf(file, "\n#define SOPC_HAS_%s 1\n", dinfo.dev->cfgname); */

      p = ptf_find(dinfo.dev->ptf, &pi, item, "class", 0);
      if(p)
      {
        aclass.value = p->value;
        ptf_match(cfg, &matchaclass, fwrite_device_header, &dinfo);
      };
    };
  };

  ptf_match(cfg, &matchepilog, fwrite_value, file);
}


