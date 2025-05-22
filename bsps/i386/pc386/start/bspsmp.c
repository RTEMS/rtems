/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 Jan Sommer, Deutsches Zentrum fur Luft- und Raumfahrt e. V. (DLR)
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

#include <bsp/apic.h>
#include <bsp/smp-imps.h>
#include <bsp.h>
#include <rtems.h>

void _CPU_SMP_Prepare_start_multitasking( void )
{
  /* Do nothing */
}

bool _CPU_SMP_Start_processor( uint32_t cpu_index )
{
  (void) cpu_index;

  return true;
}


uint32_t _CPU_SMP_Get_current_processor( void )
{
  return imps_apic_cpu_map[APIC_ID(IMPS_LAPIC_READ(LAPIC_ID))];
}

uint32_t _CPU_SMP_Initialize( void )
{
  /* XXX need to deal with finding too many cores */

  return (uint32_t) imps_probe();
}

void _CPU_SMP_Finalize_initialization( uint32_t cpu_count )
{
  if ( cpu_count > 1 )
    ipi_install_irq();
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
  send_ipi( target_processor_index, 0x30 );
}
