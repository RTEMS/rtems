/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2013, 2014 embedded brains GmbH
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

#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/arm-a9mpcore-start.h>

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
  uint32_t sctlr_val;

  sctlr_val = arm_cp15_get_control();

  /*
   * Current U-boot loader seems to start kernel image
   * with I and D caches on and MMU enabled.
   * If RTEMS application image finds that cache is on
   * during startup then disable caches.
   */
  if ( sctlr_val & (ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M ) ) {
    if ( sctlr_val & (ARM_CP15_CTRL_C | ARM_CP15_CTRL_M ) ) {
      /*
       * If the data cache is on then ensure that it is clean
       * before switching off to be extra carefull.
       */
      arm_cp15_data_cache_clean_all_levels();
    }
    arm_cp15_flush_prefetch_buffer();
    sctlr_val &= ~ ( ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M | ARM_CP15_CTRL_A );
    arm_cp15_set_control( sctlr_val );
  }
  arm_cp15_instruction_cache_invalidate();
  /*
   * The care should be taken there that no shared levels
   * are invalidated by secondary CPUs in SMP case.
   * It is not problem on Zynq because level of coherency
   * is L1 only and higher level is not maintained and seen
   * by CP15. So no special care to limit levels on the secondary
   * are required there.
   */
  arm_cp15_data_cache_invalidate_all_levels();
  arm_cp15_branch_predictor_invalidate_all();
  arm_cp15_tlb_invalidate();
  arm_cp15_flush_prefetch_buffer();
  arm_a9mpcore_start_hook_0();
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  arm_a9mpcore_start_hook_1();
  bsp_start_copy_sections();
  zynq_setup_mmu_and_cache();

#if !defined(RTEMS_SMP) \
  && (defined(BSP_DATA_CACHE_ENABLED) \
    || defined(BSP_INSTRUCTION_CACHE_ENABLED))
  /* Enable unified L2 cache */
  rtems_cache_enable_data();
#endif

  bsp_start_clear_bss();
}
