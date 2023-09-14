/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreISR
 *
 * @brief This source file contains the definition of ::_ISR_Vector_table and
 *   the implementation of _ISR_Handler_initialization().
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/score/isr.h>
#include <rtems/score/address.h>
#include <rtems/score/percpu.h>
#include <rtems/config.h>

const char * const volatile _ISR_Stack_size_object = _ISR_Stack_size;

void _ISR_Handler_initialization( void )
{
  uint32_t  cpu_max;
  uint32_t  cpu_index;
  size_t    stack_size;
  char     *stack_low;

  stack_size = rtems_configuration_get_interrupt_stack_size();
  cpu_max = rtems_configuration_get_maximum_processors();
  stack_low = _ISR_Stack_area_begin;

  for ( cpu_index = 0 ; cpu_index < cpu_max; ++cpu_index ) {
    Per_CPU_Control *cpu;
    char            *stack_high;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    stack_high = _Addresses_Add_offset( stack_low, stack_size );

    cpu->interrupt_stack_low = stack_low;
    cpu->interrupt_stack_high = stack_high;

    /*
     * Interrupt stack might have to be aligned and/or setup in a specific
     * way.  Do not use the local low or high variables here since
     * _CPU_Interrupt_stack_setup() is a nasty macro that might want to play
     * with the real memory locations.
     */
#if defined(_CPU_Interrupt_stack_setup)
    _CPU_Interrupt_stack_setup(
      cpu->interrupt_stack_low,
      cpu->interrupt_stack_high
    );
#endif

    stack_low = stack_high;
  }
}
