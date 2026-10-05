/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuSparcValResume
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

#include <stdint.h>
#include <string.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/sparc.h>

#include "tx-register-check.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuSparcValResume spec:/score/cpu/sparc/val/resume
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Checks that the exception resume restores the registers of the
 *   exception frame.
 *
 * The two traps sit next to each other. The handler of the first one writes a
 * value into every member of the frame. The second one reports what the resume
 * restored. A register check holds at most 64 checks, so three actions share
 * the members of the frame.
 *
 * This test case performs the following actions:
 *
 * - Trap. Let the handler write a new value into every global and output
 *   register, the Y register, the floating-point registers, the FSR and the
 *   first register window of the frame. Resume, and trap again to capture what
 *   the resume restored. Run this once for each value with the value inverted,
 *   then once unchanged.
 *
 *   - Check that the resume restored the SPARC global register G1.
 *
 *   - Check that the resume restored the SPARC global register G2.
 *
 *   - Check that the resume restored the SPARC global register G3.
 *
 *   - Check that the resume restored the SPARC global register G4.
 *
 *   - Check that the resume restored the SPARC global register G5.
 *
 *   - Check that the resume restored the SPARC global register G7.
 *
 *   - Check that the resume restored the SPARC output register O0.
 *
 *   - Check that the resume restored the SPARC output register O1.
 *
 *   - Check that the resume restored the SPARC output register O2.
 *
 *   - Check that the resume restored the SPARC output register O3.
 *
 *   - Check that the resume restored the SPARC output register O4.
 *
 *   - Check that the resume restored the SPARC output register O5.
 *
 *   - Check that the resume restored the SPARC output register O7.
 *
 *   - Check that the resume restored the SPARC register Y.
 *
 *   - Check that the resume restored the SPARC FSR.
 *
 *   - Check that the resume restored the SPARC floating-point registers F0 and
 *     F1.
 *
 *   - Check that the resume restored the SPARC floating-point registers F2 and
 *     F3.
 *
 *   - Check that the resume restored the SPARC floating-point registers F4 and
 *     F5.
 *
 *   - Check that the resume restored the SPARC floating-point registers F6 and
 *     F7.
 *
 *   - Check that the resume restored the SPARC floating-point registers F8 and
 *     F9.
 *
 *   - Check that the resume restored the SPARC floating-point registers F10
 *     and F11.
 *
 *   - Check that the resume restored the SPARC floating-point registers F12
 *     and F13.
 *
 *   - Check that the resume restored the SPARC floating-point registers F14
 *     and F15.
 *
 *   - Check that the resume restored the SPARC floating-point registers F16
 *     and F17.
 *
 *   - Check that the resume restored the SPARC floating-point registers F18
 *     and F19.
 *
 *   - Check that the resume restored the SPARC floating-point registers F20
 *     and F21.
 *
 *   - Check that the resume restored the SPARC floating-point registers F22
 *     and F23.
 *
 *   - Check that the resume restored the SPARC floating-point registers F24
 *     and F25.
 *
 *   - Check that the resume restored the SPARC floating-point registers F26
 *     and F27.
 *
 *   - Check that the resume restored the SPARC floating-point registers F28
 *     and F29.
 *
 *   - Check that the resume restored the SPARC floating-point registers F30
 *     and F31.
 *
 *   - Check that the resume restored the SPARC local register L0 of window 0.
 *
 *   - Check that the resume restored the SPARC local register L1 of window 0.
 *
 *   - Check that the resume restored the SPARC local register L2 of window 0.
 *
 *   - Check that the resume restored the SPARC local register L3 of window 0.
 *
 *   - Check that the resume restored the SPARC local register L4 of window 0.
 *
 *   - Check that the resume restored the SPARC local register L5 of window 0.
 *
 *   - Check that the resume restored the SPARC local register L6 of window 0.
 *
 *   - Check that the resume restored the SPARC local register L7 of window 0.
 *
 *   - Check that the resume restored the SPARC input register I0 of window 0.
 *
 *   - Check that the resume restored the SPARC input register I1 of window 0.
 *
 *   - Check that the resume restored the SPARC input register I2 of window 0.
 *
 *   - Check that the resume restored the SPARC input register I3 of window 0.
 *
 *   - Check that the resume restored the SPARC input register I4 of window 0.
 *
 *   - Check that the resume restored the SPARC input register I5 of window 0.
 *
 *   - Check that the resume restored the SPARC input register I6 of window 0.
 *
 *   - Check that the resume restored the SPARC input register I7 of window 0.
 *
 *   - Check that the resume restored the stack pointer, which is O6.
 *
 *   - Check that the resume restored the PSR.
 *
 *   - Check that the resume restored the WIM.
 *
 *   - Check that the second frame lies where the first one lay.
 *
 *   - Check that the frame of the first trap reports the address of the
 *     trapping instruction.
 *
 *   - Check that both traps happened.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Trap. Let the handler write a new value into the register windows one to
 *   three of the frame. Resume, and trap again to capture what the resume
 *   restored. Run this once for each value with the value inverted, then once
 *   unchanged.
 *
 *   - Check that the resume restored the SPARC local register L0 of window 1.
 *
 *   - Check that the resume restored the SPARC local register L1 of window 1.
 *
 *   - Check that the resume restored the SPARC local register L2 of window 1.
 *
 *   - Check that the resume restored the SPARC local register L3 of window 1.
 *
 *   - Check that the resume restored the SPARC local register L4 of window 1.
 *
 *   - Check that the resume restored the SPARC local register L5 of window 1.
 *
 *   - Check that the resume restored the SPARC local register L6 of window 1.
 *
 *   - Check that the resume restored the SPARC local register L7 of window 1.
 *
 *   - Check that the resume restored the SPARC input register I0 of window 1.
 *
 *   - Check that the resume restored the SPARC input register I1 of window 1.
 *
 *   - Check that the resume restored the SPARC input register I2 of window 1.
 *
 *   - Check that the resume restored the SPARC input register I3 of window 1.
 *
 *   - Check that the resume restored the SPARC input register I4 of window 1.
 *
 *   - Check that the resume restored the SPARC input register I5 of window 1.
 *
 *   - Check that the resume restored the SPARC input register I6 of window 1.
 *
 *   - Check that the resume restored the SPARC input register I7 of window 1.
 *
 *   - Check that the resume restored the SPARC local register L0 of window 2.
 *
 *   - Check that the resume restored the SPARC local register L1 of window 2.
 *
 *   - Check that the resume restored the SPARC local register L2 of window 2.
 *
 *   - Check that the resume restored the SPARC local register L3 of window 2.
 *
 *   - Check that the resume restored the SPARC local register L4 of window 2.
 *
 *   - Check that the resume restored the SPARC local register L5 of window 2.
 *
 *   - Check that the resume restored the SPARC local register L6 of window 2.
 *
 *   - Check that the resume restored the SPARC local register L7 of window 2.
 *
 *   - Check that the resume restored the SPARC input register I0 of window 2.
 *
 *   - Check that the resume restored the SPARC input register I1 of window 2.
 *
 *   - Check that the resume restored the SPARC input register I2 of window 2.
 *
 *   - Check that the resume restored the SPARC input register I3 of window 2.
 *
 *   - Check that the resume restored the SPARC input register I4 of window 2.
 *
 *   - Check that the resume restored the SPARC input register I5 of window 2.
 *
 *   - Check that the resume restored the SPARC input register I6 of window 2.
 *
 *   - Check that the resume restored the SPARC input register I7 of window 2.
 *
 *   - Check that the resume restored the SPARC local register L0 of window 3.
 *
 *   - Check that the resume restored the SPARC local register L1 of window 3.
 *
 *   - Check that the resume restored the SPARC local register L2 of window 3.
 *
 *   - Check that the resume restored the SPARC local register L3 of window 3.
 *
 *   - Check that the resume restored the SPARC local register L4 of window 3.
 *
 *   - Check that the resume restored the SPARC local register L5 of window 3.
 *
 *   - Check that the resume restored the SPARC local register L6 of window 3.
 *
 *   - Check that the resume restored the SPARC local register L7 of window 3.
 *
 *   - Check that the resume restored the SPARC input register I0 of window 3.
 *
 *   - Check that the resume restored the SPARC input register I1 of window 3.
 *
 *   - Check that the resume restored the SPARC input register I2 of window 3.
 *
 *   - Check that the resume restored the SPARC input register I3 of window 3.
 *
 *   - Check that the resume restored the SPARC input register I4 of window 3.
 *
 *   - Check that the resume restored the SPARC input register I5 of window 3.
 *
 *   - Check that the resume restored the SPARC input register I6 of window 3.
 *
 *   - Check that the resume restored the SPARC input register I7 of window 3.
 *
 *   - Check that both traps happened.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Trap. Let the handler write a new value into the register windows four to
 *   six of the frame. Resume, and trap again to capture what the resume
 *   restored. Run this once for each value with the value inverted, then once
 *   unchanged.
 *
 *   - Check that the resume restored the SPARC local register L0 of window 4.
 *
 *   - Check that the resume restored the SPARC local register L1 of window 4.
 *
 *   - Check that the resume restored the SPARC local register L2 of window 4.
 *
 *   - Check that the resume restored the SPARC local register L3 of window 4.
 *
 *   - Check that the resume restored the SPARC local register L4 of window 4.
 *
 *   - Check that the resume restored the SPARC local register L5 of window 4.
 *
 *   - Check that the resume restored the SPARC local register L6 of window 4.
 *
 *   - Check that the resume restored the SPARC local register L7 of window 4.
 *
 *   - Check that the resume restored the SPARC input register I0 of window 4.
 *
 *   - Check that the resume restored the SPARC input register I1 of window 4.
 *
 *   - Check that the resume restored the SPARC input register I2 of window 4.
 *
 *   - Check that the resume restored the SPARC input register I3 of window 4.
 *
 *   - Check that the resume restored the SPARC input register I4 of window 4.
 *
 *   - Check that the resume restored the SPARC input register I5 of window 4.
 *
 *   - Check that the resume restored the SPARC input register I6 of window 4.
 *
 *   - Check that the resume restored the SPARC input register I7 of window 4.
 *
 *   - Check that the resume restored the SPARC local register L0 of window 5.
 *
 *   - Check that the resume restored the SPARC local register L1 of window 5.
 *
 *   - Check that the resume restored the SPARC local register L2 of window 5.
 *
 *   - Check that the resume restored the SPARC local register L3 of window 5.
 *
 *   - Check that the resume restored the SPARC local register L4 of window 5.
 *
 *   - Check that the resume restored the SPARC local register L5 of window 5.
 *
 *   - Check that the resume restored the SPARC local register L6 of window 5.
 *
 *   - Check that the resume restored the SPARC local register L7 of window 5.
 *
 *   - Check that the resume restored the SPARC input register I0 of window 5.
 *
 *   - Check that the resume restored the SPARC input register I1 of window 5.
 *
 *   - Check that the resume restored the SPARC input register I2 of window 5.
 *
 *   - Check that the resume restored the SPARC input register I3 of window 5.
 *
 *   - Check that the resume restored the SPARC input register I4 of window 5.
 *
 *   - Check that the resume restored the SPARC input register I5 of window 5.
 *
 *   - Check that the resume restored the SPARC input register I6 of window 5.
 *
 *   - Check that the resume restored the SPARC input register I7 of window 5.
 *
 *   - Check that the resume restored the SPARC local register L0 of window 6.
 *
 *   - Check that the resume restored the SPARC local register L1 of window 6.
 *
 *   - Check that the resume restored the SPARC local register L2 of window 6.
 *
 *   - Check that the resume restored the SPARC local register L3 of window 6.
 *
 *   - Check that the resume restored the SPARC local register L4 of window 6.
 *
 *   - Check that the resume restored the SPARC local register L5 of window 6.
 *
 *   - Check that the resume restored the SPARC local register L6 of window 6.
 *
 *   - Check that the resume restored the SPARC local register L7 of window 6.
 *
 *   - Check that the resume restored the SPARC input register I0 of window 6.
 *
 *   - Check that the resume restored the SPARC input register I1 of window 6.
 *
 *   - Check that the resume restored the SPARC input register I2 of window 6.
 *
 *   - Check that the resume restored the SPARC input register I3 of window 6.
 *
 *   - Check that the resume restored the SPARC input register I4 of window 6.
 *
 *   - Check that the resume restored the SPARC input register I5 of window 6.
 *
 *   - Check that the resume restored the SPARC input register I6 of window 6.
 *
 *   - Check that the resume restored the SPARC input register I7 of window 6.
 *
 *   - Check that both traps happened.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * @{
 */

#define WORD UINT64_C( 0xffffffff )

/* RD, NS, fcc, aexc and cexc of the FSR */
#define FSR_TEST_MASK 0xc0400fff

typedef enum {
  MEMBER_GLOBAL,
  MEMBER_OUTPUT,
  MEMBER_Y,
  MEMBER_FSR,
  MEMBER_FP,
  MEMBER_LOCAL,
  MEMBER_INPUT
} MemberKind;

typedef struct {
  MemberKind kind;
  uint8_t    index;
} Member;

/* These are the frame members which the handler of the first trap writes. */
static const Member resume_members[] = {
  { MEMBER_GLOBAL, 1 },
  { MEMBER_GLOBAL, 2 },
  { MEMBER_GLOBAL, 3 },
  { MEMBER_GLOBAL, 4 },
  { MEMBER_GLOBAL, 5 },
  { MEMBER_GLOBAL, 7 },
  { MEMBER_OUTPUT, 0 },
  { MEMBER_OUTPUT, 1 },
  { MEMBER_OUTPUT, 2 },
  { MEMBER_OUTPUT, 3 },
  { MEMBER_OUTPUT, 4 },
  { MEMBER_OUTPUT, 5 },
  { MEMBER_OUTPUT, 7 },
  { MEMBER_Y, 0 },
#if SPARC_HAS_FPU == 1
  { MEMBER_FSR, 0 },
  { MEMBER_FP, 0 },
  { MEMBER_FP, 1 },
  { MEMBER_FP, 2 },
  { MEMBER_FP, 3 },
  { MEMBER_FP, 4 },
  { MEMBER_FP, 5 },
  { MEMBER_FP, 6 },
  { MEMBER_FP, 7 },
  { MEMBER_FP, 8 },
  { MEMBER_FP, 9 },
  { MEMBER_FP, 10 },
  { MEMBER_FP, 11 },
  { MEMBER_FP, 12 },
  { MEMBER_FP, 13 },
  { MEMBER_FP, 14 },
  { MEMBER_FP, 15 },
#endif
  { MEMBER_LOCAL, 0 },
  { MEMBER_LOCAL, 1 },
  { MEMBER_LOCAL, 2 },
  { MEMBER_LOCAL, 3 },
  { MEMBER_LOCAL, 4 },
  { MEMBER_LOCAL, 5 },
  { MEMBER_LOCAL, 6 },
  { MEMBER_LOCAL, 7 },
  { MEMBER_INPUT, 0 },
  { MEMBER_INPUT, 1 },
  { MEMBER_INPUT, 2 },
  { MEMBER_INPUT, 3 },
  { MEMBER_INPUT, 4 },
  { MEMBER_INPUT, 5 },
  { MEMBER_INPUT, 6 },
  { MEMBER_INPUT, 7 },
  { MEMBER_LOCAL, 8 },
  { MEMBER_LOCAL, 9 },
  { MEMBER_LOCAL, 10 },
  { MEMBER_LOCAL, 11 },
  { MEMBER_LOCAL, 12 },
  { MEMBER_LOCAL, 13 },
  { MEMBER_LOCAL, 14 },
  { MEMBER_LOCAL, 15 },
  { MEMBER_INPUT, 8 },
  { MEMBER_INPUT, 9 },
  { MEMBER_INPUT, 10 },
  { MEMBER_INPUT, 11 },
  { MEMBER_INPUT, 12 },
  { MEMBER_INPUT, 13 },
  { MEMBER_INPUT, 14 },
  { MEMBER_INPUT, 15 },
  { MEMBER_LOCAL, 16 },
  { MEMBER_LOCAL, 17 },
  { MEMBER_LOCAL, 18 },
  { MEMBER_LOCAL, 19 },
  { MEMBER_LOCAL, 20 },
  { MEMBER_LOCAL, 21 },
  { MEMBER_LOCAL, 22 },
  { MEMBER_LOCAL, 23 },
  { MEMBER_INPUT, 16 },
  { MEMBER_INPUT, 17 },
  { MEMBER_INPUT, 18 },
  { MEMBER_INPUT, 19 },
  { MEMBER_INPUT, 20 },
  { MEMBER_INPUT, 21 },
  { MEMBER_INPUT, 22 },
  { MEMBER_INPUT, 23 },
  { MEMBER_LOCAL, 24 },
  { MEMBER_LOCAL, 25 },
  { MEMBER_LOCAL, 26 },
  { MEMBER_LOCAL, 27 },
  { MEMBER_LOCAL, 28 },
  { MEMBER_LOCAL, 29 },
  { MEMBER_LOCAL, 30 },
  { MEMBER_LOCAL, 31 },
  { MEMBER_INPUT, 24 },
  { MEMBER_INPUT, 25 },
  { MEMBER_INPUT, 26 },
  { MEMBER_INPUT, 27 },
  { MEMBER_INPUT, 28 },
  { MEMBER_INPUT, 29 },
  { MEMBER_INPUT, 30 },
  { MEMBER_INPUT, 31 },
  { MEMBER_LOCAL, 32 },
  { MEMBER_LOCAL, 33 },
  { MEMBER_LOCAL, 34 },
  { MEMBER_LOCAL, 35 },
  { MEMBER_LOCAL, 36 },
  { MEMBER_LOCAL, 37 },
  { MEMBER_LOCAL, 38 },
  { MEMBER_LOCAL, 39 },
  { MEMBER_INPUT, 32 },
  { MEMBER_INPUT, 33 },
  { MEMBER_INPUT, 34 },
  { MEMBER_INPUT, 35 },
  { MEMBER_INPUT, 36 },
  { MEMBER_INPUT, 37 },
  { MEMBER_INPUT, 38 },
  { MEMBER_INPUT, 39 },
  { MEMBER_LOCAL, 40 },
  { MEMBER_LOCAL, 41 },
  { MEMBER_LOCAL, 42 },
  { MEMBER_LOCAL, 43 },
  { MEMBER_LOCAL, 44 },
  { MEMBER_LOCAL, 45 },
  { MEMBER_LOCAL, 46 },
  { MEMBER_LOCAL, 47 },
  { MEMBER_INPUT, 40 },
  { MEMBER_INPUT, 41 },
  { MEMBER_INPUT, 42 },
  { MEMBER_INPUT, 43 },
  { MEMBER_INPUT, 44 },
  { MEMBER_INPUT, 45 },
  { MEMBER_INPUT, 46 },
  { MEMBER_INPUT, 47 },
  { MEMBER_LOCAL, 48 },
  { MEMBER_LOCAL, 49 },
  { MEMBER_LOCAL, 50 },
  { MEMBER_LOCAL, 51 },
  { MEMBER_LOCAL, 52 },
  { MEMBER_LOCAL, 53 },
  { MEMBER_LOCAL, 54 },
  { MEMBER_LOCAL, 55 },
  { MEMBER_INPUT, 48 },
  { MEMBER_INPUT, 49 },
  { MEMBER_INPUT, 50 },
  { MEMBER_INPUT, 51 },
  { MEMBER_INPUT, 52 },
  { MEMBER_INPUT, 53 },
  { MEMBER_INPUT, 54 },
  { MEMBER_INPUT, 55 },
};

typedef enum {
  MEMBER_INDEX_G1,
  MEMBER_INDEX_G2,
  MEMBER_INDEX_G3,
  MEMBER_INDEX_G4,
  MEMBER_INDEX_G5,
  MEMBER_INDEX_G7,
  MEMBER_INDEX_O0,
  MEMBER_INDEX_O1,
  MEMBER_INDEX_O2,
  MEMBER_INDEX_O3,
  MEMBER_INDEX_O4,
  MEMBER_INDEX_O5,
  MEMBER_INDEX_O7,
  MEMBER_INDEX_Y,
#if SPARC_HAS_FPU == 1
  MEMBER_INDEX_FSR,
  MEMBER_INDEX_F0_F1,
  MEMBER_INDEX_F2_F3,
  MEMBER_INDEX_F4_F5,
  MEMBER_INDEX_F6_F7,
  MEMBER_INDEX_F8_F9,
  MEMBER_INDEX_F10_F11,
  MEMBER_INDEX_F12_F13,
  MEMBER_INDEX_F14_F15,
  MEMBER_INDEX_F16_F17,
  MEMBER_INDEX_F18_F19,
  MEMBER_INDEX_F20_F21,
  MEMBER_INDEX_F22_F23,
  MEMBER_INDEX_F24_F25,
  MEMBER_INDEX_F26_F27,
  MEMBER_INDEX_F28_F29,
  MEMBER_INDEX_F30_F31,
#endif
  MEMBER_INDEX_W0_L0,
  MEMBER_INDEX_W0_L1,
  MEMBER_INDEX_W0_L2,
  MEMBER_INDEX_W0_L3,
  MEMBER_INDEX_W0_L4,
  MEMBER_INDEX_W0_L5,
  MEMBER_INDEX_W0_L6,
  MEMBER_INDEX_W0_L7,
  MEMBER_INDEX_W0_I0,
  MEMBER_INDEX_W0_I1,
  MEMBER_INDEX_W0_I2,
  MEMBER_INDEX_W0_I3,
  MEMBER_INDEX_W0_I4,
  MEMBER_INDEX_W0_I5,
  MEMBER_INDEX_W0_I6,
  MEMBER_INDEX_W0_I7,
  MEMBER_INDEX_W1_L0,
  MEMBER_INDEX_W1_L1,
  MEMBER_INDEX_W1_L2,
  MEMBER_INDEX_W1_L3,
  MEMBER_INDEX_W1_L4,
  MEMBER_INDEX_W1_L5,
  MEMBER_INDEX_W1_L6,
  MEMBER_INDEX_W1_L7,
  MEMBER_INDEX_W1_I0,
  MEMBER_INDEX_W1_I1,
  MEMBER_INDEX_W1_I2,
  MEMBER_INDEX_W1_I3,
  MEMBER_INDEX_W1_I4,
  MEMBER_INDEX_W1_I5,
  MEMBER_INDEX_W1_I6,
  MEMBER_INDEX_W1_I7,
  MEMBER_INDEX_W2_L0,
  MEMBER_INDEX_W2_L1,
  MEMBER_INDEX_W2_L2,
  MEMBER_INDEX_W2_L3,
  MEMBER_INDEX_W2_L4,
  MEMBER_INDEX_W2_L5,
  MEMBER_INDEX_W2_L6,
  MEMBER_INDEX_W2_L7,
  MEMBER_INDEX_W2_I0,
  MEMBER_INDEX_W2_I1,
  MEMBER_INDEX_W2_I2,
  MEMBER_INDEX_W2_I3,
  MEMBER_INDEX_W2_I4,
  MEMBER_INDEX_W2_I5,
  MEMBER_INDEX_W2_I6,
  MEMBER_INDEX_W2_I7,
  MEMBER_INDEX_W3_L0,
  MEMBER_INDEX_W3_L1,
  MEMBER_INDEX_W3_L2,
  MEMBER_INDEX_W3_L3,
  MEMBER_INDEX_W3_L4,
  MEMBER_INDEX_W3_L5,
  MEMBER_INDEX_W3_L6,
  MEMBER_INDEX_W3_L7,
  MEMBER_INDEX_W3_I0,
  MEMBER_INDEX_W3_I1,
  MEMBER_INDEX_W3_I2,
  MEMBER_INDEX_W3_I3,
  MEMBER_INDEX_W3_I4,
  MEMBER_INDEX_W3_I5,
  MEMBER_INDEX_W3_I6,
  MEMBER_INDEX_W3_I7,
  MEMBER_INDEX_W4_L0,
  MEMBER_INDEX_W4_L1,
  MEMBER_INDEX_W4_L2,
  MEMBER_INDEX_W4_L3,
  MEMBER_INDEX_W4_L4,
  MEMBER_INDEX_W4_L5,
  MEMBER_INDEX_W4_L6,
  MEMBER_INDEX_W4_L7,
  MEMBER_INDEX_W4_I0,
  MEMBER_INDEX_W4_I1,
  MEMBER_INDEX_W4_I2,
  MEMBER_INDEX_W4_I3,
  MEMBER_INDEX_W4_I4,
  MEMBER_INDEX_W4_I5,
  MEMBER_INDEX_W4_I6,
  MEMBER_INDEX_W4_I7,
  MEMBER_INDEX_W5_L0,
  MEMBER_INDEX_W5_L1,
  MEMBER_INDEX_W5_L2,
  MEMBER_INDEX_W5_L3,
  MEMBER_INDEX_W5_L4,
  MEMBER_INDEX_W5_L5,
  MEMBER_INDEX_W5_L6,
  MEMBER_INDEX_W5_L7,
  MEMBER_INDEX_W5_I0,
  MEMBER_INDEX_W5_I1,
  MEMBER_INDEX_W5_I2,
  MEMBER_INDEX_W5_I3,
  MEMBER_INDEX_W5_I4,
  MEMBER_INDEX_W5_I5,
  MEMBER_INDEX_W5_I6,
  MEMBER_INDEX_W5_I7,
  MEMBER_INDEX_W6_L0,
  MEMBER_INDEX_W6_L1,
  MEMBER_INDEX_W6_L2,
  MEMBER_INDEX_W6_L3,
  MEMBER_INDEX_W6_L4,
  MEMBER_INDEX_W6_L5,
  MEMBER_INDEX_W6_L6,
  MEMBER_INDEX_W6_L7,
  MEMBER_INDEX_W6_I0,
  MEMBER_INDEX_W6_I1,
  MEMBER_INDEX_W6_I2,
  MEMBER_INDEX_W6_I3,
  MEMBER_INDEX_W6_I4,
  MEMBER_INDEX_W6_I5,
  MEMBER_INDEX_W6_I6,
  MEMBER_INDEX_W6_I7,
  MEMBER_INDEX_MAX
} MemberIndex;

static uint64_t GetMember( const CPU_Exception_frame *frame, size_t i )
{
  const Member *m;

  m = &resume_members[ i ];

  switch ( m->kind ) {
    case MEMBER_GLOBAL:
      return frame->global[ m->index ];
    case MEMBER_OUTPUT:
      return frame->output[ m->index ];
    case MEMBER_Y:
      return frame->y;
#if SPARC_HAS_FPU == 1
    case MEMBER_FSR:
      return frame->fsr;
    case MEMBER_FP:
      return frame->fp[ m->index ];
#endif
    case MEMBER_LOCAL:
      return frame->windows[ m->index / 8 ].local[ m->index % 8 ];
    default:
      return frame->windows[ m->index / 8 ].input[ m->index % 8 ];
  }
}

static void SetMember( CPU_Exception_frame *frame, size_t i, uint64_t value )
{
  const Member *m;

  m = &resume_members[ i ];

  switch ( m->kind ) {
    case MEMBER_GLOBAL:
      frame->global[ m->index ] = (uint32_t) value;
      break;
    case MEMBER_OUTPUT:
      frame->output[ m->index ] = (uint32_t) value;
      break;
    case MEMBER_Y:
      frame->y = (uint32_t) value;
      break;
#if SPARC_HAS_FPU == 1
    case MEMBER_FSR:
      frame->fsr = (uint32_t) value;
      break;
    case MEMBER_FP:
      frame->fp[ m->index ] = value;
      break;
#endif
    case MEMBER_LOCAL:
      frame->windows[ m->index / 8 ].local[ m->index % 8 ] = (uint32_t) value;
      break;
    default:
      frame->windows[ m->index / 8 ].input[ m->index % 8 ] = (uint32_t) value;
      break;
  }
}

static uint64_t GetPattern( size_t i )
{
  const Member *m;

  m = &resume_members[ i ];

  if ( m->kind == MEMBER_FSR ) {
    return 0x404008a5;
  }

  if ( m->kind == MEMBER_FP ) {
    return UINT64_C( 0x8182838485868788 ) + UINT64_C( 0x0101010101010101 ) * i;
  }

  return UINT32_C( 0x31323334 ) + UINT32_C( 0x01010101 ) * (uint32_t) i;
}

/*
 * A run of a register check changes the value of one member of its group.
 * The handler of the first trap writes the value of the table of initial
 * values for each member of the current group and the pattern for each
 * other member.
 */
typedef struct {
  size_t    begin;
  size_t    end;
  uint64_t *initial;
} ResumeGroup;

static const ResumeGroup *resume_group;

static CPU_Exception_frame resume_before;

static CPU_Exception_frame resume_after;

static const CPU_Exception_frame *resume_frame[ 2 ];

static int resume_trap;

extern const char resume_trap1_label[];

extern const char resume_trap2_label[];

extern const char resume_done_label[];

#define RESUME_STACK_WORDS 128

static uint64_t resume_stack[ RESUME_STACK_WORDS ];

void ResumeRun( void );

/*
 * The two traps sit next to each other.  The first one captures the register
 * set, and its handler writes a value into every member of the frame.  The
 * second one captures what the resume restored.
 */
__asm__( "  .globl ResumeRun\n"
         "ResumeRun:\n"
         "  save %sp, -96, %sp\n"
         "  .globl resume_trap1_label\n"
         "resume_trap1_label:\n"
         "  unimp 0\n"
         "  .globl resume_trap2_label\n"
         "resume_trap2_label:\n"
         "  unimp 0\n"
         "  .globl resume_done_label\n"
         "resume_done_label:\n"
         "  ret\n"
         "   restore\n" );

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

  if ( resume_trap < 2 ) {
    resume_frame[ resume_trap ] = frame;
  }

  ++resume_trap;

  if ( resume_trap == 1 ) {
    size_t i;

    resume_before = *frame;

    for ( i = 0; i < MEMBER_INDEX_MAX; ++i ) {
      uint64_t value;

      if ( i >= resume_group->begin && i < resume_group->end ) {
        value = resume_group->initial[ i - resume_group->begin ];
      } else {
        value = GetPattern( i );
      }

      SetMember( frame, i, value );
    }

    /*
     * The stack pointer takes a trampoline stack.  A window trap between the
     * two traps would write to it.
     */
    frame->output[ 6 ] = (uint32_t) &resume_stack[ RESUME_STACK_WORDS - 12 ];

    /*
     * The thread pointer and the windows hold values of the test until the
     * second trap, so no interrupt may arrive in between.
     */
    frame->psr |= SPARC_PSR_PIL_MASK;
    frame->pc = (uint32_t) resume_trap2_label;
    frame->npc = frame->pc + 4;
  } else {
    resume_after = *frame;
    SetFatalHandler( NULL, NULL );
    *frame = resume_before;
    frame->pc = (uint32_t) resume_done_label;
    frame->npc = frame->pc + 4;
  }

  _CPU_Exception_resume( frame );
}

