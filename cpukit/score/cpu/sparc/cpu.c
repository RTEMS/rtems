/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUSPARC
 *
 * @brief This source file contains static assertions to ensure the consistency
 *   of interfaces used in C and assembler and it contains the SPARC-specific
 *   implementation of _CPU_Initialize(), _CPU_ISR_Get_level(), and
 *   _CPU_Context_Initialize().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <rtems/score/percpu.h>
#include <rtems/score/tls.h>
#include <rtems/score/thread.h>

#if SPARC_HAS_FPU == 1
  RTEMS_STATIC_ASSERT(
    offsetof( Per_CPU_Control, cpu_per_cpu.fsr)
      == SPARC_PER_CPU_FSR_OFFSET,
    SPARC_PER_CPU_FSR_OFFSET
  );

  #if defined(SPARC_USE_LAZY_FP_SWITCH)
    RTEMS_STATIC_ASSERT(
      offsetof( Per_CPU_Control, cpu_per_cpu.fp_owner)
        == SPARC_PER_CPU_FP_OWNER_OFFSET,
      SPARC_PER_CPU_FP_OWNER_OFFSET
    );
  #endif
#endif

#define SPARC_ASSERT_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(Context_Control, field) == off ## _OFFSET, \
    Context_Control_offset_ ## field \
  )

SPARC_ASSERT_OFFSET(g5, G5);
SPARC_ASSERT_OFFSET(g7, G7);

RTEMS_STATIC_ASSERT(
  offsetof(Context_Control, l0_and_l1) == L0_OFFSET,
  Context_Control_offset_L0
);

RTEMS_STATIC_ASSERT(
  offsetof(Context_Control, l0_and_l1) + 4 == L1_OFFSET,
  Context_Control_offset_L1
);

SPARC_ASSERT_OFFSET(l2, L2);
SPARC_ASSERT_OFFSET(l3, L3);
SPARC_ASSERT_OFFSET(l4, L4);
SPARC_ASSERT_OFFSET(l5, L5);
SPARC_ASSERT_OFFSET(l6, L6);
SPARC_ASSERT_OFFSET(l7, L7);
SPARC_ASSERT_OFFSET(i0, I0);
SPARC_ASSERT_OFFSET(i1, I1);
SPARC_ASSERT_OFFSET(i2, I2);
SPARC_ASSERT_OFFSET(i3, I3);
SPARC_ASSERT_OFFSET(i4, I4);
SPARC_ASSERT_OFFSET(i5, I5);
SPARC_ASSERT_OFFSET(i6_fp, I6_FP);
SPARC_ASSERT_OFFSET(i7, I7);
SPARC_ASSERT_OFFSET(o6_sp, O6_SP);
SPARC_ASSERT_OFFSET(o7, O7);
SPARC_ASSERT_OFFSET(psr, PSR);
SPARC_ASSERT_OFFSET(isr_dispatch_disable, ISR_DISPATCH_DISABLE_STACK);

#if defined(RTEMS_SMP)
SPARC_ASSERT_OFFSET(is_executing, SPARC_CONTEXT_CONTROL_IS_EXECUTING);
#endif

#define SPARC_ASSERT_ISF_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(CPU_Interrupt_frame, field) == ISF_ ## off ## _OFFSET, \
    CPU_Interrupt_frame_offset_ ## field \
  )

SPARC_ASSERT_ISF_OFFSET(psr, PSR);
SPARC_ASSERT_ISF_OFFSET(pc, PC);
SPARC_ASSERT_ISF_OFFSET(npc, NPC);
SPARC_ASSERT_ISF_OFFSET(g1, G1);
SPARC_ASSERT_ISF_OFFSET(g2, G2);
SPARC_ASSERT_ISF_OFFSET(g3, G3);
SPARC_ASSERT_ISF_OFFSET(g4, G4);
SPARC_ASSERT_ISF_OFFSET(g5, G5);
SPARC_ASSERT_ISF_OFFSET(g7, G7);
SPARC_ASSERT_ISF_OFFSET(i0, I0);
SPARC_ASSERT_ISF_OFFSET(i1, I1);
SPARC_ASSERT_ISF_OFFSET(i2, I2);
SPARC_ASSERT_ISF_OFFSET(i3, I3);
SPARC_ASSERT_ISF_OFFSET(i4, I4);
SPARC_ASSERT_ISF_OFFSET(i5, I5);
SPARC_ASSERT_ISF_OFFSET(i6_fp, I6_FP);
SPARC_ASSERT_ISF_OFFSET(i7, I7);
SPARC_ASSERT_ISF_OFFSET(y, Y);
SPARC_ASSERT_ISF_OFFSET(tpc, TPC);

