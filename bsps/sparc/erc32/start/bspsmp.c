/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 embedded brains GmbH
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
#include <rtems/score/assert.h>
#include <rtems/sysinit.h>
#include <bsp.h>

#define IPI_VECTOR SPARC_SYNCHRONOUS_TRAP( 0x91 )

uint32_t _CPU_SMP_Get_current_processor( void )
{
  return 0;
}

uint32_t _CPU_SMP_Initialize( void )
{
  return 1;
}

bool _CPU_SMP_Start_processor( uint32_t cpu_index )
{
  (void) cpu_index;
  return true;
}

void _CPU_SMP_Finalize_initialization( uint32_t cpu_count )
{
  _Assert( cpu_count == 1 );
  (void) cpu_count;
}

void _CPU_SMP_Prepare_start_multitasking( void )
{
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
  _Assert( target_processor_index == 0 );
  (void) target_processor_index;
  __asm__ volatile( "ta 0x11; nop " );
}

static rtems_isr bsp_inter_processor_interrupt(
  rtems_vector_number vector
)
{
  _SMP_Inter_processor_interrupt_handler( _Per_CPU_Get() );
}

static void erc32_install_inter_processor_interrupt( void )
{
  set_vector( bsp_inter_processor_interrupt, IPI_VECTOR, 1 );
}

RTEMS_SYSINIT_ITEM(
  erc32_install_inter_processor_interrupt,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST
);
