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
#include "memory.h"

memory_desc *find_memory(device_desc *devices)
{
  struct ptf *p;
  struct ptf_item pi;
  memory_desc *tmd, *memory;

  /********************************************************/
  /* Check which of the devices are memory, sort by size */

  if(devices)
  {
    struct ptf *p, *s;
    struct ptf_item pi;
    device_desc *dd;

    memory = NULL;

    for(dd = devices; dd; dd=dd->next)
    {
      p = ptf_find(dd->ptf->sub, &pi, item, "Is_Memory_Device", "1");
      if(p != NULL && pi.level>0)
      {
        s = pi.item[pi.level-1];
        p = ptf_find(s, &pi, item, "Base_Address", 0);
      };

      if(p != NULL)
      {
        tmd = (memory_desc*)malloc(sizeof(memory_desc));

        if(tmd != NULL)
        {
          tmd->base = strtoul(p->value, 0, 0);

          p = ptf_find(s, &pi, item, "Address_Span", 0);
          if(p != 0)
          {
            tmd->size = strtoul(p->value, 0, 0);
          }
          else
          {
            tmd->size = 0;
            p = ptf_find(s, &pi, item, "Address_Width", 0);
            if(p) tmd->size = 1 << strtoul(p->value, 0, 0);
            p = ptf_find(s, &pi, item, "Data_Width", 0);
            if(p) tmd->size *= (strtoul(p->value, 0, 0) >> 3);
          };

          if(tmd->size == 0)
          {
            free(tmd);
          }
          else
          {
            tmd->dev = dd;

            if(memory == NULL)
            {
              tmd->next = NULL;
              memory = tmd;
            }
            else
            {
              if(tmd->size > memory->size)
              {
                tmd->next = memory;
                memory = tmd;
              }
              else
              {
                memory_desc *uplink = memory;
                while(uplink->next != NULL && uplink->next->size > tmd->size) uplink=uplink->next;
                tmd->next = uplink->next;
                uplink->next = tmd;
              };
            };
          };
        };
      };
    };
  };

  return memory;
}