#define SPARC_ASSERT_FP_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(Context_Control_fp, field) == SPARC_FP_CONTEXT_OFFSET_ ## off, \
    Context_Control_fp_offset_ ## field \
  )

SPARC_ASSERT_FP_OFFSET(f0_f1, F0_F1);
SPARC_ASSERT_FP_OFFSET(f2_f3, F2_F3);
SPARC_ASSERT_FP_OFFSET(f4_f5, F4_F5);
SPARC_ASSERT_FP_OFFSET(f6_f7, F6_F7);
SPARC_ASSERT_FP_OFFSET(f8_f9, F8_F9);
SPARC_ASSERT_FP_OFFSET(f10_f11, F10_F11);
SPARC_ASSERT_FP_OFFSET(f12_f13, F12_F13);
SPARC_ASSERT_FP_OFFSET(f14_f15, F14_F15);
SPARC_ASSERT_FP_OFFSET(f16_f17, F16_F17);
SPARC_ASSERT_FP_OFFSET(f18_f19, F18_F19);
SPARC_ASSERT_FP_OFFSET(f20_f21, F20_F21);
SPARC_ASSERT_FP_OFFSET(f22_f23, F22_F23);
SPARC_ASSERT_FP_OFFSET(f24_f25, F24_F25);
SPARC_ASSERT_FP_OFFSET(f26_f27, F26_F27);
SPARC_ASSERT_FP_OFFSET(f28_f29, F28_F29);
SPARC_ASSERT_FP_OFFSET(f30_f31, F30_F31);
SPARC_ASSERT_FP_OFFSET(fsr, FSR);

RTEMS_STATIC_ASSERT(
  sizeof(SPARC_Minimum_stack_frame) == SPARC_MINIMUM_STACK_FRAME_SIZE,
  SPARC_MINIMUM_STACK_FRAME_SIZE
);

/* https://devel.rtems.org/ticket/2352 */
RTEMS_STATIC_ASSERT(
  sizeof(CPU_Interrupt_frame) % CPU_ALIGNMENT == 0,
  CPU_Interrupt_frame_alignment
);

#define SPARC_ASSERT_REGISTER_WINDOW_OFFSET( member, off ) \
  RTEMS_STATIC_ASSERT( \
    offsetof( SPARC_Register_window, member ) == \
      RTEMS_XCONCAT( SPARC_REGISTER_WINDOW_OFFSET_, off ), \
    SPARC_Register_window ## member \
  )

SPARC_ASSERT_REGISTER_WINDOW_OFFSET( local[ 0 ], LOCAL( 0 ) );
SPARC_ASSERT_REGISTER_WINDOW_OFFSET( local[ 1 ], LOCAL( 1 ) );
SPARC_ASSERT_REGISTER_WINDOW_OFFSET( input[ 0 ], INPUT( 0 ) );
SPARC_ASSERT_REGISTER_WINDOW_OFFSET( input[ 1 ], INPUT( 1 ) );

RTEMS_STATIC_ASSERT(
  sizeof( SPARC_Register_window ) == SPARC_REGISTER_WINDOW_SIZE,
  SPARC_REGISTER_WINDOW_SIZE
);

#define SPARC_ASSERT_EXCEPTION_OFFSET( member, off ) \
  RTEMS_STATIC_ASSERT( \
    offsetof( CPU_Exception_frame, member ) == \
      RTEMS_XCONCAT( SPARC_EXCEPTION_OFFSET_, off ), \
    CPU_Exception_frame_offset_ ## member \
  )

