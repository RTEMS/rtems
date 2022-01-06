/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This source file contains the implementation of this BSP's startup
 *   hooks.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
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

#include <bsp.h>
#include <bsp/start.h>

#ifdef RTEMS_SMP
#include <rtems/score/aarch64-system-registers.h>
#include <rtems/score/smpimpl.h>

#include <bsp/irq-generic.h>
#endif

BSP_START_TEXT_SECTION void bsp_start_hook_0( void )
{
  /* do nothing */
}

BSP_START_TEXT_SECTION void bsp_start_hook_1( void )
{
#ifdef RTEMS_SMP
  uint32_t cpu_index_self;

  cpu_index_self = _SMP_Get_current_processor();

  if ( cpu_index_self != 0 ) {
    if (
      cpu_index_self >= rtems_configuration_get_maximum_processors()
      || !_SMP_Should_start_processor( cpu_index_self )
    ) {
      while ( true ) {
        _AARCH64_Wait_for_event();
      }
    }

    /* Change the VBAR from the start to the normal vector table */
    AArch64_start_set_vector_base();

    zynqmp_setup_secondary_cpu_mmu_and_cache();
    arm_gic_irq_initialize_secondary_cpu();

    bsp_interrupt_vector_enable( ARM_GIC_IRQ_SGI_0 );
    _SMP_Start_multitasking_on_secondary_processor(
      _Per_CPU_Get_by_index( cpu_index_self )
    );
    /* Unreached */
  }
#endif /* RTEMS_SMP */

  AArch64_start_set_vector_base();
  bsp_start_copy_sections();
  zynqmp_setup_mmu_and_cache();
  bsp_start_clear_bss();
}