static void ResumeRunTraps( const ResumeGroup *group )
{
  resume_group = group;
  resume_trap = 0;
  resume_frame[ 0 ] = NULL;
  resume_frame[ 1 ] = NULL;
  memset( &resume_before, 0, sizeof( resume_before ) );
  memset( &resume_after, 0, sizeof( resume_after ) );
  SetFatalHandler( ResumeFatal, NULL );
  ResumeRun();
  SetFatalHandler( NULL, NULL );
}

typedef enum {
  RESUME_A_G1,
  RESUME_A_G2,
  RESUME_A_G3,
  RESUME_A_G4,
  RESUME_A_G5,
  RESUME_A_G7,
  RESUME_A_O0,
  RESUME_A_O1,
  RESUME_A_O2,
  RESUME_A_O3,
  RESUME_A_O4,
  RESUME_A_O5,
  RESUME_A_O7,
  RESUME_A_Y,
#if SPARC_HAS_FPU == 1
  RESUME_A_FSR,
  RESUME_A_F0_F1,
  RESUME_A_F2_F3,
  RESUME_A_F4_F5,
  RESUME_A_F6_F7,
  RESUME_A_F8_F9,
  RESUME_A_F10_F11,
  RESUME_A_F12_F13,
  RESUME_A_F14_F15,
  RESUME_A_F16_F17,
  RESUME_A_F18_F19,
  RESUME_A_F20_F21,
  RESUME_A_F22_F23,
  RESUME_A_F24_F25,
  RESUME_A_F26_F27,
  RESUME_A_F28_F29,
  RESUME_A_F30_F31,
#endif
  RESUME_A_W0_L0,
  RESUME_A_W0_L1,
  RESUME_A_W0_L2,
  RESUME_A_W0_L3,
  RESUME_A_W0_L4,
  RESUME_A_W0_L5,
  RESUME_A_W0_L6,
  RESUME_A_W0_L7,
  RESUME_A_W0_I0,
  RESUME_A_W0_I1,
  RESUME_A_W0_I2,
  RESUME_A_W0_I3,
  RESUME_A_W0_I4,
  RESUME_A_W0_I5,
  RESUME_A_W0_I6,
  RESUME_A_W0_I7,
  RESUME_A_SP,
  RESUME_A_PSR,
  RESUME_A_WIM,
  RESUME_A_FRAME,
  RESUME_A_TRAP1_PC,
  RESUME_A_TRAPS,
  RESUME_A_MAX
} ResumeAIndex;

