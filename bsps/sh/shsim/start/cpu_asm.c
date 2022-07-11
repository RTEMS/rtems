/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Support for SuperH Simulator in GDB
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/sh.h>

unsigned long  *_old_stack_ptr;

register unsigned long  *stack_ptr __asm__ ("r15");

void __ISR_Handler(uint32_t vector);

/*
 *  This routine provides the RTEMS interrupt management.
 */
void __ISR_Handler( uint32_t   vector)
{
  ISR_Level level;

  _ISR_Local_disable( level );

  _Thread_Dispatch_disable();

  if ( _ISR_Nest_level == 0 )
    {
      /* Install irq stack */
      _old_stack_ptr = stack_ptr;
      stack_ptr = _CPU_Interrupt_stack_high;
    }

  _ISR_Nest_level++;

  _ISR_Local_enable( level );

  /* call isp */
  if ( _ISR_Vector_table[ vector])
    (*_ISR_Vector_table[ vector ])( vector );

  _ISR_Local_disable( level );

  _Thread_Dispatch_unnest( _Per_CPU_Get() );

  _ISR_Nest_level--;

  if ( _ISR_Nest_level == 0 )
    /* restore old stack pointer */
    stack_ptr = _old_stack_ptr;

  _ISR_Local_enable( level );

  if ( _ISR_Nest_level )
    return;

  if ( !_Thread_Dispatch_is_enabled() ) {
    return;
  }

  if ( _Thread_Dispatch_necessary ) {
    _Thread_Dispatch();
  }
}
