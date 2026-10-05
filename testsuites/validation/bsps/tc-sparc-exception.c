/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuSparcValException
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/cpuimpl.h>

#include "tr-sparc-exception-frame.h"
#include "tx-register-check.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuSparcValException spec:/score/cpu/sparc/val/exception
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the default exception handling.
 *
 * This test case performs the following actions:
 *
 * - Initialize the exception frame. Load a pattern into each register. Execute
 *   an illegal instruction. Save the exception frame. Continue after the
 *   trapping instruction. Run this through the register check, once for each
 *   value inverted, then once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Initialize the exception frame. Load a pattern into each register. Load a
 *   word from an address which is not aligned. Save the exception frame.
 *   Continue after the trapping instruction. Run this through the register
 *   check, once for each value inverted, then once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Initialize the exception frame. Load a pattern into each register. Add two
 *   values with a tag which is not zero and trap on overflow. Save the
 *   exception frame. Continue after the trapping instruction. Run this through
 *   the register check, once for each value inverted, then once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Initialize the exception frame. Load a pattern into each register. Divide
 *   by zero. Save the exception frame. Continue after the trapping
 *   instruction. Run this through the register check, once for each value
 *   inverted, then once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Initialize the exception frame. Load a pattern into each register. Execute
 *   a software trap with the trap number five. Save the exception frame.
 *   Continue after the trapping instruction. Run this through the register
 *   check, once for each value inverted, then once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Initialize the exception frame. Load a pattern into each register. Call a
 *   function at an address without memory. Save the exception frame. The call
 *   sets O7 to the address of the call. Continue after the delay slot of the
 *   call. Run this through the register check, once for each value inverted,
 *   then once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Initialize the exception frame. Load a pattern into each register. Clear
 *   PSR[S]. Read the PSR in user mode. Save the exception frame. Set PSR[PS]
 *   in the exception frame, so that the scenario continues in supervisor mode.
 *   Continue after the trapping instruction. Run this through the register
 *   check, once for each value inverted, then once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Initialize the exception frame. Load a pattern into each register. Load a
 *   word from an address without memory. Save the exception frame. The
 *   scenario loads the address into G2. Continue after the trapping
 *   instruction. Run this through the register check, once for each value
 *   inverted, then once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Initialize the exception frame. Load a pattern into each register. Execute
 *   a coprocessor operate instruction. Save the exception frame. Continue
 *   after the trapping instruction. Run this through the register check, once
 *   for each value inverted, then once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Initialize the exception frame. Load a pattern into each register. Clear
 *   PSR[EF]. Move a floating-point register. Save the exception frame. Set
 *   PSR[EF] in the exception frame. Continue after the trapping instruction.
 *   Run this through the register check, once for each value inverted, then
 *   once unchanged.
 *
 *   - Check that the exception frame has the expected trap type.
 *
 * - Execute each software trap which has no handler of its own, except the
 *   breakpoint trap 0x81. Count the traps for which the exception frame has
 *   the trap type of the software trap. Continue after each trapping
 *   instruction. Run this through the register check, once with the expected
 *   count inverted, then once unchanged.
 *
 *   - Check that the default handler of each software trap without a handler
 *     of its own set the trap member of the exception frame to the value 128
 *     plus the trap number.
 *
 *   - Check that the run with the changed value flagged exactly the check of
 *     that value, and that each run recorded the check.
 *
 * @{
 */

extern const char sparc_illegal_instruction_label[];

static void IllegalInstructionScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile( ASM_PREPARE ".globl sparc_illegal_instruction_label\n"
                                "sparc_illegal_instruction_label:\n"
                                "unimp 0\n" ASM_FINISH
                    :
                    :
                    : ASM_CLOBBER );
  exception_expected_pc = sparc_illegal_instruction_label;
}

extern const char sparc_mem_address_not_aligned_label[];

static void MemAddressNotAlignedScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile( ASM_PREPARE ".globl sparc_mem_address_not_aligned_label\n"
                                "sparc_mem_address_not_aligned_label:\n"
                                "ld [%%sp + 1], %%g0\n" ASM_FINISH
                    :
                    :
                    : ASM_CLOBBER );
  exception_expected_pc = sparc_mem_address_not_aligned_label;
}

extern const char sparc_tag_overflow_label[];

