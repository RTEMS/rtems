/**
 *  @file
 *
 *  @ingroup RTEMSBSPsARMShared
 *
 *  @brief A9MPCORE_START Support
 */

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

#ifndef LIBBSP_ARM_SHARED_ARM_A9MPCORE_START_H
#define LIBBSP_ARM_SHARED_ARM_A9MPCORE_START_H

#include <rtems/score/smpimpl.h>

#include <libcpu/arm-cp15.h>

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-a9mpcore-regs.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/arm-errata.h>
#include <dev/irq/arm-gic-irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BSP_START_TEXT_SECTION static inline void arm_a9mpcore_start_scu_invalidate(
  volatile a9mpcore_scu *scu,
  uint32_t cpu_id,
  uint32_t ways
)
{
  scu->invss = (ways & 0xf) << ((cpu_id & 0x3) * 4);
}

BSP_START_TEXT_SECTION static inline void
arm_a9mpcore_start_errata_764369_handler(volatile a9mpcore_scu *scu)
{
  if (arm_errata_is_applicable_processor_errata_764369()) {
    scu->diagn_ctrl |= A9MPCORE_SCU_DIAGN_CTRL_MIGRATORY_BIT_DISABLE;
  }
}

BSP_START_TEXT_SECTION static inline void
arm_a9mpcore_start_scu_enable(volatile a9mpcore_scu *scu)
{
  scu->ctrl |= A9MPCORE_SCU_CTRL_SCU_EN;
  arm_a9mpcore_start_errata_764369_handler(scu);
}

#ifdef RTEMS_SMP
BSP_START_TEXT_SECTION static inline void
arm_a9mpcore_start_on_secondary_processor(void)
{
  uint32_t ctrl;

  arm_gic_irq_initialize_secondary_cpu();

  /* Change the VBAR from the start to the normal vector table */
  arm_cp15_set_vector_base_address(bsp_vector_table_begin);

  ctrl = arm_cp15_start_setup_mmu_and_cache(
    0,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_set_domain_access_control(
    ARM_CP15_DAC_DOMAIN(ARM_MMU_DEFAULT_CLIENT_DOMAIN, ARM_CP15_DAC_CLIENT)
  );

  /* FIXME: Sharing the translation table between processors is brittle */
  arm_cp15_set_translation_table_base(
    (uint32_t *) bsp_translation_table_base
  );

  ctrl |= ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M;
  arm_cp15_set_control(ctrl);

  _SMP_Start_multitasking_on_secondary_processor(_Per_CPU_Get());
}

BSP_START_TEXT_SECTION static inline void
arm_a9mpcore_start_enable_smp_in_auxiliary_control(void)
{
  /*
   * Enable cache coherency support and cache/MMU maintenance broadcasts for
   * this processor.
   */
  uint32_t actlr = arm_cp15_get_auxiliary_control();
  actlr |= ARM_CORTEX_A9_ACTL_SMP | ARM_CORTEX_A9_ACTL_FW;
  arm_cp15_set_auxiliary_control(actlr);
}

BSP_START_TEXT_SECTION static inline void
arm_a9mpcore_start_errata_794072_handler(void)
{
  uint32_t diag;

  /*
   * Workaround for Errata 794072: A short loop including a DMB instruction
   * might cause a denial of service on another which executes a CP15 broadcast
   * operation.
   */
  diag = arm_cp15_get_diagnostic_control();
  diag |= 1U << 4;
  arm_cp15_set_diagnostic_control(diag);
}

BSP_START_TEXT_SECTION static inline void
arm_a9mpcore_start_errata_845369_handler(void)
{
  uint32_t diag;

  /*
   * Workaround for Errata 845369: Under Very Rare Timing Circumstances
   * Transition into Streaming Mode Might Create Data Corruption.
   */
  diag = arm_cp15_get_diagnostic_control();
  diag |= 1U << 22;
  arm_cp15_set_diagnostic_control(diag);
}
#endif

BSP_START_TEXT_SECTION static inline void arm_a9mpcore_start_hook_0(void)
{
  volatile a9mpcore_scu *scu =
    (volatile a9mpcore_scu *) BSP_ARM_A9MPCORE_SCU_BASE;
  uint32_t cpu_id = arm_cortex_a9_get_multiprocessor_cpu_id();

  if (cpu_id == 0) {
    arm_a9mpcore_start_scu_enable(scu);
  }

#ifdef RTEMS_SMP
  arm_a9mpcore_start_errata_794072_handler();
  arm_a9mpcore_start_errata_845369_handler();
  arm_a9mpcore_start_enable_smp_in_auxiliary_control();
#endif

  arm_a9mpcore_start_scu_invalidate(scu, cpu_id, 0xf);

#ifdef RTEMS_SMP
  if (cpu_id != 0) {
    arm_a9mpcore_start_on_secondary_processor();
  }
#endif
}

BSP_START_TEXT_SECTION static inline void arm_a9mpcore_start_global_timer(void)
{
  volatile a9mpcore_gt *gt = (volatile a9mpcore_gt *) BSP_ARM_A9MPCORE_GT_BASE;

  gt->ctrl = 0;
  gt->cntrlower = 0;
  gt->cntrupper = 0;
  gt->ctrl = A9MPCORE_GT_CTRL_TMR_EN;
}

BSP_START_TEXT_SECTION static inline void arm_a9mpcore_start_hook_1(void)
{
  arm_a9mpcore_start_global_timer();
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_A9MPCORE_START_H */