RTEMS_STATIC_ASSERT( RESUME_A_G1 == 0, resume_a_first );

static const RegisterCheckSource resume_a_sources[ RESUME_A_MAX ] = {
  { "g1", REGISTER_CHECK_INITIAL, WORD },
  { "g2", REGISTER_CHECK_INITIAL, WORD },
  { "g3", REGISTER_CHECK_INITIAL, WORD },
  { "g4", REGISTER_CHECK_INITIAL, WORD },
  { "g5", REGISTER_CHECK_INITIAL, WORD },
  { "g7", REGISTER_CHECK_INITIAL, WORD },
  { "o0", REGISTER_CHECK_INITIAL, WORD },
  { "o1", REGISTER_CHECK_INITIAL, WORD },
  { "o2", REGISTER_CHECK_INITIAL, WORD },
  { "o3", REGISTER_CHECK_INITIAL, WORD },
  { "o4", REGISTER_CHECK_INITIAL, WORD },
  { "o5", REGISTER_CHECK_INITIAL, WORD },
  { "o7", REGISTER_CHECK_INITIAL, WORD },
  { "y", REGISTER_CHECK_INITIAL, WORD },
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
  { "w0 l0", REGISTER_CHECK_INITIAL, WORD },
  { "w0 l1", REGISTER_CHECK_INITIAL, WORD },
  { "w0 l2", REGISTER_CHECK_INITIAL, WORD },
  { "w0 l3", REGISTER_CHECK_INITIAL, WORD },
  { "w0 l4", REGISTER_CHECK_INITIAL, WORD },
  { "w0 l5", REGISTER_CHECK_INITIAL, WORD },
  { "w0 l6", REGISTER_CHECK_INITIAL, WORD },
  { "w0 l7", REGISTER_CHECK_INITIAL, WORD },
  { "w0 i0", REGISTER_CHECK_INITIAL, WORD },
  { "w0 i1", REGISTER_CHECK_INITIAL, WORD },
  { "w0 i2", REGISTER_CHECK_INITIAL, WORD },
  { "w0 i3", REGISTER_CHECK_INITIAL, WORD },
  { "w0 i4", REGISTER_CHECK_INITIAL, WORD },
  { "w0 i5", REGISTER_CHECK_INITIAL, WORD },
  { "w0 i6", REGISTER_CHECK_INITIAL, WORD },
  { "w0 i7", REGISTER_CHECK_INITIAL, WORD },
  { "sp", REGISTER_CHECK_EXPECTED, WORD },
  { "psr", REGISTER_CHECK_EXPECTED, WORD },
  { "wim", REGISTER_CHECK_EXPECTED, WORD },
  { "frame", REGISTER_CHECK_EXPECTED, WORD },
  { "trap 1 pc", REGISTER_CHECK_EXPECTED, WORD },
  { "traps", REGISTER_CHECK_EXPECTED, WORD },
};

