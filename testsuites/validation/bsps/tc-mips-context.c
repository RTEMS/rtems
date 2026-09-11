/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuMipsValContext
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <stdint.h>
#include <string.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/percpu.h>
#include <rtems/score/thread.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuMipsValContext spec:/score/cpu/mips/val/context
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Checks that the context switch saves and restores every call-saved
 *   register.
 *
 * The action calls the switch directly.  A call through the operating system
 * runs a chain of functions.  A function of the chain can preserve a register
 * on its own stack.  This hides a defect of the switch in that register.
 *
 * The set of a MIPS call-saved register holds 11 registers and the action
 * covers 10 of them.  It loads a pattern into 16 to 23.  For 30 it reads the
 * value of the caller, because the compiler can hold a frame of its own in
 * that register.  A requirement of its own covers 29.
 *
 * No check covers 28.  One value of the global pointer serves the whole
 * program of this port.  The size criterion of the small data area is zero, so
 * the area is empty.  A switch of the register would therefore observe
 * nothing.
 *
 * The action reads the registers of its checks out of the exception frame, so
 * each check also observes the save of the exception path.
 *
 * This test case performs the following actions:
 *
 * - Load a distinct pattern into every MIPS call-saved register which takes
 *   one and into every MIPS call-saved floating-point register.  Call the
 *   context switch directly.  The heir switches back.  Trap, so the frame
 *   reports every register.
 *
 *   - Check that the switch restored the MIPS register $s0.
 *
 *   - Check that the switch restored the MIPS register $s1.
 *
 *   - Check that the switch restored the MIPS register $s2.
 *
 *   - Check that the switch restored the MIPS register $s3.
 *
 *   - Check that the switch restored the MIPS register $s4.
 *
 *   - Check that the switch restored the MIPS register $s5.
 *
 *   - Check that the switch restored the MIPS register $s6.
 *
 *   - Check that the switch restored the MIPS register $s7.
 *
 *   - Check that the switch restored the MIPS register $f21.
 *
 *   - Check that the switch restored the MIPS register $f22.
 *
 *   - Check that the switch restored the MIPS register $f23.
 *
 *   - Check that the switch restored the MIPS register $f24.
 *
 *   - Check that the switch restored the MIPS register $f25.
 *
 *   - Check that the switch restored the MIPS register $f26.
 *
 *   - Check that the switch restored the MIPS register $f27.
 *
 *   - Check that the switch restored the MIPS register $f28.
 *
 *   - Check that the switch restored the MIPS register $f29.
 *
 *   - Check that the switch restored the MIPS register $f30.
 *
 *   - Check that the switch restored the MIPS register $f31.
 *
 *   - Check that the switch restored the rounding mode of the MIPS register
 *     fcr31.
 *
 *   - Check that the switch saved the MIPS register $s0.
 *
 *   - Check that the switch saved the MIPS register $s1.
 *
 *   - Check that the switch saved the MIPS register $s2.
 *
 *   - Check that the switch saved the MIPS register $s3.
 *
 *   - Check that the switch saved the MIPS register $s4.
 *
 *   - Check that the switch saved the MIPS register $s5.
 *
 *   - Check that the switch saved the MIPS register $s6.
 *
 *   - Check that the switch saved the MIPS register $s7.
 *
 *   - Check that the switch saved the MIPS register $f20.
 *
 *   - Check that the switch saved the MIPS register $f21.
 *
 *   - Check that the switch saved the MIPS register $f22.
 *
 *   - Check that the switch saved the MIPS register $f23.
 *
 *   - Check that the switch saved the MIPS register $f24.
 *
 *   - Check that the switch saved the MIPS register $f25.
 *
 *   - Check that the switch saved the MIPS register $f26.
 *
 *   - Check that the switch saved the MIPS register $f27.
 *
 *   - Check that the switch saved the MIPS register $f28.
 *
 *   - Check that the switch saved the MIPS register $f29.
 *
 *   - Check that the switch saved the MIPS register $f30.
 *
 *   - Check that the switch saved the MIPS register $f31.
 *
 *   - Check that the switch saved the rounding mode of the MIPS register
 *     fcr31.
 *
 *   - Check that the switch saved the MIPS register $fp.  The register is
 *     call-saved and the compiler can hold a frame of its own in it.  The
 *     action therefore reads the value of the caller rather than a pattern.
 *
 *   - Check that the switch restored the MIPS register $fp.
 *
 *   - Check that the context initialization enables no trap of the MIPS
 *     register fcr31.
 *
 *   - Check that the context initialization sets the flush to zero flag of the
 *     MIPS register fcr31.
 *
 *   - Check that the round trip reached the trap.  The heir returns through
 *     the stack pointer of the caller.
 *
 *   - Check that the switch saved the return address of the caller.
 *
 * @{
 */

#define _RTEMS_TMTEST27
#include <tm27.h>

extern const char context_done_label[];

static CPU_Exception_frame context_frame;

static volatile int context_trap;

/*
 * The frame pointer of the caller, which the psABI makes call-saved.  The
 * switch stores it, and no pattern can reach it, because the compiler can
 * use the register for a frame of its own.  The assembly below names the
 * two objects, so neither one is static.
 */
uint32_t context_fp_before;

uint32_t context_fp_after;

#if MIPS_HAS_FPU == 1
/* The floating-point environment which the context initialization gives. */
static uint32_t context_initial_fcr31;
#endif

Context_Control context_executing;

Context_Control context_heir;

static long context_heir_stack[ 256 ];

/*
 * The rounding mode of the control and status register of coprocessor 1.
 * The value rounds toward minus infinity, which no other part of the test
 * selects.  The flush to zero flag keeps the setting of the port.
 */
#define CTX_FCSR 0x01000003

#define CTX_LOAD         \
  "li $16, 0x11121314\n" \
  "li $17, 0x12131415\n" \
  "li $18, 0x13141516\n" \
  "li $19, 0x14151617\n" \
  "li $20, 0x15161718\n" \
  "li $21, 0x16171819\n" \
  "li $22, 0x1718191a\n" \
  "li $23, 0x18191a1b\n"

#if MIPS_HAS_FPU == 1
#define CTX_LOAD_FPU                        \
  "li $27, 0x31323334\n"                    \
  "mtc1 $27, $f20\n"                        \
  "li $27, 0x32333435\n"                    \
  "mtc1 $27, $f21\n"                        \
  "li $27, 0x33343536\n"                    \
  "mtc1 $27, $f22\n"                        \
  "li $27, 0x34353637\n"                    \
  "mtc1 $27, $f23\n"                        \
  "li $27, 0x35363738\n"                    \
  "mtc1 $27, $f24\n"                        \
  "li $27, 0x36373839\n"                    \
  "mtc1 $27, $f25\n"                        \
  "li $27, 0x3738393a\n"                    \
  "mtc1 $27, $f26\n"                        \
  "li $27, 0x38393a3b\n"                    \
  "mtc1 $27, $f27\n"                        \
  "li $27, 0x393a3b3c\n"                    \
  "mtc1 $27, $f28\n"                        \
  "li $27, 0x3a3b3c3d\n"                    \
  "mtc1 $27, $f29\n"                        \
  "li $27, 0x3b3c3d3e\n"                    \
  "mtc1 $27, $f30\n"                        \
  "li $27, 0x3c3d3e3f\n"                    \
  "mtc1 $27, $f31\n"                        \
  "li $27, " RTEMS_XSTRING( CTX_FCSR ) "\n" \
                                       "ctc1 $27, $31\n"
#else
#define CTX_LOAD_FPU
#endif

static void ContextFatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  CPU_Exception_frame *frame;

  (void) arg;

  if ( source != RTEMS_FATAL_SOURCE_EXCEPTION ) {
    return;
  }

  frame = (CPU_Exception_frame *) code;
  context_frame = *frame;
  ++context_trap;
  SetFatalHandler( NULL, NULL );
  frame->epc = (__MIPS_REGISTER_TYPE) (uintptr_t) context_done_label;
  _CPU_Exception_resume( frame );
}

