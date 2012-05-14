/*  Blackfin MMU Support
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <rtems.h>

#include <libcpu/memoryRegs.h>
#include "mmu.h"

/* NOTE: see notes in mmu.h */

void bfin_mmu_init(bfin_mmu_config_t *config) {
  intptr_t addr;
  intptr_t data;
  int i;

  addr = (intptr_t) ICPLB_ADDR0;
  data = (intptr_t) ICPLB_DATA0;
  for (i = 0; i < sizeof(config->instruction) / sizeof(config->instruction[0]);
       i++) {
    *(uint32_t volatile *) addr = (uint32_t) config->instruction[i].address;
    addr += ICPLB_ADDR_PITCH;
    *(uint32_t volatile *) data = config->instruction[i].flags;
    data += ICPLB_DATA_PITCH;
  }
  *(uint32_t volatile *) IMEM_CONTROL |= IMEM_CONTROL_ENICPLB;
  addr = (intptr_t) DCPLB_ADDR0;
  data = (intptr_t) DCPLB_DATA0;
  for (i = 0; i < sizeof(config->data) / sizeof(config->data[0]); i++) {
    *(uint32_t volatile *) addr = (uint32_t) config->data[i].address;
    addr += DCPLB_ADDR_PITCH;
    *(uint32_t volatile *) data = config->data[i].flags;
    data += DCPLB_DATA_PITCH;
  }
  *(uint32_t volatile *) DMEM_CONTROL |= DMEM_CONTROL_ENDCPLB;
}