static const RegisterCheckSlot resume_a_slots[ RESUME_A_MAX ] = {
  { "g1", RESUME_A_G1, WORD },
  { "g2", RESUME_A_G2, WORD },
  { "g3", RESUME_A_G3, WORD },
  { "g4", RESUME_A_G4, WORD },
  { "g5", RESUME_A_G5, WORD },
  { "g7", RESUME_A_G7, WORD },
  { "o0", RESUME_A_O0, WORD },
  { "o1", RESUME_A_O1, WORD },
  { "o2", RESUME_A_O2, WORD },
  { "o3", RESUME_A_O3, WORD },
  { "o4", RESUME_A_O4, WORD },
  { "o5", RESUME_A_O5, WORD },
  { "o7", RESUME_A_O7, WORD },
  { "y", RESUME_A_Y, WORD },
#if SPARC_HAS_FPU == 1
  { "fsr", RESUME_A_FSR, FSR_TEST_MASK },
  { "f0_f1", RESUME_A_F0_F1, UINT64_MAX },
  { "f2_f3", RESUME_A_F2_F3, UINT64_MAX },
  { "f4_f5", RESUME_A_F4_F5, UINT64_MAX },
  { "f6_f7", RESUME_A_F6_F7, UINT64_MAX },
  { "f8_f9", RESUME_A_F8_F9, UINT64_MAX },
  { "f10_f11", RESUME_A_F10_F11, UINT64_MAX },
  { "f12_f13", RESUME_A_F12_F13, UINT64_MAX },
  { "f14_f15", RESUME_A_F14_F15, UINT64_MAX },
  { "f16_f17", RESUME_A_F16_F17, UINT64_MAX },
  { "f18_f19", RESUME_A_F18_F19, UINT64_MAX },
  { "f20_f21", RESUME_A_F20_F21, UINT64_MAX },
  { "f22_f23", RESUME_A_F22_F23, UINT64_MAX },
  { "f24_f25", RESUME_A_F24_F25, UINT64_MAX },
  { "f26_f27", RESUME_A_F26_F27, UINT64_MAX },
  { "f28_f29", RESUME_A_F28_F29, UINT64_MAX },
  { "f30_f31", RESUME_A_F30_F31, UINT64_MAX },
#endif
  { "w0 l0", RESUME_A_W0_L0, WORD },
  { "w0 l1", RESUME_A_W0_L1, WORD },
  { "w0 l2", RESUME_A_W0_L2, WORD },
  { "w0 l3", RESUME_A_W0_L3, WORD },
  { "w0 l4", RESUME_A_W0_L4, WORD },
  { "w0 l5", RESUME_A_W0_L5, WORD },
  { "w0 l6", RESUME_A_W0_L6, WORD },
  { "w0 l7", RESUME_A_W0_L7, WORD },
  { "w0 i0", RESUME_A_W0_I0, WORD },
  { "w0 i1", RESUME_A_W0_I1, WORD },
  { "w0 i2", RESUME_A_W0_I2, WORD },
  { "w0 i3", RESUME_A_W0_I3, WORD },
  { "w0 i4", RESUME_A_W0_I4, WORD },
  { "w0 i5", RESUME_A_W0_I5, WORD },
  { "w0 i6", RESUME_A_W0_I6, WORD },
  { "w0 i7", RESUME_A_W0_I7, WORD },
  { "sp", RESUME_A_SP, WORD },
  { "psr", RESUME_A_PSR, WORD },
  { "wim", RESUME_A_WIM, WORD },
  { "frame", RESUME_A_FRAME, WORD },
  { "trap 1 pc", RESUME_A_TRAP1_PC, WORD },
  { "traps", RESUME_A_TRAPS, WORD },
};

