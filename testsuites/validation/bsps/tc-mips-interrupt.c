/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuMipsValInterrupt
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

#include <rtems.h>
#include <stdint.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/mips.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuMipsValInterrupt spec:/score/cpu/mips/val/interrupt
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Checks that interrupt processing preserves the registers of the
 *   interrupted context.
 *
 * The action needs a BSP which supplies the TM27 support.  A BSP without it
 * raises no interrupt.
 *
 * The action reads every register of its checks out of the exception frame, so
 * each check also observes the save of the exception path.  The set of a MIPS
 * wrapper-private register is empty on this architecture, so no check covers
 * one.
 *
 * This test case performs the following actions:
 *
 * - Load a distinct pattern into every MIPS call-used register and every MIPS
 *   call-used floating-point register.  Raise the interrupt.  Trap, so the
 *   frame captures every register.
 *
 *   - Check that the interrupt happened.  Without it the test proves nothing
 *     about interrupt processing.
 *
 *   - Check that the interrupt preserved the MIPS register $at.
 *
 *   - Check that the interrupt preserved the MIPS register $v0.
 *
 *   - Check that the interrupt preserved the MIPS register $v1.
 *
 *   - Check that the interrupt preserved the MIPS register $a0.
 *
 *   - Check that the interrupt preserved the MIPS register $a1.
 *
 *   - Check that the interrupt preserved the MIPS register $a2.
 *
 *   - Check that the interrupt preserved the MIPS register $a3.
 *
 *   - Check that the interrupt preserved the MIPS register $t0.
 *
 *   - Check that the interrupt preserved the MIPS register $t1.
 *
 *   - Check that the interrupt preserved the MIPS register $t2.
 *
 *   - Check that the interrupt preserved the MIPS register $t3.
 *
 *   - Check that the interrupt preserved the MIPS register $t4.
 *
 *   - Check that the interrupt preserved the MIPS register $t5.
 *
 *   - Check that the interrupt preserved the MIPS register $t6.
 *
 *   - Check that the interrupt preserved the MIPS register $t7.
 *
 *   - Check that the interrupt preserved the MIPS register $t8.
 *
 *   - Check that the interrupt preserved the MIPS register $t9.
 *
 *   - Check that the interrupt preserved the MIPS register lo.
 *
 *   - Check that the interrupt preserved the MIPS register hi.
 *
 *   - Check that the interrupt preserved the MIPS register $f0.
 *
 *   - Check that the interrupt preserved the MIPS register $f1.
 *
 *   - Check that the interrupt preserved the MIPS register $f2.
 *
 *   - Check that the interrupt preserved the MIPS register $f3.
 *
 *   - Check that the interrupt preserved the MIPS register $f4.
 *
 *   - Check that the interrupt preserved the MIPS register $f5.
 *
 *   - Check that the interrupt preserved the MIPS register $f6.
 *
 *   - Check that the interrupt preserved the MIPS register $f7.
 *
 *   - Check that the interrupt preserved the MIPS register $f8.
 *
 *   - Check that the interrupt preserved the MIPS register $f9.
 *
 *   - Check that the interrupt preserved the MIPS register $f10.
 *
 *   - Check that the interrupt preserved the MIPS register $f11.
 *
 *   - Check that the interrupt preserved the MIPS register $f12.
 *
 *   - Check that the interrupt preserved the MIPS register $f13.
 *
 *   - Check that the interrupt preserved the MIPS register $f14.
 *
 *   - Check that the interrupt preserved the MIPS register $f15.
 *
 *   - Check that the interrupt preserved the MIPS register $f16.
 *
 *   - Check that the interrupt preserved the MIPS register $f17.
 *
 *   - Check that the interrupt preserved the MIPS register $f18.
 *
 *   - Check that the interrupt preserved the MIPS register $f19.
 *
 *   - Check that the interrupt preserved the rounding mode of the MIPS
 *     register fcr31.
 *
 *   - Check that the interrupt preserved the interrupt enable of the MIPS
 *     status register.
 *
 * @{
 */

#define _RTEMS_TMTEST27
#include <tm27.h>

extern const char interrupt_done_label[];

static CPU_Exception_frame interrupt_frame;

volatile int interrupt_count;

static CallWithinISRRequest interrupt_request;

static rtems_interrupt_level interrupt_level;

/*
 * The rounding mode of the control and status register of coprocessor 1.
 * The value rounds toward minus infinity, which no other part of the test
 * selects.  The flush to zero flag keeps the setting of the port.
 */
#define ISR_FCSR 0x01000003

/*
 * The bit of the status register which carries the interrupt enable of the
 * interrupted context.  A MIPS I exception shifts the three-deep enable
 * stack of the register, so the previous bit holds the value.  A MIPS III
 * and a MIPS32 exception raise the exception level and leave the enable
 * alone.
 */
#if __mips == 1
#define ISR_SR_ENABLE SR_IEP
#else
#define ISR_SR_ENABLE SR_IE
#endif

#define ISR_LOAD         \
  "li $2, 0x72737475\n"  \
  "li $3, 0x73747576\n"  \
  "li $4, 0x74757677\n"  \
  "li $5, 0x75767778\n"  \
  "li $6, 0x76777879\n"  \
  "li $7, 0x7778797a\n"  \
  "li $8, 0x78797a7b\n"  \
  "li $9, 0x797a7b7c\n"  \
  "li $10, 0x7a7b7c7d\n" \
  "li $11, 0x7b7c7d7e\n" \
  "li $12, 0x7c7d7e7f\n" \
  "li $13, 0x7d7e7f80\n" \
  "li $14, 0x7e7f8081\n" \
  "li $15, 0x7f808182\n" \
  "li $24, 0x80818283\n" \
  "li $25, 0x81828384\n" \
  "li $27, 0x82838485\n" \
  "mtlo $27\n"           \
  "li $27, 0x83848586\n" \
  "mthi $27\n"

#if MIPS_HAS_FPU == 1
#define ISR_LOAD_FPU                        \
  "li $27, 0x91929394\n"                    \
  "mtc1 $27, $f0\n"                         \
  "li $27, 0x91939495\n"                    \
  "mtc1 $27, $f1\n"                         \
  "li $27, 0x91949596\n"                    \
  "mtc1 $27, $f2\n"                         \
  "li $27, 0x91959697\n"                    \
  "mtc1 $27, $f3\n"                         \
  "li $27, 0x91969798\n"                    \
  "mtc1 $27, $f4\n"                         \
  "li $27, 0x91979899\n"                    \
  "mtc1 $27, $f5\n"                         \
  "li $27, 0x9198999a\n"                    \
  "mtc1 $27, $f6\n"                         \
  "li $27, 0x91999a9b\n"                    \
  "mtc1 $27, $f7\n"                         \
  "li $27, 0x919a9b9c\n"                    \
  "mtc1 $27, $f8\n"                         \
  "li $27, 0x919b9c9d\n"                    \
  "mtc1 $27, $f9\n"                         \
  "li $27, 0x919c9d9e\n"                    \
  "mtc1 $27, $f10\n"                        \
  "li $27, 0x919d9e9f\n"                    \
  "mtc1 $27, $f11\n"                        \
  "li $27, 0x919e9fa0\n"                    \
  "mtc1 $27, $f12\n"                        \
  "li $27, 0x919fa0a1\n"                    \
  "mtc1 $27, $f13\n"                        \
  "li $27, 0x91a0a1a2\n"                    \
  "mtc1 $27, $f14\n"                        \
  "li $27, 0x91a1a2a3\n"                    \
  "mtc1 $27, $f15\n"                        \
  "li $27, 0x91a2a3a4\n"                    \
  "mtc1 $27, $f16\n"                        \
  "li $27, 0x91a3a4a5\n"                    \
  "mtc1 $27, $f17\n"                        \
  "li $27, 0x91a4a5a6\n"                    \
  "mtc1 $27, $f18\n"                        \
  "li $27, 0x91a5a6a7\n"                    \
  "mtc1 $27, $f19\n"                        \
  "li $27, " RTEMS_XSTRING( ISR_FCSR ) "\n" \
                                       "ctc1 $27, $31\n"
#else
#define ISR_LOAD_FPU
#endif

static void InterruptHandler( void *arg )
{
  (void) arg;
  ++interrupt_count;
}

/*
 * The trap after the interrupt captures every register.  The frame is the
 * store-out, so no register under test addresses it.
 */
static void InterruptFatal(
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
  interrupt_frame = *frame;
  SetFatalHandler( NULL, NULL );
  frame->epc = (__MIPS_REGISTER_TYPE) (uintptr_t) interrupt_done_label;
  _CPU_Exception_resume( frame );
}

/**
 * @brief Load a distinct pattern into every MIPS call-used register and every
 *   MIPS call-used floating-point register.  Raise the interrupt.  Trap, so
 *   the frame captures every register.
 */
static void ScoreCpuMipsValInterrupt_Action_0( void )
{
  const CPU_Exception_frame *frame;

  interrupt_count = 0;
  interrupt_request.handler = InterruptHandler;
  interrupt_request.arg = NULL;
  SetFatalHandler( InterruptFatal, NULL );

  /*
   * The interrupt is raised while it is masked, so no call happens after the
   * block loads the registers.  The block unmasks it and waits for it.  The
   * wait reads the counter through $k0 and $k1, which belong to the operating
   * system and which the interrupt wrapper owns.
   */
  rtems_interrupt_local_disable( interrupt_level );
  CallWithinISRSubmit( &interrupt_request );
  __asm__ volatile( "  .set push\n"
                    "  .set noreorder\n" ISR_LOAD_FPU ISR_LOAD "  .set noat\n"
                    "  li $1, 0x71727374\n"
                    "  mfc0 $26, $12\n"
                    "  nop\n"
                    "  ori $26, $26, 1\n"
                    "  mtc0 $26, $12\n"
                    "  nop\n"
                    "1:\n"
                    "  la $27, interrupt_count\n"
                    "  lw $26, 0($27)\n"
                    "  beqz $26, 1b\n"
                    "  nop\n"
                    "  .word -1\n"
                    "  .globl interrupt_done_label\n"
                    "interrupt_done_label:\n"
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
                      "$24",
                      "$25",
                      "hi",
                      "lo" );
  rtems_interrupt_local_enable( interrupt_level );
  frame = &interrupt_frame;

  /*
   * Check that the interrupt happened.  Without it the test proves nothing
   * about interrupt processing.
   */
  T_eq_int( interrupt_count, 1 );

  /*
   * Check that the interrupt preserved the MIPS register $at.
   */
  T_eq_u32( (uint32_t) frame->at, 0x71727374 );

  /*
   * Check that the interrupt preserved the MIPS register $v0.
   */
  T_eq_u32( (uint32_t) frame->v0, 0x72737475 );

  /*
   * Check that the interrupt preserved the MIPS register $v1.
   */
  T_eq_u32( (uint32_t) frame->v1, 0x73747576 );

  /*
   * Check that the interrupt preserved the MIPS register $a0.
   */
  T_eq_u32( (uint32_t) frame->a0, 0x74757677 );

  /*
   * Check that the interrupt preserved the MIPS register $a1.
   */
  T_eq_u32( (uint32_t) frame->a1, 0x75767778 );

  /*
   * Check that the interrupt preserved the MIPS register $a2.
   */
  T_eq_u32( (uint32_t) frame->a2, 0x76777879 );

  /*
   * Check that the interrupt preserved the MIPS register $a3.
   */
  T_eq_u32( (uint32_t) frame->a3, 0x7778797a );

  /*
   * Check that the interrupt preserved the MIPS register $t0.
   */
  T_eq_u32( (uint32_t) frame->t0, 0x78797a7b );

  /*
   * Check that the interrupt preserved the MIPS register $t1.
   */
  T_eq_u32( (uint32_t) frame->t1, 0x797a7b7c );

  /*
   * Check that the interrupt preserved the MIPS register $t2.
   */
  T_eq_u32( (uint32_t) frame->t2, 0x7a7b7c7d );

  /*
   * Check that the interrupt preserved the MIPS register $t3.
   */
  T_eq_u32( (uint32_t) frame->t3, 0x7b7c7d7e );

  /*
   * Check that the interrupt preserved the MIPS register $t4.
   */
  T_eq_u32( (uint32_t) frame->t4, 0x7c7d7e7f );

  /*
   * Check that the interrupt preserved the MIPS register $t5.
   */
  T_eq_u32( (uint32_t) frame->t5, 0x7d7e7f80 );

  /*
   * Check that the interrupt preserved the MIPS register $t6.
   */
  T_eq_u32( (uint32_t) frame->t6, 0x7e7f8081 );

  /*
   * Check that the interrupt preserved the MIPS register $t7.
   */
  T_eq_u32( (uint32_t) frame->t7, 0x7f808182 );

  /*
   * Check that the interrupt preserved the MIPS register $t8.
   */
  T_eq_u32( (uint32_t) frame->t8, 0x80818283 );

  /*
   * Check that the interrupt preserved the MIPS register $t9.
   */
  T_eq_u32( (uint32_t) frame->t9, 0x81828384 );

  /*
   * Check that the interrupt preserved the MIPS register lo.
   */
  T_eq_u32( (uint32_t) frame->mdlo, 0x82838485 );

  /*
   * Check that the interrupt preserved the MIPS register hi.
   */
  T_eq_u32( (uint32_t) frame->mdhi, 0x83848586 );

  /*
   * Check that the interrupt preserved the MIPS register $f0.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f0, 0x91929394 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f1.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f1, 0x91939495 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f2.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f2, 0x91949596 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f3.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f3, 0x91959697 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f4.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f4, 0x91969798 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f5.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f5, 0x91979899 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f6.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f6, 0x9198999a );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f7.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f7, 0x91999a9b );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f8.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f8, 0x919a9b9c );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f9.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f9, 0x919b9c9d );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f10.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f10, 0x919c9d9e );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f11.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f11, 0x919d9e9f );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f12.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f12, 0x919e9fa0 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f13.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f13, 0x919fa0a1 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f14.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f14, 0x91a0a1a2 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f15.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f15, 0x91a1a2a3 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f16.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f16, 0x91a2a3a4 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f17.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f17, 0x91a3a4a5 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f18.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f18, 0x91a4a5a6 );
  #endif

  /*
   * Check that the interrupt preserved the MIPS register $f19.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->f19, 0x91a5a6a7 );
  #endif

  /*
   * Check that the interrupt preserved the rounding mode of the MIPS register
   * fcr31.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) frame->fcsr & 0x3, ISR_FCSR & 0x3 );
  #endif

  /*
   * Check that the interrupt preserved the interrupt enable of the MIPS status
   * register.
   */
  T_eq_u32( (uint32_t) frame->c0_sr & ISR_SR_ENABLE, ISR_SR_ENABLE );
}

/**
 * @fn void T_case_body_ScoreCpuMipsValInterrupt( void )
 */
T_TEST_CASE( ScoreCpuMipsValInterrupt )
{
  ScoreCpuMipsValInterrupt_Action_0();
}

/** @} */