static void TagOverflowScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile( ASM_PREPARE ".globl sparc_tag_overflow_label\n"
                                "sparc_tag_overflow_label:\n"
                                "taddcctv %%g2, %%g3, %%g0\n" ASM_FINISH
                    :
                    :
                    : ASM_CLOBBER );
  exception_expected_pc = sparc_tag_overflow_label;
}

extern const char sparc_division_by_zero_label[];

static void DivisionByZeroScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile( ASM_PREPARE ".globl sparc_division_by_zero_label\n"
                                "sparc_division_by_zero_label:\n"
                                "udiv %%g1, %%g0, %%g0\n" ASM_FINISH
                    :
                    :
                    : ASM_CLOBBER );
  exception_expected_pc = sparc_division_by_zero_label;
}

extern const char sparc_software_trap_label[];

static void SoftwareTrapScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile( ASM_PREPARE ".globl sparc_software_trap_label\n"
                                "sparc_software_trap_label:\n"
                                "ta 5\n" ASM_FINISH
                    :
                    :
                    : ASM_CLOBBER );
  exception_expected_pc = sparc_software_trap_label;
}

extern const char sparc_instruction_access_exception_label[];

extern const char sparc_instruction_access_exception_resume[];
static void       InstructionAccessScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile(
    ASM_PREPARE
    ".globl sparc_instruction_access_exception_label\n"
    "sparc_instruction_access_exception_label:\n"
    "call 0xd0000000\n"
    "nop\n.globl sparc_instruction_access_exception_resume\nsparc_instruction_access_exception_resume:\n" ASM_FINISH
    :
    :
    : ASM_CLOBBER
  );
  exception_expected_pc = (const void *) 0xd0000000;
}

static const ExceptionFrameVariant instruction_access_variant = {
  .resume = sparc_instruction_access_exception_resume,
  .override_count = 1,
  .overrides = {
    { EXCEPTION_FRAME_SOURCE_O7, sparc_instruction_access_exception_label }
  }
};

extern const char sparc_privileged_instruction_label[];

static void PrivilegedInstructionScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile(
    ASM_PREPARE_CLEAR( "0x80" ) ".globl sparc_privileged_instruction_label\n"
                                "sparc_privileged_instruction_label:\n"
                                "rd %%psr, %%g0\n" ASM_FINISH
    :
    :
    : ASM_CLOBBER
  );
  exception_expected_pc = sparc_privileged_instruction_label;
}

static const ExceptionFrameVariant privileged_instruction_variant = {
  .psr_set = SPARC_PSR_PS_MASK
};

extern const char sparc_data_access_exception_label[];

static void DataAccessScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile( ASM_PREPARE "set 0xd0000000, %%g2\n"
                                ".globl sparc_data_access_exception_label\n"
                                "sparc_data_access_exception_label:\n"
                                "ld [%%g2], %%g0\n" ASM_FINISH
                    :
                    :
                    : ASM_CLOBBER );
  exception_expected_pc = sparc_data_access_exception_label;
}

static const ExceptionFrameVariant data_access_variant = {
  .override_count = 1,
  .overrides = { { EXCEPTION_FRAME_SOURCE_G2, (const void *) 0xd0000000 } }
};

extern const char sparc_cp_disabled_label[];

static void CpDisabledScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile( ASM_PREPARE ".globl sparc_cp_disabled_label\n"
                                "sparc_cp_disabled_label:\n"
                                ".word 0x81b00000\n" ASM_FINISH
                    :
                    :
                    : ASM_CLOBBER );
  exception_expected_pc = sparc_cp_disabled_label;
}

#if defined( RTEMS_SMP ) && SPARC_HAS_FPU == 1
extern const char sparc_fp_disabled_label[];

static void FpDisabledScenario( void )
{
  (void) ExceptionFrameInitialize();
  __asm__ volatile(
    ASM_PREPARE_CLEAR( "0x1000" ) ".globl sparc_fp_disabled_label\n"
                                  "sparc_fp_disabled_label:\n"
                                  "fmovs %%f0, %%f0\n" ASM_FINISH
    :
    :
    : ASM_CLOBBER
  );
  exception_expected_pc = sparc_fp_disabled_label;
}

static const ExceptionFrameVariant fp_disabled_variant = {
  .psr_set = SPARC_PSR_EF_MASK
};
#endif

/*
 * The software traps 0x80, 0x83, 0x89, and 0x8a have handlers of their own.
 * In SMP configurations, the software trap 0x8b has a handler of its own.  The
 * debug support unit of the GR740 stops the processor at the breakpoint trap
 * 0x81.
 */
