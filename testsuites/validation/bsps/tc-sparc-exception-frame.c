/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuSparcValExceptionFrame
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

#include <string.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/isr.h>
#include <rtems/score/percpu.h>

#include "tr-sparc-exception-frame.h"
#include "tx-register-check.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuSparcValExceptionFrame \
 *   spec:/score/cpu/sparc/val/exception-frame
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Checks that an exception frame has the expected values.
 *
 * The scenario of the caller traps in the inner of two register windows which
 * the prepare block enters. The outer window is a window which the trap did
 * not enter.
 *
 * This test case performs the following actions:
 *
 * - Run the scenario of the caller through the register check, once for each
 *   value with the value inverted, then once unchanged.
 *
 *   - Check that the SPARC global register G1 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC global register G2 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC global register G3 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC global register G4 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC global register G5 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC output register O0 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC output register O1 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC output register O2 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC output register O3 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC output register O4 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC output register O5 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC output register O7 was saved to the exception
 *     frame.
 *
 *   - Check that the SPARC local register L0 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC local register L1 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC local register L2 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC local register L3 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC local register L4 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC local register L5 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC local register L6 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC local register L7 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC input register I0 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC input register I1 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC input register I2 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC input register I3 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC input register I4 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC input register I5 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC input register I7 of the trapped window was saved
 *     to the exception frame.
 *
 *   - Check that the SPARC local register L0 of the window which the trap did
 *     not enter was saved to the exception frame.
 *
 *   - Check that the SPARC local register L1 of the window which the trap did
 *     not enter was saved to the exception frame.
 *
 *   - Check that the SPARC local register L2 of the window which the trap did
 *     not enter was saved to the exception frame.
 *
 *   - Check that the SPARC local register L3 of the window which the trap did
 *     not enter was saved to the exception frame.
 *
 *   - Check that the SPARC local register L4 of the window which the trap did
 *     not enter was saved to the exception frame.
 *
 *   - Check that the SPARC local register L5 of the window which the trap did
 *     not enter was saved to the exception frame.
 *
 *   - Check that the SPARC local register L6 of the window which the trap did
 *     not enter was saved to the exception frame.
 *
 *   - Check that the SPARC local register L7 of the window which the trap did
 *     not enter was saved to the exception frame.
 *
 *   - Check that the SPARC register Y was saved to the exception frame.
 *
 *   - Check that the integer condition codes of the PSR were saved to the
 *     exception frame.
 *
 *   - Check that the SPARC FSR was saved to the exception frame.
 *
 *   - Check that the SPARC floating-point registers F0 and F1 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F2 and F3 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F4 and F5 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F6 and F7 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F8 and F9 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F10 and F11 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F12 and F13 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F14 and F15 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F16 and F17 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F18 and F19 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F20 and F21 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F22 and F23 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F24 and F25 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F26 and F27 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F28 and F29 were saved to
 *     the exception frame.
 *
 *   - Check that the SPARC floating-point registers F30 and F31 were saved to
 *     the exception frame.
 *
 *   - Check that the exception frame reports the SPARC global register G0 as
 *     zero.
 *
 *   - Check that the exception frame reports the per-CPU control in the SPARC
 *     global register G6.
 *
 *   - Check that the exception frame reports the thread pointer in the SPARC
 *     global register G7.
 *
 *   - Check that the exception frame reports the stack pointer of the trapped
 *     window, which is O6.
 *
 *   - Check that the exception frame reports the frame pointer of the trapped
 *     window, which is I6.
 *
 *   - Check that the exception frame reports the address of the trapping
 *     instruction.
 *
 *   - Check that the exception frame is on the interrupt stack.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/cpu/sparc/val/exception-frame test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreCpuSparcValExceptionFrame_Run() parameter.
   */
  void ( *scenario )( void );

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreCpuSparcValExceptionFrame_Run() parameter.
   */
  uint32_t trap;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreCpuSparcValExceptionFrame_Run() parameter.
   */
  const ExceptionFrameVariant *variant;
} ScoreCpuSparcValExceptionFrame_Context;

static ScoreCpuSparcValExceptionFrame_Context
  ScoreCpuSparcValExceptionFrame_Instance;

#define WORD UINT64_C( 0xffffffff )

