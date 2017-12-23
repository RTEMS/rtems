/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef RTEMS_SCORE_ARMV4_H
#define RTEMS_SCORE_ARMV4_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef ARM_MULTILIB_ARCH_V4

void bsp_interrupt_dispatch( void );

void _ARMV4_Exception_interrupt( void );

typedef void ARMV4_Exception_abort_handler( CPU_Exception_frame *frame );

void _ARMV4_Exception_data_abort_set_handler(
  ARMV4_Exception_abort_handler handler
);

void _ARMV4_Exception_data_abort( void );

void _ARMV4_Exception_prefetch_abort_set_handler(
  ARMV4_Exception_abort_handler handler
);

void _ARMV4_Exception_prefetch_abort( void );

void _ARMV4_Exception_undef_default( void );

void _ARMV4_Exception_swi_default( void );

void _ARMV4_Exception_data_abort_default( void );

void _ARMV4_Exception_pref_abort_default( void );

void _ARMV4_Exception_reserved_default( void );

void _ARMV4_Exception_irq_default( void );

void _ARMV4_Exception_fiq_default( void );

static inline uint32_t _ARMV4_Status_irq_enable( void )
{
  uint32_t arm_switch_reg;
  uint32_t psr;

  RTEMS_COMPILER_MEMORY_BARRIER();

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[psr], cpsr\n"
    "bic %[arm_switch_reg], %[psr], #0x80\n"
    "msr cpsr, %[arm_switch_reg]\n"
    ARM_SWITCH_BACK
    : [arm_switch_reg] "=&r" (arm_switch_reg), [psr] "=&r" (psr)
  );

  return psr;
}

static inline void _ARMV4_Status_restore( uint32_t psr )
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "msr cpsr, %[psr]\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [psr] "r" (psr)
  );

  RTEMS_COMPILER_MEMORY_BARRIER();
}

#endif /* ARM_MULTILIB_ARCH_V4 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SCORE_ARMV4_H */
