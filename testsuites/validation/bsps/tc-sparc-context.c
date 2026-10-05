/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuSparcValContext
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

#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/percpu.h>
#include <rtems/score/sparc.h>
#include <rtems/score/thread.h>

#include "tx-register-check.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuSparcValContext spec:/score/cpu/sparc/val/context
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Checks that the context switch saves and restores every call-saved
 *   register and the floating-point environment.
 *
 * The first action calls the switch directly. A call through the operating
 * system runs a chain of functions. A function of the chain may preserve a
 * register on its own stack. This hides a defect of the switch in that
 * register.
 *
 * This test case performs the following actions:
 *
 * - Flush the register windows. Load a distinct pattern into every call-saved
 *   register. Call the context switch directly. The heir switches back. Trap,
 *   so the frame reports every register. Run this once for each register with
 *   the pattern of that register inverted, then once unchanged.
 *
 *   - Check that the switch restored the SPARC local register L0.
 *
 *   - Check that the switch restored the SPARC local register L1.
 *
 *   - Check that the switch restored the SPARC local register L2.
 *
 *   - Check that the switch restored the SPARC local register L3.
 *
 *   - Check that the switch restored the SPARC local register L4.
 *
 *   - Check that the switch restored the SPARC local register L5.
 *
 *   - Check that the switch restored the SPARC local register L6.
 *
 *   - Check that the switch restored the SPARC local register L7.
 *
 *   - Check that the switch restored the SPARC input register I0.
 *
 *   - Check that the switch restored the SPARC input register I1.
 *
 *   - Check that the switch restored the SPARC input register I2.
 *
 *   - Check that the switch restored the SPARC input register I3.
 *
 *   - Check that the switch restored the SPARC input register I4.
 *
 *   - Check that the switch restored the SPARC input register I5.
 *
 *   - Check that the switch restored the SPARC input register I7.
 *
 *   - Check that the switch saved the SPARC local register L0.
 *
 *   - Check that the switch saved the SPARC local register L1.
 *
 *   - Check that the switch saved the SPARC local register L2.
 *
 *   - Check that the switch saved the SPARC local register L3.
 *
 *   - Check that the switch saved the SPARC local register L4.
 *
 *   - Check that the switch saved the SPARC local register L5.
 *
 *   - Check that the switch saved the SPARC local register L6.
 *
 *   - Check that the switch saved the SPARC local register L7.
 *
 *   - Check that the switch saved the SPARC input register I0.
 *
 *   - Check that the switch saved the SPARC input register I1.
 *
 *   - Check that the switch saved the SPARC input register I2.
 *
 *   - Check that the switch saved the SPARC input register I3.
 *
 *   - Check that the switch saved the SPARC input register I4.
 *
 *   - Check that the switch saved the SPARC input register I5.
 *
 *   - Check that the switch saved the SPARC input register I7.
 *
 *   - Check that the switch restored the SPARC global register G5.
 *
 *   - Check that the switch saved the SPARC global register G5.
 *
 *   - Check that the switch restored the integer condition codes.
 *
 *   - Check that the switch saved the integer condition codes.
 *
 *   - Check that the switch restored the stack pointer, which is O6.
 *
 *   - Check that the switch saved the stack pointer, which is O6.
 *
 *   - Check that the switch restored the address of the call, which is O7.
 *
 *   - Check that the switch saved the address of the call, which is O7.
 *
 *   - Check that the switch restored the frame pointer, which is I6.
 *
 *   - Check that the switch saved the frame pointer, which is I6.
 *
 *   - Check that the switch left the per-CPU control G6 unchanged.
 *
 *   - Check that the switch restored the thread pointer G7 of the executing
 *     context.
 *
 *   - Check that the switch restored the processor interrupt level.
 *
 *   - Check that the heir received the SPARC global register G5 of its
 *     context.
 *
 *   - Check that the heir received the thread pointer G7 of its context.
 *
 *   - Check that the heir received the stack pointer of its context.
 *
 *   - Check that the trap happened and that each run with a changed value
 *     flagged exactly the checks of that value, and that each run recorded
 *     every check.
 *
 * - Let task A set its FSR. Dispatch to task D through an interrupt. Let task
 *   A start task C, which records its initial FSR and sets its FSR. Record the
 *   FSR of task A and of task C after the switches. Run this once for each FSR
 *   with the pattern of that FSR inverted, then once unchanged.
 *
 *   - Check that the switches preserved the FSR of task A.
 *
 *   - Check that the switches preserved the FSR of task C.
 *
 *   - Check that task C started with the initial FSR.
 *
 *   - Check that the interrupt dispatched to task D once.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Let task A load a pattern into each floating-point register and the FSR
 *   with interrupts disabled. Raise an interrupt which resumes task E of
 *   higher priority. Enable interrupts in task A. Task E loads other values
 *   into each floating-point register and the FSR, counts its run, and
 *   suspends itself. Task A waits for the count and stores its registers. Run
 *   this once for each register with the pattern of that register inverted,
 *   then once unchanged.
 *
 *   - Check that the thread dispatch of the interrupt preserved each
 *     floating-point register and the FSR of task A.
 *
 *   - Check that task E ran once.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Let a task without the floating-point attribute execute a floating-point
 *   instruction. Record the fatal source and the fatal code. Jump back to the
 *   task. Run this once for each value with the expected value inverted, then
 *   once unchanged.
 *
 *   - Check that the floating-point disabled trap handler terminated the
 *     system with the fatal source INTERNAL_ERROR_CORE.
 *
 *   - Check that the floating-point disabled trap handler terminated the
 *     system with the fatal code
 *     INTERNAL_ERROR_ILLEGAL_USE_OF_FLOATING_POINT_UNIT.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Let task R set its FSR. Dispatch to task S through an interrupt. Let task
 *   S use the floating-point unit, so the floating-point disabled trap handler
 *   saves the floating-point context of task R. Dispatch to task U through an
 *   interrupt, so task S owns the floating-point unit. Let task U suspend task
 *   S and restart task R. Let the restarted task R record its FSR. Run this
 *   once for each value with that value inverted, then once unchanged.
 *
 *   - Check that the floating-point disabled trap handler saved the FSR of
 *     task R before the restart.
 *
 *   - Check that task S owned the floating-point unit at the restart.
 *
 *   - Check that the restarted task R started with the initial FSR.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * @{
 */

extern const char context_done_label[];

#define WORD UINT64_C( 0xffffffff )

#define PSR_ICC_MASK 0x00f00000

#define CONTEXT_HEIR_G5 0x31323334

#define CONTEXT_HEIR_G7 0x41424344

typedef enum {
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
  SOURCE_G5,
  SOURCE_ICC,
  SOURCE_SP,
  SOURCE_O7,
  SOURCE_FP,
  SOURCE_G6,
  SOURCE_G7,
  SOURCE_PIL,
  SOURCE_HEIR_G5,
  SOURCE_HEIR_G7,
  SOURCE_HEIR_SP,
  SOURCE_TRAP,
  SOURCE_MAX
} Source;

typedef enum {
  SLOT_FRAME_L0,
  SLOT_FRAME_L1,
  SLOT_FRAME_L2,
  SLOT_FRAME_L3,
  SLOT_FRAME_L4,
  SLOT_FRAME_L5,
  SLOT_FRAME_L6,
  SLOT_FRAME_L7,
  SLOT_FRAME_I0,
  SLOT_FRAME_I1,
  SLOT_FRAME_I2,
  SLOT_FRAME_I3,
  SLOT_FRAME_I4,
  SLOT_FRAME_I5,
  SLOT_FRAME_I7,
  SLOT_SAVED_L0,
  SLOT_SAVED_L1,
  SLOT_SAVED_L2,
  SLOT_SAVED_L3,
  SLOT_SAVED_L4,
  SLOT_SAVED_L5,
  SLOT_SAVED_L6,
  SLOT_SAVED_L7,
  SLOT_SAVED_I0,
  SLOT_SAVED_I1,
  SLOT_SAVED_I2,
  SLOT_SAVED_I3,
  SLOT_SAVED_I4,
  SLOT_SAVED_I5,
  SLOT_SAVED_I7,
  SLOT_FRAME_G5,
  SLOT_SAVED_G5,
  SLOT_FRAME_ICC,
  SLOT_SAVED_ICC,
  SLOT_FRAME_SP,
  SLOT_SAVED_SP,
  SLOT_FRAME_O7,
  SLOT_SAVED_O7,
  SLOT_FRAME_FP,
  SLOT_SAVED_FP,
  SLOT_FRAME_G6,
  SLOT_FRAME_G7,
  SLOT_FRAME_PIL,
  SLOT_HEIR_G5,
  SLOT_HEIR_G7,
  SLOT_HEIR_SP,
  SLOT_TRAP,
  SLOT_MAX
} Slot;

/* The runner loads each register from the byte offset eight times its source
 * plus four.
 */
RTEMS_STATIC_ASSERT( SOURCE_L0 == 0, source_l0 );
RTEMS_STATIC_ASSERT( SOURCE_I0 == 8, source_i0 );
RTEMS_STATIC_ASSERT( SOURCE_G5 == 15, source_g5 );
RTEMS_STATIC_ASSERT( SOURCE_ICC == 16, source_icc );

static const RegisterCheckSource context_sources[ SOURCE_MAX ] = {
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
  { "g5", REGISTER_CHECK_INITIAL, WORD },
  { "icc", REGISTER_CHECK_INITIAL, PSR_ICC_MASK },
  { "sp", REGISTER_CHECK_EXPECTED, WORD },
  { "o7", REGISTER_CHECK_EXPECTED, WORD },
  { "fp", REGISTER_CHECK_EXPECTED, WORD },
  { "g6", REGISTER_CHECK_EXPECTED, WORD },
  { "g7", REGISTER_CHECK_EXPECTED, WORD },
  { "pil", REGISTER_CHECK_EXPECTED, WORD },
  { "heir g5", REGISTER_CHECK_EXPECTED, WORD },
  { "heir g7", REGISTER_CHECK_EXPECTED, WORD },
  { "heir sp", REGISTER_CHECK_EXPECTED, WORD },
  { "trap", REGISTER_CHECK_EXPECTED, WORD },
};

static const RegisterCheckSlot context_slots[ SLOT_MAX ] = {
  { "frame l0", SOURCE_L0, WORD },
  { "frame l1", SOURCE_L1, WORD },
  { "frame l2", SOURCE_L2, WORD },
  { "frame l3", SOURCE_L3, WORD },
  { "frame l4", SOURCE_L4, WORD },
  { "frame l5", SOURCE_L5, WORD },
  { "frame l6", SOURCE_L6, WORD },
  { "frame l7", SOURCE_L7, WORD },
  { "frame i0", SOURCE_I0, WORD },
  { "frame i1", SOURCE_I1, WORD },
  { "frame i2", SOURCE_I2, WORD },
  { "frame i3", SOURCE_I3, WORD },
  { "frame i4", SOURCE_I4, WORD },
  { "frame i5", SOURCE_I5, WORD },
  { "frame i7", SOURCE_I7, WORD },
  { "saved l0", SOURCE_L0, WORD },
  { "saved l1", SOURCE_L1, WORD },
  { "saved l2", SOURCE_L2, WORD },
  { "saved l3", SOURCE_L3, WORD },
  { "saved l4", SOURCE_L4, WORD },
  { "saved l5", SOURCE_L5, WORD },
  { "saved l6", SOURCE_L6, WORD },
  { "saved l7", SOURCE_L7, WORD },
  { "saved i0", SOURCE_I0, WORD },
  { "saved i1", SOURCE_I1, WORD },
  { "saved i2", SOURCE_I2, WORD },
  { "saved i3", SOURCE_I3, WORD },
  { "saved i4", SOURCE_I4, WORD },
  { "saved i5", SOURCE_I5, WORD },
  { "saved i7", SOURCE_I7, WORD },
  { "frame g5", SOURCE_G5, WORD },
  { "saved g5", SOURCE_G5, WORD },
  { "frame icc", SOURCE_ICC, PSR_ICC_MASK },
  { "saved icc", SOURCE_ICC, PSR_ICC_MASK },
  { "frame sp", SOURCE_SP, WORD },
  { "saved sp", SOURCE_SP, WORD },
  { "frame o7", SOURCE_O7, WORD },
  { "saved o7", SOURCE_O7, WORD },
  { "frame fp", SOURCE_FP, WORD },
  { "saved fp", SOURCE_FP, WORD },
  { "frame g6", SOURCE_G6, WORD },
  { "frame g7", SOURCE_G7, WORD },
  { "frame pil", SOURCE_PIL, SPARC_PSR_PIL_MASK },
  { "heir g5", SOURCE_HEIR_G5, WORD },
  { "heir g7", SOURCE_HEIR_G7, WORD },
  { "heir sp", SOURCE_HEIR_SP, WORD },
  { "trap", SOURCE_TRAP, WORD },
};

static const uint64_t context_patterns[ SOURCE_MAX ] = {
  0x11121314,
  0x15161718,
  0x191a1b1c,
  0x1d1e1f20,
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
  0x00500000,
};

uint64_t context_initial[ SOURCE_MAX ];

Context_Control context_executing;

Context_Control context_heir;

static uint64_t context_heir_stack[ 128 ];

uint64_t context_fake_stack[ 16 ];

/* The true frame pointer, the true g5 and the true g7 of the runner */
uint64_t context_saved[ 2 ];

/* The stack pointer and the address of the call before the switch */
uint32_t context_expected[ 2 ];

/* The g5, the g7 and the stack pointer which the heir received */
uint32_t context_heir_seen[ 3 ];

static CPU_Exception_frame context_frame;

static volatile int context_trap;

static uint32_t context_g7;

static const Per_CPU_Control *context_cpu;

static uint32_t context_psr;

void ContextHeirEntry( void );

void ContextRunSwitch( void );

/*
 * The heir switches straight back.  It is assembly, so no prologue saves a
 * register of the context under test.
 */
__asm__( "  .globl ContextHeirEntry\n"
         "ContextHeirEntry:\n"
         "  set context_heir_seen, %o2\n"
         "  st %g5, [%o2 + 0]\n"
         "  st %g7, [%o2 + 4]\n"
         "  st %sp, [%o2 + 8]\n"
         "  set context_heir, %o0\n"
         "  set context_executing, %o1\n"
         "  call _CPU_Context_switch\n"
         "   nop\n"
         "  unimp 0\n" );

/*
 * The trap of the window flush makes every window of the runner invalid, so
 * the window flush of the switch writes through no frame pointer of the
 * test.  The runner then loads a value into the frame pointer like into any
 * other call-saved register.  It loads g1 first, because g1 holds the
 * address of the table.
 */
__asm__( "  .globl ContextRunSwitch\n"
         "ContextRunSwitch:\n"
         "  save %sp, -96, %sp\n"
         "  ta 3\n"
         "  set context_saved, %g1\n"
         "  std %i6, [%g1 + 0]\n"
         "  st %g5, [%g1 + 8]\n"
         "  st %g7, [%g1 + 12]\n"
         "  set context_expected, %g1\n"
         "  st %sp, [%g1 + 0]\n"
         "  set .Lcontext_call, %g2\n"
         "  st %g2, [%g1 + 4]\n"
         "  set context_initial, %g1\n"
         "  ld [%g1 + 132], %g2\n"
         "  set 0x00f00000, %g3\n"
         "  and %g2, %g3, %g2\n"
         "  rd %psr, %g4\n"
         "  andn %g4, %g3, %g4\n"
         "  or %g4, %g2, %g4\n"
         "  wr %g4, 0, %psr\n"
         "  nop\n"
         "  nop\n"
         "  nop\n"
         "  ld [%g1 + 4], %l0\n"
         "  ld [%g1 + 12], %l1\n"
         "  ld [%g1 + 20], %l2\n"
         "  ld [%g1 + 28], %l3\n"
         "  ld [%g1 + 36], %l4\n"
         "  ld [%g1 + 44], %l5\n"
         "  ld [%g1 + 52], %l6\n"
         "  ld [%g1 + 60], %l7\n"
         "  ld [%g1 + 68], %i0\n"
         "  ld [%g1 + 76], %i1\n"
         "  ld [%g1 + 84], %i2\n"
         "  ld [%g1 + 92], %i3\n"
         "  ld [%g1 + 100], %i4\n"
         "  ld [%g1 + 108], %i5\n"
         "  ld [%g1 + 116], %i7\n"
         "  ld [%g1 + 124], %g5\n"
         "  set context_fake_stack + 64, %i6\n"
         "  set context_executing, %o0\n"
         "  set context_heir, %o1\n"
         ".Lcontext_call:\n"
         "  call _CPU_Context_switch\n"
         "   nop\n"
         "  unimp 0\n"
         "  .globl context_done_label\n"
         "context_done_label:\n"
         "  set context_saved, %g1\n"
         "  ld [%g1 + 8], %g5\n"
         "  ld [%g1 + 12], %g7\n"
         "  ldd [%g1 + 0], %i6\n"
         "  ret\n"
         "   restore\n" );

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
  /* The handler runs on the processor which takes the trap */
  context_cpu = _Per_CPU_Get_snapshot();
  context_frame = *frame;
  ++context_trap;
  SetFatalHandler( NULL, NULL );
  frame->pc = (uint32_t) context_done_label;
  frame->npc = frame->pc + 4;
  _CPU_Exception_resume( frame );
}

