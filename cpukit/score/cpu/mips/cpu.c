/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 * @ingroup RTEMSScoreCPUMIPS
 *
 *  @brief MIPS CPU Dependent Source
 */

/*
 * MIPS CPU Dependent Source
 *
 *  2002:       Greg Menke (gregory.menke@gsfc.nasa.gov)
 *      Overhauled interrupt level and interrupt enable/disable code
 *      to more exactly support MIPS.  Our mods were for MIPS1 processors
 *      MIPS3 ports are affected, though apps written to the old behavior
 *      should still work OK.
 *
 *  Conversion to MIPS port by Alan Cudmore <alanc@linuxstart.com> and
 *           Joel Sherrill <joel@OARcorp.com>.
 *
 *    These changes made the code conditional on standard cpp predefines,
 *    merged the mips1 and mips3 code sequences as much as possible,
 *    and moved some of the assembly code to C.  Alan did much of the
 *    initial analysis and rework.  Joel took over from there and
 *    wrote the JMR3904 BSP so this could be tested.  Joel also
 *    added the new interrupt vectoring support in libcpu and
 *    tried to better support the various interrupt controllers.
 *
 *  Original MIP64ORION port by Craig Lebakken <craigl@transition.com>
 *           COPYRIGHT (c) 1996 by Transition Networks Inc.
 *
 *         To anyone who acknowledges that this file is provided "AS IS"
 *         without any express or implied warranty:
 *             permission to use, copy, modify, and distribute this file
 *             for any purpose is hereby granted without fee, provided that
 *             the above copyright notice and this notice appears in all
 *             copies, and that the name of Transition Networks not be used in
 *             advertising or publicity pertaining to distribution of the
 *             software without specific, written prior permission.
 *             Transition Networks makes no representations about the
 *             suitability of this software for any purpose.
 *
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

#if CPU_HARDWARE_FP
Context_Control_fp _CPU_Null_fp_context;
#endif

/*
** Exception stack frame pointer used in cpu_asm to pass the exception stack frame
** address to the context switch code.
*/
#if (__mips == 1) || (__mips == 32)
typedef uint32_t ESF_PTR_TYPE;
#elif (__mips == 3)
typedef uint64_t ESF_PTR_TYPE;
#else
#error "unknown MIPS ISA"
#endif

ESF_PTR_TYPE __exceptionStackFrame = 0;
/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *    thread_dispatch - address of dispatching routine
 */

void _CPU_Initialize(void)
{
  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

#if CPU_HARDWARE_FP
  /* FP context initialization support goes here */
  _CPU_Null_fp_context.fpcs = 0x1000000; 	/* Set FS flag in floating point coprocessor
  						   control register to prevent underflow and
  						   inexact exceptions */
#endif
}

uint32_t   _CPU_ISR_Get_level( void )
{
  unsigned int sr;

  mips_get_sr( sr );

  return ( sr & SR_INTERRUPT_ENABLE_BITS ) != 0 ? 0 : 1;
}

void _CPU_ISR_Set_level( uint32_t   new_level )
{
  unsigned int sr;

  mips_get_sr( sr );

  if ( new_level == 0 ) {
    sr |= SR_INTERRUPT_ENABLE_BITS;
  } else {
    sr &= ~SR_INTERRUPT_ENABLE_BITS;
  }

  mips_set_sr( sr );
}

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uintptr_t        *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp,
  void             *tls_area
)
{
  (void) is_fp;
  (void) tls_area;

  uintptr_t             stack_tmp;
  __MIPS_REGISTER_TYPE  intlvl = new_level & 0xff;
  __MIPS_REGISTER_TYPE  c0_sr;

  stack_tmp  = (uintptr_t)stack_base;
  stack_tmp += ((size) - CPU_STACK_ALIGNMENT);
  stack_tmp &= (__MIPS_REGISTER_TYPE) ~(CPU_STACK_ALIGNMENT - 1);

  the_context->sp = (__MIPS_REGISTER_TYPE) stack_tmp;
  the_context->fp = (__MIPS_REGISTER_TYPE) stack_tmp;
  the_context->ra = (__MIPS_REGISTER_TYPE) (uintptr_t)entry_point;

  /*
   * The interrupt mask is not part of the context.  It is global state of the
   * interrupt controller, so that a source disabled with
   * rtems_interrupt_vector_disable() stays disabled across a context switch.
   * The level carries the interrupt enable bit.
   */
  c0_sr = SR_CU0 | _EXTRABITS;

  if ( intlvl == 0 ) {
    c0_sr |= _INTON;
  }
#if MIPS_HAS_FPU == 1
  if ( is_fp ) {
    c0_sr |= SR_CU1;
  }
#endif
  the_context->c0_sr = c0_sr;
}
/*
 *  _CPU_Internal_threads_Idle_thread_body
 *
 *  NOTES:
 *
 *  1. This is the same as the regular CPU independent algorithm.
 *
 *  2. If you implement this using a "halt", "idle", or "shutdown"
 *     instruction, then don't forget to put it in an infinite loop.
 *
 *  3. Be warned. Some processors with onboard DMA have been known
 *     to stop the DMA if the CPU were put in IDLE mode.  This might
 *     also be a problem with other on-chip peripherals.  So use this
 *     hook with caution.
 */

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
  (void) ignored;

#if (__mips == 3) || (__mips == 32)
   for( ; ; )
     __asm__ volatile("wait"); /* use wait to enter low power mode */
#elif __mips == 1
   for( ; ; )
     ;
#else
#error "IDLE: __mips not set to 1 or 3"
#endif
}
