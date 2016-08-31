/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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
