/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuSparcValInterruptL7
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
#include <rtems/score/percpu.h>
#include <rtems/score/sparc.h>

#include "tx-register-check.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuSparcValInterruptL7 spec:/score/cpu/sparc/val/interrupt-l7
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Checks that interrupt processing preserves the registers of the
 *   interrupted context.
 *
 * The action needs a BSP which supplies the TM27 support. A BSP without it
 * raises no interrupt. The wrapper works in a window of its own, so the
 * register window preserves the local and the input registers. The local
 * register L6 holds the PSR which unmasks the interrupt, so the test checks no
 * value of it.
 *
 * This test case performs the following actions:
 *
 * - Load a distinct pattern into every register the interrupt has to preserve.
 *   Raise the interrupt. Trap, so the frame captures every register. Run this
 *   once for each register with the pattern of that register inverted, then
 *   once unchanged.
 *
 *   - Check that the interrupt preserved the SPARC global register G1.
 *
 *   - Check that the interrupt preserved the SPARC global register G2.
 *
 *   - Check that the interrupt preserved the SPARC global register G3.
 *
 *   - Check that the interrupt preserved the SPARC global register G4.
 *
 *   - Check that the interrupt preserved the SPARC global register G5.
 *
 *   - Check that the interrupt preserved the SPARC output register O0.
 *
 *   - Check that the interrupt preserved the SPARC output register O1.
 *
 *   - Check that the interrupt preserved the SPARC output register O2.
 *
 *   - Check that the interrupt preserved the SPARC output register O3.
 *
 *   - Check that the interrupt preserved the SPARC output register O4.
 *
 *   - Check that the interrupt preserved the SPARC output register O5.
 *
 *   - Check that the interrupt preserved the SPARC output register O7.
 *
 *   - Check that the interrupt preserved the SPARC local register L0.
 *
 *   - Check that the interrupt preserved the SPARC local register L1.
 *
 *   - Check that the interrupt preserved the SPARC local register L2.
 *
 *   - Check that the interrupt preserved the SPARC local register L3.
 *
 *   - Check that the interrupt preserved the SPARC local register L4.
 *
 *   - Check that the interrupt preserved the SPARC local register L5.
 *
 *   - Check that the interrupt preserved the SPARC local register L7.
 *
 *   - Check that the interrupt preserved the SPARC input register I0.
 *
 *   - Check that the interrupt preserved the SPARC input register I1.
 *
 *   - Check that the interrupt preserved the SPARC input register I2.
 *
 *   - Check that the interrupt preserved the SPARC input register I3.
 *
 *   - Check that the interrupt preserved the SPARC input register I4.
 *
 *   - Check that the interrupt preserved the SPARC input register I5.
 *
 *   - Check that the interrupt preserved the SPARC input register I7.
 *
 *   - Check that the interrupt preserved the SPARC register Y.
 *
 *   - Check that the interrupt preserved the integer condition codes of the
 *     PSR.
 *
 *   - Check that the interrupt preserved the SPARC FSR.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers F0
 *     and F1.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers F2
 *     and F3.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers F4
 *     and F5.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers F6
 *     and F7.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers F8
 *     and F9.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F10 and F11.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F12 and F13.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F14 and F15.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F16 and F17.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F18 and F19.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F20 and F21.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F22 and F23.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F24 and F25.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F26 and F27.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F28 and F29.
 *
 *   - Check that the interrupt preserved the SPARC floating-point registers
 *     F30 and F31.
 *
 *   - Check that the interrupt left the per-CPU control G6 unchanged.
 *
 *   - Check that the interrupt preserved the thread pointer G7.
 *
 *   - Check that the interrupt preserved the stack pointer, which is O6.
 *
 *   - Check that the interrupt preserved the frame pointer, which is I6.
 *
 *   - Check that the interrupt preserved the processor interrupt level.
 *
 *   - Check that the interrupted context continued at the instruction after
 *     the interrupt and reached the trap.
 *
 *   - Check that interrupt processing cleared PSR[EF] before it called the
 *     handler.
 *
 *   - Check that the interrupt happened once before the trap.
 *
 *   - Check that the handler ran on the processor of the interrupted context.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * @{
 */

#define _RTEMS_TMTEST27
#include <tm27.h>

#define WORD UINT64_C( 0xffffffff )

#define PSR_ICC_MASK 0x00f00000

#define PSR_ICC_BITS 0x00500000

/* RD, NS, fcc, aexc and cexc of the FSR */
#define FSR_TEST_MASK 0xc0400fff

#define FSR_TEST_BITS 0x404008a5

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
  SOURCE_L7,
  SOURCE_I0,
  SOURCE_I1,
  SOURCE_I2,
  SOURCE_I3,
  SOURCE_I4,
  SOURCE_I5,
  SOURCE_I7,
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
  SOURCE_G6,
  SOURCE_G7,
  SOURCE_SP,
  SOURCE_FP,
  SOURCE_PIL,
  SOURCE_PC,
  SOURCE_EF,
  SOURCE_COUNT_AT_TRAP,
  SOURCE_PROCESSOR,
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
  SLOT_L7,
  SLOT_I0,
  SLOT_I1,
  SLOT_I2,
  SLOT_I3,
  SLOT_I4,
  SLOT_I5,
  SLOT_I7,
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
  SLOT_G6,
  SLOT_G7,
  SLOT_SP,
  SLOT_FP,
  SLOT_PIL,
  SLOT_PC,
  SLOT_EF,
  SLOT_COUNT_AT_TRAP,
  SLOT_PROCESSOR,
  SLOT_MAX
} Slot;