typedef enum {
  SOURCE_G1,
  SOURCE_G2,
  SOURCE_G3,
  SOURCE_G4,
  SOURCE_G5,
  SOURCE_O0,
  SOURCE_O1,
  SOURCE_O2,
  SOURCE_O3,
  SOURCE_O4,
  SOURCE_O5,
  SOURCE_O7,
  SOURCE_L0,
  SOURCE_L1,
  SOURCE_L2,
  SOURCE_L3,
  SOURCE_L4,
  SOURCE_L5,
  SOURCE_L6,
  SOURCE_L7,
  SOURCE_I0,
  SOURCE_I1,
  SOURCE_I2,
  SOURCE_I3,
  SOURCE_I4,
  SOURCE_I5,
  SOURCE_I7,
  SOURCE_OUTER_L0,
  SOURCE_OUTER_L1,
  SOURCE_OUTER_L2,
  SOURCE_OUTER_L3,
  SOURCE_OUTER_L4,
  SOURCE_OUTER_L5,
  SOURCE_OUTER_L6,
  SOURCE_OUTER_L7,
  SOURCE_Y,
  SOURCE_ICC,
#if SPARC_HAS_FPU == 1
  SOURCE_FSR,
  SOURCE_F0_F1,
  SOURCE_F2_F3,
  SOURCE_F4_F5,
  SOURCE_F6_F7,
  SOURCE_F8_F9,
  SOURCE_F10_F11,
  SOURCE_F12_F13,
  SOURCE_F14_F15,
  SOURCE_F16_F17,
  SOURCE_F18_F19,
  SOURCE_F20_F21,
  SOURCE_F22_F23,
  SOURCE_F24_F25,
  SOURCE_F26_F27,
  SOURCE_F28_F29,
  SOURCE_F30_F31,
#endif
  SOURCE_G0,
  SOURCE_G6,
  SOURCE_G7,
  SOURCE_SP,
  SOURCE_FP,
  SOURCE_PC,
  SOURCE_TRAP,
  SOURCE_STACK,
  SOURCE_MAX
} Source;

typedef enum {
  SLOT_G1,
  SLOT_G2,
  SLOT_G3,
  SLOT_G4,
  SLOT_G5,
  SLOT_O0,
  SLOT_O1,
  SLOT_O2,
  SLOT_O3,
  SLOT_O4,
  SLOT_O5,
  SLOT_O7,
  SLOT_L0,
  SLOT_L1,
  SLOT_L2,
  SLOT_L3,
  SLOT_L4,
  SLOT_L5,
  SLOT_L6,
  SLOT_L7,
  SLOT_I0,
  SLOT_I1,
  SLOT_I2,
  SLOT_I3,
  SLOT_I4,
  SLOT_I5,
  SLOT_I7,
  SLOT_OUTER_L0,
  SLOT_OUTER_L1,
  SLOT_OUTER_L2,
  SLOT_OUTER_L3,
  SLOT_OUTER_L4,
  SLOT_OUTER_L5,
  SLOT_OUTER_L6,
  SLOT_OUTER_L7,
  SLOT_Y,
  SLOT_ICC,
#if SPARC_HAS_FPU == 1
  SLOT_FSR,
  SLOT_F0_F1,
  SLOT_F2_F3,
  SLOT_F4_F5,
  SLOT_F6_F7,
  SLOT_F8_F9,
  SLOT_F10_F11,
  SLOT_F12_F13,
  SLOT_F14_F15,
  SLOT_F16_F17,
  SLOT_F18_F19,
  SLOT_F20_F21,
  SLOT_F22_F23,
  SLOT_F24_F25,
  SLOT_F26_F27,
  SLOT_F28_F29,
  SLOT_F30_F31,
#endif
  SLOT_G0,
  SLOT_G6,
  SLOT_G7,
  SLOT_SP,
  SLOT_FP,
  SLOT_PC,
  SLOT_TRAP,
  SLOT_STACK,
  SLOT_MAX
} Slot;

RTEMS_STATIC_ASSERT( SOURCE_G1 == 0, source_g1 );
RTEMS_STATIC_ASSERT( SOURCE_O0 == 5, source_o0 );
RTEMS_STATIC_ASSERT( SOURCE_L0 == 12, source_l0 );
RTEMS_STATIC_ASSERT( SOURCE_I0 == 20, source_i0 );
RTEMS_STATIC_ASSERT( SOURCE_OUTER_L0 == 27, source_outer_l0 );
RTEMS_STATIC_ASSERT( SOURCE_Y == 35, source_y );
RTEMS_STATIC_ASSERT( SOURCE_ICC == 36, source_icc );
RTEMS_STATIC_ASSERT( SOURCE_G2 == EXCEPTION_FRAME_SOURCE_G2, source_g2 );
RTEMS_STATIC_ASSERT( SOURCE_O7 == EXCEPTION_FRAME_SOURCE_O7, source_o7 );
#if SPARC_HAS_FPU == 1
RTEMS_STATIC_ASSERT( SOURCE_FSR == 37, source_fsr );
RTEMS_STATIC_ASSERT( SOURCE_F0_F1 == 38, source_f0_f1 );
#endif

