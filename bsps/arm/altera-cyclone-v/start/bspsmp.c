/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
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

#include <bsp/start.h>

#include <bsp/socal/alt_rstmgr.h>
#include <bsp/socal/alt_sysmgr.h>
#include <bsp/socal/hps.h>
#include <bsp/socal/socal.h>

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  bool started;

  if (cpu_index == 1) {
    alt_write_word(
      ALT_SYSMGR_ROMCODE_ADDR + ALT_SYSMGR_ROMCODE_CPU1STARTADDR_OFST,
      ALT_SYSMGR_ROMCODE_CPU1STARTADDR_VALUE_SET((uint32_t) _start)
    );

    alt_clrbits_word(
      ALT_RSTMGR_MPUMODRST_ADDR,
      ALT_RSTMGR_MPUMODRST_CPU1_SET_MSK
    );

    /*
     * Wait for secondary processor to complete its basic initialization so
     * that we can enable the unified L2 cache.
     */
    started = _Per_CPU_State_wait_for_non_initial_state(cpu_index, 0);
  } else {
    started = false;
  }

  return started;
}
