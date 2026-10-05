/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuSparcValInterruptTraps
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

#include <rtems.h>
#include <string.h>
#include <rtems/score/cpu.h>

#include "tx-register-check.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuSparcValInterruptTraps \
 *   spec:/score/cpu/sparc/val/interrupt-traps
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the software traps which disable and enable interrupts.
 *
 * This test case performs the following actions:
 *
 * - Enter two new register windows. Load a pattern into each register except
 *   G1, G6, G7, O6, I6, and the PSR. Set the integer condition codes to a
 *   pattern. Load G1. Execute the software trap 9. Store each register and the
 *   PSR. Restore the PSR before the trap. Run this through the register check,
 *   once for each value inverted, then once unchanged.
 *
 *   - Check that the trap set the PIL to 15, returned the previous PSR except
 *     the CWP field in G1, and preserved every other register.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Enter two new register windows. Load a pattern into each register except
 *   G1, G6, G7, O6, I6, and the PSR. Set the integer condition codes to a
 *   pattern. Load a pattern into G1. Execute the software trap 10. Store each
 *   register and the PSR. Restore the PSR before the trap. Run this through
 *   the register check, once for each value inverted, then once unchanged.
 *
 *   - Check that the trap set the PIL to the PIL field of G1 and preserved
 *     every register.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Enter two new register windows. Load a pattern into each register except
 *   G1, G6, G7, O6, I6, and the PSR. Set the integer condition codes to a
 *   pattern. Load a pattern into G1. Execute the software trap 11. Store each
 *   register and the PSR. Restore the PSR before the trap. Run this through
 *   the register check, once for each value inverted, then once unchanged.
 *
 *   - Check that the trap set the PIL to 15 and PSR[EF] to the EF bit of G1,
 *     returned the previous PSR except the CWP field in G1, and preserved
 *     every other register.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * @{
 */

#define WORD UINT64_C( 0xffffffff )

/* RD, NS, fcc, aexc and cexc of the FSR */
#define FSR_TEST_MASK UINT64_C( 0xc0400fff )

#define FSR_TEST_BITS 0x404008a5

/* The pattern of G1 for the interrupt enable trap sets the PIL to five */
#define PATTERN_G1_IRQEN 0x6b5a0500

/* The pattern of G1 for the interrupt disable trap with FPU sets PSR[EF] */
#define PATTERN_G1_IRQDISFP 0x6b5a1000

/*
 * The table of values after the trap holds one word for each integer source
 * and the FSR, then G1, the PSR after the trap, and the PSR before the trap.
 */
#define AFTER_G1 29

#define AFTER_PSR 30

#define AFTER_PSR_BEFORE 31

/*
 * The scenario loads each value from the table of initial values.  A value
 * of one word sits at the byte offset eight times its source plus four,
 * since SPARC is big-endian.  A pair of floating-point registers sits at the
 * byte offset eight times its source.  The scenario loads G1 last.
 */
uint64_t irq_trap_initial[ 45 ];

uint32_t irq_trap_g1;

uint32_t irq_trap_after[ AFTER_PSR_BEFORE + 1 ];

#if SPARC_HAS_FPU == 1
uint64_t irq_trap_fp_after[ 16 ];

#define ASM_LOAD_FPU          \
  "ld [%%g1 + 228], %%fsr\n"  \
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
  "nop\n"                     \
  "nop\n"                     \
  "nop\n"

#define ASM_STORE_FPU             \
  "st %%fsr, [%%g1 + 112]\n"      \
  "set irq_trap_fp_after, %%g1\n" \
  "std %%f0, [%%g1 + 0]\n"        \
  "std %%f2, [%%g1 + 8]\n"        \
  "std %%f4, [%%g1 + 16]\n"       \
  "std %%f6, [%%g1 + 24]\n"       \
  "std %%f8, [%%g1 + 32]\n"       \
  "std %%f10, [%%g1 + 40]\n"      \
  "std %%f12, [%%g1 + 48]\n"      \
  "std %%f14, [%%g1 + 56]\n"      \
  "std %%f16, [%%g1 + 64]\n"      \
  "std %%f18, [%%g1 + 72]\n"      \
  "std %%f20, [%%g1 + 80]\n"      \
  "std %%f22, [%%g1 + 88]\n"      \
  "std %%f24, [%%g1 + 96]\n"      \
  "std %%f26, [%%g1 + 104]\n"     \
  "std %%f28, [%%g1 + 112]\n"     \
  "std %%f30, [%%g1 + 120]\n"     \
  "nop\n"
#else
#define ASM_LOAD_FPU

#define ASM_STORE_FPU
#endif

#define ASM_LOAD                                                        \
  "save %%sp, -96, %%sp\n"                                              \
  "save %%sp, -160, %%sp\n"                                             \
  "set irq_trap_initial, %%g1\n" ASM_LOAD_FPU "ld [%%g1 + 212], %%g2\n" \
  "wr %%g2, 0, %%y\n"                                                   \
  "rd %%psr, %%g2\n"                                                    \
  "set 0x00f00000, %%g3\n"                                              \
  "andn %%g2, %%g3, %%g2\n"                                             \
  "ld [%%g1 + 220], %%g4\n"                                             \
  "and %%g4, %%g3, %%g4\n"                                              \
  "or %%g2, %%g4, %%g2\n"                                               \
  "wr %%g2, 0, %%psr\n"                                                 \
  "nop\n"                                                               \
  "nop\n"                                                               \
  "nop\n"                                                               \
  "rd %%psr, %%g2\n"                                                    \
  "set irq_trap_after, %%g3\n"                                          \
  "st %%g2, [%%g3 + 124]\n"                                             \
  "ld [%%g1 + 4], %%g2\n"                                               \
  "ld [%%g1 + 12], %%g3\n"                                              \
  "ld [%%g1 + 20], %%g4\n"                                              \
  "ld [%%g1 + 28], %%g5\n"                                              \
  "ld [%%g1 + 36], %%o0\n"                                              \
  "ld [%%g1 + 44], %%o1\n"                                              \
  "ld [%%g1 + 52], %%o2\n"                                              \
  "ld [%%g1 + 60], %%o3\n"                                              \
  "ld [%%g1 + 68], %%o4\n"                                              \
  "ld [%%g1 + 76], %%o5\n"                                              \
  "ld [%%g1 + 84], %%o7\n"                                              \
  "ld [%%g1 + 92], %%l0\n"                                              \
  "ld [%%g1 + 100], %%l1\n"                                             \
  "ld [%%g1 + 108], %%l2\n"                                             \
  "ld [%%g1 + 116], %%l3\n"                                             \
  "ld [%%g1 + 124], %%l4\n"                                             \
  "ld [%%g1 + 132], %%l5\n"                                             \
  "ld [%%g1 + 140], %%l6\n"                                             \
  "ld [%%g1 + 148], %%l7\n"                                             \
  "ld [%%g1 + 156], %%i0\n"                                             \
  "ld [%%g1 + 164], %%i1\n"                                             \
  "ld [%%g1 + 172], %%i2\n"                                             \
  "ld [%%g1 + 180], %%i3\n"                                             \
  "ld [%%g1 + 188], %%i4\n"                                             \
  "ld [%%g1 + 196], %%i5\n"                                             \
  "ld [%%g1 + 204], %%i7\n"                                             \
  "set irq_trap_g1, %%g1\n"                                             \
  "ld [%%g1], %%g1\n"

#define ASM_STORE                                     \
  "st %%g1, [%%sp + 64]\n"                            \
  "rd %%psr, %%g1\n"                                  \
  "st %%g1, [%%sp + 68]\n"                            \
  "set irq_trap_after, %%g1\n"                        \
  "ld [%%g1 + 124], %%g1\n"                           \
  "wr %%g1, 0, %%psr\n"                               \
  "nop\n"                                             \
  "nop\n"                                             \
  "nop\n"                                             \
  "set irq_trap_after, %%g1\n"                        \
  "st %%g2, [%%g1 + 0]\n"                             \
  "st %%g3, [%%g1 + 4]\n"                             \
  "st %%g4, [%%g1 + 8]\n"                             \
  "st %%g5, [%%g1 + 12]\n"                            \
  "st %%o0, [%%g1 + 16]\n"                            \
  "st %%o1, [%%g1 + 20]\n"                            \
  "st %%o2, [%%g1 + 24]\n"                            \
  "st %%o3, [%%g1 + 28]\n"                            \
  "st %%o4, [%%g1 + 32]\n"                            \
  "st %%o5, [%%g1 + 36]\n"                            \
  "st %%o7, [%%g1 + 40]\n"                            \
  "st %%l0, [%%g1 + 44]\n"                            \
  "st %%l1, [%%g1 + 48]\n"                            \
  "st %%l2, [%%g1 + 52]\n"                            \
  "st %%l3, [%%g1 + 56]\n"                            \
  "st %%l4, [%%g1 + 60]\n"                            \
  "st %%l5, [%%g1 + 64]\n"                            \
  "st %%l6, [%%g1 + 68]\n"                            \
  "st %%l7, [%%g1 + 72]\n"                            \
  "st %%i0, [%%g1 + 76]\n"                            \
  "st %%i1, [%%g1 + 80]\n"                            \
  "st %%i2, [%%g1 + 84]\n"                            \
  "st %%i3, [%%g1 + 88]\n"                            \
  "st %%i4, [%%g1 + 92]\n"                            \
  "st %%i5, [%%g1 + 96]\n"                            \
  "st %%i7, [%%g1 + 100]\n"                           \
  "rd %%y, %%g2\n"                                    \
  "st %%g2, [%%g1 + 104]\n"                           \
  "ld [%%sp + 64], %%g2\n"                            \
  "st %%g2, [%%g1 + 116]\n"                           \
  "ld [%%sp + 68], %%g2\n"                            \
  "st %%g2, [%%g1 + 120]\n" ASM_STORE_FPU "restore\n" \
  "restore\n"

#define ASM_CLOBBER "memory", "cc", "g1", "g2", "g3", "g4", "g5"

typedef enum {
  COMMON_G2,
  COMMON_G3,
  COMMON_G4,
  COMMON_G5,
  COMMON_O0,
  COMMON_O1,
  COMMON_O2,
  COMMON_O3,
  COMMON_O4,
  COMMON_O5,
  COMMON_O7,
  COMMON_L0,
  COMMON_L1,
  COMMON_L2,
  COMMON_L3,
  COMMON_L4,
  COMMON_L5,
  COMMON_L6,
  COMMON_L7,
  COMMON_I0,
  COMMON_I1,
  COMMON_I2,
  COMMON_I3,
  COMMON_I4,
  COMMON_I5,
  COMMON_I7,
  COMMON_Y,
  COMMON_ICC,
#if SPARC_HAS_FPU == 1
  COMMON_FSR,
  COMMON_F0_F1,
  COMMON_F2_F3,
  COMMON_F4_F5,
  COMMON_F6_F7,
  COMMON_F8_F9,
  COMMON_F10_F11,
  COMMON_F12_F13,
  COMMON_F14_F15,
  COMMON_F16_F17,
  COMMON_F18_F19,
  COMMON_F20_F21,
  COMMON_F22_F23,
  COMMON_F24_F25,
  COMMON_F26_F27,
  COMMON_F28_F29,
  COMMON_F30_F31,
#endif
  COMMON_COUNT
} Common;

RTEMS_STATIC_ASSERT( COMMON_ICC + 1 == AFTER_G1 - 1, after_g1 );

RTEMS_STATIC_ASSERT(
  COMMON_COUNT <= RTEMS_ARRAY_SIZE( irq_trap_initial ),
  initial
);

RTEMS_STATIC_ASSERT( COMMON_G2 == 0, common_g2 );
RTEMS_STATIC_ASSERT( COMMON_O0 == 4, common_o0 );
RTEMS_STATIC_ASSERT( COMMON_L0 == 11, common_l0 );
RTEMS_STATIC_ASSERT( COMMON_I0 == 19, common_i0 );
RTEMS_STATIC_ASSERT( COMMON_Y == 26, common_y );
RTEMS_STATIC_ASSERT( COMMON_ICC == 27, common_icc );
#if SPARC_HAS_FPU == 1
RTEMS_STATIC_ASSERT( COMMON_FSR == 28, common_fsr );
#endif
#if SPARC_HAS_FPU == 1
RTEMS_STATIC_ASSERT( COMMON_F0_F1 == 29, common_f0_f1 );
#endif

static const uint64_t irq_trap_patterns[ COMMON_COUNT ] = {
  0x6b004080,
  0x6b014181,
  0x6b024282,
  0x6b034383,
  0x6b044484,
  0x6b054585,
  0x6b064686,
  0x6b074787,
  0x6b084888,
  0x6b094989,
  0x6b0a4a8a,
  0x6b0b4b8b,
  0x6b0c4c8c,
  0x6b0d4d8d,
  0x6b0e4e8e,
  0x6b0f4f8f,
  0x6b105090,
  0x6b115191,
  0x6b125292,
  0x6b135393,
  0x6b145494,
  0x6b155595,
  0x6b165696,
  0x6b175797,
  0x6b185898,
  0x6b195999,
  0x6b1a5a9a,
  0x00500000,
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

static uint64_t IrqTrapAfter( size_t source )
{
#if SPARC_HAS_FPU == 1
  if ( source >= COMMON_F0_F1 ) {
    return irq_trap_fp_after[ source - COMMON_F0_F1 ];
  }
#endif

  return irq_trap_after[ source ];
}

typedef struct {
  void ( *scenario )( void );
  uint64_t g1;
  size_t   extra_count;
} IrqTrapArg;

/*
 * The run records the slot of each common source, then the slots of the
 * trap.
 */
static void IrqTrapRecordCommon( RegisterCheck *self )
{
  size_t i;

  for ( i = 0; i < COMMON_COUNT; ++i ) {
    uint64_t actual;

    if ( i == COMMON_ICC ) {
      actual = irq_trap_after[ AFTER_PSR ];
    } else {
      actual = IrqTrapAfter( i );
    }

    RegisterCheckRecord( self, i, actual, irq_trap_patterns[ i ] );
  }
}

static void IrqTrapPrepare( uint64_t *initial, uint64_t g1 )
{
  memcpy( initial, irq_trap_patterns, sizeof( irq_trap_patterns ) );
  initial[ COMMON_COUNT ] = g1;
}

static const RegisterCheckSource irq_trap_irqdis_sources[] = {
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
  { "y", REGISTER_CHECK_INITIAL, WORD },
  { "icc", REGISTER_CHECK_INITIAL, UINT64_C( 0x00f00000 ) },
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
  { "psr", REGISTER_CHECK_EXPECTED, WORD },
  { "pil", REGISTER_CHECK_EXPECTED, WORD },
};

static const RegisterCheckSlot irq_trap_irqdis_slots[] = {
  { "g2", COMMON_G2, WORD },
  { "g3", COMMON_G3, WORD },
  { "g4", COMMON_G4, WORD },
  { "g5", COMMON_G5, WORD },
  { "o0", COMMON_O0, WORD },
  { "o1", COMMON_O1, WORD },
  { "o2", COMMON_O2, WORD },
  { "o3", COMMON_O3, WORD },
  { "o4", COMMON_O4, WORD },
  { "o5", COMMON_O5, WORD },
  { "o7", COMMON_O7, WORD },
  { "l0", COMMON_L0, WORD },
  { "l1", COMMON_L1, WORD },
  { "l2", COMMON_L2, WORD },
  { "l3", COMMON_L3, WORD },
  { "l4", COMMON_L4, WORD },
  { "l5", COMMON_L5, WORD },
  { "l6", COMMON_L6, WORD },
  { "l7", COMMON_L7, WORD },
  { "i0", COMMON_I0, WORD },
  { "i1", COMMON_I1, WORD },
  { "i2", COMMON_I2, WORD },
  { "i3", COMMON_I3, WORD },
  { "i4", COMMON_I4, WORD },
  { "i5", COMMON_I5, WORD },
  { "i7", COMMON_I7, WORD },
  { "y", COMMON_Y, WORD },
  { "icc", COMMON_ICC, UINT64_C( 0x00f00000 ) },
#if SPARC_HAS_FPU == 1
  { "fsr", COMMON_FSR, FSR_TEST_MASK },
  { "f0_f1", COMMON_F0_F1, UINT64_MAX },
  { "f2_f3", COMMON_F2_F3, UINT64_MAX },
  { "f4_f5", COMMON_F4_F5, UINT64_MAX },
  { "f6_f7", COMMON_F6_F7, UINT64_MAX },
  { "f8_f9", COMMON_F8_F9, UINT64_MAX },
  { "f10_f11", COMMON_F10_F11, UINT64_MAX },
  { "f12_f13", COMMON_F12_F13, UINT64_MAX },
  { "f14_f15", COMMON_F14_F15, UINT64_MAX },
  { "f16_f17", COMMON_F16_F17, UINT64_MAX },
  { "f18_f19", COMMON_F18_F19, UINT64_MAX },
  { "f20_f21", COMMON_F20_F21, UINT64_MAX },
  { "f22_f23", COMMON_F22_F23, UINT64_MAX },
  { "f24_f25", COMMON_F24_F25, UINT64_MAX },
  { "f26_f27", COMMON_F26_F27, UINT64_MAX },
  { "f28_f29", COMMON_F28_F29, UINT64_MAX },
  { "f30_f31", COMMON_F30_F31, UINT64_MAX },
#endif
  { "g1", COMMON_COUNT + 0, UINT64_C( 0xffffffe0 ) },
  { "pil", COMMON_COUNT + 1, UINT64_C( 0x00000f00 ) },
};

static uint64_t irq_trap_irqdis_initial[ COMMON_COUNT + 2 ];

static void InterruptDisableScenario( void )
{
  __asm__ volatile( ASM_LOAD "ta 9\n" ASM_STORE
                    :
                    :
                    : ASM_CLOBBER );
}

static void InterruptDisableRun( RegisterCheck *self, void *arg )
{
  const uint32_t *after;

  (void) arg;
  after = irq_trap_after;
  memset( irq_trap_after, 0, sizeof( irq_trap_after ) );
  memcpy(
    irq_trap_initial,
    self->initial,
    sizeof( irq_trap_initial[ 0 ] ) * COMMON_COUNT
  );
  irq_trap_g1 = (uint32_t) self->initial[ COMMON_COUNT ];
  InterruptDisableScenario();
  IrqTrapRecordCommon( self );
  RegisterCheckRecord(
    self,
    COMMON_COUNT + 0,
    after[ AFTER_G1 ],
    after[ AFTER_PSR_BEFORE ]
  );
  RegisterCheckRecord(
    self,
    COMMON_COUNT + 1,
    after[ AFTER_PSR ],
    0x00000f00
  );
}

static RegisterCheck irq_trap_irqdis_check = {
  .sources = irq_trap_irqdis_sources,
  .source_count = RTEMS_ARRAY_SIZE( irq_trap_irqdis_sources ),
  .slots = irq_trap_irqdis_slots,
  .slot_count = RTEMS_ARRAY_SIZE( irq_trap_irqdis_slots ),
  .initial = irq_trap_irqdis_initial,
  .run = InterruptDisableRun
};

static const RegisterCheckSource irq_trap_irqen_sources[] = {
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
  { "y", REGISTER_CHECK_INITIAL, WORD },
  { "icc", REGISTER_CHECK_INITIAL, UINT64_C( 0x00f00000 ) },
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
  { "g1", REGISTER_CHECK_INITIAL, WORD },
};

static const RegisterCheckSlot irq_trap_irqen_slots[] = {
  { "g2", COMMON_G2, WORD },
  { "g3", COMMON_G3, WORD },
  { "g4", COMMON_G4, WORD },
  { "g5", COMMON_G5, WORD },
  { "o0", COMMON_O0, WORD },
  { "o1", COMMON_O1, WORD },
  { "o2", COMMON_O2, WORD },
  { "o3", COMMON_O3, WORD },
  { "o4", COMMON_O4, WORD },
  { "o5", COMMON_O5, WORD },
  { "o7", COMMON_O7, WORD },
  { "l0", COMMON_L0, WORD },
  { "l1", COMMON_L1, WORD },
  { "l2", COMMON_L2, WORD },
  { "l3", COMMON_L3, WORD },
  { "l4", COMMON_L4, WORD },
  { "l5", COMMON_L5, WORD },
  { "l6", COMMON_L6, WORD },
  { "l7", COMMON_L7, WORD },
  { "i0", COMMON_I0, WORD },
  { "i1", COMMON_I1, WORD },
  { "i2", COMMON_I2, WORD },
  { "i3", COMMON_I3, WORD },
  { "i4", COMMON_I4, WORD },
  { "i5", COMMON_I5, WORD },
  { "i7", COMMON_I7, WORD },
  { "y", COMMON_Y, WORD },
  { "icc", COMMON_ICC, UINT64_C( 0x00f00000 ) },
#if SPARC_HAS_FPU == 1
  { "fsr", COMMON_FSR, FSR_TEST_MASK },
  { "f0_f1", COMMON_F0_F1, UINT64_MAX },
  { "f2_f3", COMMON_F2_F3, UINT64_MAX },
  { "f4_f5", COMMON_F4_F5, UINT64_MAX },
  { "f6_f7", COMMON_F6_F7, UINT64_MAX },
  { "f8_f9", COMMON_F8_F9, UINT64_MAX },
  { "f10_f11", COMMON_F10_F11, UINT64_MAX },
  { "f12_f13", COMMON_F12_F13, UINT64_MAX },
  { "f14_f15", COMMON_F14_F15, UINT64_MAX },
  { "f16_f17", COMMON_F16_F17, UINT64_MAX },
  { "f18_f19", COMMON_F18_F19, UINT64_MAX },
  { "f20_f21", COMMON_F20_F21, UINT64_MAX },
  { "f22_f23", COMMON_F22_F23, UINT64_MAX },
  { "f24_f25", COMMON_F24_F25, UINT64_MAX },
  { "f26_f27", COMMON_F26_F27, UINT64_MAX },
  { "f28_f29", COMMON_F28_F29, UINT64_MAX },
  { "f30_f31", COMMON_F30_F31, UINT64_MAX },
#endif
  { "g1", COMMON_COUNT + 0, WORD },
  { "pil", COMMON_COUNT + 0, UINT64_C( 0x00000f00 ) },
};

static uint64_t irq_trap_irqen_initial[ COMMON_COUNT + 1 ];

static void InterruptEnableScenario( void )
{
  __asm__ volatile( ASM_LOAD "ta 10\n" ASM_STORE
                    :
                    :
                    : ASM_CLOBBER );
}

static void InterruptEnableRun( RegisterCheck *self, void *arg )
{
  const uint32_t *after;

  (void) arg;
  after = irq_trap_after;
  memset( irq_trap_after, 0, sizeof( irq_trap_after ) );
  memcpy(
    irq_trap_initial,
    self->initial,
    sizeof( irq_trap_initial[ 0 ] ) * COMMON_COUNT
  );
  irq_trap_g1 = (uint32_t) self->initial[ COMMON_COUNT ];
  InterruptEnableScenario();
  IrqTrapRecordCommon( self );
  RegisterCheckRecord(
    self,
    COMMON_COUNT + 0,
    after[ AFTER_G1 ],
    PATTERN_G1_IRQEN
  );
  RegisterCheckRecord(
    self,
    COMMON_COUNT + 1,
    after[ AFTER_PSR ],
    PATTERN_G1_IRQEN
  );
}

static RegisterCheck irq_trap_irqen_check = {
  .sources = irq_trap_irqen_sources,
  .source_count = RTEMS_ARRAY_SIZE( irq_trap_irqen_sources ),
  .slots = irq_trap_irqen_slots,
  .slot_count = RTEMS_ARRAY_SIZE( irq_trap_irqen_slots ),
  .initial = irq_trap_irqen_initial,
  .run = InterruptEnableRun
};

#if defined( SPARC_USE_SYNCHRONOUS_FP_SWITCH )
static const RegisterCheckSource irq_trap_irqdisfp_sources[] = {
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
  { "y", REGISTER_CHECK_INITIAL, WORD },
  { "icc", REGISTER_CHECK_INITIAL, UINT64_C( 0x00f00000 ) },
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
  { "g1", REGISTER_CHECK_INITIAL, WORD },
  { "psr", REGISTER_CHECK_EXPECTED, WORD },
  { "pil", REGISTER_CHECK_EXPECTED, WORD },
};

static const RegisterCheckSlot irq_trap_irqdisfp_slots[] = {
  { "g2", COMMON_G2, WORD },
  { "g3", COMMON_G3, WORD },
  { "g4", COMMON_G4, WORD },
  { "g5", COMMON_G5, WORD },
  { "o0", COMMON_O0, WORD },
  { "o1", COMMON_O1, WORD },
  { "o2", COMMON_O2, WORD },
  { "o3", COMMON_O3, WORD },
  { "o4", COMMON_O4, WORD },
  { "o5", COMMON_O5, WORD },
  { "o7", COMMON_O7, WORD },
  { "l0", COMMON_L0, WORD },
  { "l1", COMMON_L1, WORD },
  { "l2", COMMON_L2, WORD },
  { "l3", COMMON_L3, WORD },
  { "l4", COMMON_L4, WORD },
  { "l5", COMMON_L5, WORD },
  { "l6", COMMON_L6, WORD },
  { "l7", COMMON_L7, WORD },
  { "i0", COMMON_I0, WORD },
  { "i1", COMMON_I1, WORD },
  { "i2", COMMON_I2, WORD },
  { "i3", COMMON_I3, WORD },
  { "i4", COMMON_I4, WORD },
  { "i5", COMMON_I5, WORD },
  { "i7", COMMON_I7, WORD },
  { "y", COMMON_Y, WORD },
  { "icc", COMMON_ICC, UINT64_C( 0x00f00000 ) },
#if SPARC_HAS_FPU == 1
  { "fsr", COMMON_FSR, FSR_TEST_MASK },
  { "f0_f1", COMMON_F0_F1, UINT64_MAX },
  { "f2_f3", COMMON_F2_F3, UINT64_MAX },
  { "f4_f5", COMMON_F4_F5, UINT64_MAX },
  { "f6_f7", COMMON_F6_F7, UINT64_MAX },
  { "f8_f9", COMMON_F8_F9, UINT64_MAX },
  { "f10_f11", COMMON_F10_F11, UINT64_MAX },
  { "f12_f13", COMMON_F12_F13, UINT64_MAX },
  { "f14_f15", COMMON_F14_F15, UINT64_MAX },
  { "f16_f17", COMMON_F16_F17, UINT64_MAX },
  { "f18_f19", COMMON_F18_F19, UINT64_MAX },
  { "f20_f21", COMMON_F20_F21, UINT64_MAX },
  { "f22_f23", COMMON_F22_F23, UINT64_MAX },
  { "f24_f25", COMMON_F24_F25, UINT64_MAX },
  { "f26_f27", COMMON_F26_F27, UINT64_MAX },
  { "f28_f29", COMMON_F28_F29, UINT64_MAX },
  { "f30_f31", COMMON_F30_F31, UINT64_MAX },
#endif
  { "g1", COMMON_COUNT + 1, UINT64_C( 0xffffffe0 ) },
  { "ef", COMMON_COUNT + 0, UINT64_C( 0x00001000 ) },
  { "pil", COMMON_COUNT + 2, UINT64_C( 0x00000f00 ) },
};

static uint64_t irq_trap_irqdisfp_initial[ COMMON_COUNT + 3 ];

static void InterruptDisableFpScenario( void )
{
  __asm__ volatile( ASM_LOAD "ta 11\n" ASM_STORE
                    :
                    :
                    : ASM_CLOBBER );
}

static void InterruptDisableFpRun( RegisterCheck *self, void *arg )
{
  const uint32_t *after;

  (void) arg;
  after = irq_trap_after;
  memset( irq_trap_after, 0, sizeof( irq_trap_after ) );
  memcpy(
    irq_trap_initial,
    self->initial,
    sizeof( irq_trap_initial[ 0 ] ) * COMMON_COUNT
  );
  irq_trap_g1 = (uint32_t) self->initial[ COMMON_COUNT ];
  InterruptDisableFpScenario();
  IrqTrapRecordCommon( self );
  RegisterCheckRecord(
    self,
    COMMON_COUNT + 0,
    after[ AFTER_G1 ],
    after[ AFTER_PSR_BEFORE ]
  );
  RegisterCheckRecord(
    self,
    COMMON_COUNT + 1,
    after[ AFTER_PSR ],
    PATTERN_G1_IRQDISFP
  );
  RegisterCheckRecord(
    self,
    COMMON_COUNT + 2,
    after[ AFTER_PSR ],
    0x00000f00
  );
}

static RegisterCheck irq_trap_irqdisfp_check = {
  .sources = irq_trap_irqdisfp_sources,
  .source_count = RTEMS_ARRAY_SIZE( irq_trap_irqdisfp_sources ),
  .slots = irq_trap_irqdisfp_slots,
  .slot_count = RTEMS_ARRAY_SIZE( irq_trap_irqdisfp_slots ),
  .initial = irq_trap_irqdisfp_initial,
  .run = InterruptDisableFpRun
};
#endif

/**
 * @brief Enter two new register windows. Load a pattern into each register
 *   except G1, G6, G7, O6, I6, and the PSR. Set the integer condition codes to
 *   a pattern. Load G1. Execute the software trap 9. Store each register and
 *   the PSR. Restore the PSR before the trap. Run this through the register
 *   check, once for each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValInterruptTraps_Action_0( void )
{
  IrqTrapPrepare( irq_trap_irqdis_initial, 0 );
  RegisterCheckRun( &irq_trap_irqdis_check );

  /*
   * Check that the trap set the PIL to 15, returned the previous PSR except
   * the CWP field in G1, and preserved every other register.
   */
  for ( size_t i = 0; i < irq_trap_irqdis_check.slot_count; ++i ) {
    RegisterCheckVerify( &irq_trap_irqdis_check, i );
  }

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  RegisterCheckReport( &irq_trap_irqdis_check );
}

/**
 * @brief Enter two new register windows. Load a pattern into each register
 *   except G1, G6, G7, O6, I6, and the PSR. Set the integer condition codes to
 *   a pattern. Load a pattern into G1. Execute the software trap 10. Store
 *   each register and the PSR. Restore the PSR before the trap. Run this
 *   through the register check, once for each value inverted, then once
 *   unchanged.
 */
static void ScoreCpuSparcValInterruptTraps_Action_1( void )
{
  IrqTrapPrepare( irq_trap_irqen_initial, PATTERN_G1_IRQEN );
  RegisterCheckRun( &irq_trap_irqen_check );

  /*
   * Check that the trap set the PIL to the PIL field of G1 and preserved every
   * register.
   */
  for ( size_t i = 0; i < irq_trap_irqen_check.slot_count; ++i ) {
    RegisterCheckVerify( &irq_trap_irqen_check, i );
  }

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  RegisterCheckReport( &irq_trap_irqen_check );
}

/**
 * @brief Enter two new register windows. Load a pattern into each register
 *   except G1, G6, G7, O6, I6, and the PSR. Set the integer condition codes to
 *   a pattern. Load a pattern into G1. Execute the software trap 11. Store
 *   each register and the PSR. Restore the PSR before the trap. Run this
 *   through the register check, once for each value inverted, then once
 *   unchanged.
 */
static void ScoreCpuSparcValInterruptTraps_Action_2( void )
{
  #if defined( SPARC_USE_SYNCHRONOUS_FP_SWITCH )
  IrqTrapPrepare( irq_trap_irqdisfp_initial, PATTERN_G1_IRQDISFP );
  RegisterCheckRun( &irq_trap_irqdisfp_check );
  #endif

  /*
   * Check that the trap set the PIL to 15 and PSR[EF] to the EF bit of G1,
   * returned the previous PSR except the CWP field in G1, and preserved every
   * other register.
   */
  #if defined( SPARC_USE_SYNCHRONOUS_FP_SWITCH )
  for ( size_t i = 0; i < irq_trap_irqdisfp_check.slot_count; ++i ) {
    RegisterCheckVerify( &irq_trap_irqdisfp_check, i );
  }
  #endif

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  #if defined( SPARC_USE_SYNCHRONOUS_FP_SWITCH )
  RegisterCheckReport( &irq_trap_irqdisfp_check );
  #endif
}

/**
 * @fn void T_case_body_ScoreCpuSparcValInterruptTraps( void )
 */
T_TEST_CASE( ScoreCpuSparcValInterruptTraps )
{
  ScoreCpuSparcValInterruptTraps_Action_0();
  ScoreCpuSparcValInterruptTraps_Action_1();
  ScoreCpuSparcValInterruptTraps_Action_2();
}

/** @} */
