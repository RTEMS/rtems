/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Shared
 *
 * @brief SMP startup and interop code.
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <bsp/irq.h>

static void bsp_inter_processor_interrupt( void *arg )
{
  (void) arg;

  _SMP_Inter_processor_interrupt_handler( _Per_CPU_Get() );
}

uint32_t _CPU_SMP_Initialize( void )
{
  return arm_gic_irq_processor_count();
}

static rtems_interrupt_entry aarch64_ipi_entry;

void _CPU_SMP_Finalize_initialization( uint32_t cpu_count )
{
  (void) cpu_count;

  rtems_status_code sc;

  rtems_interrupt_entry_initialize(
    &aarch64_ipi_entry,
    bsp_inter_processor_interrupt,
    NULL,
    "IPI"
  );
  sc = rtems_interrupt_entry_install(
    ARM_GIC_IRQ_SGI_0,
    RTEMS_INTERRUPT_UNIQUE,
    &aarch64_ipi_entry
  );
  _Assert_Unused_variable_equals( sc, RTEMS_SUCCESSFUL );
}

void _CPU_SMP_Prepare_start_multitasking( void )
{
  /* Do nothing */
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
  arm_gic_trigger_sgi(
    ARM_GIC_IRQ_SGI_0,
    1U << target_processor_index
  );
}

uint32_t _CPU_SMP_Get_current_processor( void )
{
  return _Per_CPU_Get_index( _CPU_Get_current_per_CPU_control() );
}