SPARC_ASSERT_EXCEPTION_OFFSET( psr, PSR );
SPARC_ASSERT_EXCEPTION_OFFSET( pc, PC );
SPARC_ASSERT_EXCEPTION_OFFSET( npc, NPC );
SPARC_ASSERT_EXCEPTION_OFFSET( trap, TRAP );
SPARC_ASSERT_EXCEPTION_OFFSET( wim, WIM );
SPARC_ASSERT_EXCEPTION_OFFSET( y, Y );
SPARC_ASSERT_EXCEPTION_OFFSET( global[ 0 ], GLOBAL( 0 ) );
SPARC_ASSERT_EXCEPTION_OFFSET( global[ 1 ], GLOBAL( 1 ) );
SPARC_ASSERT_EXCEPTION_OFFSET( output[ 0 ], OUTPUT( 0 ) );
SPARC_ASSERT_EXCEPTION_OFFSET( output[ 1 ], OUTPUT( 1 ) );

#if SPARC_HAS_FPU == 1
SPARC_ASSERT_EXCEPTION_OFFSET( fsr, FSR );
SPARC_ASSERT_EXCEPTION_OFFSET( fp[ 0 ], FP( 0 ) );
SPARC_ASSERT_EXCEPTION_OFFSET( fp[ 1 ], FP( 1 ) );
#endif

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Exception_frame ) == SPARC_EXCEPTION_FRAME_SIZE,
  SPARC_EXCEPTION_FRAME_SIZE
);

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Exception_frame ) % CPU_ALIGNMENT == 0,
  CPU_Exception_frame_alignment
);

/*
 *  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS: NONE
 *
 *  Output Parameters: NONE
 *
 *  NOTE: There is no need to save the pointer to the thread dispatch routine.
 *        The SPARC's assembly code can reference it directly with no problems.
 */

void _CPU_Initialize(void)
{
#if defined(SPARC_USE_LAZY_FP_SWITCH)
  __asm__ volatile (
    ".global SPARC_THREAD_CONTROL_REGISTERS_FP_CONTEXT_OFFSET\n"
    ".set SPARC_THREAD_CONTROL_REGISTERS_FP_CONTEXT_OFFSET, %0\n"
    ".global SPARC_THREAD_CONTROL_FP_CONTEXT_OFFSET\n"
    ".set SPARC_THREAD_CONTROL_FP_CONTEXT_OFFSET, %1\n"
    :
    : "i" (offsetof(Thread_Control, Registers.fp_context)),
      "i" (offsetof(Thread_Control, fp_context))
  );
#endif
}

uint32_t   _CPU_ISR_Get_level( void )
{
  uint32_t   level;

  sparc_get_interrupt_level( level );

  return level;
}

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
    uint32_t     stack_high;  /* highest "stack aligned" address */
    uint32_t     tmp_psr;

    /*
     *  On CPUs with stacks which grow down (i.e. SPARC), we build the stack
     *  based on the stack_high address.
     */

    stack_high = ((uint32_t)(stack_base) + size);
    stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

    /*
     *  See the README in this directory for a diagram of the stack.
     */

    the_context->o7    = ((uint32_t) entry_point) - 8;
    the_context->o6_sp = stack_high - SPARC_MINIMUM_STACK_FRAME_SIZE;
    the_context->i6_fp = 0;

    /*
     *  Build the PSR for the task.  Most everything can be 0 and the
     *  CWP is corrected during the context switch.
     *
     *  The EF bit determines if the floating point unit is available.
     *  The FPU is ONLY enabled if the context is associated with an FP task
     *  and this SPARC model has an FPU.
     */

    sparc_get_psr( tmp_psr );
    tmp_psr &= ~SPARC_PSR_PIL_MASK;
    tmp_psr |= (new_level << 8) & SPARC_PSR_PIL_MASK;
    tmp_psr &= ~SPARC_PSR_EF_MASK;      /* disabled by default */

    /* _CPU_Context_restore_heir() relies on this */
    _Assert( ( tmp_psr & SPARC_PSR_ET_MASK ) != 0 );

#if (SPARC_HAS_FPU == 1)
    /*
     *  If this bit is not set, then a task gets a fault when it accesses
     *  a floating point register.  This is a nice way to detect floating
     *  point tasks which are not currently declared as such.
     */

    if ( is_fp )
      tmp_psr |= SPARC_PSR_EF_MASK;
#endif
    the_context->psr = tmp_psr;

  /*
   *  Since THIS thread is being created, there is no way that THIS
   *  thread can have an interrupt stack frame on its stack.
   */
  the_context->isr_dispatch_disable = 0;

  if ( tls_area != NULL ) {
    void *tcb = _TLS_Initialize_area( tls_area );

    the_context->g7 = (uintptr_t) tcb;
  }
}
