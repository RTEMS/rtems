/**
 * @file
 *
 * @ingroup arm_start
 *
 * @brief Rasberry Pi startup code.
 */

/*
 * Copyright (c) 2013. Hesham AL-Matary
 * Copyright (c) 2013 by Alan Cudmore
 * based on work by:
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

#include <bspopts.h>
#include <bsp/start.h>
#include <bsp/raspberrypi.h>
#include <libcpu/arm-cp15.h>
#include <bsp.h>
#include <bsp/linker-symbols.h>
#include <bsp/arm-cp15-start.h>

#ifdef RTEMS_SMP
#include <rtems/score/smp.h>
#endif

void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
  uint32_t sctlr_val;
#ifdef RTEMS_SMP
  uint32_t cpu_index_self = _SMP_Get_current_processor();
#endif /* RTEMS_SMP */

  sctlr_val = arm_cp15_get_control();

  /*
   * Current U-boot loader seems to start kernel image
   * with I and D caches on and MMU enabled.
   * If RTEMS application image finds that cache is on
   * during startup then disable caches.
   */
  if (sctlr_val & (ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M)) {
    if (sctlr_val & (ARM_CP15_CTRL_C | ARM_CP15_CTRL_M)) {
      /*
       * If the data cache is on then ensure that it is clean
       * before switching off to be extra carefull.
       */
#ifdef RTEMS_SMP
      if (cpu_index_self != 0) {
        arm_cp15_data_cache_clean_level(0);
        arm_cp15_cache_invalidate_level(0, 0);
      } else
#endif /* RTEMS_SMP */
      {
        rtems_cache_flush_entire_data();
        rtems_cache_invalidate_entire_data();
      }
    }
    arm_cp15_flush_prefetch_buffer();
    sctlr_val &= ~(ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M | ARM_CP15_CTRL_A);
    arm_cp15_set_control(sctlr_val);
  }
#ifdef RTEMS_SMP
  if (cpu_index_self != 0) {
    arm_cp15_cache_invalidate_level(0, 0);
  } else
#endif /* RTEMS_SMP */
  {
    rtems_cache_invalidate_entire_data();
  }
  rtems_cache_invalidate_entire_instruction();
  arm_cp15_branch_predictor_invalidate_all();
  arm_cp15_tlb_invalidate();
  arm_cp15_flush_prefetch_buffer();

  /* Clear Translation Table Base Control Register */
  arm_cp15_set_translation_table_base_control_register(0);

  /* Clear Secure or Non-secure Vector Base Address Register */
  arm_cp15_set_vector_base_address(bsp_vector_table_begin);

#ifdef RTEMS_SMP
  if (cpu_index_self == 0) {
    rpi_ipi_initialize();
  } else {
    rpi_start_rtems_on_secondary_processor();
  }
#endif
}

BSP_START_TEXT_SECTION static void bsp_memory_management_initialize(void)
{
  uint32_t ctrl = arm_cp15_get_control();

  ctrl |= ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_S | ARM_CP15_CTRL_XP;

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &arm_cp15_start_mmu_config_table[0],
    arm_cp15_start_mmu_config_table_size
  );
}

void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  bsp_memory_management_initialize();
  bsp_start_clear_bss();

  rpi_video_init();
}
