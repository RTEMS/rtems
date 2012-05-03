/**
 * @file
 *
 * @ingroup arm
 *
 * @brief ARM cache defines and implementation.
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBCPU_ARM_CACHE__H
#define LIBCPU_ARM_CACHE__H

#ifdef __ARM_ARCH_5TEJ__
  #include <libcpu/arm-cp15.h>

  #define CPU_DATA_CACHE_ALIGNMENT 32
  #define CPU_INSTRUCTION_CACHE_ALIGNMENT 32

  static inline void _CPU_cache_flush_1_data_line(const void *d_addr)
  {
    arm_cp15_data_cache_clean_line(d_addr);
  }

  static inline void _CPU_cache_invalidate_1_data_line(const void *d_addr)
  {
    arm_cp15_data_cache_invalidate_line(d_addr);
  }

  static inline void _CPU_cache_freeze_data(void)
  {
    /* TODO */
  }

  static inline void _CPU_cache_unfreeze_data(void)
  {
    /* TODO */
  }

  static inline void _CPU_cache_invalidate_1_instruction_line(const void *d_addr)
  {
    arm_cp15_instruction_cache_invalidate_line(d_addr);
  }

  static inline void _CPU_cache_freeze_instruction(void)
  {
    /* TODO */
  }

  static inline void _CPU_cache_unfreeze_instruction(void)
  {
    /* TODO */
  }

  static inline void _CPU_cache_flush_entire_data(void)
  {
    arm_cp15_data_cache_test_and_clean();
  }

  static inline void _CPU_cache_invalidate_entire_data(void)
  {
    arm_cp15_data_cache_invalidate();
  }

  static inline void _CPU_cache_enable_data(void)
  {
    rtems_interrupt_level level;
    uint32_t ctrl;

    rtems_interrupt_disable(level);
    ctrl = arm_cp15_get_control();
    ctrl |= ARM_CP15_CTRL_C;
    arm_cp15_set_control(ctrl);
    rtems_interrupt_enable(level);
  }

  static inline void _CPU_cache_disable_data(void)
  {
    rtems_interrupt_level level;
    uint32_t ctrl;

    rtems_interrupt_disable(level);
    arm_cp15_data_cache_test_and_clean_and_invalidate();
    ctrl = arm_cp15_get_control();
    ctrl &= ~ARM_CP15_CTRL_C;
    arm_cp15_set_control(ctrl);
    rtems_interrupt_enable(level);
  }

  static inline void _CPU_cache_invalidate_entire_instruction(void)
  {
    arm_cp15_instruction_cache_invalidate();
  }

  static inline void _CPU_cache_enable_instruction(void)
  {
    rtems_interrupt_level level;
    uint32_t ctrl;

    rtems_interrupt_disable(level);
    ctrl = arm_cp15_get_control();
    ctrl |= ARM_CP15_CTRL_I;
    arm_cp15_set_control(ctrl);
    rtems_interrupt_enable(level);
  }

  static inline void _CPU_cache_disable_instruction(void)
  {
    rtems_interrupt_level level;
    uint32_t ctrl;

    rtems_interrupt_disable(level);
    ctrl = arm_cp15_get_control();
    ctrl &= ~ARM_CP15_CTRL_I;
    arm_cp15_set_control(ctrl);
    rtems_interrupt_enable(level);
  }
#endif

#endif /* LIBCPU_ARM_CACHE__H */
