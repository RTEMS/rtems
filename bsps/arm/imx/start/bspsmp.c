/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
