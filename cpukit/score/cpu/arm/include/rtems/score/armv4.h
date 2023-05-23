/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This header file provides interfaces of the ARMv4 architecture
 *   support.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#ifndef RTEMS_SCORE_ARMV4_H
#define RTEMS_SCORE_ARMV4_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSScoreCPUARM
 *
 * @{
 */

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

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SCORE_ARMV4_H */
