/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/smpimpl.h>

#include <arm/freescale/imx/imx_srcreg.h>
#include <arm/freescale/imx/imx_gpcreg.h>

#include <bsp/start.h>

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  bool started;

  if (cpu_index == 1) {
    volatile imx_src *src = (volatile imx_src *) 0x30390000;
    volatile imx_gpc *gpc = (volatile imx_gpc *) 0x303a0000;

    src->gpr3 = (uint32_t) _start;
    gpc->pgc_a7core0_ctrl |= IMX_GPC_PGC_CTRL_PCR;
    gpc->cpu_pgc_sw_pup_req |= IMX_GPC_CPU_PGC_CORE1_A7;

    while ((gpc->cpu_pgc_pup_status1 & IMX_GPC_CPU_PGC_CORE1_A7) != 0) {
      /* Wait */
    }

    gpc->pgc_a7core0_ctrl &= ~IMX_GPC_PGC_CTRL_PCR;
    src->a7rcr1 |= IMX_SRC_A7RCR1_A7_CORE1_ENABLE;

    started = true;
  } else {
    started = false;
  }

  return started;
}