RTEMS_STATIC_ASSERT( SOURCE_G1 == 0, source_g1 );
RTEMS_STATIC_ASSERT( SOURCE_O0 == 5, source_o0 );
RTEMS_STATIC_ASSERT( SOURCE_L0 == 12, source_l0 );
RTEMS_STATIC_ASSERT( SOURCE_I0 == 19, source_i0 );
RTEMS_STATIC_ASSERT( SOURCE_Y == 26, source_y );
RTEMS_STATIC_ASSERT( SOURCE_ICC == 27, source_icc );
#if SPARC_HAS_FPU == 1
RTEMS_STATIC_ASSERT( SOURCE_FSR == 28, source_fsr );
RTEMS_STATIC_ASSERT( SOURCE_F0_F1 == 29, source_f0_f1 );
#endif

static const RegisterCheckSource interrupt_sources[ SOURCE_MAX ] = {
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
  { "l7", REGISTER_CHECK_INITIAL, WORD },
  { "i0", REGISTER_CHECK_INITIAL, WORD },
  { "i1", REGISTER_CHECK_INITIAL, WORD },
  { "i2", REGISTER_CHECK_INITIAL, WORD },
  { "i3", REGISTER_CHECK_INITIAL, WORD },
  { "i4", REGISTER_CHECK_INITIAL, WORD },
  { "i5", REGISTER_CHECK_INITIAL, WORD },
  { "i7", REGISTER_CHECK_INITIAL, WORD },
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
  { "g6", REGISTER_CHECK_EXPECTED, WORD },
  { "g7", REGISTER_CHECK_EXPECTED, WORD },
  { "sp", REGISTER_CHECK_EXPECTED, WORD },
  { "fp", REGISTER_CHECK_EXPECTED, WORD },
  { "pil", REGISTER_CHECK_EXPECTED, SPARC_PSR_PIL_MASK },
  { "pc", REGISTER_CHECK_EXPECTED, WORD },
  { "ef", REGISTER_CHECK_EXPECTED, WORD },
  { "count at trap", REGISTER_CHECK_EXPECTED, WORD },
  { "processor", REGISTER_CHECK_EXPECTED, WORD },
};

