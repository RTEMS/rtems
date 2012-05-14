/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include "ptf.h"
#include "clocks.h"

#include <stdlib.h>

void add_clock_spec(struct ptf_item *pi, void *arg)
{
  clock_desc **clocks = arg;
  clock_desc *new_clock;
  unsigned long freq;

  new_clock = (clock_desc*)malloc(sizeof(clock_desc));
  if(new_clock == NULL) return;

  new_clock->freq = strtoul(pi->item[pi->level]->value, 0, 0);

  new_clock->cfgname = NULL;
  new_clock->name = pi->item[pi->level-1]->value;
  new_clock->next = *clocks;


  *clocks = new_clock;
}

void set_clock_cfgname(struct ptf_item *pi, void *arg)
{
  clock_desc *clock = arg;
  clock->cfgname = pi->item[pi->level]->name;
}

clock_desc *find_clocks( struct ptf *sopc, struct ptf *cfg )
{
    clock_desc *clocks, *reverse;

    struct ptf system        = { section, "SYSTEM", 0, 0, 0 };
    struct ptf wizargs       = { section, "WIZARD_SCRIPT_ARGUMENTS", 0, 0, 0 };
    struct ptf all           = { section, "CLOCKS", 0, 0, 0 };
    struct ptf clock         = { section, "CLOCK", 0, 0, 0 };
    struct ptf freq          = { item,    "frequency", 0, 0, 0 };
    struct ptf_item clk_spec = { 5, &system, &wizargs, &all, &clock, &freq };

    struct ptf named     = { item, 0, 0, 0, 0 };
    struct ptf_item clk_cfg = { 2, &all, &named };

    clocks = NULL;
    ptf_match(sopc, &clk_spec, add_clock_spec, &clocks);

    /* Reverse the linked list and look for configured names */

    reverse = NULL;
    while(clocks)
    {
      clock_desc *tmp = clocks;
      clocks = clocks->next;
      tmp->next = reverse;
      reverse = tmp;

      named.value = tmp->name;
      ptf_match(cfg, &clk_cfg, set_clock_cfgname, tmp);
      if(tmp->cfgname == NULL) tmp->cfgname = ptf_defused_name(tmp->name);
    };

    return reverse;
}



