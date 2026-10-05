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

#ifndef _TR_SPARC_EXCEPTION_FRAME_H
#define _TR_SPARC_EXCEPTION_FRAME_H

#include <rtems.h>
#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreCpuSparcValExceptionFrame
 *
 * @{
 */

/*
 * The block loads each value from the table of initial values.  A value of
 * one word sits at the byte offset eight times its source plus four, since
 * SPARC is big-endian.  A pair of floating-point registers sits at the byte
 * offset eight times its source.
 */
#define PSR_ICC_MASK 0x00f00000

#define PSR_ICC_BITS 0x00500000

/* RD, NS, fcc, aexc and cexc of the FSR */
#define FSR_TEST_MASK 0xc0400fff

#define FSR_TEST_BITS 0x404008a5

#if SPARC_HAS_FPU == 1
#define ASM_PREPARE_FPU       \
  "ldd [%%g1 + 304], %%f0\n"  \
  "ldd [%%g1 + 312], %%f2\n"  \
  "ldd [%%g1 + 320], %%f4\n"  \
  "ldd [%%g1 + 328], %%f6\n"  \
  "ldd [%%g1 + 336], %%f8\n"  \
  "ldd [%%g1 + 344], %%f10\n" \
  "ldd [%%g1 + 352], %%f12\n" \
  "ldd [%%g1 + 360], %%f14\n" \
  "ldd [%%g1 + 368], %%f16\n" \
  "ldd [%%g1 + 376], %%f18\n" \
  "ldd [%%g1 + 384], %%f20\n" \
  "ldd [%%g1 + 392], %%f22\n" \
  "ldd [%%g1 + 400], %%f24\n" \
  "ldd [%%g1 + 408], %%f26\n" \
  "ldd [%%g1 + 416], %%f28\n" \
  "ldd [%%g1 + 424], %%f30\n" \
  "ld [%%g1 + 300], %%fsr\n"  \
  "nop\n"                     \
  "nop\n"                     \
  "nop\n"

#define ASM_CLOBBER_FPU                                                       \
  , "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", \
    "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21",     \
    "f22", "f23", "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31"
#else
#define ASM_PREPARE_FPU

#define ASM_CLOBBER_FPU
#endif

/*
 * The block enters two new register windows.  It loads the local registers
 * of the outer window, then every register of the inner window.  It clears
 * the PSR bits of the clear mask after the floating-point loads.  It loads
 * g1 last, because g1 holds the address of the table.  The scenario traps
 * in the inner window.
 */
#define ASM_PREPARE_CLEAR( clear )                               \
  "save %%sp, -96, %%sp\n"                                       \
  "set sparc_exception_initial, %%g1\n"                          \
  "ld [%%g1 + 220], %%l0\n"                                      \
  "ld [%%g1 + 228], %%l1\n"                                      \
  "ld [%%g1 + 236], %%l2\n"                                      \
  "ld [%%g1 + 244], %%l3\n"                                      \
  "ld [%%g1 + 252], %%l4\n"                                      \
  "ld [%%g1 + 260], %%l5\n"                                      \
  "ld [%%g1 + 268], %%l6\n"                                      \
  "ld [%%g1 + 276], %%l7\n"                                      \
  "save %%sp, -96, %%sp\n"                                       \
  "set sparc_exception_expected, %%g1\n"                         \
  "st %%sp, [%%g1 + 0]\n"                                        \
  "st %%fp, [%%g1 + 4]\n"                                        \
  "set sparc_exception_initial, %%g1\n" ASM_PREPARE_FPU          \
  "ld [%%g1 + 284], %%g2\n"                                      \
  "wr %%g2, 0, %%y\n"                                            \
  "rd %%psr, %%g2\n"                                             \
  "set " RTEMS_XSTRING( PSR_ICC_MASK ) ", %%g3\n"                \
                                       "andn %%g2, %%g3, %%g2\n" \
                                       "ld [%%g1 + 292], %%g4\n" \
                                       "and %%g4, %%g3, %%g4\n"  \
                                       "or %%g2, %%g4, %%g2\n"   \
                                       "set " clear ", %%g3\n"   \
                                       "andn %%g2, %%g3, %%g2\n" \
                                       "wr %%g2, 0, %%psr\n"     \
                                       "nop\n"                   \
                                       "nop\n"                   \
                                       "nop\n"                   \
                                       "ld [%%g1 + 100], %%l0\n" \
                                       "ld [%%g1 + 108], %%l1\n" \
                                       "ld [%%g1 + 116], %%l2\n" \
                                       "ld [%%g1 + 124], %%l3\n" \
                                       "ld [%%g1 + 132], %%l4\n" \
                                       "ld [%%g1 + 140], %%l5\n" \
                                       "ld [%%g1 + 148], %%l6\n" \
                                       "ld [%%g1 + 156], %%l7\n" \
                                       "ld [%%g1 + 164], %%i0\n" \
                                       "ld [%%g1 + 172], %%i1\n" \
                                       "ld [%%g1 + 180], %%i2\n" \
                                       "ld [%%g1 + 188], %%i3\n" \
                                       "ld [%%g1 + 196], %%i4\n" \
                                       "ld [%%g1 + 204], %%i5\n" \
                                       "ld [%%g1 + 212], %%i7\n" \
                                       "ld [%%g1 + 44], %%o0\n"  \
                                       "ld [%%g1 + 52], %%o1\n"  \
                                       "ld [%%g1 + 60], %%o2\n"  \
                                       "ld [%%g1 + 68], %%o3\n"  \
                                       "ld [%%g1 + 76], %%o4\n"  \
                                       "ld [%%g1 + 84], %%o5\n"  \
                                       "ld [%%g1 + 92], %%o7\n"  \
                                       "ld [%%g1 + 12], %%g2\n"  \
                                       "ld [%%g1 + 20], %%g3\n"  \
                                       "ld [%%g1 + 28], %%g4\n"  \
                                       "ld [%%g1 + 36], %%g5\n"  \
                                       "ld [%%g1 + 4], %%g1\n"

#define ASM_PREPARE ASM_PREPARE_CLEAR( "0" )

/* The scenario continues after the trapping instruction. */
#define ASM_FINISH \
  "restore\n"      \
  "restore\n"

#define ASM_CLOBBER \
  "memory", "cc", "g1", "g2", "g3", "g4", "g5" ASM_CLOBBER_FPU

extern uint64_t sparc_exception_initial[];

/* The sources which a scenario may override */
#define EXCEPTION_FRAME_SOURCE_G2 1

#define EXCEPTION_FRAME_SOURCE_O7 11

typedef struct {
  uint32_t    source;
  const void *value;
} ExceptionFrameOverride;

/*
 * A scenario variant sets the value of each override instead of the value of
 * the table.  The fatal handler continues at the resume address, or after
 * the trapping instruction if the address is NULL.  It sets the PSR bits of
 * the set mask in the exception frame.
 */
typedef struct {
  const void            *resume;
  uint32_t               psr_set;
  size_t                 override_count;
  ExceptionFrameOverride overrides[ 2 ];
} ExceptionFrameVariant;

/* The stack pointer and the frame pointer of the inner window */
extern uint32_t sparc_exception_expected[ 2 ];

extern const void *exception_expected_pc;

CPU_Exception_frame *ExceptionFrameInitialize( void );

void ExceptionFrameCheckTrap( void );

/**
 * @brief Runs the parameterized test case.
 *
 * @param scenario is the scenario. It traps once.
 *
 * @param trap is the expected trap type.
 *
 * @param variant is the variant of the scenario, or NULL for no variant.
 */
void ScoreCpuSparcValExceptionFrame_Run(
  void ( *scenario )( void ),
  uint32_t                     trap,
  const ExceptionFrameVariant *variant
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_SPARC_EXCEPTION_FRAME_H */
