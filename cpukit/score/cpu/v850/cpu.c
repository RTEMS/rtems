/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief v850 CPU Initialize
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

#include <rtems/score/cpuimpl.h>
#include <rtems/score/isr.h>

#include <string.h> /* for memset */

/*
 *  v850 Specific Information:
 *
 *  So far nothing known to be needed at this point during initialization.
 */
void _CPU_Initialize(void)
{
}

void _CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr error )
{
  __asm__ __volatile__ ( "di" );
  __asm__ __volatile__ ( "mov %0, r10; " : "=r" ((error)) );
  __asm__ __volatile__ ( "halt" );
}

/*
 *  v850 Specific Information:
 *
 *  This method returns 0 if interrupts are enabled and 1 if they are disabled.
 *  The v850 only has two interrupt levels (on and off).
 */
uint32_t _CPU_ISR_Get_level( void )
{
  unsigned int psw;

  v850_get_psw( psw );

  if ( (psw & V850_PSW_INTERRUPT_DISABLE_MASK) == V850_PSW_INTERRUPT_DISABLE )
    return 1;

  return 0;
}

/*
 *  v850 Specific Information:
 *
 *  This method initializes a v850 context control structure.
 */
void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp,
  void             *tls_area
)
{
  uint32_t  stack_high;  /* highest "stack aligned" address */
  uint32_t  psw;         /* highest "stack aligned" address */

  memset( the_context, 0, sizeof(Context_Control) );

  /*
   *  On CPUs with stacks which grow down, we build the stack
   *  based on the stack_high address.
   */
  stack_high = ((uint32_t)(stack_base) + size);
  stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

  v850_get_psw( psw );
  psw &= ~V850_PSW_INTERRUPT_DISABLE_MASK;
  if ( new_level )
    psw |= V850_PSW_INTERRUPT_DISABLE;
  else
    psw |= V850_PSW_INTERRUPT_ENABLE;

  the_context->r31              = (uint32_t) entry_point;
  the_context->r3_stack_pointer = stack_high;
  the_context->psw              = psw;

#if 0
  printk( "the_context = %p\n",      the_context );
  printk( "stack base  = 0x%08x\n",  stack_base );
  printk( "stack size  = 0x%08x\n",  size );
  printk( "sp          = 0x%08x\n",  the_context->r3_stack_pointer );
  printk( "psw         = 0x%08x\n",  the_context->psw );
#endif
}