static uint64_t resume_a_initial[ RESUME_A_MAX ];

static const ResumeGroup resume_a_group = {
  .begin = MEMBER_INDEX_G1,
  .end = MEMBER_INDEX_W0_I7 + 1,
  .initial = resume_a_initial
};

static void ResumeARun( RegisterCheck *self, void *arg )
{
  size_t i;

  (void) arg;
  ResumeRunTraps( &resume_a_group );

  for ( i = MEMBER_INDEX_G1; i <= MEMBER_INDEX_W0_I7; ++i ) {
    RegisterCheckRecord(
      self,
      i - MEMBER_INDEX_G1,
      GetMember( &resume_after, i ),
      GetPattern( i )
    );
  }

  RegisterCheckRecord(
    self,
    RESUME_A_SP,
    resume_after.output[ 6 ],
    (uintptr_t) &resume_stack[ RESUME_STACK_WORDS - 12 ]
  );
  RegisterCheckRecord(
    self,
    RESUME_A_PSR,
    resume_after.psr,
    resume_before.psr | SPARC_PSR_PIL_MASK
  );
  RegisterCheckRecord(
    self,
    RESUME_A_WIM,
    resume_after.wim,
    resume_before.wim
  );
  RegisterCheckRecord(
    self,
    RESUME_A_FRAME,
    (uintptr_t) resume_frame[ 1 ],
    (uintptr_t) resume_frame[ 0 ]
  );
  RegisterCheckRecord(
    self,
    RESUME_A_TRAP1_PC,
    resume_before.pc,
    (uintptr_t) resume_trap1_label
  );
  RegisterCheckRecord( self, RESUME_A_TRAPS, (uint64_t) resume_trap, 2 );
}

static RegisterCheck resume_a_check = {
  .sources = resume_a_sources,
  .source_count = RESUME_A_MAX,
  .slots = resume_a_slots,
  .slot_count = RESUME_A_MAX,
  .initial = resume_a_initial,
  .run = ResumeARun
};

static void ResumeAPrepare( void )
{
  size_t i;

  for ( i = MEMBER_INDEX_G1; i <= MEMBER_INDEX_W0_I7; ++i ) {
    resume_a_initial[ i - MEMBER_INDEX_G1 ] = GetPattern( i );
  }
}

typedef enum {
  RESUME_B_W1_L0,
  RESUME_B_W1_L1,
  RESUME_B_W1_L2,
  RESUME_B_W1_L3,
  RESUME_B_W1_L4,
  RESUME_B_W1_L5,
  RESUME_B_W1_L6,
  RESUME_B_W1_L7,
  RESUME_B_W1_I0,
  RESUME_B_W1_I1,
  RESUME_B_W1_I2,
  RESUME_B_W1_I3,
  RESUME_B_W1_I4,
  RESUME_B_W1_I5,
  RESUME_B_W1_I6,
  RESUME_B_W1_I7,
  RESUME_B_W2_L0,
  RESUME_B_W2_L1,
  RESUME_B_W2_L2,
  RESUME_B_W2_L3,
  RESUME_B_W2_L4,
  RESUME_B_W2_L5,
  RESUME_B_W2_L6,
  RESUME_B_W2_L7,
  RESUME_B_W2_I0,
  RESUME_B_W2_I1,
  RESUME_B_W2_I2,
  RESUME_B_W2_I3,
  RESUME_B_W2_I4,
  RESUME_B_W2_I5,
  RESUME_B_W2_I6,
  RESUME_B_W2_I7,
  RESUME_B_W3_L0,
  RESUME_B_W3_L1,
  RESUME_B_W3_L2,
  RESUME_B_W3_L3,
  RESUME_B_W3_L4,
  RESUME_B_W3_L5,
  RESUME_B_W3_L6,
  RESUME_B_W3_L7,
  RESUME_B_W3_I0,
  RESUME_B_W3_I1,
  RESUME_B_W3_I2,
  RESUME_B_W3_I3,
  RESUME_B_W3_I4,
  RESUME_B_W3_I5,
  RESUME_B_W3_I6,
  RESUME_B_W3_I7,
  RESUME_B_TRAPS,
  RESUME_B_MAX
} ResumeBIndex;

RTEMS_STATIC_ASSERT( RESUME_B_W1_L0 == 0, resume_b_first );

static const RegisterCheckSource resume_b_sources[ RESUME_B_MAX ] = {
  { "w1 l0", REGISTER_CHECK_INITIAL, WORD },
  { "w1 l1", REGISTER_CHECK_INITIAL, WORD },
  { "w1 l2", REGISTER_CHECK_INITIAL, WORD },
  { "w1 l3", REGISTER_CHECK_INITIAL, WORD },
  { "w1 l4", REGISTER_CHECK_INITIAL, WORD },
  { "w1 l5", REGISTER_CHECK_INITIAL, WORD },
  { "w1 l6", REGISTER_CHECK_INITIAL, WORD },
  { "w1 l7", REGISTER_CHECK_INITIAL, WORD },
  { "w1 i0", REGISTER_CHECK_INITIAL, WORD },
  { "w1 i1", REGISTER_CHECK_INITIAL, WORD },
  { "w1 i2", REGISTER_CHECK_INITIAL, WORD },
  { "w1 i3", REGISTER_CHECK_INITIAL, WORD },
  { "w1 i4", REGISTER_CHECK_INITIAL, WORD },
  { "w1 i5", REGISTER_CHECK_INITIAL, WORD },
  { "w1 i6", REGISTER_CHECK_INITIAL, WORD },
  { "w1 i7", REGISTER_CHECK_INITIAL, WORD },
  { "w2 l0", REGISTER_CHECK_INITIAL, WORD },
  { "w2 l1", REGISTER_CHECK_INITIAL, WORD },
  { "w2 l2", REGISTER_CHECK_INITIAL, WORD },
  { "w2 l3", REGISTER_CHECK_INITIAL, WORD },
  { "w2 l4", REGISTER_CHECK_INITIAL, WORD },
  { "w2 l5", REGISTER_CHECK_INITIAL, WORD },
  { "w2 l6", REGISTER_CHECK_INITIAL, WORD },
  { "w2 l7", REGISTER_CHECK_INITIAL, WORD },
  { "w2 i0", REGISTER_CHECK_INITIAL, WORD },
  { "w2 i1", REGISTER_CHECK_INITIAL, WORD },
  { "w2 i2", REGISTER_CHECK_INITIAL, WORD },
  { "w2 i3", REGISTER_CHECK_INITIAL, WORD },
  { "w2 i4", REGISTER_CHECK_INITIAL, WORD },
  { "w2 i5", REGISTER_CHECK_INITIAL, WORD },
  { "w2 i6", REGISTER_CHECK_INITIAL, WORD },
  { "w2 i7", REGISTER_CHECK_INITIAL, WORD },
  { "w3 l0", REGISTER_CHECK_INITIAL, WORD },
  { "w3 l1", REGISTER_CHECK_INITIAL, WORD },
  { "w3 l2", REGISTER_CHECK_INITIAL, WORD },
  { "w3 l3", REGISTER_CHECK_INITIAL, WORD },
  { "w3 l4", REGISTER_CHECK_INITIAL, WORD },
  { "w3 l5", REGISTER_CHECK_INITIAL, WORD },
  { "w3 l6", REGISTER_CHECK_INITIAL, WORD },
  { "w3 l7", REGISTER_CHECK_INITIAL, WORD },
  { "w3 i0", REGISTER_CHECK_INITIAL, WORD },
  { "w3 i1", REGISTER_CHECK_INITIAL, WORD },
  { "w3 i2", REGISTER_CHECK_INITIAL, WORD },
  { "w3 i3", REGISTER_CHECK_INITIAL, WORD },
  { "w3 i4", REGISTER_CHECK_INITIAL, WORD },
  { "w3 i5", REGISTER_CHECK_INITIAL, WORD },
  { "w3 i6", REGISTER_CHECK_INITIAL, WORD },
  { "w3 i7", REGISTER_CHECK_INITIAL, WORD },
  { "traps", REGISTER_CHECK_EXPECTED, WORD },
};