static const RegisterCheckSlot interrupt_slots[ SLOT_MAX ] = {
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
  { "l7", SOURCE_L7, WORD },
  { "i0", SOURCE_I0, WORD },
  { "i1", SOURCE_I1, WORD },
  { "i2", SOURCE_I2, WORD },
  { "i3", SOURCE_I3, WORD },
  { "i4", SOURCE_I4, WORD },
  { "i5", SOURCE_I5, WORD },
  { "i7", SOURCE_I7, WORD },
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
  { "g6", SOURCE_G6, WORD },
  { "g7", SOURCE_G7, WORD },
  { "sp", SOURCE_SP, WORD },
  { "fp", SOURCE_FP, WORD },
  { "pil", SOURCE_PIL, SPARC_PSR_PIL_MASK },
  { "pc", SOURCE_PC, WORD },
  { "ef", SOURCE_EF, WORD },
  { "count at trap", SOURCE_COUNT_AT_TRAP, WORD },
  { "processor", SOURCE_PROCESSOR, WORD },
};

static const uint64_t interrupt_patterns[ SOURCE_MAX ] = {
  0x21222324,
  0x25262728,
  0x292a2b2c,
  0x2d2e2f30,
  0x31323334,
  0x35363738,
  0x393a3b3c,
  0x3d3e3f40,
  0x41424344,
  0x45464748,
  0x494a4b4c,
  0x4d4e4f50,
  0x51525354,
  0x55565758,
  0x595a5b5c,
  0x5d5e5f60,
  0x61626364,
  0x65666768,
  0x696a6b6c,
  0x6d6e6f70,
  0x71727374,
  0x75767778,
  0x797a7b7c,
  0x7d7e7f80,
  0x81828384,
  0x85868788,
  0x898a8b8c,
  PSR_ICC_BITS,
#if SPARC_HAS_FPU == 1
  FSR_TEST_BITS,
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
  0xc0c1c2c3c4c5c6c7,
  0xc8c9cacbcccdcecf,
  0xd0d1d2d3d4d5d6d7,
  0xd8d9dadbdcdddedf,
  0xe0e1e2e3e4e5e6e7,
#endif
};

/*
 * The block enters a new register window.  It loads each value from the
 * table of initial values.  A value of one word sits at the byte offset eight
 * times its source plus four, since SPARC is big-endian.  A pair of
 * floating-point registers sits at the byte offset eight times its source.
 * It loads g1 last, because g1 holds the address of the table.  The local
 * register l6 holds the PSR which unmasks the interrupt.
 */
#if SPARC_HAS_FPU == 1
#define ISR_LOAD_FPU          \
  "ldd [%%g1 + 232], %%f0\n"  \
  "ldd [%%g1 + 240], %%f2\n"  \
  "ldd [%%g1 + 248], %%f4\n"  \
  "ldd [%%g1 + 256], %%f6\n"  \
  "ldd [%%g1 + 264], %%f8\n"  \
  "ldd [%%g1 + 272], %%f10\n" \
  "ldd [%%g1 + 280], %%f12\n" \
  "ldd [%%g1 + 288], %%f14\n" \
  "ldd [%%g1 + 296], %%f16\n" \
  "ldd [%%g1 + 304], %%f18\n" \
  "ldd [%%g1 + 312], %%f20\n" \
  "ldd [%%g1 + 320], %%f22\n" \
  "ldd [%%g1 + 328], %%f24\n" \
  "ldd [%%g1 + 336], %%f26\n" \
  "ldd [%%g1 + 344], %%f28\n" \
  "ldd [%%g1 + 352], %%f30\n" \
  "ld [%%g1 + 228], %%fsr\n"

#define ISR_CLOBBER_FPU                                                       \
  , "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", \
    "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21",     \
    "f22", "f23", "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31"
#else
#define ISR_LOAD_FPU

#define ISR_CLOBBER_FPU
#endif

