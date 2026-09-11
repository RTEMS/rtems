/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuMipsValResume
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
 * @defgroup ScoreCpuMipsValResume spec:/score/cpu/mips/val/resume
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Checks that the exception resume restores the registers of the
 *   exception frame.
 *
 * The frame holds 64 registers which the interrupted context owns, and the
 * action checks all 64.  It writes no pattern into the stack pointer, which
 * the resume computes from the address of the frame.  It writes none into 26
 * and 27, which carry the state of the resume.  It writes none into 0, which
 * reads as zero.  The cause and the bad virtual address report why the
 * exception happened, and no write of the processor accepts them.
 *
 * This test case performs the following actions:
 *
 * - Trap.  Let the handler write a new value into every MIPS user-visible
 *   register and every MIPS user-visible floating-point register of the frame.
 *   Resume, and trap again to capture what the resume restored.
 *
 *   - Check that both traps happened.  Without them the test proves nothing
 *     about the resume.
 *
 *   - Check that the resume restored the MIPS register $s0.
 *
 *   - Check that the resume restored the MIPS register $s1.
 *
 *   - Check that the resume restored the MIPS register $s2.
 *
 *   - Check that the resume restored the MIPS register $s3.
 *
 *   - Check that the resume restored the MIPS register $s4.
 *
 *   - Check that the resume restored the MIPS register $s5.
 *
 *   - Check that the resume restored the MIPS register $s6.
 *
 *   - Check that the resume restored the MIPS register $s7.
 *
 *   - Check that the resume restored the MIPS register $at.
 *
 *   - Check that the resume restored the MIPS register $v0.
 *
 *   - Check that the resume restored the MIPS register $v1.
 *
 *   - Check that the resume restored the MIPS register $a0.
 *
 *   - Check that the resume restored the MIPS register $a1.
 *
 *   - Check that the resume restored the MIPS register $a2.
 *
 *   - Check that the resume restored the MIPS register $a3.
 *
 *   - Check that the resume restored the MIPS register $t0.
 *
 *   - Check that the resume restored the MIPS register $t1.
 *
 *   - Check that the resume restored the MIPS register $t2.
 *
 *   - Check that the resume restored the MIPS register $t3.
 *
 *   - Check that the resume restored the MIPS register $t4.
 *
 *   - Check that the resume restored the MIPS register $t5.
 *
 *   - Check that the resume restored the MIPS register $t6.
 *
 *   - Check that the resume restored the MIPS register $t7.
 *
 *   - Check that the resume restored the MIPS register $t8.
 *
 *   - Check that the resume restored the MIPS register $t9.
 *
 *   - Check that the resume restored the MIPS register $fp.
 *
 *   - Check that the resume restored the MIPS register $ra.
 *
 *   - Check that the resume restored the MIPS register $gp.  The stub between
 *     the two traps reports it, because it puts the value of the compiler back
 *     before the second trap.
 *
 *   - Check that the resume restored the MIPS register lo.
 *
 *   - Check that the resume restored the MIPS register hi.
 *
 *   - Check that the resume restored the MIPS register $f0.
 *
 *   - Check that the resume restored the MIPS register $f1.
 *
 *   - Check that the resume restored the MIPS register $f2.
 *
 *   - Check that the resume restored the MIPS register $f3.
 *
 *   - Check that the resume restored the MIPS register $f4.
 *
 *   - Check that the resume restored the MIPS register $f5.
 *
 *   - Check that the resume restored the MIPS register $f6.
 *
 *   - Check that the resume restored the MIPS register $f7.
 *
 *   - Check that the resume restored the MIPS register $f8.
 *
 *   - Check that the resume restored the MIPS register $f9.
 *
 *   - Check that the resume restored the MIPS register $f10.
 *
 *   - Check that the resume restored the MIPS register $f11.
 *
 *   - Check that the resume restored the MIPS register $f12.
 *
 *   - Check that the resume restored the MIPS register $f13.
 *
 *   - Check that the resume restored the MIPS register $f14.
 *
 *   - Check that the resume restored the MIPS register $f15.
 *
 *   - Check that the resume restored the MIPS register $f16.
 *
 *   - Check that the resume restored the MIPS register $f17.
 *
 *   - Check that the resume restored the MIPS register $f18.
 *
 *   - Check that the resume restored the MIPS register $f19.
 *
 *   - Check that the resume restored the MIPS register $f20.
 *
 *   - Check that the resume restored the MIPS register $f21.
 *
 *   - Check that the resume restored the MIPS register $f22.
 *
 *   - Check that the resume restored the MIPS register $f23.
 *
 *   - Check that the resume restored the MIPS register $f24.
 *
 *   - Check that the resume restored the MIPS register $f25.
 *
 *   - Check that the resume restored the MIPS register $f26.
 *
 *   - Check that the resume restored the MIPS register $f27.
 *
 *   - Check that the resume restored the MIPS register $f28.
 *
 *   - Check that the resume restored the MIPS register $f29.
 *
 *   - Check that the resume restored the MIPS register $f30.
 *
 *   - Check that the resume restored the MIPS register $f31.
 *
 *   - Check that the resume restored the rounding mode of the MIPS register
 *     fcr31.
 *
 *   - Check that the resume restored the MIPS status register.  The second
 *     trap reports the register which the first resume installed.
 *
 *   - Check that the second frame lies where the first one lay.  The resume
 *     adds the size of the frame to the address of the frame.  The stack of
 *     the interrupted context therefore does not grow with each exception.
 *
 *   - Check that the frame of the first trap reports the address of the
 *     trapping instruction.
 *
 *   - Check that the frame of the first trap lies on the stack of the
 *     interrupted thread.  One processor runs this test, so that stack is the
 *     stack of the processor the handler runs on.
 *
 * @{
 */

extern const char resume_trap1_label[];

extern const char resume_trap2_label[];

extern const char resume_done_label[];

static CPU_Exception_frame resume_before;

static CPU_Exception_frame resume_after;

/* The address of the frame of each trap, which the handler receives. */
static const CPU_Exception_frame *resume_frame[ 2 ];

static int resume_trap;

/*
 * The global pointer of the C code.  The resume installs a pattern in it,
 * so the stub between the two traps puts the true value back.  The first
 * word takes the pattern, the second holds the true value and the third
 * holds the value which $t0 carries into the second trap.
 */
uint32_t resume_probe[ 3 ];

/*
 * The rounding mode of the control and status register of coprocessor 1.
 * The value rounds toward minus infinity, which no other part of the test
 * selects.  The flush to zero flag keeps the setting of the port.
 */
#define RESUME_FCSR 0x01000003

void ResumeRun( void );

/*
 * The two traps sit next to each other.  The compiler addresses a small
 * data object relative to the global pointer, so the C code of the handler
 * needs the true value of that register.
 */
__asm__( "  .text\n"
         "  .align 2\n"
         "  .globl ResumeRun\n"
         "  .set noreorder\n"
         "ResumeRun:\n"
         "  addiu $sp, $sp, -16\n"
         "  sw $ra, 8($sp)\n"
         "  .globl resume_trap1_label\n"
         "resume_trap1_label:\n"
         "  .word -1\n"
         "  .globl resume_trap2_label\n"
         "resume_trap2_label:\n"
         "  la $t0, resume_probe\n"
         "  sw $gp, 0($t0)\n"
         "  lw $gp, 4($t0)\n"
         "  lw $t0, 8($t0)\n"
         "  .word -1\n"
         "  .globl resume_done_label\n"
         "resume_done_label:\n"
         "  lw $ra, 8($sp)\n"
         "  jr $ra\n"
         "  addiu $sp, $sp, 16\n"
         "  .set reorder\n" );

/*
 * The handler writes a new value into every register of the frame, then
 * resumes.  The second trap captures what the resume restored.
 */
static void ResumeFatal(
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
  ++resume_trap;
  resume_frame[ resume_trap - 1 ] = frame;

  if ( resume_trap == 1 ) {
    resume_before = *frame;

    frame->s0 = 0x41424344;
    frame->s1 = 0x42434445;
    frame->s2 = 0x43444546;
    frame->s3 = 0x44454647;
    frame->s4 = 0x45464748;
    frame->s5 = 0x46474849;
    frame->s6 = 0x4748494a;
    frame->s7 = 0x48494a4b;
    frame->at = 0x51525354;
    frame->v0 = 0x52535455;
    frame->v1 = 0x53545556;
    frame->a0 = 0x54555657;
    frame->a1 = 0x55565758;
    frame->a2 = 0x56575859;
    frame->a3 = 0x5758595a;
    frame->t0 = 0x58595a5b;
    frame->t1 = 0x595a5b5c;
    frame->t2 = 0x5a5b5c5d;
    frame->t3 = 0x5b5c5d5e;
    frame->t4 = 0x5c5d5e5f;
    frame->t5 = 0x5d5e5f60;
    frame->t6 = 0x5e5f6061;
    frame->t7 = 0x5f606162;
    frame->t8 = 0x60616263;
    frame->t9 = 0x61626364;
    frame->fp = 0x62636465;
    frame->ra = 0x63646566;
    frame->gp = 0x64656667;
    frame->mdlo = 0x71727374;
    frame->mdhi = 0x72737475;

#if MIPS_HAS_FPU == 1
    {
      int i;

      for ( i = 0; i < 32; ++i ) {
        ( &frame->f0 )[ i ] = 0x61626364 +
                              (__MIPS_FPU_REGISTER_TYPE) i * 0x01010101;
      }

      frame->fcsr = RESUME_FCSR;
    }
#endif

    frame->epc = (__MIPS_REGISTER_TYPE) (uintptr_t) resume_trap2_label;
  } else {
    resume_after = *frame;
    SetFatalHandler( NULL, NULL );

    /*
     * The action continues in the stub with whatever the resume restored.
     * Put the whole frame back, so the compiler finds the registers it
     * expects.  Only the exception program counter moves on.
     */
    *frame = resume_before;
    frame->epc = (__MIPS_REGISTER_TYPE) (uintptr_t) resume_done_label;
  }

  _CPU_Exception_resume( frame );
}

static const CPU_Exception_frame *ResumeRunTwoTraps( void )
{
  uint32_t gp;

  resume_trap = 0;
  memset( &resume_after, 0, sizeof( resume_after ) );
  __asm__ volatile( "move %0, $gp"
                    : "=r"( gp ) );
  resume_probe[ 0 ] = 0;
  resume_probe[ 1 ] = gp;
  resume_probe[ 2 ] = 0x58595a5b;
  SetFatalHandler( ResumeFatal, NULL );
  ResumeRun();
  SetFatalHandler( NULL, NULL );

  return &resume_after;
}

/**
 * @brief Trap.  Let the handler write a new value into every MIPS user-visible
 *   register and every MIPS user-visible floating-point register of the frame.
 *   Resume, and trap again to capture what the resume restored.
 */
static void ScoreCpuMipsValResume_Action_0( void )
{
  const CPU_Exception_frame *after;

  after = ResumeRunTwoTraps();

  /*
   * Check that both traps happened.  Without them the test proves nothing
   * about the resume.
   */
  T_eq_int( resume_trap, 2 );

  /*
   * Check that the resume restored the MIPS register $s0.
   */
  T_eq_u32( (uint32_t) after->s0, 0x41424344 );

  /*
   * Check that the resume restored the MIPS register $s1.
   */
  T_eq_u32( (uint32_t) after->s1, 0x42434445 );

  /*
   * Check that the resume restored the MIPS register $s2.
   */
  T_eq_u32( (uint32_t) after->s2, 0x43444546 );

  /*
   * Check that the resume restored the MIPS register $s3.
   */
  T_eq_u32( (uint32_t) after->s3, 0x44454647 );

  /*
   * Check that the resume restored the MIPS register $s4.
   */
  T_eq_u32( (uint32_t) after->s4, 0x45464748 );

  /*
   * Check that the resume restored the MIPS register $s5.
   */
  T_eq_u32( (uint32_t) after->s5, 0x46474849 );

  /*
   * Check that the resume restored the MIPS register $s6.
   */
  T_eq_u32( (uint32_t) after->s6, 0x4748494a );

  /*
   * Check that the resume restored the MIPS register $s7.
   */
  T_eq_u32( (uint32_t) after->s7, 0x48494a4b );

  /*
   * Check that the resume restored the MIPS register $at.
   */
  T_eq_u32( (uint32_t) after->at, 0x51525354 );

  /*
   * Check that the resume restored the MIPS register $v0.
   */
  T_eq_u32( (uint32_t) after->v0, 0x52535455 );

  /*
   * Check that the resume restored the MIPS register $v1.
   */
  T_eq_u32( (uint32_t) after->v1, 0x53545556 );

  /*
   * Check that the resume restored the MIPS register $a0.
   */
  T_eq_u32( (uint32_t) after->a0, 0x54555657 );

  /*
   * Check that the resume restored the MIPS register $a1.
   */
  T_eq_u32( (uint32_t) after->a1, 0x55565758 );

  /*
   * Check that the resume restored the MIPS register $a2.
   */
  T_eq_u32( (uint32_t) after->a2, 0x56575859 );

  /*
   * Check that the resume restored the MIPS register $a3.
   */
  T_eq_u32( (uint32_t) after->a3, 0x5758595a );

  /*
   * Check that the resume restored the MIPS register $t0.
   */
  T_eq_u32( (uint32_t) after->t0, 0x58595a5b );

  /*
   * Check that the resume restored the MIPS register $t1.
   */
  T_eq_u32( (uint32_t) after->t1, 0x595a5b5c );

  /*
   * Check that the resume restored the MIPS register $t2.
   */
  T_eq_u32( (uint32_t) after->t2, 0x5a5b5c5d );

  /*
   * Check that the resume restored the MIPS register $t3.
   */
  T_eq_u32( (uint32_t) after->t3, 0x5b5c5d5e );

  /*
   * Check that the resume restored the MIPS register $t4.
   */
  T_eq_u32( (uint32_t) after->t4, 0x5c5d5e5f );

  /*
   * Check that the resume restored the MIPS register $t5.
   */
  T_eq_u32( (uint32_t) after->t5, 0x5d5e5f60 );

  /*
   * Check that the resume restored the MIPS register $t6.
   */
  T_eq_u32( (uint32_t) after->t6, 0x5e5f6061 );

  /*
   * Check that the resume restored the MIPS register $t7.
   */
  T_eq_u32( (uint32_t) after->t7, 0x5f606162 );

  /*
   * Check that the resume restored the MIPS register $t8.
   */
  T_eq_u32( (uint32_t) after->t8, 0x60616263 );

  /*
   * Check that the resume restored the MIPS register $t9.
   */
  T_eq_u32( (uint32_t) after->t9, 0x61626364 );

  /*
   * Check that the resume restored the MIPS register $fp.
   */
  T_eq_u32( (uint32_t) after->fp, 0x62636465 );

  /*
   * Check that the resume restored the MIPS register $ra.
   */
  T_eq_u32( (uint32_t) after->ra, 0x63646566 );

  /*
   * Check that the resume restored the MIPS register $gp.  The stub between
   * the two traps reports it, because it puts the value of the compiler back
   * before the second trap.
   */
  T_eq_u32( resume_probe[ 0 ], 0x64656667 );

  /*
   * Check that the resume restored the MIPS register lo.
   */
  T_eq_u32( (uint32_t) after->mdlo, 0x71727374 );

  /*
   * Check that the resume restored the MIPS register hi.
   */
  T_eq_u32( (uint32_t) after->mdhi, 0x72737475 );

  /*
   * Check that the resume restored the MIPS register $f0.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f0, 0x61626364 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f1.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f1, 0x62636465 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f2.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f2, 0x63646566 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f3.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f3, 0x64656667 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f4.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f4, 0x65666768 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f5.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f5, 0x66676869 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f6.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f6, 0x6768696a );
  #endif

  /*
   * Check that the resume restored the MIPS register $f7.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f7, 0x68696a6b );
  #endif

  /*
   * Check that the resume restored the MIPS register $f8.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f8, 0x696a6b6c );
  #endif

  /*
   * Check that the resume restored the MIPS register $f9.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f9, 0x6a6b6c6d );
  #endif

  /*
   * Check that the resume restored the MIPS register $f10.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f10, 0x6b6c6d6e );
  #endif

  /*
   * Check that the resume restored the MIPS register $f11.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f11, 0x6c6d6e6f );
  #endif

  /*
   * Check that the resume restored the MIPS register $f12.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f12, 0x6d6e6f70 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f13.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f13, 0x6e6f7071 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f14.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f14, 0x6f707172 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f15.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f15, 0x70717273 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f16.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f16, 0x71727374 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f17.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f17, 0x72737475 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f18.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f18, 0x73747576 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f19.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f19, 0x74757677 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f20.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f20, 0x75767778 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f21.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f21, 0x76777879 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f22.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f22, 0x7778797a );
  #endif

  /*
   * Check that the resume restored the MIPS register $f23.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f23, 0x78797a7b );
  #endif

  /*
   * Check that the resume restored the MIPS register $f24.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f24, 0x797a7b7c );
  #endif

  /*
   * Check that the resume restored the MIPS register $f25.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f25, 0x7a7b7c7d );
  #endif

  /*
   * Check that the resume restored the MIPS register $f26.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f26, 0x7b7c7d7e );
  #endif

  /*
   * Check that the resume restored the MIPS register $f27.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f27, 0x7c7d7e7f );
  #endif

  /*
   * Check that the resume restored the MIPS register $f28.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f28, 0x7d7e7f80 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f29.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f29, 0x7e7f8081 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f30.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f30, 0x7f808182 );
  #endif

  /*
   * Check that the resume restored the MIPS register $f31.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->f31, 0x80818283 );
  #endif

  /*
   * Check that the resume restored the rounding mode of the MIPS register
   * fcr31.
   */
  #if MIPS_HAS_FPU == 1
  T_eq_u32( (uint32_t) after->fcsr & 0x3, RESUME_FCSR & 0x3 );
  #endif

  /*
   * Check that the resume restored the MIPS status register.  The second trap
   * reports the register which the first resume installed.
   */
  T_eq_u32( (uint32_t) after->c0_sr, (uint32_t) resume_before.c0_sr );

  /*
   * Check that the second frame lies where the first one lay.  The resume adds
   * the size of the frame to the address of the frame.  The stack of the
   * interrupted context therefore does not grow with each exception.
   */
  T_eq_ptr( resume_frame[ 1 ], resume_frame[ 0 ] );

  /*
   * Check that the frame of the first trap reports the address of the trapping
   * instruction.
   */
  T_eq_u32(
    (uint32_t) resume_before.epc,
    (uint32_t) (uintptr_t) resume_trap1_label
  );

  /*
   * Check that the frame of the first trap lies on the stack of the
   * interrupted thread.  One processor runs this test, so that stack is the
   * stack of the processor the handler runs on.
   */
  {
    const Thread_Control *executing;
    uintptr_t             area;

    executing = _Thread_Get_executing();
    area = (uintptr_t) executing->Start.Initial_stack.area;
    T_true( (uintptr_t) resume_frame[ 0 ] >= area );
    T_true(
      (uintptr_t) resume_frame[ 0 ] <
      area + executing->Start.Initial_stack.size
    );
  }
}

/**
 * @fn void T_case_body_ScoreCpuMipsValResume( void )
 */
T_TEST_CASE( ScoreCpuMipsValResume )
{
  ScoreCpuMipsValResume_Action_0();
}

/** @} */