static const RegisterCheckSlot resume_b_slots[ RESUME_B_MAX ] = {
  { "w1 l0", RESUME_B_W1_L0, WORD },
  { "w1 l1", RESUME_B_W1_L1, WORD },
  { "w1 l2", RESUME_B_W1_L2, WORD },
  { "w1 l3", RESUME_B_W1_L3, WORD },
  { "w1 l4", RESUME_B_W1_L4, WORD },
  { "w1 l5", RESUME_B_W1_L5, WORD },
  { "w1 l6", RESUME_B_W1_L6, WORD },
  { "w1 l7", RESUME_B_W1_L7, WORD },
  { "w1 i0", RESUME_B_W1_I0, WORD },
  { "w1 i1", RESUME_B_W1_I1, WORD },
  { "w1 i2", RESUME_B_W1_I2, WORD },
  { "w1 i3", RESUME_B_W1_I3, WORD },
  { "w1 i4", RESUME_B_W1_I4, WORD },
  { "w1 i5", RESUME_B_W1_I5, WORD },
  { "w1 i6", RESUME_B_W1_I6, WORD },
  { "w1 i7", RESUME_B_W1_I7, WORD },
  { "w2 l0", RESUME_B_W2_L0, WORD },
  { "w2 l1", RESUME_B_W2_L1, WORD },
  { "w2 l2", RESUME_B_W2_L2, WORD },
  { "w2 l3", RESUME_B_W2_L3, WORD },
  { "w2 l4", RESUME_B_W2_L4, WORD },
  { "w2 l5", RESUME_B_W2_L5, WORD },
  { "w2 l6", RESUME_B_W2_L6, WORD },
  { "w2 l7", RESUME_B_W2_L7, WORD },
  { "w2 i0", RESUME_B_W2_I0, WORD },
  { "w2 i1", RESUME_B_W2_I1, WORD },
  { "w2 i2", RESUME_B_W2_I2, WORD },
  { "w2 i3", RESUME_B_W2_I3, WORD },
  { "w2 i4", RESUME_B_W2_I4, WORD },
  { "w2 i5", RESUME_B_W2_I5, WORD },
  { "w2 i6", RESUME_B_W2_I6, WORD },
  { "w2 i7", RESUME_B_W2_I7, WORD },
  { "w3 l0", RESUME_B_W3_L0, WORD },
  { "w3 l1", RESUME_B_W3_L1, WORD },
  { "w3 l2", RESUME_B_W3_L2, WORD },
  { "w3 l3", RESUME_B_W3_L3, WORD },
  { "w3 l4", RESUME_B_W3_L4, WORD },
  { "w3 l5", RESUME_B_W3_L5, WORD },
  { "w3 l6", RESUME_B_W3_L6, WORD },
  { "w3 l7", RESUME_B_W3_L7, WORD },
  { "w3 i0", RESUME_B_W3_I0, WORD },
  { "w3 i1", RESUME_B_W3_I1, WORD },
  { "w3 i2", RESUME_B_W3_I2, WORD },
  { "w3 i3", RESUME_B_W3_I3, WORD },
  { "w3 i4", RESUME_B_W3_I4, WORD },
  { "w3 i5", RESUME_B_W3_I5, WORD },
  { "w3 i6", RESUME_B_W3_I6, WORD },
  { "w3 i7", RESUME_B_W3_I7, WORD },
  { "traps", RESUME_B_TRAPS, WORD },
};

static uint64_t resume_b_initial[ RESUME_B_MAX ];

static const ResumeGroup resume_b_group = {
  .begin = MEMBER_INDEX_W1_L0,
  .end = MEMBER_INDEX_W3_I7 + 1,
  .initial = resume_b_initial
};

static void ResumeBRun( RegisterCheck *self, void *arg )
{
  size_t i;

  (void) arg;
  ResumeRunTraps( &resume_b_group );

  for ( i = MEMBER_INDEX_W1_L0; i <= MEMBER_INDEX_W3_I7; ++i ) {
    RegisterCheckRecord(
      self,
      i - MEMBER_INDEX_W1_L0,
      GetMember( &resume_after, i ),
      GetPattern( i )
    );
  }

  RegisterCheckRecord( self, RESUME_B_TRAPS, (uint64_t) resume_trap, 2 );
}

static RegisterCheck resume_b_check = {
  .sources = resume_b_sources,
  .source_count = RESUME_B_MAX,
  .slots = resume_b_slots,
  .slot_count = RESUME_B_MAX,
  .initial = resume_b_initial,
  .run = ResumeBRun
};

static void ResumeBPrepare( void )
{
  size_t i;

  for ( i = MEMBER_INDEX_W1_L0; i <= MEMBER_INDEX_W3_I7; ++i ) {
    resume_b_initial[ i - MEMBER_INDEX_W1_L0 ] = GetPattern( i );
  }
}

typedef enum {
  RESUME_C_W4_L0,
  RESUME_C_W4_L1,
  RESUME_C_W4_L2,
  RESUME_C_W4_L3,
  RESUME_C_W4_L4,
  RESUME_C_W4_L5,
  RESUME_C_W4_L6,
  RESUME_C_W4_L7,
  RESUME_C_W4_I0,
  RESUME_C_W4_I1,
  RESUME_C_W4_I2,
  RESUME_C_W4_I3,
  RESUME_C_W4_I4,
  RESUME_C_W4_I5,
  RESUME_C_W4_I6,
  RESUME_C_W4_I7,
  RESUME_C_W5_L0,
  RESUME_C_W5_L1,
  RESUME_C_W5_L2,
  RESUME_C_W5_L3,
  RESUME_C_W5_L4,
  RESUME_C_W5_L5,
  RESUME_C_W5_L6,
  RESUME_C_W5_L7,
  RESUME_C_W5_I0,
  RESUME_C_W5_I1,
  RESUME_C_W5_I2,
  RESUME_C_W5_I3,
  RESUME_C_W5_I4,
  RESUME_C_W5_I5,
  RESUME_C_W5_I6,
  RESUME_C_W5_I7,
  RESUME_C_W6_L0,
  RESUME_C_W6_L1,
  RESUME_C_W6_L2,
  RESUME_C_W6_L3,
  RESUME_C_W6_L4,
  RESUME_C_W6_L5,
  RESUME_C_W6_L6,
  RESUME_C_W6_L7,
  RESUME_C_W6_I0,
  RESUME_C_W6_I1,
  RESUME_C_W6_I2,
  RESUME_C_W6_I3,
  RESUME_C_W6_I4,
  RESUME_C_W6_I5,
  RESUME_C_W6_I6,
  RESUME_C_W6_I7,
  RESUME_C_TRAPS,
  RESUME_C_MAX
} ResumeCIndex;

RTEMS_STATIC_ASSERT( RESUME_C_W4_L0 == 0, resume_c_first );

static const RegisterCheckSource resume_c_sources[ RESUME_C_MAX ] = {
  { "w4 l0", REGISTER_CHECK_INITIAL, WORD },
  { "w4 l1", REGISTER_CHECK_INITIAL, WORD },
  { "w4 l2", REGISTER_CHECK_INITIAL, WORD },
  { "w4 l3", REGISTER_CHECK_INITIAL, WORD },
  { "w4 l4", REGISTER_CHECK_INITIAL, WORD },
  { "w4 l5", REGISTER_CHECK_INITIAL, WORD },
  { "w4 l6", REGISTER_CHECK_INITIAL, WORD },
  { "w4 l7", REGISTER_CHECK_INITIAL, WORD },
  { "w4 i0", REGISTER_CHECK_INITIAL, WORD },
  { "w4 i1", REGISTER_CHECK_INITIAL, WORD },
  { "w4 i2", REGISTER_CHECK_INITIAL, WORD },
  { "w4 i3", REGISTER_CHECK_INITIAL, WORD },
  { "w4 i4", REGISTER_CHECK_INITIAL, WORD },
  { "w4 i5", REGISTER_CHECK_INITIAL, WORD },
  { "w4 i6", REGISTER_CHECK_INITIAL, WORD },
  { "w4 i7", REGISTER_CHECK_INITIAL, WORD },
  { "w5 l0", REGISTER_CHECK_INITIAL, WORD },
  { "w5 l1", REGISTER_CHECK_INITIAL, WORD },
  { "w5 l2", REGISTER_CHECK_INITIAL, WORD },
  { "w5 l3", REGISTER_CHECK_INITIAL, WORD },
  { "w5 l4", REGISTER_CHECK_INITIAL, WORD },
  { "w5 l5", REGISTER_CHECK_INITIAL, WORD },
  { "w5 l6", REGISTER_CHECK_INITIAL, WORD },
  { "w5 l7", REGISTER_CHECK_INITIAL, WORD },
  { "w5 i0", REGISTER_CHECK_INITIAL, WORD },
  { "w5 i1", REGISTER_CHECK_INITIAL, WORD },
  { "w5 i2", REGISTER_CHECK_INITIAL, WORD },
  { "w5 i3", REGISTER_CHECK_INITIAL, WORD },
  { "w5 i4", REGISTER_CHECK_INITIAL, WORD },
  { "w5 i5", REGISTER_CHECK_INITIAL, WORD },
  { "w5 i6", REGISTER_CHECK_INITIAL, WORD },
  { "w5 i7", REGISTER_CHECK_INITIAL, WORD },
  { "w6 l0", REGISTER_CHECK_INITIAL, WORD },
  { "w6 l1", REGISTER_CHECK_INITIAL, WORD },
  { "w6 l2", REGISTER_CHECK_INITIAL, WORD },
  { "w6 l3", REGISTER_CHECK_INITIAL, WORD },
  { "w6 l4", REGISTER_CHECK_INITIAL, WORD },
  { "w6 l5", REGISTER_CHECK_INITIAL, WORD },
  { "w6 l6", REGISTER_CHECK_INITIAL, WORD },
  { "w6 l7", REGISTER_CHECK_INITIAL, WORD },
  { "w6 i0", REGISTER_CHECK_INITIAL, WORD },
  { "w6 i1", REGISTER_CHECK_INITIAL, WORD },
  { "w6 i2", REGISTER_CHECK_INITIAL, WORD },
  { "w6 i3", REGISTER_CHECK_INITIAL, WORD },
  { "w6 i4", REGISTER_CHECK_INITIAL, WORD },
  { "w6 i5", REGISTER_CHECK_INITIAL, WORD },
  { "w6 i6", REGISTER_CHECK_INITIAL, WORD },
  { "w6 i7", REGISTER_CHECK_INITIAL, WORD },
  { "traps", REGISTER_CHECK_EXPECTED, WORD },
};