static const RegisterCheckSource
  exception_frame_default_sources[ SOURCE_MAX ] = {
    { "g1", REGISTER_CHECK_INITIAL, WORD },
    { "g2", REGISTER_CHECK_INITIAL, WORD },
    { "g3", REGISTER_CHECK_INITIAL, WORD },
    { "g4", REGISTER_CHECK_INITIAL, WORD },
    { "g5", REGISTER_CHECK_INITIAL, WORD },
    { "o0", REGISTER_CHECK_INITIAL, WORD },
    { "o1", REGISTER_CHECK_INITIAL, WORD },
    { "o2", REGISTER_CHECK_INITIAL, WORD },
    { "o3", REGISTER_CHECK_INITIAL, WORD },
    { "o4", REGISTER_CHECK_INITIAL, WORD },
    { "o5", REGISTER_CHECK_INITIAL, WORD },
    { "o7", REGISTER_CHECK_INITIAL, WORD },
    { "l0", REGISTER_CHECK_INITIAL, WORD },
    { "l1", REGISTER_CHECK_INITIAL, WORD },
    { "l2", REGISTER_CHECK_INITIAL, WORD },
    { "l3", REGISTER_CHECK_INITIAL, WORD },
    { "l4", REGISTER_CHECK_INITIAL, WORD },
    { "l5", REGISTER_CHECK_INITIAL, WORD },
    { "l6", REGISTER_CHECK_INITIAL, WORD },
    { "l7", REGISTER_CHECK_INITIAL, WORD },
    { "i0", REGISTER_CHECK_INITIAL, WORD },
    { "i1", REGISTER_CHECK_INITIAL, WORD },
    { "i2", REGISTER_CHECK_INITIAL, WORD },
    { "i3", REGISTER_CHECK_INITIAL, WORD },
    { "i4", REGISTER_CHECK_INITIAL, WORD },
    { "i5", REGISTER_CHECK_INITIAL, WORD },
    { "i7", REGISTER_CHECK_INITIAL, WORD },
    { "outer l0", REGISTER_CHECK_INITIAL, WORD },
    { "outer l1", REGISTER_CHECK_INITIAL, WORD },
    { "outer l2", REGISTER_CHECK_INITIAL, WORD },
    { "outer l3", REGISTER_CHECK_INITIAL, WORD },
    { "outer l4", REGISTER_CHECK_INITIAL, WORD },
    { "outer l5", REGISTER_CHECK_INITIAL, WORD },
    { "outer l6", REGISTER_CHECK_INITIAL, WORD },
    { "outer l7", REGISTER_CHECK_INITIAL, WORD },
    { "y", REGISTER_CHECK_INITIAL, WORD },
    { "icc", REGISTER_CHECK_INITIAL, PSR_ICC_MASK },
#if SPARC_HAS_FPU == 1
    { "fsr", REGISTER_CHECK_INITIAL, FSR_TEST_MASK },
    { "f0_f1", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f2_f3", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f4_f5", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f6_f7", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f8_f9", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f10_f11", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f12_f13", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f14_f15", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f16_f17", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f18_f19", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f20_f21", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f22_f23", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f24_f25", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f26_f27", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f28_f29", REGISTER_CHECK_INITIAL, UINT64_MAX },
    { "f30_f31", REGISTER_CHECK_INITIAL, UINT64_MAX },
#endif
    { "g0", REGISTER_CHECK_EXPECTED, WORD },
    { "g6", REGISTER_CHECK_EXPECTED, WORD },
    { "g7", REGISTER_CHECK_EXPECTED, WORD },
    { "sp", REGISTER_CHECK_EXPECTED, WORD },
    { "fp", REGISTER_CHECK_EXPECTED, WORD },
    { "pc", REGISTER_CHECK_EXPECTED, WORD },
    { "trap", REGISTER_CHECK_EXPECTED, WORD },
    { "stack", REGISTER_CHECK_EXPECTED, WORD },
};