static uint32_t GetSavedL0L1( size_t i )
{
  uint32_t l0_and_l1[ 2 ];

  memcpy( l0_and_l1, &context_executing.l0_and_l1, sizeof( l0_and_l1 ) );
  return l0_and_l1[ i ];
}

static void ContextRun( RegisterCheck *self, void *arg )
{
  const uint64_t *patterns;
  uint32_t        g7;

  (void) arg;
  context_trap = 0;
  context_cpu = NULL;
  memset( &context_frame, 0, sizeof( context_frame ) );
  memset( &context_executing, 0, sizeof( context_executing ) );
  memset( context_heir_seen, 0, sizeof( context_heir_seen ) );

  __asm__ volatile( "mov %%g7, %0"
                    : "=r"( g7 ) );
  context_g7 = g7;
  context_psr = _CPU_ISR_Get_level() << 8;

  _CPU_Context_Initialize(
    &context_heir,
    (uint32_t *) context_heir_stack,
    sizeof( context_heir_stack ),
    0,
    ContextHeirEntry,
    false,
    NULL
  );
  context_heir.g5 = CONTEXT_HEIR_G5;
  context_heir.g7 = CONTEXT_HEIR_G7;

  /* The switch restores g7 and never saves it */
  context_executing.g7 = g7;

  SetFatalHandler( ContextFatal, NULL );
  ContextRunSwitch();
  SetFatalHandler( NULL, NULL );
  patterns = context_patterns;

  RegisterCheckRecord(
    self,
    SLOT_FRAME_L0,
    context_frame.windows[ 0 ].local[ 0 ],
    patterns[ SOURCE_L0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_L1,
    context_frame.windows[ 0 ].local[ 1 ],
    patterns[ SOURCE_L1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_L2,
    context_frame.windows[ 0 ].local[ 2 ],
    patterns[ SOURCE_L2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_L3,
    context_frame.windows[ 0 ].local[ 3 ],
    patterns[ SOURCE_L3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_L4,
    context_frame.windows[ 0 ].local[ 4 ],
    patterns[ SOURCE_L4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_L5,
    context_frame.windows[ 0 ].local[ 5 ],
    patterns[ SOURCE_L5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_L6,
    context_frame.windows[ 0 ].local[ 6 ],
    patterns[ SOURCE_L6 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_L7,
    context_frame.windows[ 0 ].local[ 7 ],
    patterns[ SOURCE_L7 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_I0,
    context_frame.windows[ 0 ].input[ 0 ],
    patterns[ SOURCE_I0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_I1,
    context_frame.windows[ 0 ].input[ 1 ],
    patterns[ SOURCE_I1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_I2,
    context_frame.windows[ 0 ].input[ 2 ],
    patterns[ SOURCE_I2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_I3,
    context_frame.windows[ 0 ].input[ 3 ],
    patterns[ SOURCE_I3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_I4,
    context_frame.windows[ 0 ].input[ 4 ],
    patterns[ SOURCE_I4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_I5,
    context_frame.windows[ 0 ].input[ 5 ],
    patterns[ SOURCE_I5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_I7,
    context_frame.windows[ 0 ].input[ 7 ],
    patterns[ SOURCE_I7 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_L0,
    GetSavedL0L1( 0 ),
    patterns[ SOURCE_L0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_L1,
    GetSavedL0L1( 1 ),
    patterns[ SOURCE_L1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_L2,
    context_executing.l2,
    patterns[ SOURCE_L2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_L3,
    context_executing.l3,
    patterns[ SOURCE_L3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_L4,
    context_executing.l4,
    patterns[ SOURCE_L4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_L5,
    context_executing.l5,
    patterns[ SOURCE_L5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_L6,
    context_executing.l6,
    patterns[ SOURCE_L6 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_L7,
    context_executing.l7,
    patterns[ SOURCE_L7 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_I0,
    context_executing.i0,
    patterns[ SOURCE_I0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_I1,
    context_executing.i1,
    patterns[ SOURCE_I1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_I2,
    context_executing.i2,
    patterns[ SOURCE_I2 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_I3,
    context_executing.i3,
    patterns[ SOURCE_I3 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_I4,
    context_executing.i4,
    patterns[ SOURCE_I4 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_I5,
    context_executing.i5,
    patterns[ SOURCE_I5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_I7,
    context_executing.i7,
    patterns[ SOURCE_I7 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_G5,
    context_frame.global[ 5 ],
    patterns[ SOURCE_G5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_G5,
    context_executing.g5,
    patterns[ SOURCE_G5 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_ICC,
    context_frame.psr,
    patterns[ SOURCE_ICC ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_ICC,
    context_executing.psr,
    patterns[ SOURCE_ICC ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_SP,
    context_frame.output[ 6 ],
    context_expected[ 0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_SP,
    context_executing.o6_sp,
    context_expected[ 0 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_O7,
    context_frame.output[ 7 ],
    context_expected[ 1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_O7,
    context_executing.o7,
    context_expected[ 1 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_FP,
    context_frame.windows[ 0 ].input[ 6 ],
    (uintptr_t) &context_fake_stack[ 8 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_SAVED_FP,
    context_executing.i6_fp,
    (uintptr_t) &context_fake_stack[ 8 ]
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_G6,
    context_frame.global[ 6 ],
    (uintptr_t) context_cpu
  );
  RegisterCheckRecord(
    self,
    SLOT_FRAME_G7,
    context_frame.global[ 7 ],
    context_g7
  );
  RegisterCheckRecord( self, SLOT_FRAME_PIL, context_frame.psr, context_psr );
  RegisterCheckRecord(
    self,
    SLOT_HEIR_G5,
    context_heir_seen[ 0 ],
    CONTEXT_HEIR_G5
  );
  RegisterCheckRecord(
    self,
    SLOT_HEIR_G7,
    context_heir_seen[ 1 ],
    CONTEXT_HEIR_G7
  );
  RegisterCheckRecord(
    self,
    SLOT_HEIR_SP,
    context_heir_seen[ 2 ],
    context_heir.o6_sp
  );
  RegisterCheckRecord( self, SLOT_TRAP, (uint64_t) context_trap, 1 );
}

static RegisterCheck context_check = {
  .sources = context_sources,
  .source_count = SOURCE_MAX,
  .slots = context_slots,
  .slot_count = SLOT_MAX,
  .initial = context_initial,
  .run = ContextRun
};

static void CheckSlot( Slot slot )
{
  RegisterCheckVerify( &context_check, slot );
}

/*
 * The second action checks the FSR with tasks.  Task A sets its FSR.  An
 * interrupt dispatch to task D, which uses no floating-point unit, leaves no
 * owner of the floating-point unit in the lazy switch.  Task A then starts
 * task C, which records its initial FSR and sets its own FSR.  Task A records
 * its FSR, and task C records its FSR after a switch back.
 */
typedef enum {
  FP_SOURCE_A_FSR,
  FP_SOURCE_C_FSR,
  FP_SOURCE_INITIAL_FSR,
  FP_SOURCE_DISPATCH,
  FP_SOURCE_MAX
} FpSource;

typedef enum {
  FP_SLOT_A_FSR,
  FP_SLOT_C_FSR,
  FP_SLOT_INITIAL_FSR,
  FP_SLOT_DISPATCH,
  FP_SLOT_MAX
} FpSlot;

/* RD, NS, fcc, aexc and cexc of the FSR */
#define FSR_TEST_MASK 0xc0400fff

static const RegisterCheckSource fp_sources[ FP_SOURCE_MAX ] = {
  { "a fsr", REGISTER_CHECK_INITIAL, FSR_TEST_MASK },
  { "c fsr", REGISTER_CHECK_INITIAL, FSR_TEST_MASK },
  { "initial fsr", REGISTER_CHECK_EXPECTED, FSR_TEST_MASK },
  { "dispatch", REGISTER_CHECK_EXPECTED, WORD }
};

static const RegisterCheckSlot fp_slots[ FP_SLOT_MAX ] = {
  { "a fsr", FP_SOURCE_A_FSR, FSR_TEST_MASK },
  { "c fsr", FP_SOURCE_C_FSR, FSR_TEST_MASK },
  { "initial fsr", FP_SOURCE_INITIAL_FSR, FSR_TEST_MASK },
  { "dispatch", FP_SOURCE_DISPATCH, WORD }
};

static const uint64_t fp_patterns[ FP_SOURCE_MAX ] = {
  0x404008a5,
  0x80000652
};

static uint64_t fp_initial[ FP_SOURCE_MAX ];

typedef struct {
  rtems_id runner;
  rtems_id a;
  rtems_id c;
  rtems_id d;
  uint32_t a_fsr;
  uint32_t c_fsr;
  uint32_t initial_fsr;
  uint32_t dispatch;
} FpContext;

static FpContext fp_context;

static uint32_t GetFsr( void )
{
  uint32_t fsr;

  __asm__ volatile( "st %%fsr, %0"
                    : "=m"( fsr ) );
  return fsr;
}

static void SetFsr( uint32_t fsr )
{
  __asm__ volatile( "ld %0, %%fsr\nnop\nnop\nnop"
                    :
                    : "m"( fsr ) );
}

static rtems_id CreateFpTask( rtems_name name, rtems_task_priority priority )
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_task_create(
    name,
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &id
  );
  T_quiet_rsc_success( sc );

  return id;
}

static void FpTaskD( rtems_task_argument arg )
{
  FpContext *ctx;

  ctx = (FpContext *) arg;

  while ( true ) {
    SuspendSelf();
    ++ctx->dispatch;
  }
}

static void FpTaskC( rtems_task_argument arg )
{
  FpContext *ctx;

  ctx = (FpContext *) arg;
  ctx->initial_fsr = GetFsr();
  SetFsr( (uint32_t) fp_initial[ FP_SOURCE_C_FSR ] );
  SuspendSelf();
  ctx->c_fsr = GetFsr();
  SendEvents( ctx->runner, RTEMS_EVENT_0 );
  SuspendSelf();
}

static void ResumeTaskD( void *arg )
{
  FpContext *ctx;

  ctx = (FpContext *) arg;
  ResumeTask( ctx->d );
}

static void FpTaskA( rtems_task_argument arg )
{
  FpContext *ctx;

  ctx = (FpContext *) arg;
  SetFsr( (uint32_t) fp_initial[ FP_SOURCE_A_FSR ] );
  CallWithinISR( ResumeTaskD, ctx );
  StartTask( ctx->c, FpTaskC, ctx );
  ctx->a_fsr = GetFsr();
  ResumeTask( ctx->c );
  SuspendSelf();
}

static void FpRun( RegisterCheck *self, void *arg )
{
  FpContext *ctx;

  ctx = arg;
  SetSelfPriority( PRIO_NORMAL );
  ctx->runner = rtems_task_self();
  ctx->a_fsr = 0;
  ctx->c_fsr = 0;
  ctx->initial_fsr = 0;
  ctx->dispatch = 0;
  ctx->d = CreateTask( "FPD ", PRIO_HIGH );
  StartTask( ctx->d, FpTaskD, ctx );
  ctx->c = CreateFpTask( rtems_build_name( 'F', 'P', 'C', ' ' ), PRIO_NORMAL );
  ctx->a = CreateFpTask( rtems_build_name( 'F', 'P', 'A', ' ' ), PRIO_LOW );
  StartTask( ctx->a, FpTaskA, ctx );
  (void) ReceiveAnyEvents();
  DeleteTask( ctx->a );
  DeleteTask( ctx->c );
  DeleteTask( ctx->d );
  RestoreRunnerPriority();

  RegisterCheckRecord(
    self,
    FP_SLOT_A_FSR,
    ctx->a_fsr,
    fp_patterns[ FP_SOURCE_A_FSR ]
  );
  RegisterCheckRecord(
    self,
    FP_SLOT_C_FSR,
    ctx->c_fsr,
    fp_patterns[ FP_SOURCE_C_FSR ]
  );
  RegisterCheckRecord(
    self,
    FP_SLOT_INITIAL_FSR,
    ctx->initial_fsr,
    SPARC_FSR_INITIAL
  );
  RegisterCheckRecord( self, FP_SLOT_DISPATCH, ctx->dispatch, 1 );
}

static RegisterCheck fp_check = {
  .sources = fp_sources,
  .source_count = FP_SOURCE_MAX,
  .slots = fp_slots,
  .slot_count = FP_SLOT_MAX,
  .initial = fp_initial,
  .run = FpRun,
  .arg = &fp_context
};

#if SPARC_HAS_FPU == 1
/*
 * The third action checks the floating-point registers across a thread
 * dispatch of an interrupt.  Task A loads its floating-point registers and
 * its FSR with interrupts disabled.  It enables interrupts with a pending
 * interrupt.  The interrupt resumes task E, which loads other values into
 * every floating-point register and the FSR.  Task A stores its registers
 * after task E suspended itself.
 */
typedef enum {
  DISPATCH_SOURCE_F0_F1,
  DISPATCH_SOURCE_FSR = DISPATCH_SOURCE_F0_F1 + 16,
  DISPATCH_SOURCE_COUNT,
  DISPATCH_SOURCE_MAX
} DispatchSource;

typedef enum {
  DISPATCH_SLOT_F0_F1,
  DISPATCH_SLOT_FSR = DISPATCH_SLOT_F0_F1 + 16,
  DISPATCH_SLOT_COUNT,
  DISPATCH_SLOT_MAX
} DispatchSlot;

static const RegisterCheckSource dispatch_sources[ DISPATCH_SOURCE_MAX ] = {
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
  { "fsr", REGISTER_CHECK_INITIAL, FSR_TEST_MASK },
  { "count", REGISTER_CHECK_EXPECTED, WORD }
};

static const RegisterCheckSlot dispatch_slots[ DISPATCH_SLOT_MAX ] = {
  { "f0_f1", DISPATCH_SOURCE_F0_F1 + 0, UINT64_MAX },
  { "f2_f3", DISPATCH_SOURCE_F0_F1 + 1, UINT64_MAX },
  { "f4_f5", DISPATCH_SOURCE_F0_F1 + 2, UINT64_MAX },
  { "f6_f7", DISPATCH_SOURCE_F0_F1 + 3, UINT64_MAX },
  { "f8_f9", DISPATCH_SOURCE_F0_F1 + 4, UINT64_MAX },
  { "f10_f11", DISPATCH_SOURCE_F0_F1 + 5, UINT64_MAX },
  { "f12_f13", DISPATCH_SOURCE_F0_F1 + 6, UINT64_MAX },
  { "f14_f15", DISPATCH_SOURCE_F0_F1 + 7, UINT64_MAX },
  { "f16_f17", DISPATCH_SOURCE_F0_F1 + 8, UINT64_MAX },
  { "f18_f19", DISPATCH_SOURCE_F0_F1 + 9, UINT64_MAX },
  { "f20_f21", DISPATCH_SOURCE_F0_F1 + 10, UINT64_MAX },
  { "f22_f23", DISPATCH_SOURCE_F0_F1 + 11, UINT64_MAX },
  { "f24_f25", DISPATCH_SOURCE_F0_F1 + 12, UINT64_MAX },
  { "f26_f27", DISPATCH_SOURCE_F0_F1 + 13, UINT64_MAX },
  { "f28_f29", DISPATCH_SOURCE_F0_F1 + 14, UINT64_MAX },
  { "f30_f31", DISPATCH_SOURCE_F0_F1 + 15, UINT64_MAX },
  { "fsr", DISPATCH_SOURCE_FSR, FSR_TEST_MASK },
  { "count", DISPATCH_SOURCE_COUNT, WORD }
};

static const uint64_t dispatch_patterns[ DISPATCH_SOURCE_MAX ] = {
  0xc0c1c2c3c4c5c6c7,
  0xc8c9cacbcccdcecf,
  0xd0d1d2d3d4d5d6d7,
  0xd8d9dadbdcdddedf,
  0xe0e1e2e3e4e5e6e7,
  0xe8e9eaebecedeeef,
  0xf0f1f2f3f4f5f6f7,
  0xf8f9fafbfcfdfeff,
  0x0001020304050607,
  0x08090a0b0c0d0e0f,
  0x1011121314151617,
  0x18191a1b1c1d1e1f,
  0x2021222324252627,
  0x28292a2b2c2d2e2f,
  0x3031323334353637,
  0x38393a3b3c3d3e3f,
  0x404008a5
};

static uint64_t dispatch_initial[ DISPATCH_SOURCE_MAX ];

typedef struct {
  rtems_id             runner;
  rtems_id             a;
  rtems_id             e;
  volatile uint32_t    count;
  uint32_t             fsr;
  uint64_t             fp[ 16 ];
  CallWithinISRRequest request;
} DispatchContext;

static DispatchContext dispatch_context;

static void DispatchTaskE( rtems_task_argument arg )
{
  DispatchContext *ctx;
  uint64_t         clobber[ 16 ];
  uint32_t         fsr;
  size_t           i;

  ctx = (DispatchContext *) arg;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( clobber ); ++i ) {
    clobber[ i ] = ~dispatch_patterns[ i ];
  }

  fsr = (uint32_t) ~dispatch_patterns[ DISPATCH_SOURCE_FSR ];

  while ( true ) {
    __asm__ volatile( "ldd [%[initial] + 0], %%f0\n"
                      "ldd [%[initial] + 8], %%f2\n"
                      "ldd [%[initial] + 16], %%f4\n"
                      "ldd [%[initial] + 24], %%f6\n"
                      "ldd [%[initial] + 32], %%f8\n"
                      "ldd [%[initial] + 40], %%f10\n"
                      "ldd [%[initial] + 48], %%f12\n"
                      "ldd [%[initial] + 56], %%f14\n"
                      "ldd [%[initial] + 64], %%f16\n"
                      "ldd [%[initial] + 72], %%f18\n"
                      "ldd [%[initial] + 80], %%f20\n"
                      "ldd [%[initial] + 88], %%f22\n"
                      "ldd [%[initial] + 96], %%f24\n"
                      "ldd [%[initial] + 104], %%f26\n"
                      "ldd [%[initial] + 112], %%f28\n"
                      "ldd [%[initial] + 120], %%f30\n"
                      "ld %[fsr], %%fsr\n"
                      "nop\n"
                      "nop\n"
                      "nop\n"
                      :
                      : [initial] "r"( clobber ), [fsr] "m"( fsr )
                      : "memory",
                        "f0",
                        "f1",
                        "f2",
                        "f3",
                        "f4",
                        "f5",
                        "f6",
                        "f7",
                        "f8",
                        "f9",
                        "f10",
                        "f11",
                        "f12",
                        "f13",
                        "f14",
                        "f15",
                        "f16",
                        "f17",
                        "f18",
                        "f19",
                        "f20",
                        "f21",
                        "f22",
                        "f23",
                        "f24",
                        "f25",
                        "f26",
                        "f27",
                        "f28",
                        "f29",
                        "f30",
                        "f31" );
    ++ctx->count;
    SuspendSelf();
  }
}

static void ResumeTaskE( void *arg )
{
  DispatchContext *ctx;

  ctx = (DispatchContext *) arg;
  ResumeTask( ctx->e );
}

static void DispatchTaskA( rtems_task_argument arg )
{
  DispatchContext      *ctx;
  rtems_interrupt_level level;
  uint32_t              fsr;
  uint32_t              spins;

  ctx = (DispatchContext *) arg;
  fsr = (uint32_t) dispatch_initial[ DISPATCH_SOURCE_FSR ];
  spins = 10000000;
  ctx->request.handler = ResumeTaskE;
  ctx->request.arg = ctx;
  rtems_interrupt_local_disable( level );
  CallWithinISRSubmit( &ctx->request );
  __asm__ volatile( "ldd [%[initial] + 0], %%f0\n"
                    "ldd [%[initial] + 8], %%f2\n"
                    "ldd [%[initial] + 16], %%f4\n"
                    "ldd [%[initial] + 24], %%f6\n"
                    "ldd [%[initial] + 32], %%f8\n"
                    "ldd [%[initial] + 40], %%f10\n"
                    "ldd [%[initial] + 48], %%f12\n"
                    "ldd [%[initial] + 56], %%f14\n"
                    "ldd [%[initial] + 64], %%f16\n"
                    "ldd [%[initial] + 72], %%f18\n"
                    "ldd [%[initial] + 80], %%f20\n"
                    "ldd [%[initial] + 88], %%f22\n"
                    "ldd [%[initial] + 96], %%f24\n"
                    "ldd [%[initial] + 104], %%f26\n"
                    "ldd [%[initial] + 112], %%f28\n"
                    "ldd [%[initial] + 120], %%f30\n"
                    "ld %[fsr], %%fsr\n"
                    "nop\n"
                    "nop\n"
                    "nop\n"
                    "mov %[level], %%g1\n"
                    "ta 10\n"
                    "1:\n"
                    "ld [%[count]], %%g2\n"
                    "cmp %%g2, 0\n"
                    "bne 2f\n"
                    "nop\n"
                    "subcc %[spins], 1, %[spins]\n"
                    "bne 1b\n"
                    "nop\n"
                    "2:\n"
                    "std %%f0, [%[actual] + 0]\n"
                    "std %%f2, [%[actual] + 8]\n"
                    "std %%f4, [%[actual] + 16]\n"
                    "std %%f6, [%[actual] + 24]\n"
                    "std %%f8, [%[actual] + 32]\n"
                    "std %%f10, [%[actual] + 40]\n"
                    "std %%f12, [%[actual] + 48]\n"
                    "std %%f14, [%[actual] + 56]\n"
                    "std %%f16, [%[actual] + 64]\n"
                    "std %%f18, [%[actual] + 72]\n"
                    "std %%f20, [%[actual] + 80]\n"
                    "std %%f22, [%[actual] + 88]\n"
                    "std %%f24, [%[actual] + 96]\n"
                    "std %%f26, [%[actual] + 104]\n"
                    "std %%f28, [%[actual] + 112]\n"
                    "std %%f30, [%[actual] + 120]\n"
                    "st %%fsr, [%[actual_fsr]]\n"
                    : [spins] "+r"( spins )
                    : [initial] "r"( dispatch_initial ),
                      [fsr] "m"( fsr ),
                      [level] "r"( level ),
                      [count] "r"( &ctx->count ),
                      [actual] "r"( ctx->fp ),
                      [actual_fsr] "r"( &ctx->fsr )
                    : "memory",
                      "cc",
                      "g1",
                      "g2",
                      "f0",
                      "f1",
                      "f2",
                      "f3",
                      "f4",
                      "f5",
                      "f6",
                      "f7",
                      "f8",
                      "f9",
                      "f10",
                      "f11",
                      "f12",
                      "f13",
                      "f14",
                      "f15",
                      "f16",
                      "f17",
                      "f18",
                      "f19",
                      "f20",
                      "f21",
                      "f22",
                      "f23",
                      "f24",
                      "f25",
                      "f26",
                      "f27",
                      "f28",
                      "f29",
                      "f30",
                      "f31" );
  SendEvents( ctx->runner, RTEMS_EVENT_0 );
  SuspendSelf();
}

static void DispatchRun( RegisterCheck *self, void *arg )
{
  DispatchContext *ctx;
  size_t           i;

  ctx = arg;
  memset( ctx->fp, 0, sizeof( ctx->fp ) );
  ctx->fsr = 0;
  SetSelfPriority( PRIO_NORMAL );
  ctx->runner = rtems_task_self();
  ctx->e = CreateFpTask( rtems_build_name( 'F', 'P', 'E', ' ' ), PRIO_HIGH );
  ctx->a = CreateFpTask( rtems_build_name( 'F', 'P', 'A', ' ' ), PRIO_LOW );
#if defined( RTEMS_SMP )
  SetAffinityOne( ctx->e, rtems_scheduler_get_processor() );
  SetAffinityOne( ctx->a, rtems_scheduler_get_processor() );
#endif
  StartTask( ctx->e, DispatchTaskE, ctx );
  ctx->count = 0;
  StartTask( ctx->a, DispatchTaskA, ctx );
  (void) ReceiveAnyEvents();
  DeleteTask( ctx->a );
  DeleteTask( ctx->e );
  RestoreRunnerPriority();

  for ( i = 0; i < 16; ++i ) {
    RegisterCheckRecord(
      self,
      DISPATCH_SLOT_F0_F1 + i,
      ctx->fp[ i ],
      dispatch_patterns[ DISPATCH_SOURCE_F0_F1 + i ]
    );
  }

  RegisterCheckRecord(
    self,
    DISPATCH_SLOT_FSR,
    ctx->fsr,
    dispatch_patterns[ DISPATCH_SOURCE_FSR ]
  );
  RegisterCheckRecord( self, DISPATCH_SLOT_COUNT, ctx->count, 1 );
}

static RegisterCheck dispatch_check = {
  .sources = dispatch_sources,
  .source_count = DISPATCH_SOURCE_MAX,
  .slots = dispatch_slots,
  .slot_count = DISPATCH_SLOT_MAX,
  .initial = dispatch_initial,
  .run = DispatchRun,
  .arg = &dispatch_context
};
#endif

#if defined( SPARC_USE_LAZY_FP_SWITCH )
/*
 * The fourth action executes a floating-point instruction in a task without
 * the floating-point attribute.  The fatal handler jumps back to the task.
 */
typedef enum {
  ILLEGAL_SOURCE_SOURCE,
  ILLEGAL_SOURCE_CODE,
  ILLEGAL_SOURCE_MAX
} IllegalSource;

typedef enum {
  ILLEGAL_SLOT_SOURCE,
  ILLEGAL_SLOT_CODE,
  ILLEGAL_SLOT_MAX
} IllegalSlot;

static const RegisterCheckSource illegal_sources[ ILLEGAL_SOURCE_MAX ] = {
  { "source", REGISTER_CHECK_EXPECTED, WORD },
  { "code", REGISTER_CHECK_EXPECTED, WORD }
};

static const RegisterCheckSlot illegal_slots[ ILLEGAL_SLOT_MAX ] = {
  { "source", ILLEGAL_SOURCE_SOURCE, WORD },
  { "code", ILLEGAL_SOURCE_CODE, WORD }
};

static uint64_t illegal_initial[ ILLEGAL_SOURCE_MAX ];

typedef struct {
  rtems_id           runner;
  jmp_buf            before_fatal;
  rtems_fatal_source source;
  rtems_fatal_code   code;
} IllegalContext;

static IllegalContext illegal_context;

static void IllegalFatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  IllegalContext *ctx;

  ctx = arg;
  SetFatalHandler( NULL, NULL );
  ctx->source = source;
  ctx->code = code;
  longjmp( ctx->before_fatal, 1 );
}

static void IllegalTask( rtems_task_argument arg )
{
  IllegalContext       *ctx;
  rtems_interrupt_level level;

  ctx = (IllegalContext *) arg;
  rtems_interrupt_local_disable( level );
  rtems_interrupt_local_enable( level );
  SetFatalHandler( IllegalFatal, ctx );

  if ( setjmp( ctx->before_fatal ) == 0 ) {
    __asm__ volatile( "fmovs %%f0, %%f0"
                      :
                      :
                      : "f0" );
  }

  /* The termination disabled interrupts */
  rtems_interrupt_local_enable( level );
  SendEvents( ctx->runner, RTEMS_EVENT_0 );
  SuspendSelf();
}

static void IllegalRun( RegisterCheck *self, void *arg )
{
  IllegalContext *ctx;
  rtems_id        id;

  ctx = arg;
  ctx->source = 0;
  ctx->code = 0;
  ctx->runner = rtems_task_self();
  SetSelfPriority( PRIO_NORMAL );
  id = CreateTask( "ILLE", PRIO_HIGH );
  StartTask( id, IllegalTask, ctx );
  (void) ReceiveAnyEvents();
  DeleteTask( id );
  RestoreRunnerPriority();
  RegisterCheckRecord(
    self,
    ILLEGAL_SLOT_SOURCE,
    ctx->source,
    INTERNAL_ERROR_CORE
  );
  RegisterCheckRecord(
    self,
    ILLEGAL_SLOT_CODE,
    ctx->code,
    INTERNAL_ERROR_ILLEGAL_USE_OF_FLOATING_POINT_UNIT
  );
}

static RegisterCheck illegal_check = {
  .sources = illegal_sources,
  .source_count = ILLEGAL_SOURCE_MAX,
  .slots = illegal_slots,
  .slot_count = ILLEGAL_SLOT_MAX,
  .initial = illegal_initial,
  .run = IllegalRun,
  .arg = &illegal_context
};
#endif

#if defined( SPARC_USE_LAZY_FP_SWITCH )
/*
 * The fifth action restarts a task for which the lazy floating-point switch
 * holds a saved floating-point context.  Task R sets its FSR.  An interrupt
 * dispatches to task S, whose use of the floating-point unit saves the
 * context of task R.  An interrupt dispatches from task S to task U, so task
 * S owns the floating-point unit.  Task U suspends task S and restarts task
 * R.  The owner clears PSR[EF] of task R, so the restarted task R takes the
 * floating-point disabled trap at its first use of the floating-point unit.
 */
typedef enum {
  RESTART_SOURCE_R_FSR,
  RESTART_SOURCE_OWNER,
  RESTART_SOURCE_INITIAL_FSR,
  RESTART_SOURCE_MAX
} RestartSource;

typedef enum {
  RESTART_SLOT_SAVED_FSR,
  RESTART_SLOT_OWNER,
  RESTART_SLOT_INITIAL_FSR,
  RESTART_SLOT_MAX
} RestartSlot;

/* The FSR of task S differs from the FSR of task R and the initial FSR */
#define RESTART_S_FSR 0x80000652

static const RegisterCheckSource restart_sources[ RESTART_SOURCE_MAX ] = {
  { "r fsr", REGISTER_CHECK_INITIAL, FSR_TEST_MASK },
  { "owner", REGISTER_CHECK_EXPECTED, WORD },
  { "initial fsr", REGISTER_CHECK_EXPECTED, FSR_TEST_MASK }
};

static const RegisterCheckSlot restart_slots[ RESTART_SLOT_MAX ] = {
  { "saved fsr", RESTART_SOURCE_R_FSR, FSR_TEST_MASK },
  { "owner", RESTART_SOURCE_OWNER, WORD },
  { "initial fsr", RESTART_SOURCE_INITIAL_FSR, FSR_TEST_MASK }
};

static const uint64_t restart_patterns[ RESTART_SOURCE_MAX ] = { 0x404008a5 };

static uint64_t restart_initial[ RESTART_SOURCE_MAX ];

typedef struct {
  rtems_id runner;
  rtems_id r;
  rtems_id s;
  rtems_id u;
  bool     restarted;
  uint32_t saved_fsr;
  uint32_t owner;
  uint32_t restart_fsr;
} RestartContext;

static RestartContext restart_context;

static void RestartSendEvent( void *arg )
{
  SendEvents( *(rtems_id *) arg, RTEMS_EVENT_0 );
}

static void RestartTaskR( rtems_task_argument arg )
{
  RestartContext *ctx;

  ctx = (RestartContext *) arg;

  if ( ctx->restarted ) {
    ctx->restart_fsr = GetFsr();
    SendEvents( ctx->runner, RTEMS_EVENT_0 );
    SuspendSelf();
  }

  SetFsr( (uint32_t) restart_initial[ RESTART_SOURCE_R_FSR ] );
  CallWithinISR( RestartSendEvent, &ctx->s );
  SuspendSelf();
}

static void RestartTaskS( rtems_task_argument arg )
{
  RestartContext *ctx;

  ctx = (RestartContext *) arg;
  (void) ReceiveAnyEvents();
  SetFsr( RESTART_S_FSR );
  CallWithinISR( RestartSendEvent, &ctx->u );
  SuspendSelf();
}

static void RestartTaskU( rtems_task_argument arg )
{
  RestartContext           *ctx;
  const Context_Control_fp *fp;
  const Thread_Control     *owner;

  ctx = (RestartContext *) arg;
  (void) ReceiveAnyEvents();
  fp = GetThread( ctx->r )->Registers.fp_context;
  ctx->saved_fsr = fp != NULL ? fp->fsr : 0;
  owner = _Per_CPU_Get_snapshot()->cpu_per_cpu.fp_owner;
  ctx->owner = owner == GetThread( ctx->s );
  SuspendTask( ctx->s );
  ctx->restarted = true;
  RestartTask( ctx->r, ctx );
  SuspendSelf();
}

static void RestartRun( RegisterCheck *self, void *arg )
{
  RestartContext *ctx;

  ctx = arg;
  ctx->runner = rtems_task_self();
  ctx->restarted = false;
  ctx->saved_fsr = 0;
  ctx->owner = 0;
  ctx->restart_fsr = 0;
  SetSelfPriority( PRIO_NORMAL );
  ctx->u = CreateTask( "RSTU", PRIO_ULTRA_HIGH );
  StartTask( ctx->u, RestartTaskU, ctx );
  ctx->s = CreateFpTask(
    rtems_build_name( 'R', 'S', 'T', 'S' ),
    PRIO_VERY_HIGH
  );
  StartTask( ctx->s, RestartTaskS, ctx );
  ctx->r = CreateFpTask( rtems_build_name( 'R', 'S', 'T', 'R' ), PRIO_HIGH );
  StartTask( ctx->r, RestartTaskR, ctx );
  (void) ReceiveAnyEvents();
  DeleteTask( ctx->r );
  DeleteTask( ctx->s );
  DeleteTask( ctx->u );
  RestoreRunnerPriority();
  RegisterCheckRecord(
    self,
    RESTART_SLOT_SAVED_FSR,
    ctx->saved_fsr,
    restart_patterns[ RESTART_SOURCE_R_FSR ]
  );
  RegisterCheckRecord( self, RESTART_SLOT_OWNER, ctx->owner, 1 );
  RegisterCheckRecord(
    self,
    RESTART_SLOT_INITIAL_FSR,
    ctx->restart_fsr,
    SPARC_FSR_INITIAL
  );
}

static RegisterCheck restart_check = {
  .sources = restart_sources,
  .source_count = RESTART_SOURCE_MAX,
  .slots = restart_slots,
  .slot_count = RESTART_SLOT_MAX,
  .initial = restart_initial,
  .run = RestartRun,
  .arg = &restart_context
};
#endif

/**
 * @brief Flush the register windows. Load a distinct pattern into every
 *   call-saved register. Call the context switch directly. The heir switches
 *   back. Trap, so the frame reports every register. Run this once for each
 *   register with the pattern of that register inverted, then once unchanged.
 */
static void ScoreCpuSparcValContext_Action_0( void )
{
  memcpy( context_initial, context_patterns, sizeof( context_initial ) );
  RegisterCheckRun( &context_check );

  /*
   * Check that the switch restored the SPARC local register L0.
   */
  CheckSlot( SLOT_FRAME_L0 );

  /*
   * Check that the switch restored the SPARC local register L1.
   */
  CheckSlot( SLOT_FRAME_L1 );

  /*
   * Check that the switch restored the SPARC local register L2.
   */
  CheckSlot( SLOT_FRAME_L2 );

  /*
   * Check that the switch restored the SPARC local register L3.
   */
  CheckSlot( SLOT_FRAME_L3 );

  /*
   * Check that the switch restored the SPARC local register L4.
   */
  CheckSlot( SLOT_FRAME_L4 );

  /*
   * Check that the switch restored the SPARC local register L5.
   */
  CheckSlot( SLOT_FRAME_L5 );

  /*
   * Check that the switch restored the SPARC local register L6.
   */
  CheckSlot( SLOT_FRAME_L6 );

  /*
   * Check that the switch restored the SPARC local register L7.
   */
  CheckSlot( SLOT_FRAME_L7 );

  /*
   * Check that the switch restored the SPARC input register I0.
   */
  CheckSlot( SLOT_FRAME_I0 );

  /*
   * Check that the switch restored the SPARC input register I1.
   */
  CheckSlot( SLOT_FRAME_I1 );

  /*
   * Check that the switch restored the SPARC input register I2.
   */
  CheckSlot( SLOT_FRAME_I2 );

  /*
   * Check that the switch restored the SPARC input register I3.
   */
  CheckSlot( SLOT_FRAME_I3 );

  /*
   * Check that the switch restored the SPARC input register I4.
   */
  CheckSlot( SLOT_FRAME_I4 );

  /*
   * Check that the switch restored the SPARC input register I5.
   */
  CheckSlot( SLOT_FRAME_I5 );

  /*
   * Check that the switch restored the SPARC input register I7.
   */
  CheckSlot( SLOT_FRAME_I7 );

  /*
   * Check that the switch saved the SPARC local register L0.
   */
  CheckSlot( SLOT_SAVED_L0 );

  /*
   * Check that the switch saved the SPARC local register L1.
   */
  CheckSlot( SLOT_SAVED_L1 );

  /*
   * Check that the switch saved the SPARC local register L2.
   */
  CheckSlot( SLOT_SAVED_L2 );

  /*
   * Check that the switch saved the SPARC local register L3.
   */
  CheckSlot( SLOT_SAVED_L3 );

  /*
   * Check that the switch saved the SPARC local register L4.
   */
  CheckSlot( SLOT_SAVED_L4 );

  /*
   * Check that the switch saved the SPARC local register L5.
   */
  CheckSlot( SLOT_SAVED_L5 );

  /*
   * Check that the switch saved the SPARC local register L6.
   */
  CheckSlot( SLOT_SAVED_L6 );

  /*
   * Check that the switch saved the SPARC local register L7.
   */
  CheckSlot( SLOT_SAVED_L7 );

  /*
   * Check that the switch saved the SPARC input register I0.
   */
  CheckSlot( SLOT_SAVED_I0 );

  /*
   * Check that the switch saved the SPARC input register I1.
   */
  CheckSlot( SLOT_SAVED_I1 );

  /*
   * Check that the switch saved the SPARC input register I2.
   */
  CheckSlot( SLOT_SAVED_I2 );

  /*
   * Check that the switch saved the SPARC input register I3.
   */
  CheckSlot( SLOT_SAVED_I3 );

  /*
   * Check that the switch saved the SPARC input register I4.
   */
  CheckSlot( SLOT_SAVED_I4 );

  /*
   * Check that the switch saved the SPARC input register I5.
   */
  CheckSlot( SLOT_SAVED_I5 );

  /*
   * Check that the switch saved the SPARC input register I7.
   */
  CheckSlot( SLOT_SAVED_I7 );

  /*
   * Check that the switch restored the SPARC global register G5.
   */
  CheckSlot( SLOT_FRAME_G5 );

  /*
   * Check that the switch saved the SPARC global register G5.
   */
  CheckSlot( SLOT_SAVED_G5 );

  /*
   * Check that the switch restored the integer condition codes.
   */
  CheckSlot( SLOT_FRAME_ICC );

  /*
   * Check that the switch saved the integer condition codes.
   */
  CheckSlot( SLOT_SAVED_ICC );

  /*
   * Check that the switch restored the stack pointer, which is O6.
   */
  CheckSlot( SLOT_FRAME_SP );

  /*
   * Check that the switch saved the stack pointer, which is O6.
   */
  CheckSlot( SLOT_SAVED_SP );

  /*
   * Check that the switch restored the address of the call, which is O7.
   */
  CheckSlot( SLOT_FRAME_O7 );

  /*
   * Check that the switch saved the address of the call, which is O7.
   */
  CheckSlot( SLOT_SAVED_O7 );

  /*
   * Check that the switch restored the frame pointer, which is I6.
   */
  CheckSlot( SLOT_FRAME_FP );

  /*
   * Check that the switch saved the frame pointer, which is I6.
   */
  CheckSlot( SLOT_SAVED_FP );

  /*
   * Check that the switch left the per-CPU control G6 unchanged.
   */
  CheckSlot( SLOT_FRAME_G6 );

  /*
   * Check that the switch restored the thread pointer G7 of the executing
   * context.
   */
  CheckSlot( SLOT_FRAME_G7 );

  /*
   * Check that the switch restored the processor interrupt level.
   */
  CheckSlot( SLOT_FRAME_PIL );

  /*
   * Check that the heir received the SPARC global register G5 of its context.
   */
  CheckSlot( SLOT_HEIR_G5 );

  /*
   * Check that the heir received the thread pointer G7 of its context.
   */
  CheckSlot( SLOT_HEIR_G7 );

  /*
   * Check that the heir received the stack pointer of its context.
   */
  CheckSlot( SLOT_HEIR_SP );

  /*
   * Check that the trap happened and that each run with a changed value
   * flagged exactly the checks of that value, and that each run recorded every
   * check.
   */
  RegisterCheckReport( &context_check );
}

/**
 * @brief Let task A set its FSR. Dispatch to task D through an interrupt. Let
 *   task A start task C, which records its initial FSR and sets its FSR.
 *   Record the FSR of task A and of task C after the switches. Run this once
 *   for each FSR with the pattern of that FSR inverted, then once unchanged.
 */
static void ScoreCpuSparcValContext_Action_1( void )
{
  #if SPARC_HAS_FPU == 1
  memcpy( fp_initial, fp_patterns, sizeof( fp_initial ) );
  RegisterCheckRun( &fp_check );
  #endif

  /*
   * Check that the switches preserved the FSR of task A.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &fp_check, FP_SLOT_A_FSR );
  #endif

  /*
   * Check that the switches preserved the FSR of task C.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &fp_check, FP_SLOT_C_FSR );
  #endif

  /*
   * Check that task C started with the initial FSR.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &fp_check, FP_SLOT_INITIAL_FSR );
  #endif

  /*
   * Check that the interrupt dispatched to task D once.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &fp_check, FP_SLOT_DISPATCH );
  #endif

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckReport( &fp_check );
  #endif
}

/**
 * @brief Let task A load a pattern into each floating-point register and the
 *   FSR with interrupts disabled. Raise an interrupt which resumes task E of
 *   higher priority. Enable interrupts in task A. Task E loads other values
 *   into each floating-point register and the FSR, counts its run, and
 *   suspends itself. Task A waits for the count and stores its registers. Run
 *   this once for each register with the pattern of that register inverted,
 *   then once unchanged.
 */
static void ScoreCpuSparcValContext_Action_2( void )
{
  #if SPARC_HAS_FPU == 1
  memcpy( dispatch_initial, dispatch_patterns, sizeof( dispatch_initial ) );
  RegisterCheckRun( &dispatch_check );
  #endif

  /*
   * Check that the thread dispatch of the interrupt preserved each
   * floating-point register and the FSR of task A.
   */
  #if SPARC_HAS_FPU == 1
  for ( size_t i = 0; i < DISPATCH_SLOT_COUNT; ++i ) {
    RegisterCheckVerify( &dispatch_check, i );
  }
  #endif

  /*
   * Check that task E ran once.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckVerify( &dispatch_check, DISPATCH_SLOT_COUNT );
  #endif

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  #if SPARC_HAS_FPU == 1
  RegisterCheckReport( &dispatch_check );
  #endif
}

/**
 * @brief Let a task without the floating-point attribute execute a
 *   floating-point instruction. Record the fatal source and the fatal code.
 *   Jump back to the task. Run this once for each value with the expected
 *   value inverted, then once unchanged.
 */
static void ScoreCpuSparcValContext_Action_3( void )
{
  #if defined( SPARC_USE_LAZY_FP_SWITCH )
  RegisterCheckRun( &illegal_check );
  #endif

  /*
   * Check that the floating-point disabled trap handler terminated the system
   * with the fatal source INTERNAL_ERROR_CORE.
   */
  #if defined( SPARC_USE_LAZY_FP_SWITCH )
  RegisterCheckVerify( &illegal_check, ILLEGAL_SLOT_SOURCE );
  #endif

  /*
   * Check that the floating-point disabled trap handler terminated the system
   * with the fatal code INTERNAL_ERROR_ILLEGAL_USE_OF_FLOATING_POINT_UNIT.
   */
  #if defined( SPARC_USE_LAZY_FP_SWITCH )
  RegisterCheckVerify( &illegal_check, ILLEGAL_SLOT_CODE );
  #endif

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  #if defined( SPARC_USE_LAZY_FP_SWITCH )
  RegisterCheckReport( &illegal_check );
  #endif
}

/**
 * @brief Let task R set its FSR. Dispatch to task S through an interrupt. Let
 *   task S use the floating-point unit, so the floating-point disabled trap
 *   handler saves the floating-point context of task R. Dispatch to task U
 *   through an interrupt, so task S owns the floating-point unit. Let task U
 *   suspend task S and restart task R. Let the restarted task R record its
 *   FSR. Run this once for each value with that value inverted, then once
 *   unchanged.
 */
static void ScoreCpuSparcValContext_Action_4( void )
{
  #if defined( SPARC_USE_LAZY_FP_SWITCH )
  memcpy( restart_initial, restart_patterns, sizeof( restart_initial ) );
  RegisterCheckRun( &restart_check );
  #endif

  /*
   * Check that the floating-point disabled trap handler saved the FSR of task
   * R before the restart.
   */
  #if defined( SPARC_USE_LAZY_FP_SWITCH )
  RegisterCheckVerify( &restart_check, RESTART_SLOT_SAVED_FSR );
  #endif

  /*
   * Check that task S owned the floating-point unit at the restart.
   */
  #if defined( SPARC_USE_LAZY_FP_SWITCH )
  RegisterCheckVerify( &restart_check, RESTART_SLOT_OWNER );
  #endif

  /*
   * Check that the restarted task R started with the initial FSR.
   */
  #if defined( SPARC_USE_LAZY_FP_SWITCH )
  RegisterCheckVerify( &restart_check, RESTART_SLOT_INITIAL_FSR );
  #endif

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  #if defined( SPARC_USE_LAZY_FP_SWITCH )
  RegisterCheckReport( &restart_check );
  #endif
}

/**
 * @fn void T_case_body_ScoreCpuSparcValContext( void )
 */
T_TEST_CASE( ScoreCpuSparcValContext )
{
  ScoreCpuSparcValContext_Action_0();
  ScoreCpuSparcValContext_Action_1();
  ScoreCpuSparcValContext_Action_2();
  ScoreCpuSparcValContext_Action_3();
  ScoreCpuSparcValContext_Action_4();
}

/** @} */
