/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief NIOS2 Exception and Interrupt Handler
 *
 * @note Derived from c4x/irq.c
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/nios2-utility.h>
#include <rtems/score/interr.h>

/*
 *  This routine provides the RTEMS interrupt management.
 *
 *  Upon entry, interrupts are disabled
 */

unsigned long *_old_stack_ptr;

/*
 * Prototypes
 */
void __ISR_Handler(void);
void __Exception_Handler(CPU_Exception_frame *efr);

register unsigned long  *stack_ptr __asm__ ("sp");

static inline void __IIC_Handler(void)
{
  uint32_t active;
  uint32_t mask;
  uint32_t vector;

  /*
   * Obtain from the interrupt controller a bit list of pending interrupts,
   * and then process the highest priority interrupt. This process loops,
   * loading the active interrupt list on each pass until ipending
   * return zero.
   *
   * The maximum interrupt latency for the highest priority interrupt is
   * reduced by finding out which interrupts are pending as late as possible.
   * Consider the case where the high priority interupt is asserted during
   * the interrupt entry sequence for a lower priority interrupt to see why
   * this is the case.
   */

  active = _Nios2_Get_ctlreg_ipending();

  while (active)
  {
    vector = 0;
    mask = 1;

    /*
     * Test each bit in turn looking for an active interrupt. Once one is
     * found call it to clear the interrupt condition.
     */

    while (active)
    {
      if (active & mask)
      {
        if ( _ISR_Vector_table[ vector] )
          (*_ISR_Vector_table[ vector ])(vector);
        active &= ~mask;
      }
      mask <<= 1;
      ++vector;
    };

    active = _Nios2_Get_ctlreg_ipending();
  }

}

void __ISR_Handler(void)
{
  register uint32_t level;

  /* Interrupts are disabled upon entry to this Handler */

  if ( _ISR_Nest_level == 0 ) {
    /* Install irq stack */
    _old_stack_ptr = stack_ptr;
    stack_ptr = _CPU_Interrupt_stack_high - 4;
  }

  _ISR_Nest_level++;

  _Thread_Dispatch_disable();

  __IIC_Handler();

  /* Make sure that interrupts are disabled again */
  _CPU_ISR_Disable( level );

  _Thread_Dispatch_unnest( _Per_CPU_Get() );

  _ISR_Nest_level--;

  if( _ISR_Nest_level == 0) {
    stack_ptr = _old_stack_ptr;

    if( _Thread_Dispatch_is_enabled() )
    {
      if ( _Thread_Dispatch_necessary ) {
        _CPU_ISR_Enable( level );
        _Thread_Dispatch();
        /* may have switched to another task and not return here immed. */
        _CPU_ISR_Disable( level ); /* Keep _pairs_ of Enable/Disable */
      }
    }
  }

  _CPU_ISR_Enable( level );
}

void __Exception_Handler(CPU_Exception_frame *efr)
{
  _CPU_Fatal_halt(RTEMS_FATAL_SOURCE_EXCEPTION, 0xECC0); /* source ignored */
}