static bool IsSkippedSoftwareTrap( uint32_t trap )
{
  switch ( trap ) {
    case 0x80:
    case 0x81:
    case 0x83:
    case 0x89:
    case 0x8a:
#if defined( SPARC_USE_SYNCHRONOUS_FP_SWITCH )
    case 0x8b:
#endif
      return true;
    default:
      return false;
  }
}

static uint32_t software_trap_expected;

static uint32_t software_trap_count;

static void SoftwareTrapFatal(
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

  if ( frame->trap == software_trap_expected ) {
    ++software_trap_count;
  }

  /* Continue after the trapping instruction */
  frame->pc = frame->npc;
  frame->npc = frame->pc + 4;
  _CPU_Exception_resume( frame );
}

typedef enum {
  SOFTWARE_TRAP_SOURCE_COUNT,
  SOFTWARE_TRAP_SOURCE_MAX
} SoftwareTrapSource;

typedef enum {
  SOFTWARE_TRAP_SLOT_COUNT,
  SOFTWARE_TRAP_SLOT_MAX
} SoftwareTrapSlot;

static const RegisterCheckSource software_trap_sources[] = {
  { "count", REGISTER_CHECK_EXPECTED, UINT64_C( 0xffffffff ) }
};

static const RegisterCheckSlot software_trap_slots[] = {
  { "count", SOFTWARE_TRAP_SOURCE_COUNT, UINT64_C( 0xffffffff ) }
};

static uint64_t software_trap_initial[ SOFTWARE_TRAP_SOURCE_MAX ];

/*
 * The run executes each software trap which it does not skip.  It
 * counts the traps for which the exception frame has the trap type of the
 * software trap.
 */
static void SoftwareTrapRun( RegisterCheck *self, void *arg )
{
  uint32_t trap;
  uint32_t expected;

  (void) arg;
  software_trap_count = 0;
  expected = 0;

  for ( trap = 0x80; trap <= 0xff; ++trap ) {
    if ( IsSkippedSoftwareTrap( trap ) ) {
      continue;
    }

    ++expected;
    software_trap_expected = trap;
    SetFatalHandler( SoftwareTrapFatal, NULL );
    __asm__ volatile( "ta %0"
                      :
                      : "r"( trap - 0x80 )
                      : "memory" );
    SetFatalHandler( NULL, NULL );
  }

  RegisterCheckRecord(
    self,
    SOFTWARE_TRAP_SLOT_COUNT,
    software_trap_count,
    expected
  );
}

