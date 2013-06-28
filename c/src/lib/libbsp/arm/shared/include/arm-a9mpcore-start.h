/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_A9MPCORE_START_H
#define LIBBSP_ARM_SHARED_ARM_A9MPCORE_START_H

#include <rtems/bspsmp.h>

#include <libcpu/arm-cp15.h>

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-a9mpcore-regs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BSP_START_TEXT_SECTION static inline uint32_t
arm_cp15_get_control(void);

BSP_START_TEXT_SECTION static inline void
arm_cp15_set_control(uint32_t val);

BSP_START_TEXT_SECTION static inline uint32_t
arm_cp15_get_auxiliary_control(void);

BSP_START_TEXT_SECTION static inline void
arm_cp15_set_auxiliary_control(uint32_t val);

BSP_START_TEXT_SECTION static inline void
arm_cp15_set_vector_base_address(void *base);

BSP_START_TEXT_SECTION static inline arm_a9mpcore_start_set_vector_base(void)
{
  /*
   * Do not use bsp_vector_table_begin == 0, since this will get optimized away.
  */
  if (bsp_vector_table_end != bsp_vector_table_size) {
    uint32_t ctrl;

    /*
     * For now we assume that every Cortex-A9 MPCore has the Security Extensions.
     * Later it might be necessary to evaluate the ID_PFR1 register.
     */
    arm_cp15_set_vector_base_address(bsp_vector_table_begin);

    ctrl = arm_cp15_get_control();
    ctrl &= ~ARM_CP15_CTRL_V;
    arm_cp15_set_control(ctrl);
  }
}

BSP_START_TEXT_SECTION static inline arm_a9mpcore_start_hook_0(void)
{
#ifdef RTEMS_SMP
  volatile a9mpcore_scu *scu = (volatile a9mpcore_scu *) BSP_ARM_A9MPCORE_SCU_BASE;
  uint32_t cpu_id;
  uint32_t actlr;

  /* Enable Snoop Control Unit (SCU) */
  scu->ctrl |= A9MPCORE_SCU_CTRL_SCU_EN;

  /* Enable cache coherency support for this processor */
  actlr = arm_cp15_get_auxiliary_control();
  actlr |= ARM_CORTEX_A9_ACTL_SMP;
  arm_cp15_set_auxiliary_control(actlr);

  cpu_id = arm_cortex_a9_get_multiprocessor_cpu_id();
  if (cpu_id != 0) {
    arm_a9mpcore_start_set_vector_base();

    if (cpu_id < rtems_configuration_get_maximum_processors()) {
      uint32_t ctrl;

      arm_gic_irq_initialize_secondary_cpu();

      ctrl = arm_cp15_start_setup_mmu_and_cache(
        0,
        ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
      );

      arm_cp15_set_domain_access_control(
        ARM_CP15_DAC_DOMAIN(ARM_MMU_DEFAULT_CLIENT_DOMAIN, ARM_CP15_DAC_CLIENT)
      );

      /* FIXME: Sharing the translation table between processors is brittle */
      arm_cp15_set_translation_table_base((uint32_t *) bsp_translation_table_base);

      ctrl |= ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M;
      arm_cp15_set_control(ctrl);

      rtems_smp_secondary_cpu_initialize();
    } else {
      /* FIXME: Shutdown processor */
      while (1) {
        __asm__ volatile ("wfi");
      }
    }
  }
#endif
}

BSP_START_TEXT_SECTION static inline arm_a9mpcore_start_hook_1(void)
{
  arm_a9mpcore_start_set_vector_base();
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_A9MPCORE_START_H */