/*
 * The heir switches straight back.  It is assembly, so no prologue saves a
 * register of the context under test.
 */
void ContextHeirEntry( void );

__asm__( "  .text\n"
         "  .align 2\n"
         "  .globl ContextHeirEntry\n"
         "ContextHeirEntry:\n"
         "  la $4, context_heir\n"
         "  la $5, context_executing\n"
         "  j _CPU_Context_switch\n"
         "  nop\n" );

/*
 * The switch is called directly, so the patterns are in the registers when
 * it saves them.  Nothing spills them to a stack frame first.
 */
static const CPU_Exception_frame *ContextRunSwitch( void )
{
  context_trap = 0;
  memset( &context_executing, 0, sizeof( context_executing ) );
  _CPU_Context_Initialize(
    &context_heir,
    (uintptr_t *) context_heir_stack,
    sizeof( context_heir_stack ),
    0,
    ContextHeirEntry,
    true,
    NULL
  );
#if MIPS_HAS_FPU == 1
  context_initial_fcr31 = (uint32_t) context_heir.fcr31;
#endif
  SetFatalHandler( ContextFatal, NULL );

  __asm__ volatile( "  .set push\n"
                    "  .set noreorder\n"
                    "  la $27, context_fp_before\n"
                    "  sw $30, 0($27)\n" CTX_LOAD_FPU CTX_LOAD
                    "  la $4, context_executing\n"
                    "  la $5, context_heir\n"
                    "  jal _CPU_Context_switch\n"
                    "  nop\n"
                    "  .word -1\n"
                    "  .globl context_done_label\n"
                    "context_done_label:\n"
                    "  la $27, context_fp_after\n"
                    "  sw $30, 0($27)\n"
                    "  .set pop\n"
                    :
                    :
                    : "memory",
                      "$2",
                      "$3",
                      "$4",
                      "$5",
                      "$6",
                      "$7",
                      "$8",
                      "$9",
                      "$10",
                      "$11",
                      "$12",
                      "$13",
                      "$14",
                      "$15",
                      "$16",
                      "$17",
                      "$18",
                      "$19",
                      "$20",
                      "$21",
                      "$22",
                      "$23",
                      "$24",
                      "$25",
                      "$31" );

  return &context_frame;
}