#define ISR_LOAD                                                            \
  "save %%sp, -96, %%sp\n"                                                  \
  "set interrupt_l7_expected, %%g1\n"                                       \
  "st %%sp, [%%g1 + 0]\n"                                                   \
  "st %%fp, [%%g1 + 4]\n"                                                   \
  "set interrupt_l7_initial, %%g1\n" ISR_LOAD_FPU "ld [%%g1 + 212], %%l6\n" \
  "wr %%l6, 0, %%y\n"                                                       \
  "rd %%psr, %%l6\n"                                                        \
  "set " RTEMS_XSTRING( PSR_ICC_MASK ) ", %%g2\n"                           \
                                       "andn %%l6, %%g2, %%l6\n"            \
                                       "ld [%%g1 + 220], %%g3\n"            \
                                       "and %%g3, %%g2, %%g3\n"             \
                                       "or %%l6, %%g3, %%l6\n"              \
                                       "andn %%l6, 0xf00, %%l6\n"           \
                                       "ld [%%g1 + 100], %%l0\n"            \
                                       "ld [%%g1 + 108], %%l1\n"            \
                                       "ld [%%g1 + 116], %%l2\n"            \
                                       "ld [%%g1 + 124], %%l3\n"            \
                                       "ld [%%g1 + 132], %%l4\n"            \
                                       "ld [%%g1 + 140], %%l5\n"            \
                                       "ld [%%g1 + 148], %%l7\n"            \
                                       "ld [%%g1 + 156], %%i0\n"            \
                                       "ld [%%g1 + 164], %%i1\n"            \
                                       "ld [%%g1 + 172], %%i2\n"            \
                                       "ld [%%g1 + 180], %%i3\n"            \
                                       "ld [%%g1 + 188], %%i4\n"            \
                                       "ld [%%g1 + 196], %%i5\n"            \
                                       "ld [%%g1 + 204], %%i7\n"            \
                                       "ld [%%g1 + 44], %%o0\n"             \
                                       "ld [%%g1 + 52], %%o1\n"             \
                                       "ld [%%g1 + 60], %%o2\n"             \
                                       "ld [%%g1 + 68], %%o3\n"             \
                                       "ld [%%g1 + 76], %%o4\n"             \
                                       "ld [%%g1 + 84], %%o5\n"             \
                                       "ld [%%g1 + 92], %%o7\n"             \
                                       "ld [%%g1 + 12], %%g2\n"             \
                                       "ld [%%g1 + 20], %%g3\n"             \
                                       "ld [%%g1 + 28], %%g4\n"             \
                                       "ld [%%g1 + 36], %%g5\n"             \
                                       "ld [%%g1 + 4], %%g1\n"

extern const char interrupt_l7_trap_label[];

extern const char interrupt_l7_done_label[];

uint64_t interrupt_l7_initial[ SOURCE_MAX ];

/* The stack pointer and the frame pointer of the new window */
uint32_t interrupt_l7_expected[ 2 ];

static CPU_Exception_frame interrupt_frame;

static volatile int interrupt_count;

static int interrupt_count_at_trap;

static uint32_t interrupt_handler_psr;

static uint32_t interrupt_handler_processor;

static uint32_t interrupt_g7;

static const Per_CPU_Control *interrupt_cpu;

static uint32_t interrupt_trap_processor;

static CallWithinISRRequest interrupt_request;

static void InterruptHandler( void *arg )
{
  uint32_t psr;

  (void) arg;
  __asm__ volatile( "rd %%psr, %0"
                    : "=r"( psr ) );
  interrupt_handler_psr = psr;
  interrupt_handler_processor = rtems_scheduler_get_processor();
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
  /* The handler runs on the processor which takes the trap */
  interrupt_cpu = _Per_CPU_Get_snapshot();
  interrupt_trap_processor = _Per_CPU_Get_index( interrupt_cpu );
  interrupt_frame = *frame;
  interrupt_count_at_trap = interrupt_count;
  SetFatalHandler( NULL, NULL );
  frame->pc = (uint32_t) interrupt_l7_done_label;
  frame->npc = frame->pc + 4;
  _CPU_Exception_resume( frame );
}