static const RegisterCheckSlot exception_frame_slots[ SLOT_MAX ] = {
  { "g1", SOURCE_G1, WORD },
  { "g2", SOURCE_G2, WORD },
  { "g3", SOURCE_G3, WORD },
  { "g4", SOURCE_G4, WORD },
  { "g5", SOURCE_G5, WORD },
  { "o0", SOURCE_O0, WORD },
  { "o1", SOURCE_O1, WORD },
  { "o2", SOURCE_O2, WORD },
  { "o3", SOURCE_O3, WORD },
  { "o4", SOURCE_O4, WORD },
  { "o5", SOURCE_O5, WORD },
  { "o7", SOURCE_O7, WORD },
  { "l0", SOURCE_L0, WORD },
  { "l1", SOURCE_L1, WORD },
  { "l2", SOURCE_L2, WORD },
  { "l3", SOURCE_L3, WORD },
  { "l4", SOURCE_L4, WORD },
  { "l5", SOURCE_L5, WORD },
  { "l6", SOURCE_L6, WORD },
  { "l7", SOURCE_L7, WORD },
  { "i0", SOURCE_I0, WORD },
  { "i1", SOURCE_I1, WORD },
  { "i2", SOURCE_I2, WORD },
  { "i3", SOURCE_I3, WORD },
  { "i4", SOURCE_I4, WORD },
  { "i5", SOURCE_I5, WORD },
  { "i7", SOURCE_I7, WORD },
  { "outer l0", SOURCE_OUTER_L0, WORD },
  { "outer l1", SOURCE_OUTER_L1, WORD },
  { "outer l2", SOURCE_OUTER_L2, WORD },
  { "outer l3", SOURCE_OUTER_L3, WORD },
  { "outer l4", SOURCE_OUTER_L4, WORD },
  { "outer l5", SOURCE_OUTER_L5, WORD },
  { "outer l6", SOURCE_OUTER_L6, WORD },
  { "outer l7", SOURCE_OUTER_L7, WORD },
  { "y", SOURCE_Y, WORD },
  { "icc", SOURCE_ICC, PSR_ICC_MASK },
#if SPARC_HAS_FPU == 1
  { "fsr", SOURCE_FSR, FSR_TEST_MASK },
  { "f0_f1", SOURCE_F0_F1, UINT64_MAX },
  { "f2_f3", SOURCE_F2_F3, UINT64_MAX },
  { "f4_f5", SOURCE_F4_F5, UINT64_MAX },
  { "f6_f7", SOURCE_F6_F7, UINT64_MAX },
  { "f8_f9", SOURCE_F8_F9, UINT64_MAX },
  { "f10_f11", SOURCE_F10_F11, UINT64_MAX },
  { "f12_f13", SOURCE_F12_F13, UINT64_MAX },
  { "f14_f15", SOURCE_F14_F15, UINT64_MAX },
  { "f16_f17", SOURCE_F16_F17, UINT64_MAX },
  { "f18_f19", SOURCE_F18_F19, UINT64_MAX },
  { "f20_f21", SOURCE_F20_F21, UINT64_MAX },
  { "f22_f23", SOURCE_F22_F23, UINT64_MAX },
  { "f24_f25", SOURCE_F24_F25, UINT64_MAX },
  { "f26_f27", SOURCE_F26_F27, UINT64_MAX },
  { "f28_f29", SOURCE_F28_F29, UINT64_MAX },
  { "f30_f31", SOURCE_F30_F31, UINT64_MAX },
#endif
  { "g0", SOURCE_G0, WORD },
  { "g6", SOURCE_G6, WORD },
  { "g7", SOURCE_G7, WORD },
  { "sp", SOURCE_SP, WORD },
  { "fp", SOURCE_FP, WORD },
  { "pc", SOURCE_PC, WORD },
  { "trap", SOURCE_TRAP, WORD },
  { "stack", SOURCE_STACK, WORD },
};

static const uint64_t exception_frame_patterns[ SOURCE_MAX ] = {
  0x10111213,
  0x14151617,
  0x18191a1b,
  0x1c1d1e1f,
  0x20212223,
  0x24252627,
  0x28292a2b,
  0x2c2d2e2f,
  0x30313233,
  0x34353637,
  0x38393a3b,
  0x3c3d3e3f,
  0x40414243,
  0x44454647,
  0x48494a4b,
  0x4c4d4e4f,
  0x50515253,
  0x54555657,
  0x58595a5b,
  0x5c5d5e5f,
  0x60616263,
  0x64656667,
  0x68696a6b,
  0x6c6d6e6f,
  0x70717273,
  0x74757677,
  0x78797a7b,
  0x7c7d7e7f,
  0x80818283,
  0x84858687,
  0x88898a8b,
  0x8c8d8e8f,
  0x90919293,
  0x94959697,
  0x98999a9b,
  0x9c9d9e9f,
  PSR_ICC_BITS,
#if SPARC_HAS_FPU == 1
  FSR_TEST_BITS,
  0x4041424344454647,
  0x48494a4b4c4d4e4f,
  0x5051525354555657,
  0x58595a5b5c5d5e5f,
  0x6061626364656667,
  0x68696a6b6c6d6e6f,
  0x7071727374757677,
  0x78797a7b7c7d7e7f,
  0x8081828384858687,
  0x88898a8b8c8d8e8f,
  0x9091929394959697,
  0x98999a9b9c9d9e9f,
  0xa0a1a2a3a4a5a6a7,
  0xa8a9aaabacadaeaf,
  0xb0b1b2b3b4b5b6b7,
  0xb8b9babbbcbdbebf,
#endif
};

uint64_t sparc_exception_initial[ SOURCE_MAX ];

/* The sources of the current variant */
static RegisterCheckSource exception_frame_sources[ SOURCE_MAX ];

/* The expected value of each source of the kind REGISTER_CHECK_INITIAL */
static uint64_t exception_frame_expected[ SOURCE_MAX ];

static const ExceptionFrameVariant *exception_frame_variant;

uint32_t sparc_exception_expected[ 2 ];

const void *exception_expected_pc;

static CPU_Exception_frame exception_frame;