/**
 * @brief Load a distinct pattern into every MIPS call-saved register which
 *   takes one and into every MIPS call-saved floating-point register.  Call
 *   the context switch directly.  The heir switches back.  Trap, so the frame
 *   reports every register.
 */
static void ScoreCpuMipsValContext_Action_0( void )
{
  const CPU_Exception_frame *frame;

  frame = ContextRunSwitch();

  /*
   * Check that the switch restored the MIPS register $s0.
   */
  T_eq_u32( (uint32_t) frame->s0, 0x11121314 );

  /*
   * Check that the switch restored the MIPS register $s1.
   */
  T_eq_u32( (uint32_t) frame->s1, 0x12131415 );

  /*
   * Check that the switch restored the MIPS register $s2.
   */
  T_eq_u32( (uint32_t) frame->s2, 0x13141516 );

  /*
   * Check that the switch restored the MIPS register $s3.
   */
  T_eq_u32( (uint32_t) frame->s3, 0x14151617 );

  /*
   * Check that the switch restored the MIPS register $s4.
   */
  T_eq_u32( (uint32_t) frame->s4, 0x15161718 );

  /*
   * Check that the switch restored the MIPS register $s5.
   */
  T_eq_u32( (uint32_t) frame->s5, 0x16171819 );

  /*
   * Check that the switch restored the MIPS register $s6.
   */
  T_eq_u32( (uint32_t) frame->s6, 0x1718191a );

  /*
   * Check that the switch restored the MIPS register $s7.
   */
  T_eq_u32( (uint32_t) frame->s7, 0x18191a1b );
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f20, 0x31323334 );
  #endif

  /*
   * Check that the switch restored the MIPS register $f21.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f21, 0x32333435 );
  #endif

  /*
   * Check that the switch restored the MIPS register $f22.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f22, 0x33343536 );
  #endif

  /*
   * Check that the switch restored the MIPS register $f23.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f23, 0x34353637 );
  #endif

  /*
   * Check that the switch restored the MIPS register $f24.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f24, 0x35363738 );
  #endif

  /*
   * Check that the switch restored the MIPS register $f25.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f25, 0x36373839 );
  #endif

  /*
   * Check that the switch restored the MIPS register $f26.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f26, 0x3738393a );
  #endif

  /*
   * Check that the switch restored the MIPS register $f27.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f27, 0x38393a3b );
  #endif

  /*
   * Check that the switch restored the MIPS register $f28.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f28, 0x393a3b3c );
  #endif

  /*
   * Check that the switch restored the MIPS register $f29.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f29, 0x3a3b3c3d );
  #endif

  /*
   * Check that the switch restored the MIPS register $f30.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f30, 0x3b3c3d3e );
  #endif

  /*
   * Check that the switch restored the MIPS register $f31.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f31, 0x3c3d3e3f );
  #endif

  /*
   * Check that the switch restored the rounding mode of the MIPS register
   * fcr31.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->fcsr & 0x3, CTX_FCSR & 0x3 );
  #endif

  /*
   * Check that the switch saved the MIPS register $s0.
   */
  T_eq_u32( (uint32_t) context_executing.s0, 0x11121314 );

  /*
   * Check that the switch saved the MIPS register $s1.
   */
  T_eq_u32( (uint32_t) context_executing.s1, 0x12131415 );

  /*
   * Check that the switch saved the MIPS register $s2.
   */
  T_eq_u32( (uint32_t) context_executing.s2, 0x13141516 );

  /*
   * Check that the switch saved the MIPS register $s3.
   */
  T_eq_u32( (uint32_t) context_executing.s3, 0x14151617 );

  /*
   * Check that the switch saved the MIPS register $s4.
   */
  T_eq_u32( (uint32_t) context_executing.s4, 0x15161718 );

  /*
   * Check that the switch saved the MIPS register $s5.
   */
  T_eq_u32( (uint32_t) context_executing.s5, 0x16171819 );

  /*
   * Check that the switch saved the MIPS register $s6.
   */
  T_eq_u32( (uint32_t) context_executing.s6, 0x1718191a );

  /*
   * Check that the switch saved the MIPS register $s7.
   */
  T_eq_u32( (uint32_t) context_executing.s7, 0x18191a1b );

  /*
   * Check that the switch saved the MIPS register $f20.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f20, 0x31323334 );
  #endif

  /*
   * Check that the switch saved the MIPS register $f21.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f21, 0x32333435 );
  #endif

  /*
   * Check that the switch saved the MIPS register $f22.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f22, 0x33343536 );
  #endif

  /*
   * Check that the switch saved the MIPS register $f23.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f23, 0x34353637 );
  #endif

  /*
   * Check that the switch saved the MIPS register $f24.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f24, 0x35363738 );
  #endif

  /*
   * Check that the switch saved the MIPS register $f25.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f25, 0x36373839 );
  #endif

  /*
   * Check that the switch saved the MIPS register $f26.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f26, 0x3738393a );
  #endif

  /*
   * Check that the switch saved the MIPS register $f27.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f27, 0x38393a3b );
  #endif

  /*
   * Check that the switch saved the MIPS register $f28.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f28, 0x393a3b3c );
  #endif

  /*
   * Check that the switch saved the MIPS register $f29.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f29, 0x3a3b3c3d );
  #endif

  /*
   * Check that the switch saved the MIPS register $f30.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f30, 0x3b3c3d3e );
  #endif

  /*
   * Check that the switch saved the MIPS register $f31.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.f31, 0x3c3d3e3f );
  #endif

  /*
   * Check that the switch saved the rounding mode of the MIPS register fcr31.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) context_executing.fcr31 & 0x3, CTX_FCSR & 0x3 );
  #endif

  /*
   * Check that the switch saved the MIPS register $fp.  The register is
   * call-saved and the compiler can hold a frame of its own in it.  The action
   * therefore reads the value of the caller rather than a pattern.
   */
  T_eq_u32( (uint32_t) context_executing.fp, context_fp_before );

  /*
   * Check that the switch restored the MIPS register $fp.
   */
  T_eq_u32( context_fp_after, context_fp_before );

  /*
   * Check that the context initialization enables no trap of the MIPS register
   * fcr31.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( context_initial_fcr31 & 0xf80, 0 );
  #endif

  /*
   * Check that the context initialization sets the flush to zero flag of the
   * MIPS register fcr31.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( context_initial_fcr31 & 0x1000000, 0x1000000 );
  #endif

  /*
   * Check that the round trip reached the trap.  The heir returns through the
   * stack pointer of the caller.
   */
  T_eq_int( context_trap, 1 );

  /*
   * Check that the switch saved the return address of the caller.
   */
  T_eq_u32(
    (uint32_t) context_executing.ra,
    (uint32_t) (uintptr_t) frame->epc
  );
}

/**
 * @fn void T_case_body_ScoreCpuMipsValContext( void )
 */
T_TEST_CASE( ScoreCpuMipsValContext )
{
  ScoreCpuMipsValContext_Action_0();
}

/** @} */
