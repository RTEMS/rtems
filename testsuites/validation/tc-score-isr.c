/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreIsrValIsr
 */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/sysinit.h>
#include <rtems/score/percpu.h>
#include <rtems/score/thread.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreIsrValIsr spec:/score/isr/val/isr
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @brief Tests general interrupt support behaviour.
 *
 * This test case performs the following actions:
 *
 * - Submit an ISR request during system initialization.  Check the stack of
 *   the interrupted context while the ISR request is serviced.  Store the
 *   result of the check in interrupted_stack_at_multitasking_start_is_valid.
 *
 *   - Check that stack of the interrupted context was valid when an interrupt
 *     was serviced during the multitasking start.
 *
 * @{
 */

static uintptr_t interrupted_stack_at_multitasking_start;

static bool interrupted_stack_at_multitasking_start_is_valid;

#if defined(__aarch64__)
void __real_bsp_interrupt_dispatch( void );

void __wrap_bsp_interrupt_dispatch( void );

void __wrap_bsp_interrupt_dispatch( void )
{
  if ( interrupted_stack_at_multitasking_start == 0 ) {
    uintptr_t             sp;
    rtems_interrupt_level level;

    rtems_interrupt_local_disable( level );
    __asm__ volatile (
      "msr spsel, #1\n"
      "mov %0, sp\n"
      "msr spsel, #0"
      : "=r" ( sp )
    );
    rtems_interrupt_local_enable( level );

    interrupted_stack_at_multitasking_start = sp;
  }

  __real_bsp_interrupt_dispatch();
}
#endif

#if defined(ARM_MULTILIB_ARCH_V4)
void __real_bsp_interrupt_dispatch( void );

void __wrap_bsp_interrupt_dispatch( void );

void __wrap_bsp_interrupt_dispatch( void )
{
  register uintptr_t sp __asm__( "9" );

  if ( interrupted_stack_at_multitasking_start == 0 ) {
    interrupted_stack_at_multitasking_start = sp;
  }

  __real_bsp_interrupt_dispatch();
}
#endif

#if defined(__microblaze__)
void __real_bsp_interrupt_dispatch( uint32_t source );

void __wrap_bsp_interrupt_dispatch( uint32_t source );

void __wrap_bsp_interrupt_dispatch( uint32_t source )
{
  register uintptr_t sp __asm__( "1" );

  if ( interrupted_stack_at_multitasking_start == 0 ) {
    interrupted_stack_at_multitasking_start = sp;
  }

  __real_bsp_interrupt_dispatch( source );
}
#endif

#if defined(__PPC__) || defined(__powerpc64__)
void __real_bsp_interrupt_dispatch( void );

void __wrap_bsp_interrupt_dispatch( void );

void __wrap_bsp_interrupt_dispatch( void )
{
  register uintptr_t sp __asm__( "14" );

  if ( interrupted_stack_at_multitasking_start == 0 ) {
    interrupted_stack_at_multitasking_start = sp;
  }

  __real_bsp_interrupt_dispatch();
}
#endif

#if defined(__riscv)
void __real__RISCV_Interrupt_dispatch(
  uintptr_t        mcause,
  Per_CPU_Control *cpu_self
);

void __wrap__RISCV_Interrupt_dispatch(
  uintptr_t        mcause,
  Per_CPU_Control *cpu_self
);

void __wrap__RISCV_Interrupt_dispatch(
  uintptr_t        mcause,
  Per_CPU_Control *cpu_self
)
{
  register uintptr_t sp __asm__( "s1" );

  if ( interrupted_stack_at_multitasking_start == 0 ) {
    interrupted_stack_at_multitasking_start = sp;
  }

  __real__RISCV_Interrupt_dispatch( mcause, cpu_self );
}
#endif

#if defined(__sparc__)
void __real__SPARC_Interrupt_dispatch( uint32_t irq );

static RTEMS_USED void InterruptDispatch( uint32_t irq, uintptr_t sp )
{
  if ( interrupted_stack_at_multitasking_start == 0 ) {
    interrupted_stack_at_multitasking_start = sp;
  }

  __real__SPARC_Interrupt_dispatch( irq );
}

__asm__ (
  "\t.section\t\".text\"\n"
  "\t.align\t4\n"
  "\t.globl\t__wrap__SPARC_Interrupt_dispatch\n"
  "\t.type\t__wrap__SPARC_Interrupt_dispatch, #function\n"
  "__wrap__SPARC_Interrupt_dispatch:\n"
  "\tmov\t%fp, %o1\n"
  "\tor\t%o7, %g0, %g1\n"
  "\tcall\tInterruptDispatch, 0\n"
  "\t or\t%g1, %g0, %o7\n"
  "\t.previous\n"
);
#endif

static void ISRHandler( void *arg )
{
  uintptr_t begin;
  uintptr_t end;

  (void) arg;

#if defined(RTEMS_SMP)
  Per_CPU_Control *cpu_self;

  cpu_self = _Per_CPU_Get();
  begin = (uintptr_t) &cpu_self->Interrupt_frame;
  end = begin + sizeof( cpu_self->Interrupt_frame );
#else
  Thread_Control *executing;

  executing = GetExecuting();
  begin = (uintptr_t) executing->Start.Initial_stack.area;
  end = begin + executing->Start.Initial_stack.size;
#endif

  interrupted_stack_at_multitasking_start_is_valid =
    ( begin <= interrupted_stack_at_multitasking_start &&
      interrupted_stack_at_multitasking_start < end );
}

static CallWithinISRRequest isr_request = {
  .handler = ISRHandler
};

static void SubmitISRRequest( void )
{
  CallWithinISRSubmit( &isr_request );
}

RTEMS_SYSINIT_ITEM(
  SubmitISRRequest,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST
);

/**
 * @brief Submit an ISR request during system initialization.  Check the stack
 *   of the interrupted context while the ISR request is serviced.  Store the
 *   result of the check in interrupted_stack_at_multitasking_start_is_valid.
 */
static void ScoreIsrValIsr_Action_0( void )
{
  /*
   * The actions are performed during system initialization and the
   * multitasking start.
   */

  /*
   * Check that stack of the interrupted context was valid when an interrupt
   * was serviced during the multitasking start.
   */
  T_true( interrupted_stack_at_multitasking_start_is_valid );
}

/**
 * @fn void T_case_body_ScoreIsrValIsr( void )
 */
T_TEST_CASE( ScoreIsrValIsr )
{
  ScoreIsrValIsr_Action_0();
}

/** @} */