static const RegisterCheckSlot resume_c_slots[ RESUME_C_MAX ] = {
  { "w4 l0", RESUME_C_W4_L0, WORD },
  { "w4 l1", RESUME_C_W4_L1, WORD },
  { "w4 l2", RESUME_C_W4_L2, WORD },
  { "w4 l3", RESUME_C_W4_L3, WORD },
  { "w4 l4", RESUME_C_W4_L4, WORD },
  { "w4 l5", RESUME_C_W4_L5, WORD },
  { "w4 l6", RESUME_C_W4_L6, WORD },
  { "w4 l7", RESUME_C_W4_L7, WORD },
  { "w4 i0", RESUME_C_W4_I0, WORD },
  { "w4 i1", RESUME_C_W4_I1, WORD },
  { "w4 i2", RESUME_C_W4_I2, WORD },
  { "w4 i3", RESUME_C_W4_I3, WORD },
  { "w4 i4", RESUME_C_W4_I4, WORD },
  { "w4 i5", RESUME_C_W4_I5, WORD },
  { "w4 i6", RESUME_C_W4_I6, WORD },
  { "w4 i7", RESUME_C_W4_I7, WORD },
  { "w5 l0", RESUME_C_W5_L0, WORD },
  { "w5 l1", RESUME_C_W5_L1, WORD },
  { "w5 l2", RESUME_C_W5_L2, WORD },
  { "w5 l3", RESUME_C_W5_L3, WORD },
  { "w5 l4", RESUME_C_W5_L4, WORD },
  { "w5 l5", RESUME_C_W5_L5, WORD },
  { "w5 l6", RESUME_C_W5_L6, WORD },
  { "w5 l7", RESUME_C_W5_L7, WORD },
  { "w5 i0", RESUME_C_W5_I0, WORD },
  { "w5 i1", RESUME_C_W5_I1, WORD },
  { "w5 i2", RESUME_C_W5_I2, WORD },
  { "w5 i3", RESUME_C_W5_I3, WORD },
  { "w5 i4", RESUME_C_W5_I4, WORD },
  { "w5 i5", RESUME_C_W5_I5, WORD },
  { "w5 i6", RESUME_C_W5_I6, WORD },
  { "w5 i7", RESUME_C_W5_I7, WORD },
  { "w6 l0", RESUME_C_W6_L0, WORD },
  { "w6 l1", RESUME_C_W6_L1, WORD },
  { "w6 l2", RESUME_C_W6_L2, WORD },
  { "w6 l3", RESUME_C_W6_L3, WORD },
  { "w6 l4", RESUME_C_W6_L4, WORD },
  { "w6 l5", RESUME_C_W6_L5, WORD },
  { "w6 l6", RESUME_C_W6_L6, WORD },
  { "w6 l7", RESUME_C_W6_L7, WORD },
  { "w6 i0", RESUME_C_W6_I0, WORD },
  { "w6 i1", RESUME_C_W6_I1, WORD },
  { "w6 i2", RESUME_C_W6_I2, WORD },
  { "w6 i3", RESUME_C_W6_I3, WORD },
  { "w6 i4", RESUME_C_W6_I4, WORD },
  { "w6 i5", RESUME_C_W6_I5, WORD },
  { "w6 i6", RESUME_C_W6_I6, WORD },
  { "w6 i7", RESUME_C_W6_I7, WORD },
  { "traps", RESUME_C_TRAPS, WORD },
};

static uint64_t resume_c_initial[ RESUME_C_MAX ];

static const ResumeGroup resume_c_group = {
  .begin = MEMBER_INDEX_W4_L0,
  .end = MEMBER_INDEX_W6_I7 + 1,
  .initial = resume_c_initial
};

static void ResumeCRun( RegisterCheck *self, void *arg )
{
  size_t i;

  (void) arg;
  ResumeRunTraps( &resume_c_group );

  for ( i = MEMBER_INDEX_W4_L0; i <= MEMBER_INDEX_W6_I7; ++i ) {
    RegisterCheckRecord(
      self,
      i - MEMBER_INDEX_W4_L0,
      GetMember( &resume_after, i ),
      GetPattern( i )
    );
  }

  RegisterCheckRecord( self, RESUME_C_TRAPS, (uint64_t) resume_trap, 2 );
}

static RegisterCheck resume_c_check = {
  .sources = resume_c_sources,
  .source_count = RESUME_C_MAX,
  .slots = resume_c_slots,
  .slot_count = RESUME_C_MAX,
  .initial = resume_c_initial,
  .run = ResumeCRun
};

static void ResumeCPrepare( void )
{
  size_t i;

  for ( i = MEMBER_INDEX_W4_L0; i <= MEMBER_INDEX_W6_I7; ++i ) {
    resume_c_initial[ i - MEMBER_INDEX_W4_L0 ] = GetPattern( i );
  }
}

/**
 * @brief Trap. Let the handler write a new value into every global and output
 *   register, the Y register, the floating-point registers, the FSR and the
 *   first register window of the frame. Resume, and trap again to capture what
 *   the resume restored. Run this once for each value with the value inverted,
 *   then once unchanged.
 */
static void ScoreCpuSparcValResume_Action_0( void )
{
  ResumeAPrepare();
  RegisterCheckRun( &resume_a_check );

  /*
   * Check that the resume restored the SPARC global register G1.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_G1 );

  /*
   * Check that the resume restored the SPARC global register G2.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_G2 );

  /*
   * Check that the resume restored the SPARC global register G3.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_G3 );

  /*
   * Check that the resume restored the SPARC global register G4.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_G4 );

  /*
   * Check that the resume restored the SPARC global register G5.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_G5 );

  /*
   * Check that the resume restored the SPARC global register G7.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_G7 );

  /*
   * Check that the resume restored the SPARC output register O0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_O0 );

  /*
   * Check that the resume restored the SPARC output register O1.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_O1 );

  /*
   * Check that the resume restored the SPARC output register O2.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_O2 );

  /*
   * Check that the resume restored the SPARC output register O3.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_O3 );

  /*
   * Check that the resume restored the SPARC output register O4.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_O4 );

  /*
   * Check that the resume restored the SPARC output register O5.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_O5 );

  /*
   * Check that the resume restored the SPARC output register O7.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_O7 );

  /*
   * Check that the resume restored the SPARC register Y.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_Y );

  /*
   * Check that the resume restored the SPARC FSR.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_FSR );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F0 and
   * F1.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F0_F1 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F2 and
   * F3.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F2_F3 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F4 and
   * F5.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F4_F5 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F6 and
   * F7.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F6_F7 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F8 and
   * F9.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F8_F9 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F10 and
   * F11.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F10_F11 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F12 and
   * F13.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F12_F13 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F14 and
   * F15.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F14_F15 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F16 and
   * F17.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F16_F17 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F18 and
   * F19.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F18_F19 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F20 and
   * F21.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F20_F21 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F22 and
   * F23.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F22_F23 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F24 and
   * F25.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F24_F25 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F26 and
   * F27.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F26_F27 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F28 and
   * F29.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F28_F29 );
  #endif

  /*
   * Check that the resume restored the SPARC floating-point registers F30 and
   * F31.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &resume_a_check, RESUME_A_F30_F31 );
  #endif

  /*
   * Check that the resume restored the SPARC local register L0 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_L0 );

  /*
   * Check that the resume restored the SPARC local register L1 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_L1 );

  /*
   * Check that the resume restored the SPARC local register L2 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_L2 );

  /*
   * Check that the resume restored the SPARC local register L3 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_L3 );

  /*
   * Check that the resume restored the SPARC local register L4 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_L4 );

  /*
   * Check that the resume restored the SPARC local register L5 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_L5 );

  /*
   * Check that the resume restored the SPARC local register L6 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_L6 );

  /*
   * Check that the resume restored the SPARC local register L7 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_L7 );

  /*
   * Check that the resume restored the SPARC input register I0 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_I0 );

  /*
   * Check that the resume restored the SPARC input register I1 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_I1 );

  /*
   * Check that the resume restored the SPARC input register I2 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_I2 );

  /*
   * Check that the resume restored the SPARC input register I3 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_I3 );

  /*
   * Check that the resume restored the SPARC input register I4 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_I4 );

  /*
   * Check that the resume restored the SPARC input register I5 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_I5 );

  /*
   * Check that the resume restored the SPARC input register I6 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_I6 );

  /*
   * Check that the resume restored the SPARC input register I7 of window 0.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_W0_I7 );

  /*
   * Check that the resume restored the stack pointer, which is O6.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_SP );

  /*
   * Check that the resume restored the PSR.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_PSR );

  /*
   * Check that the resume restored the WIM.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_WIM );

  /*
   * Check that the second frame lies where the first one lay.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_FRAME );

  /*
   * Check that the frame of the first trap reports the address of the trapping
   * instruction.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_TRAP1_PC );

  /*
   * Check that both traps happened.
   */
  RegisterCheckVerify( &resume_a_check, RESUME_A_TRAPS );

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  RegisterCheckReport( &resume_a_check );
}

