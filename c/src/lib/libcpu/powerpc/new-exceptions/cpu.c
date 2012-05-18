/*
 *  PowerPC CPU Dependent Source
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/exec/cpu/no_cpu/cpu.c:
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be found in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <string.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/context.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>
#include <rtems/score/cpu.h>
#include <rtems/powerpc/powerpc.h>

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS: NONE
 */

void _CPU_Initialize(void)
{
  /* Do nothing */
#ifdef __ALTIVEC__
  _CPU_Initialize_altivec();
#endif
}

/*PAGE
 *
 *  _CPU_Context_Initialize
 */

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp
)
{
  ppc_context *the_ppc_context;
  uint32_t   msr_value;
  uint32_t   sp;

  sp = (uint32_t)stack_base + size - PPC_MINIMUM_STACK_FRAME_SIZE;

  sp &= ~(CPU_STACK_ALIGNMENT-1);

  *((uint32_t*)sp) = 0;

  _CPU_MSR_GET( msr_value );

  /*
   * Setting the interrupt mask here is not strictly necessary
   * since the IRQ level will be established from _Thread_Handler()
   * again, as soon as the task starts execution.
   * Because we have to establish a defined state anyways we
   * can as well leave this code here.
   * I.e., simply (and unconditionally) saying
   *
   *   msr_value &= ~ppc_interrupt_get_disable_mask();
   *
   * would be an alternative.
   */

  if (!(new_level & CPU_MODES_INTERRUPT_MASK)) {
    msr_value |= ppc_interrupt_get_disable_mask();
  }
  else {
    msr_value &= ~ppc_interrupt_get_disable_mask();
  }

  /*
   *  The FP bit of the MSR should only be enabled if this is a floating
   *  point task.  Unfortunately, the vfprintf_r routine in newlib
   *  ends up pushing a floating point register regardless of whether or
   *  not a floating point number is being printed.  Serious restructuring
   *  of vfprintf.c will be required to avoid this behavior.  At this
   *  time (7 July 1997), this restructuring is not being done.
   */

  /* Make sure integer tasks have no FPU access in order to
   * catch violations. Gcc may implicitely use the FPU and
   * data corruption may happen.
   * Since we set the_contex->msr using our current MSR,
   * we must make sure MSR_FP is off if (!is_fp)...
   * Unfortunately, this means that users of vfprintf_r have to use FP
   * tasks or fix vfprintf. Furthermore, users of int-only tasks
   * must prevent gcc from using the FPU (currently -msoft-float is the
   * only way...)
   */
  if ( is_fp )
    msr_value |= PPC_MSR_FP;
  else
    msr_value &= ~PPC_MSR_FP;

  memset( the_context, 0, sizeof( *the_context ) );

  the_ppc_context = ppc_get_context( the_context );
  the_ppc_context->gpr1 = sp;
  the_ppc_context->msr = msr_value;
  the_ppc_context->lr = (uint32_t) entry_point;

#ifdef __ALTIVEC__
  _CPU_Context_initialize_altivec(the_context);
#endif
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
}

/*  _CPU_ISR_install_vector
 *
 *  This kernel routine installs the RTEMS handler for the
 *  specified vector.
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    old_handler - former ISR for this vector number
 *    new_handler - replacement ISR for this vector number
 *
 *  Output parameters:  NONE
 */

void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  BSP_panic("_CPU_ISR_install_vector called\n");
}