static void InterruptRun( RegisterCheck *self, void *arg )
{
  rtems_interrupt_level level;
  const uint64_t       *patterns;
#if SPARC_HAS_FPU == 1
  uint32_t fsr;
#endif

  (void) arg;
  memset( &interrupt_frame, 0xff, sizeof( interrupt_frame ) );
  interrupt_count = 0;
  interrupt_count_at_trap = 0;
  interrupt_handler_psr = 0xffffffff;
  interrupt_handler_processor = 0xffffffff;
  interrupt_cpu = NULL;
  interrupt_trap_processor = 0xfffffffe;
  __asm__ volatile( "mov %%g7, %0"
                    : "=r"( interrupt_g7 ) );
#if SPARC_HAS_FPU == 1
  __asm__ volatile( "st %%fsr, %0"
                    : "=m"( fsr ) );
#endif
  SetFatalHandler( InterruptFatal, NULL );

  /*
   * The interrupt is raised while it is masked, so no call happens after
   * the block loads the registers.  The write to the PSR unmasks it and
   * takes effect within three instructions, so the interrupt arrives before
   * the trap.
   */
  rtems_interrupt_local_disable( level );
  CallWithinISRSubmit( &interrupt_request );
  __asm__ volatile( ISR_LOAD "wr %%l6, 0, %%psr\n"
                             "nop\n"
                             "nop\n"
                             "nop\n"
                             ".globl interrupt_l7_trap_label\n"
                             "interrupt_l7_trap_label:\n"
                             "unimp 0\n"
                             ".globl interrupt_l7_done_label\n"
                             "interrupt_l7_done_label:\n"
                             "restore\n"
                    :
                    :
                    : "memory",
                      "cc",
                      "g1",
                      "g2",
                      "g3",
                      "g4",
                      "g5",
                      "o0",
                      "o1",
                      "o2",
                      "o3",
                      "o4",
                      "o5",
                      "o7" ISR_CLOBBER_FPU );
  rtems_interrupt_local_enable( level );
#if SPARC_HAS_FPU == 1
  __asm__ volatile( "ld %0, %%fsr\nnop\nnop\nnop"
                    :
                    : "m"( fsr ) );
#endif

  patterns = interrupt_patterns;
  RegisterCheckRecord(
    self,
    SLOT_G1,
    interrupt_frame.global[ 1 ],
    patterns[ SOURCE_G1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_G2,
    interrupt_frame.global[ 2 ],
    patterns[ SOURCE_G2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_G3,
    interrupt_frame.global[ 3 ],
    patterns[ SOURCE_G3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_G4,
    interrupt_frame.global[ 4 ],
    patterns[ SOURCE_G4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_G5,
    interrupt_frame.global[ 5 ],
    patterns[ SOURCE_G5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O0,
    interrupt_frame.output[ 0 ],
    patterns[ SOURCE_O0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O1,
    interrupt_frame.output[ 1 ],
    patterns[ SOURCE_O1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O2,
    interrupt_frame.output[ 2 ],
    patterns[ SOURCE_O2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O3,
    interrupt_frame.output[ 3 ],
    patterns[ SOURCE_O3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O4,
    interrupt_frame.output[ 4 ],
    patterns[ SOURCE_O4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O5,
    interrupt_frame.output[ 5 ],
    patterns[ SOURCE_O5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_O7,
    interrupt_frame.output[ 7 ],
    patterns[ SOURCE_O7 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L0,
    interrupt_frame.windows[ 0 ].local[ 0 ],
    patterns[ SOURCE_L0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L1,
    interrupt_frame.windows[ 0 ].local[ 1 ],
    patterns[ SOURCE_L1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L2,
    interrupt_frame.windows[ 0 ].local[ 2 ],
    patterns[ SOURCE_L2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L3,
    interrupt_frame.windows[ 0 ].local[ 3 ],
    patterns[ SOURCE_L3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L4,
    interrupt_frame.windows[ 0 ].local[ 4 ],
    patterns[ SOURCE_L4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L5,
    interrupt_frame.windows[ 0 ].local[ 5 ],
    patterns[ SOURCE_L5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_L7,
    interrupt_frame.windows[ 0 ].local[ 7 ],
    patterns[ SOURCE_L7 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I0,
    interrupt_frame.windows[ 0 ].input[ 0 ],
    patterns[ SOURCE_I0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I1,
    interrupt_frame.windows[ 0 ].input[ 1 ],
    patterns[ SOURCE_I1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I2,
    interrupt_frame.windows[ 0 ].input[ 2 ],
    patterns[ SOURCE_I2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I3,
    interrupt_frame.windows[ 0 ].input[ 3 ],
    patterns[ SOURCE_I3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I4,
    interrupt_frame.windows[ 0 ].input[ 4 ],
    patterns[ SOURCE_I4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I5,
    interrupt_frame.windows[ 0 ].input[ 5 ],
    patterns[ SOURCE_I5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_I7,
    interrupt_frame.windows[ 0 ].input[ 7 ],
    patterns[ SOURCE_I7 ]
  );
  RegisterCheckRecord( self, SLOT_Y, interrupt_frame.y, patterns[ SOURCE_Y ] );
  RegisterCheckRecord(
    self,
    SLOT_ICC,
    interrupt_frame.psr,
    patterns[ SOURCE_ICC ]
  );
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_FSR,
    interrupt_frame.fsr,
    patterns[ SOURCE_FSR ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F0_F1,
    interrupt_frame.fp[ 0 ],
    patterns[ SOURCE_F0_F1 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F2_F3,
    interrupt_frame.fp[ 1 ],
    patterns[ SOURCE_F2_F3 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F4_F5,
    interrupt_frame.fp[ 2 ],
    patterns[ SOURCE_F4_F5 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F6_F7,
    interrupt_frame.fp[ 3 ],
    patterns[ SOURCE_F6_F7 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F8_F9,
    interrupt_frame.fp[ 4 ],
    patterns[ SOURCE_F8_F9 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F10_F11,
    interrupt_frame.fp[ 5 ],
    patterns[ SOURCE_F10_F11 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F12_F13,
    interrupt_frame.fp[ 6 ],
    patterns[ SOURCE_F12_F13 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F14_F15,
    interrupt_frame.fp[ 7 ],
    patterns[ SOURCE_F14_F15 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F16_F17,
    interrupt_frame.fp[ 8 ],
    patterns[ SOURCE_F16_F17 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F18_F19,
    interrupt_frame.fp[ 9 ],
    patterns[ SOURCE_F18_F19 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F20_F21,
    interrupt_frame.fp[ 10 ],
    patterns[ SOURCE_F20_F21 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F22_F23,
    interrupt_frame.fp[ 11 ],
    patterns[ SOURCE_F22_F23 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F24_F25,
    interrupt_frame.fp[ 12 ],
    patterns[ SOURCE_F24_F25 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F26_F27,
    interrupt_frame.fp[ 13 ],
    patterns[ SOURCE_F26_F27 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F28_F29,
    interrupt_frame.fp[ 14 ],
    patterns[ SOURCE_F28_F29 ]
  );
#endif
#if SPARC_HAS_FPU == 1
  RegisterCheckRecord(
    self,
    SLOT_F30_F31,
    interrupt_frame.fp[ 15 ],
    patterns[ SOURCE_F30_F31 ]
  );
#endif
  RegisterCheckRecord(
    self,
    SLOT_G6,
    interrupt_frame.global[ 6 ],
    (uintptr_t) interrupt_cpu
  );
  RegisterCheckRecord(
    self,
    SLOT_G7,
    interrupt_frame.global[ 7 ],
    interrupt_g7
  );
  RegisterCheckRecord(
    self,
    SLOT_SP,
    interrupt_frame.output[ 6 ],
    interrupt_l7_expected[ 0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FP,
    interrupt_frame.windows[ 0 ].input[ 6 ],
    interrupt_l7_expected[ 1 ]
  );
  RegisterCheckRecord( self, SLOT_PIL, interrupt_frame.psr, 0 );
  RegisterCheckRecord(
    self,
    SLOT_PC,
    interrupt_frame.pc,
    (uintptr_t) interrupt_l7_trap_label
  );
  RegisterCheckRecord(
    self,
    SLOT_EF,
    interrupt_handler_psr & SPARC_PSR_EF_MASK,
    0
  );
  RegisterCheckRecord(
    self,
    SLOT_COUNT_AT_TRAP,
    (uint64_t) interrupt_count_at_trap,
    1
  );
  RegisterCheckRecord(
    self,
    SLOT_PROCESSOR,
    interrupt_handler_processor,
    interrupt_trap_processor
  );
}

static RegisterCheck interrupt_check = {
  .sources = interrupt_sources,
  .source_count = SOURCE_MAX,
  .slots = interrupt_slots,
  .slot_count = SLOT_MAX,
  .initial = interrupt_l7_initial,
  .run = InterruptRun
};

static void CheckSlot( Slot slot )
{
  RegisterCheckVerify( &interrupt_check, slot );
}

/**
 * @brief Load a distinct pattern into every register the interrupt has to
 *   preserve. Raise the interrupt. Trap, so the frame captures every register.
 *   Run this once for each register with the pattern of that register
 *   inverted, then once unchanged.
 */
static void ScoreCpuSparcValInterruptL7_Action_0( void )
{
  memcpy(
    interrupt_l7_initial,
    interrupt_patterns,
    sizeof( interrupt_l7_initial )
  );
  interrupt_request.handler = InterruptHandler;
  interrupt_request.arg = NULL;

  #ifdef TM27_INTERRUPT_VECTOR_DEFAULT
  RegisterCheckRun( &interrupt_check );
  #endif

  /*
   * Check that the interrupt preserved the SPARC global register G1.
   */
  CheckSlot( SLOT_G1 );

  /*
   * Check that the interrupt preserved the SPARC global register G2.
   */
  CheckSlot( SLOT_G2 );

  /*
   * Check that the interrupt preserved the SPARC global register G3.
   */
  CheckSlot( SLOT_G3 );

  /*
   * Check that the interrupt preserved the SPARC global register G4.
   */
  CheckSlot( SLOT_G4 );

  /*
   * Check that the interrupt preserved the SPARC global register G5.
   */
  CheckSlot( SLOT_G5 );

  /*
   * Check that the interrupt preserved the SPARC output register O0.
   */
  CheckSlot( SLOT_O0 );

  /*
   * Check that the interrupt preserved the SPARC output register O1.
   */
  CheckSlot( SLOT_O1 );

  /*
   * Check that the interrupt preserved the SPARC output register O2.
   */
  CheckSlot( SLOT_O2 );

  /*
   * Check that the interrupt preserved the SPARC output register O3.
   */
  CheckSlot( SLOT_O3 );

  /*
   * Check that the interrupt preserved the SPARC output register O4.
   */
  CheckSlot( SLOT_O4 );

  /*
   * Check that the interrupt preserved the SPARC output register O5.
   */
  CheckSlot( SLOT_O5 );

  /*
   * Check that the interrupt preserved the SPARC output register O7.
   */
  CheckSlot( SLOT_O7 );

  /*
   * Check that the interrupt preserved the SPARC local register L0.
   */
  CheckSlot( SLOT_L0 );

  /*
   * Check that the interrupt preserved the SPARC local register L1.
   */
  CheckSlot( SLOT_L1 );

  /*
   * Check that the interrupt preserved the SPARC local register L2.
   */
  CheckSlot( SLOT_L2 );

  /*
   * Check that the interrupt preserved the SPARC local register L3.
   */
  CheckSlot( SLOT_L3 );

  /*
   * Check that the interrupt preserved the SPARC local register L4.
   */
  CheckSlot( SLOT_L4 );

  /*
   * Check that the interrupt preserved the SPARC local register L5.
   */
  CheckSlot( SLOT_L5 );

  /*
   * Check that the interrupt preserved the SPARC local register L7.
   */
  CheckSlot( SLOT_L7 );

  /*
   * Check that the interrupt preserved the SPARC input register I0.
   */
  CheckSlot( SLOT_I0 );

  /*
   * Check that the interrupt preserved the SPARC input register I1.
   */
  CheckSlot( SLOT_I1 );

  /*
   * Check that the interrupt preserved the SPARC input register I2.
   */
  CheckSlot( SLOT_I2 );

  /*
   * Check that the interrupt preserved the SPARC input register I3.
   */
  CheckSlot( SLOT_I3 );

  /*
   * Check that the interrupt preserved the SPARC input register I4.
   */
  CheckSlot( SLOT_I4 );

  /*
   * Check that the interrupt preserved the SPARC input register I5.
   */
  CheckSlot( SLOT_I5 );

  /*
   * Check that the interrupt preserved the SPARC input register I7.
   */
  CheckSlot( SLOT_I7 );

  /*
   * Check that the interrupt preserved the SPARC register Y.
   */
  CheckSlot( SLOT_Y );

  /*
   * Check that the interrupt preserved the integer condition codes of the PSR.
   */
  CheckSlot( SLOT_ICC );

  /*
   * Check that the interrupt preserved the SPARC FSR.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_FSR );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F0
   * and F1.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F0_F1 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F2
   * and F3.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F2_F3 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F4
   * and F5.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F4_F5 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F6
   * and F7.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F6_F7 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F8
   * and F9.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F8_F9 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F10
   * and F11.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F10_F11 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F12
   * and F13.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F12_F13 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F14
   * and F15.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F14_F15 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F16
   * and F17.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F16_F17 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F18
   * and F19.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F18_F19 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F20
   * and F21.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F20_F21 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F22
   * and F23.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F22_F23 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F24
   * and F25.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F24_F25 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F26
   * and F27.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F26_F27 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F28
   * and F29.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F28_F29 );
  #endif

  /*
   * Check that the interrupt preserved the SPARC floating-point registers F30
   * and F31.
   */
  #if SPARC_HAS_FPU == 1
  CheckSlot( SLOT_F30_F31 );
  #endif

  /*
   * Check that the interrupt left the per-CPU control G6 unchanged.
   */
  CheckSlot( SLOT_G6 );

  /*
   * Check that the interrupt preserved the thread pointer G7.
   */
  CheckSlot( SLOT_G7 );

  /*
   * Check that the interrupt preserved the stack pointer, which is O6.
   */
  CheckSlot( SLOT_SP );

  /*
   * Check that the interrupt preserved the frame pointer, which is I6.
   */
  CheckSlot( SLOT_FP );

  /*
   * Check that the interrupt preserved the processor interrupt level.
   */
  CheckSlot( SLOT_PIL );

  /*
   * Check that the interrupted context continued at the instruction after the
   * interrupt and reached the trap.
   */
  CheckSlot( SLOT_PC );

  /*
   * Check that interrupt processing cleared PSR[EF] before it called the
   * handler.
   */
  CheckSlot( SLOT_EF );

  /*
   * Check that the interrupt happened once before the trap.
   */
  CheckSlot( SLOT_COUNT_AT_TRAP );

  /*
   * Check that the handler ran on the processor of the interrupted context.
   */
  CheckSlot( SLOT_PROCESSOR );

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  RegisterCheckReport( &interrupt_check );
}

/**
 * @fn void T_case_body_ScoreCpuSparcValInterruptL7( void )
 */
T_TEST_CASE( ScoreCpuSparcValInterruptL7 )
{
  ScoreCpuSparcValInterruptL7_Action_0();
}

/** @} */