/**
 * @brief Trap. Let the handler write a new value into the register windows one
 *   to three of the frame. Resume, and trap again to capture what the resume
 *   restored. Run this once for each value with the value inverted, then once
 *   unchanged.
 */
static void ScoreCpuSparcValResume_Action_1( void )
{
  ResumeBPrepare();
  RegisterCheckRun( &resume_b_check );

  /*
   * Check that the resume restored the SPARC local register L0 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_L0 );

  /*
   * Check that the resume restored the SPARC local register L1 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_L1 );

  /*
   * Check that the resume restored the SPARC local register L2 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_L2 );

  /*
   * Check that the resume restored the SPARC local register L3 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_L3 );

  /*
   * Check that the resume restored the SPARC local register L4 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_L4 );

  /*
   * Check that the resume restored the SPARC local register L5 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_L5 );

  /*
   * Check that the resume restored the SPARC local register L6 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_L6 );

  /*
   * Check that the resume restored the SPARC local register L7 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_L7 );

  /*
   * Check that the resume restored the SPARC input register I0 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_I0 );

  /*
   * Check that the resume restored the SPARC input register I1 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_I1 );

  /*
   * Check that the resume restored the SPARC input register I2 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_I2 );

  /*
   * Check that the resume restored the SPARC input register I3 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_I3 );

  /*
   * Check that the resume restored the SPARC input register I4 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_I4 );

  /*
   * Check that the resume restored the SPARC input register I5 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_I5 );

  /*
   * Check that the resume restored the SPARC input register I6 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_I6 );

  /*
   * Check that the resume restored the SPARC input register I7 of window 1.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W1_I7 );

  /*
   * Check that the resume restored the SPARC local register L0 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_L0 );

  /*
   * Check that the resume restored the SPARC local register L1 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_L1 );

  /*
   * Check that the resume restored the SPARC local register L2 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_L2 );

  /*
   * Check that the resume restored the SPARC local register L3 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_L3 );

  /*
   * Check that the resume restored the SPARC local register L4 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_L4 );

  /*
   * Check that the resume restored the SPARC local register L5 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_L5 );

  /*
   * Check that the resume restored the SPARC local register L6 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_L6 );

  /*
   * Check that the resume restored the SPARC local register L7 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_L7 );

  /*
   * Check that the resume restored the SPARC input register I0 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_I0 );

  /*
   * Check that the resume restored the SPARC input register I1 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_I1 );

  /*
   * Check that the resume restored the SPARC input register I2 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_I2 );

  /*
   * Check that the resume restored the SPARC input register I3 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_I3 );

  /*
   * Check that the resume restored the SPARC input register I4 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_I4 );

  /*
   * Check that the resume restored the SPARC input register I5 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_I5 );

  /*
   * Check that the resume restored the SPARC input register I6 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_I6 );

  /*
   * Check that the resume restored the SPARC input register I7 of window 2.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W2_I7 );

  /*
   * Check that the resume restored the SPARC local register L0 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_L0 );

  /*
   * Check that the resume restored the SPARC local register L1 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_L1 );

  /*
   * Check that the resume restored the SPARC local register L2 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_L2 );

  /*
   * Check that the resume restored the SPARC local register L3 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_L3 );

  /*
   * Check that the resume restored the SPARC local register L4 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_L4 );

  /*
   * Check that the resume restored the SPARC local register L5 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_L5 );

  /*
   * Check that the resume restored the SPARC local register L6 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_L6 );

  /*
   * Check that the resume restored the SPARC local register L7 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_L7 );

  /*
   * Check that the resume restored the SPARC input register I0 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_I0 );

  /*
   * Check that the resume restored the SPARC input register I1 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_I1 );

  /*
   * Check that the resume restored the SPARC input register I2 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_I2 );

  /*
   * Check that the resume restored the SPARC input register I3 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_I3 );

  /*
   * Check that the resume restored the SPARC input register I4 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_I4 );

  /*
   * Check that the resume restored the SPARC input register I5 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_I5 );

  /*
   * Check that the resume restored the SPARC input register I6 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_I6 );

  /*
   * Check that the resume restored the SPARC input register I7 of window 3.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_W3_I7 );

  /*
   * Check that both traps happened.
   */
  RegisterCheckVerify( &resume_b_check, RESUME_B_TRAPS );

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  RegisterCheckReport( &resume_b_check );
}

/**
 * @brief Trap. Let the handler write a new value into the register windows
 *   four to six of the frame. Resume, and trap again to capture what the
 *   resume restored. Run this once for each value with the value inverted,
 *   then once unchanged.
 */
static void ScoreCpuSparcValResume_Action_2( void )
{
  ResumeCPrepare();
  RegisterCheckRun( &resume_c_check );

  /*
   * Check that the resume restored the SPARC local register L0 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_L0 );

  /*
   * Check that the resume restored the SPARC local register L1 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_L1 );

  /*
   * Check that the resume restored the SPARC local register L2 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_L2 );

  /*
   * Check that the resume restored the SPARC local register L3 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_L3 );

  /*
   * Check that the resume restored the SPARC local register L4 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_L4 );

  /*
   * Check that the resume restored the SPARC local register L5 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_L5 );

  /*
   * Check that the resume restored the SPARC local register L6 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_L6 );

  /*
   * Check that the resume restored the SPARC local register L7 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_L7 );

  /*
   * Check that the resume restored the SPARC input register I0 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_I0 );

  /*
   * Check that the resume restored the SPARC input register I1 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_I1 );

  /*
   * Check that the resume restored the SPARC input register I2 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_I2 );

  /*
   * Check that the resume restored the SPARC input register I3 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_I3 );

  /*
   * Check that the resume restored the SPARC input register I4 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_I4 );

  /*
   * Check that the resume restored the SPARC input register I5 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_I5 );

  /*
   * Check that the resume restored the SPARC input register I6 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_I6 );

  /*
   * Check that the resume restored the SPARC input register I7 of window 4.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W4_I7 );

  /*
   * Check that the resume restored the SPARC local register L0 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_L0 );

  /*
   * Check that the resume restored the SPARC local register L1 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_L1 );

  /*
   * Check that the resume restored the SPARC local register L2 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_L2 );

  /*
   * Check that the resume restored the SPARC local register L3 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_L3 );

  /*
   * Check that the resume restored the SPARC local register L4 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_L4 );

  /*
   * Check that the resume restored the SPARC local register L5 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_L5 );

  /*
   * Check that the resume restored the SPARC local register L6 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_L6 );

  /*
   * Check that the resume restored the SPARC local register L7 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_L7 );

  /*
   * Check that the resume restored the SPARC input register I0 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_I0 );

  /*
   * Check that the resume restored the SPARC input register I1 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_I1 );

  /*
   * Check that the resume restored the SPARC input register I2 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_I2 );

  /*
   * Check that the resume restored the SPARC input register I3 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_I3 );

  /*
   * Check that the resume restored the SPARC input register I4 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_I4 );

  /*
   * Check that the resume restored the SPARC input register I5 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_I5 );

  /*
   * Check that the resume restored the SPARC input register I6 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_I6 );

  /*
   * Check that the resume restored the SPARC input register I7 of window 5.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W5_I7 );

  /*
   * Check that the resume restored the SPARC local register L0 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_L0 );

  /*
   * Check that the resume restored the SPARC local register L1 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_L1 );

  /*
   * Check that the resume restored the SPARC local register L2 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_L2 );

  /*
   * Check that the resume restored the SPARC local register L3 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_L3 );

  /*
   * Check that the resume restored the SPARC local register L4 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_L4 );

  /*
   * Check that the resume restored the SPARC local register L5 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_L5 );

  /*
   * Check that the resume restored the SPARC local register L6 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_L6 );

  /*
   * Check that the resume restored the SPARC local register L7 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_L7 );

  /*
   * Check that the resume restored the SPARC input register I0 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_I0 );

  /*
   * Check that the resume restored the SPARC input register I1 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_I1 );

  /*
   * Check that the resume restored the SPARC input register I2 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_I2 );

  /*
   * Check that the resume restored the SPARC input register I3 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_I3 );

  /*
   * Check that the resume restored the SPARC input register I4 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_I4 );

  /*
   * Check that the resume restored the SPARC input register I5 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_I5 );

  /*
   * Check that the resume restored the SPARC input register I6 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_I6 );

  /*
   * Check that the resume restored the SPARC input register I7 of window 6.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_W6_I7 );

  /*
   * Check that both traps happened.
   */
  RegisterCheckVerify( &resume_c_check, RESUME_C_TRAPS );

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  RegisterCheckReport( &resume_c_check );
}

/**
 * @fn void T_case_body_ScoreCpuSparcValResume( void )
 */
T_TEST_CASE( ScoreCpuSparcValResume )
{
  ScoreCpuSparcValResume_Action_0();
  ScoreCpuSparcValResume_Action_1();
  ScoreCpuSparcValResume_Action_2();
}

/** @} */
