/**
 * @file tms570-pom.c
 *
 * @ingroup tms570
 *
 * @brief TMS570 Parameter Overlay Module functions definitions.
 */

 /*
 * Copyright (c) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdint.h>
#include <bsp/tms570-pom.h>
#include <bsp/linker-symbols.h>
#include <bsp.h>

/**
 * @brief remaps vector table
 *
 * transfer the rtems start vector table to address 0x0
 *
 * @retval Void
 */
void tms570_pom_remap(void)
{
  int i;
  uint32_t vec_overlay_start = 0x08000000;
  uint32_t temp = 0;

  memcpy((void*)vec_overlay_start, bsp_start_vector_table_begin, 64);

  TMS570_POM.GLBCTRL = 0;

  for ( i = 0; i < TMS570_POM_REGIONS; ++i ) {
    TMS570_POM.REG[i].REGSIZE = TMS570_POM_REGSIZE_SIZE(TMS570_POM_REGSIZE_DISABLED);
  }

  TMS570_POM.REG[0].PROGSTART = TMS570_POM_PROGSTART_STARTADDRESS(0);
  TMS570_POM.REG[0].OVLSTART = TMS570_POM_OVLSTART_STARTADDRESS(vec_overlay_start);
  TMS570_POM.REG[0].REGSIZE = TMS570_POM_REGSIZE_SIZE(TMS570_POM_REGSIZE_64B);
  TMS570_POM.GLBCTRL = TMS570_POM_GLBCTRL_ON_OFF(0xa) |
                       TMS570_POM_GLBCTRL_ETO(0xa) |
                       (TMS570_POM_GLBCTRL_OTADDR(~0) & vec_overlay_start);
}