static RegisterCheck software_trap_check = {
  .sources = software_trap_sources,
  .source_count = SOFTWARE_TRAP_SOURCE_MAX,
  .slots = software_trap_slots,
  .slot_count = SOFTWARE_TRAP_SLOT_MAX,
  .initial = software_trap_initial,
  .run = SoftwareTrapRun
};

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Execute an illegal instruction. Save the exception frame. Continue after
 *   the trapping instruction. Run this through the register check, once for
 *   each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_0( void )
{
  ScoreCpuSparcValExceptionFrame_Run( IllegalInstructionScenario, 0x02, NULL );

  /*
   * Check that the exception frame has the expected trap type.
   */
  ExceptionFrameCheckTrap();
}

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Load a word from an address which is not aligned. Save the exception
 *   frame. Continue after the trapping instruction. Run this through the
 *   register check, once for each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_1( void )
{
  ScoreCpuSparcValExceptionFrame_Run(
    MemAddressNotAlignedScenario,
    0x07,
    NULL
  );

  /*
   * Check that the exception frame has the expected trap type.
   */
  ExceptionFrameCheckTrap();
}

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Add two values with a tag which is not zero and trap on overflow. Save the
 *   exception frame. Continue after the trapping instruction. Run this through
 *   the register check, once for each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_2( void )
{
  ScoreCpuSparcValExceptionFrame_Run( TagOverflowScenario, 0x0a, NULL );

  /*
   * Check that the exception frame has the expected trap type.
   */
  ExceptionFrameCheckTrap();
}

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Divide by zero. Save the exception frame. Continue after the trapping
 *   instruction. Run this through the register check, once for each value
 *   inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_3( void )
{
  ScoreCpuSparcValExceptionFrame_Run( DivisionByZeroScenario, 0x2a, NULL );

  /*
   * Check that the exception frame has the expected trap type.
   */
  ExceptionFrameCheckTrap();
}

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Execute a software trap with the trap number five. Save the exception
 *   frame. Continue after the trapping instruction. Run this through the
 *   register check, once for each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_4( void )
{
  ScoreCpuSparcValExceptionFrame_Run( SoftwareTrapScenario, 0x85, NULL );

  /*
   * Check that the exception frame has the expected trap type.
   */
  ExceptionFrameCheckTrap();
}

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Call a function at an address without memory. Save the exception frame.
 *   The call sets O7 to the address of the call. Continue after the delay slot
 *   of the call. Run this through the register check, once for each value
 *   inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_5( void )
{
  ScoreCpuSparcValExceptionFrame_Run(
    InstructionAccessScenario,
    0x01,
    &instruction_access_variant
  );

  /*
   * Check that the exception frame has the expected trap type.
   */
  ExceptionFrameCheckTrap();
}

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Clear PSR[S]. Read the PSR in user mode. Save the exception frame. Set
 *   PSR[PS] in the exception frame, so that the scenario continues in
 *   supervisor mode. Continue after the trapping instruction. Run this through
 *   the register check, once for each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_6( void )
{
  ScoreCpuSparcValExceptionFrame_Run(
    PrivilegedInstructionScenario,
    0x03,
    &privileged_instruction_variant
  );

  /*
   * Check that the exception frame has the expected trap type.
   */
  ExceptionFrameCheckTrap();
}

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Load a word from an address without memory. Save the exception frame. The
 *   scenario loads the address into G2. Continue after the trapping
 *   instruction. Run this through the register check, once for each value
 *   inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_7( void )
{
  ScoreCpuSparcValExceptionFrame_Run(
    DataAccessScenario,
    0x09,
    &data_access_variant
  );

  /*
   * Check that the exception frame has the expected trap type.
   */
  ExceptionFrameCheckTrap();
}

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Execute a coprocessor operate instruction. Save the exception frame.
 *   Continue after the trapping instruction. Run this through the register
 *   check, once for each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_8( void )
{
  ScoreCpuSparcValExceptionFrame_Run( CpDisabledScenario, 0x24, NULL );

  /*
   * Check that the exception frame has the expected trap type.
   */
  ExceptionFrameCheckTrap();
}

/**
 * @brief Initialize the exception frame. Load a pattern into each register.
 *   Clear PSR[EF]. Move a floating-point register. Save the exception frame.
 *   Set PSR[EF] in the exception frame. Continue after the trapping
 *   instruction. Run this through the register check, once for each value
 *   inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_9( void )
{
  #if defined( RTEMS_SMP ) && SPARC_HAS_FPU == 1
  ScoreCpuSparcValExceptionFrame_Run(
    FpDisabledScenario,
    0x04,
    &fp_disabled_variant
  );
  #endif

  /*
   * Check that the exception frame has the expected trap type.
   */
  #if defined( RTEMS_SMP ) && SPARC_HAS_FPU == 1
  ExceptionFrameCheckTrap();
  #endif
}

/**
 * @brief Execute each software trap which has no handler of its own, except
 *   the breakpoint trap 0x81. Count the traps for which the exception frame
 *   has the trap type of the software trap. Continue after each trapping
 *   instruction. Run this through the register check, once with the expected
 *   count inverted, then once unchanged.
 */
static void ScoreCpuSparcValException_Action_10( void )
{
  RegisterCheckRun( &software_trap_check );

  /*
   * Check that the default handler of each software trap without a handler of
   * its own set the trap member of the exception frame to the value 128 plus
   * the trap number.
   */
  RegisterCheckVerify( &software_trap_check, SOFTWARE_TRAP_SLOT_COUNT );

  /*
   * Check that the run with the changed value flagged exactly the check of
   * that value, and that each run recorded the check.
   */
  RegisterCheckReport( &software_trap_check );
}

/**
 * @fn void T_case_body_ScoreCpuSparcValException( void )
 */
T_TEST_CASE( ScoreCpuSparcValException )
{
  ScoreCpuSparcValException_Action_0();
  ScoreCpuSparcValException_Action_1();
  ScoreCpuSparcValException_Action_2();
  ScoreCpuSparcValException_Action_3();
  ScoreCpuSparcValException_Action_4();
  ScoreCpuSparcValException_Action_5();
  ScoreCpuSparcValException_Action_6();
  ScoreCpuSparcValException_Action_7();
  ScoreCpuSparcValException_Action_8();
  ScoreCpuSparcValException_Action_9();
  ScoreCpuSparcValException_Action_10();
}

/** @} */
