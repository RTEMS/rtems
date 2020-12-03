/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMFVP
 *
 * @brief This source file contains the implementation of bsp_start_hook_0()
 *   and bsp_start_hook_1().
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <bsp/start.h>

#include <rtems/score/aarch32-pmsa.h>
#include <rtems/score/aarch32-system-registers.h>

#ifdef RTEMS_SMP
#include <rtems.h>
#include <rtems/score/smpimpl.h>

#include <bsp/irq-generic.h>

#include <dev/irq/arm-gic-irq.h>
#endif

void bsp_start_hook_0( void )
{
#ifdef RTEMS_SMP
  uint32_t cpu_index_self;

  cpu_index_self = _SMP_Get_current_processor();

  if ( cpu_index_self != 0 ) {
    /*
     * The FVP jumps to the entry point of the ELF file on all processors.
     * Prevent the fatal errors SMP_FATAL_MULTITASKING_START_ON_INVALID_PROCESSOR
     * and SMP_FATAL_MULTITASKING_START_ON_UNASSIGNED_PROCESSOR this way.
     */
    if (
      cpu_index_self >= rtems_configuration_get_maximum_processors()
        || !_SMP_Should_start_processor( cpu_index_self )
    ) {
      while ( true ) {
        _ARM_Wait_for_event();
      }
    }

    _AArch32_PMSA_Initialize_default();
    arm_gic_irq_initialize_secondary_cpu();

    /* Change the VBAR from the start to the normal vector table */
    _AArch32_Write_vbar( bsp_vector_table_begin );

    bsp_interrupt_vector_enable( ARM_GIC_IRQ_SGI_0 );
    _SMP_Start_multitasking_on_secondary_processor(
      _Per_CPU_Get_by_index( cpu_index_self )
    );
  }
#endif
}

void bsp_start_hook_1( void )
{
  bsp_start_copy_sections();
  _AArch32_PMSA_Initialize_default();
  bsp_start_clear_bss();
}