static void ( *exception_frame_scenario )( void );

static const CPU_Exception_frame *volatile exception_frame_address;

static uint32_t exception_frame_g7;

static const Per_CPU_Control *exception_frame_cpu;

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  (void) arg;

  SetFatalHandler( NULL, NULL );

  if ( source == RTEMS_FATAL_SOURCE_EXCEPTION ) {
    CPU_Exception_frame *frame;

    frame = (CPU_Exception_frame *) code;
    /* The handler runs on the processor which takes the trap */
    exception_frame_cpu = _Per_CPU_Get_snapshot();
    exception_frame = *frame;
    exception_frame_address = frame;

    if ( exception_frame_variant->resume != NULL ) {
      frame->pc = (uint32_t) exception_frame_variant->resume;
    } else {
      frame->pc = frame->npc;
    }

    frame->npc = frame->pc + 4;
    frame->psr |= exception_frame_variant->psr_set;
    _CPU_Exception_resume( frame );
  }
}

CPU_Exception_frame *ExceptionFrameInitialize( void )
{
  memset( &exception_frame, 0xff, sizeof( exception_frame ) );
  exception_frame_address = NULL;
  exception_frame_cpu = NULL;
  SetFatalHandler( Fatal, NULL );

  return &exception_frame;
}

static bool IsOnInterruptStack( const CPU_Exception_frame *frame )
{
  uintptr_t address;

  address = (uintptr_t) frame;

  return address >= (uintptr_t) _ISR_Stack_area_begin &&
         address < (uintptr_t) _ISR_Stack_area_end;
}

