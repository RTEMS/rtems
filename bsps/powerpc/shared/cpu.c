/*
 *  PowerPC CPU Dependent Source
 */

/*
 *  Author:  Andrew Bray <andy@i-cubed.co.uk>
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
 *  http://www.rtems.org/license/LICENSE.
 */

#include <string.h>

#include <rtems/score/isr.h>
#include <rtems/score/context.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>
#include <rtems/score/cpu.h>
#include <rtems/score/tls.h>
#include <rtems/powerpc/powerpc.h>

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 */
void _CPU_Initialize(void)
{
#if defined(__ALTIVEC__) && !defined(PPC_MULTILIB_ALTIVEC)
  _CPU_Initialize_altivec();
#endif
}

/*
 *  _CPU_Context_Initialize
 */
void _CPU_Context_Initialize(
  Context_Control  *the_context,
  void             *stack_base,
  size_t            size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp,
  void             *tls_area
)
{
  ppc_context *the_ppc_context;
  uint32_t   msr_value = 0;
  uintptr_t  sp;
  uintptr_t  stack_alignment;

  sp = (uintptr_t) stack_base + size - PPC_MINIMUM_STACK_FRAME_SIZE;

  stack_alignment = CPU_STACK_ALIGNMENT;
  sp &= ~(stack_alignment - 1);

  sp = (uintptr_t) memset((void *) sp, 0, PPC_MINIMUM_STACK_FRAME_SIZE);

  the_ppc_context = ppc_get_context( the_context );

#if !defined(PPC_DISABLE_MSR_ACCESS)
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

#ifdef PPC_MULTILIB_FPU
  /*
   *  The FP bit of the MSR should only be enabled if this is a floating
   *  point task.  Unfortunately, the vfprintf_r routine in newlib
   *  ends up pushing a floating point register regardless of whether or
   *  not a floating point number is being printed.  Serious restructuring
   *  of vfprintf.c will be required to avoid this behavior.  At this
   *  time (7 July 1997), this restructuring is not being done.
   */
  msr_value |= MSR_FP;
#endif

#ifdef PPC_MULTILIB_ALTIVEC
  msr_value |= MSR_VE;
#endif
#endif  /* END PPC_DISABLE_MSR_ACCESS */

#ifdef PPC_MULTILIB_ALTIVEC
  the_ppc_context->vrsave = 0;
#endif

  the_ppc_context->gpr1 = sp;
  the_ppc_context->msr = msr_value;
  the_ppc_context->lr = (uintptr_t) entry_point;
  the_ppc_context->isr_dispatch_disable = 0;

#if defined(__ALTIVEC__) && !defined(PPC_MULTILIB_ALTIVEC)
  _CPU_Context_initialize_altivec( the_ppc_context );
#endif

  if ( tls_area != NULL ) {
    void *tls_block = _TLS_TCB_before_TLS_block_initialize( tls_area );

    the_ppc_context->tp = (uintptr_t) tls_block + 0x7000;
  }
}