static void ExceptionFrameRun( RegisterCheck *self, void *arg )
{
  const CPU_Exception_frame *frame;
  const uint64_t            *expected;
  uint32_t                   trap;
#if SPARC_HAS_FPU == 1
  uint32_t fsr;
#endif

  trap = *(const uint32_t *) arg;
  exception_expected_pc = NULL;
  __asm__ volatile( "mov %%g7, %0"
                    : "=r"( exception_frame_g7 ) );
#if SPARC_HAS_FPU == 1
  __asm__ volatile( "st %%fsr, %0"
                    : "=m"( fsr ) );
#endif
  ( *exception_frame_scenario )();
#if SPARC_HAS_FPU == 1
  /* The scenario leaves the FSR of the table in the processor */
  __asm__ volatile( "ld %0, %%fsr\nnop\nnop\nnop"
                    :
                    : "m"( fsr ) );
#endif
  frame = &exception_frame;
  expected = exception_frame_expected;

  RegisterCheckRecord(
    self,
    SLOT_G1,
    frame->global[ 1 ],
    expected[ SOURCE_G1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_G2,
    frame->global[ 2 ],
    expected[ SOURCE_G2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_G3,
    frame->global[ 3 ],
    expected[ SOURCE_G3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_G4,
    frame->global[ 4 ],
    expected[ SOURCE_G4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_G5,
    frame->global[ 5 ],
    expected[ SOURCE_G5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O0,
    frame->output[ 0 ],
    expected[ SOURCE_O0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O1,
    frame->output[ 1 ],
    expected[ SOURCE_O1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O2,
    frame->output[ 2 ],
    expected[ SOURCE_O2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O3,
    frame->output[ 3 ],
    expected[ SOURCE_O3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O4,
    frame->output[ 4 ],
    expected[ SOURCE_O4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O5,
    frame->output[ 5 ],
    expected[ SOURCE_O5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O7,
    frame->output[ 7 ],
    expected[ SOURCE_O7 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L0,
    frame->windows[ 0 ].local[ 0 ],
    expected[ SOURCE_L0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L1,
    frame->windows[ 0 ].local[ 1 ],
    expected[ SOURCE_L1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L2,
    frame->windows[ 0 ].local[ 2 ],
    expected[ SOURCE_L2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L3,
    frame->windows[ 0 ].local[ 3 ],
    expected[ SOURCE_L3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L4,
    frame->windows[ 0 ].local[ 4 ],
    expected[ SOURCE_L4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L5,
    frame->windows[ 0 ].local[ 5 ],
    expected[ SOURCE_L5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L6,
    frame->windows[ 0 ].local[ 6 ],
    expected[ SOURCE_L6 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L7,
    frame->windows[ 0 ].local[ 7 ],
    expected[ SOURCE_L7 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I0,
    frame->windows[ 0 ].input[ 0 ],
    expected[ SOURCE_I0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I1,
    frame->windows[ 0 ].input[ 1 ],
    expected[ SOURCE_I1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I2,
    frame->windows[ 0 ].input[ 2 ],
    expected[ SOURCE_I2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I3,
    frame->windows[ 0 ].input[ 3 ],
    expected[ SOURCE_I3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I4,
    frame->windows[ 0 ].input[ 4 ],
    expected[ SOURCE_I4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I5,
    frame->windows[ 0 ].input[ 5 ],
    expected[ SOURCE_I5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I7,
    frame->windows[ 0 ].input[ 7 ],
    expected[ SOURCE_I7 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_OUTER_L0,
    frame->windows[ 1 ].local[ 0 ],
    expected[ SOURCE_OUTER_L0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_OUTER_L1,
    frame->windows[ 1 ].local[ 1 ],
    expected[ SOURCE_OUTER_L1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_OUTER_L2,
    frame->windows[ 1 ].local[ 2 ],
    expected[ SOURCE_OUTER_L2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_OUTER_L3,
    frame->windows[ 1 ].local[ 3 ],
    expected[ SOURCE_OUTER_L3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_OUTER_L4,
    frame->windows[ 1 ].local[ 4 ],
    expected[ SOURCE_OUTER_L4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_OUTER_L5,
    frame->windows[ 1 ].local[ 5 ],
    expected[ SOURCE_OUTER_L5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_OUTER_L6,
    frame->windows[ 1 ].local[ 6 ],
    expected[ SOURCE_OUTER_L6 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_OUTER_L7,
    frame->windows[ 1 ].local[ 7 ],
    expected[ SOURCE_OUTER_L7 ]
  );
  RegisterCheckRecord( self, SLOT_Y, frame->y, expected[ SOURCE_Y ] );
  RegisterCheckRecord( self, SLOT_ICC, frame->psr, expected[ SOURCE_ICC ] );
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord( self, SLOT_FSR, frame->fsr, expected[ SOURCE_FSR ] );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F0_F1,
    frame->fp[ 0 ],
    expected[ SOURCE_F0_F1 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F2_F3,
    frame->fp[ 1 ],
    expected[ SOURCE_F2_F3 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F4_F5,
    frame->fp[ 2 ],
    expected[ SOURCE_F4_F5 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F6_F7,
    frame->fp[ 3 ],
    expected[ SOURCE_F6_F7 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F8_F9,
    frame->fp[ 4 ],
    expected[ SOURCE_F8_F9 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F10_F11,
    frame->fp[ 5 ],
    expected[ SOURCE_F10_F11 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F12_F13,
    frame->fp[ 6 ],
    expected[ SOURCE_F12_F13 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F14_F15,
    frame->fp[ 7 ],
    expected[ SOURCE_F14_F15 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F16_F17,
    frame->fp[ 8 ],
    expected[ SOURCE_F16_F17 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F18_F19,
    frame->fp[ 9 ],
    expected[ SOURCE_F18_F19 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F20_F21,
    frame->fp[ 10 ],
    expected[ SOURCE_F20_F21 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F22_F23,
    frame->fp[ 11 ],
    expected[ SOURCE_F22_F23 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F24_F25,
    frame->fp[ 12 ],
    expected[ SOURCE_F24_F25 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F26_F27,
    frame->fp[ 13 ],
    expected[ SOURCE_F26_F27 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F28_F29,
    frame->fp[ 14 ],
    expected[ SOURCE_F28_F29 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F30_F31,
    frame->fp[ 15 ],
    expected[ SOURCE_F30_F31 ]
  );
#endif
  RegisterCheckRecord( self, SLOT_G0, frame->global[ 0 ], 0 );
  RegisterCheckRecord(
    self,
    SLOT_G6,
    frame->global[ 6 ],
    (uintptr_t) exception_frame_cpu
  );
  RegisterCheckRecord( self, SLOT_G7, frame->global[ 7 ], exception_frame_g7 );
  RegisterCheckRecord(
    self,
    SLOT_SP,
    frame->output[ 6 ],
    sparc_exception_expected[ 0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FP,
    frame->windows[ 0 ].input[ 6 ],
    sparc_exception_expected[ 1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_PC,
    frame->pc,
    (uintptr_t) exception_expected_pc
  );
  RegisterCheckRecord( self, SLOT_TRAP, frame->trap, trap );
  RegisterCheckRecord(
    self,
    SLOT_STACK,
    IsOnInterruptStack( exception_frame_address ) ? 1 : 0,
    1
  );
}

static uint32_t exception_frame_trap;

static RegisterCheck exception_frame_check = {
  .sources = exception_frame_sources,
  .source_count = SOURCE_MAX,
  .slots = exception_frame_slots,
  .slot_count = SLOT_MAX,
  .initial = sparc_exception_initial,
  .run = ExceptionFrameRun,
  .arg = &exception_frame_trap
};

static void CheckSlot( Slot slot )
{
  RegisterCheckVerify( &exception_frame_check, slot );
}

void ExceptionFrameCheckTrap( void )
{
  CheckSlot( SLOT_TRAP );
}

static const ExceptionFrameVariant exception_frame_no_variant;

static void ExceptionFrameSetup(
  uint32_t                     trap,
  const ExceptionFrameVariant *variant
)
{
  size_t i;

  if ( variant == NULL ) {
    variant = &exception_frame_no_variant;
  }

  memcpy(
    sparc_exception_initial,
    exception_frame_patterns,
    sizeof( exception_frame_patterns )
  );
  memcpy(
    exception_frame_expected,
    exception_frame_patterns,
    sizeof( exception_frame_patterns )
  );
  memcpy(
    exception_frame_sources,
    exception_frame_default_sources,
    sizeof( exception_frame_default_sources )
  );

  /* The scenario sets an override itself, so the check inverts its value */
  for ( i = 0; i < variant->override_count; ++i ) {
    const ExceptionFrameOverride *override;

    override = &variant->overrides[ i ];
    exception_frame_sources[ override->source ].kind = REGISTER_CHECK_EXPECTED;
    exception_frame_expected[ override->source ] = ( uintptr_t )
                                                     override->value;
  }

  exception_frame_trap = trap;
  exception_frame_variant = variant;
}

static T_fixture ScoreCpuSparcValExceptionFrame_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreCpuSparcValExceptionFrame_Instance
};

/**
 * @brief Run the scenario of the caller through the register check, once for
 *   each value with the value inverted, then once unchanged.
 */
static void ScoreCpuSparcValExceptionFrame_Action_0(
  ScoreCpuSparcValExceptionFrame_Context *ctx
)
{
  exception_frame_scenario = ctx->scenario;
  ExceptionFrameSetup( ctx->trap, ctx->variant );
  RegisterCheckRun( &exception_frame_check );

  /*
   * Check that the SPARC global register G1 was saved to the exception frame.
   */
  CheckSlot( SLOT_G1 );

  /*
   * Check that the SPARC global register G2 was saved to the exception frame.
   */
  CheckSlot( SLOT_G2 );

  /*
   * Check that the SPARC global register G3 was saved to the exception frame.
   */
  CheckSlot( SLOT_G3 );

  /*
   * Check that the SPARC global register G4 was saved to the exception frame.
   */
  CheckSlot( SLOT_G4 );

  /*
   * Check that the SPARC global register G5 was saved to the exception frame.
   */
  CheckSlot( SLOT_G5 );

  /*
   * Check that the SPARC output register O0 was saved to the exception frame.
   */
  CheckSlot( SLOT_O0 );

  /*
   * Check that the SPARC output register O1 was saved to the exception frame.
   */
  CheckSlot( SLOT_O1 );

  /*
   * Check that the SPARC output register O2 was saved to the exception frame.
   */
  CheckSlot( SLOT_O2 );

  /*
   * Check that the SPARC output register O3 was saved to the exception frame.
   */
  CheckSlot( SLOT_O3 );

  /*
   * Check that the SPARC output register O4 was saved to the exception frame.
   */
  CheckSlot( SLOT_O4 );

  /*
   * Check that the SPARC output register O5 was saved to the exception frame.
   */
  CheckSlot( SLOT_O5 );

  /*
   * Check that the SPARC output register O7 was saved to the exception frame.
   */
  CheckSlot( SLOT_O7 );

  /*
   * Check that the SPARC local register L0 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_L0 );

  /*
   * Check that the SPARC local register L1 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_L1 );

  /*
   * Check that the SPARC local register L2 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_L2 );

  /*
   * Check that the SPARC local register L3 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_L3 );

  /*
   * Check that the SPARC local register L4 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_L4 );

  /*
   * Check that the SPARC local register L5 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_L5 );

  /*
   * Check that the SPARC local register L6 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_L6 );

  /*
   * Check that the SPARC local register L7 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_L7 );

  /*
   * Check that the SPARC input register I0 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_I0 );

  /*
   * Check that the SPARC input register I1 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_I1 );

  /*
   * Check that the SPARC input register I2 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_I2 );

  /*
   * Check that the SPARC input register I3 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_I3 );

  /*
   * Check that the SPARC input register I4 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_I4 );

  /*
   * Check that the SPARC input register I5 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_I5 );

  /*
   * Check that the SPARC input register I7 of the trapped window was saved to
   * the exception frame.
   */
  CheckSlot( SLOT_I7 );

  /*
   * Check that the SPARC local register L0 of the window which the trap did
   * not enter was saved to the exception frame.
   */
  CheckSlot( SLOT_OUTER_L0 );

  /*
   * Check that the SPARC local register L1 of the window which the trap did
   * not enter was saved to the exception frame.
   */
  CheckSlot( SLOT_OUTER_L1 );

  /*
   * Check that the SPARC local register L2 of the window which the trap did
   * not enter was saved to the exception frame.
   */
  CheckSlot( SLOT_OUTER_L2 );

  /*
   * Check that the SPARC local register L3 of the window which the trap did
   * not enter was saved to the exception frame.
   */
  CheckSlot( SLOT_OUTER_L3 );

  /*
   * Check that the SPARC local register L4 of the window which the trap did
   * not enter was saved to the exception frame.
   */
  CheckSlot( SLOT_OUTER_L4 );

  /*
   * Check that the SPARC local register L5 of the window which the trap did
   * not enter was saved to the exception frame.
   */
  CheckSlot( SLOT_OUTER_L5 );

  /*
   * Check that the SPARC local register L6 of the window which the trap did
   * not enter was saved to the exception frame.
   */
  CheckSlot( SLOT_OUTER_L6 );

  /*
   * Check that the SPARC local register L7 of the window which the trap did
   * not enter was saved to the exception frame.
   */
  CheckSlot( SLOT_OUTER_L7 );

  /*
   * Check that the SPARC register Y was saved to the exception frame.
   */
  CheckSlot( SLOT_Y );

  /*
   * Check that the integer condition codes of the PSR were saved to the
   * exception frame.
   */
  CheckSlot( SLOT_ICC );

  /*
   * Check that the SPARC FSR was saved to the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_FSR );
  #endif

  /*
   * Check that the SPARC floating-point registers F0 and F1 were saved to the
   * exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F0_F1 );
  #endif

  /*
   * Check that the SPARC floating-point registers F2 and F3 were saved to the
   * exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F2_F3 );
  #endif

  /*
   * Check that the SPARC floating-point registers F4 and F5 were saved to the
   * exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F4_F5 );
  #endif

  /*
   * Check that the SPARC floating-point registers F6 and F7 were saved to the
   * exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F6_F7 );
  #endif

  /*
   * Check that the SPARC floating-point registers F8 and F9 were saved to the
   * exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F8_F9 );
  #endif

  /*
   * Check that the SPARC floating-point registers F10 and F11 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F10_F11 );
  #endif

  /*
   * Check that the SPARC floating-point registers F12 and F13 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F12_F13 );
  #endif

  /*
   * Check that the SPARC floating-point registers F14 and F15 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F14_F15 );
  #endif

  /*
   * Check that the SPARC floating-point registers F16 and F17 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F16_F17 );
  #endif

  /*
   * Check that the SPARC floating-point registers F18 and F19 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F18_F19 );
  #endif

  /*
   * Check that the SPARC floating-point registers F20 and F21 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F20_F21 );
  #endif

  /*
   * Check that the SPARC floating-point registers F22 and F23 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F22_F23 );
  #endif

  /*
   * Check that the SPARC floating-point registers F24 and F25 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F24_F25 );
  #endif

  /*
   * Check that the SPARC floating-point registers F26 and F27 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F26_F27 );
  #endif

  /*
   * Check that the SPARC floating-point registers F28 and F29 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F28_F29 );
  #endif

  /*
   * Check that the SPARC floating-point registers F30 and F31 were saved to
   * the exception frame.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F30_F31 );
  #endif

  /*
   * Check that the exception frame reports the SPARC global register G0 as
   * zero.
   */
  CheckSlot( SLOT_G0 );

  /*
   * Check that the exception frame reports the per-CPU control in the SPARC
   * global register G6.
   */
  CheckSlot( SLOT_G6 );

  /*
   * Check that the exception frame reports the thread pointer in the SPARC
   * global register G7.
   */
  CheckSlot( SLOT_G7 );

  /*
   * Check that the exception frame reports the stack pointer of the trapped
   * window, which is O6.
   */
  CheckSlot( SLOT_SP );

  /*
   * Check that the exception frame reports the frame pointer of the trapped
   * window, which is I6.
   */
  CheckSlot( SLOT_FP );

  /*
   * Check that the exception frame reports the address of the trapping
   * instruction.
   */
  CheckSlot( SLOT_PC );

  /*
   * Check that the exception frame is on the interrupt stack.
   */
  CheckSlot( SLOT_STACK );

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  RegisterCheckReport( &exception_frame_check );
}

static T_fixture_node ScoreCpuSparcValExceptionFrame_Node;

static T_remark ScoreCpuSparcValExceptionFrame_Remark = {
  .next = NULL,
  .remark = "ScoreCpuSparcValExceptionFrame"
};

void ScoreCpuSparcValExceptionFrame_Run(
  void ( *scenario )( void ),
  uint32_t                     trap,
  const ExceptionFrameVariant *variant
)
{
  ScoreCpuSparcValExceptionFrame_Context *ctx;

  ctx = &ScoreCpuSparcValExceptionFrame_Instance;
  ctx->scenario = scenario;
  ctx->trap = trap;
  ctx->variant = variant;

  ctx = T_push_fixture(
    &ScoreCpuSparcValExceptionFrame_Node,
    &ScoreCpuSparcValExceptionFrame_Fixture
  );

  ScoreCpuSparcValExceptionFrame_Action_0( ctx );

  T_add_remark( &ScoreCpuSparcValExceptionFrame_Remark );
  T_pop_fixture();
}

/** @} */
