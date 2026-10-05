/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuSparcValWindows
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
#include <rtems/score/cpuimpl.h>

#include "tx-register-check.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuSparcValWindows spec:/score/cpu/sparc/val/windows
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the register windows.
 *
 * Each level of a chain of nested register windows loads a pattern into its
 * registers. An event in the innermost level writes the register windows to
 * the stack, or uses more register windows itself. Each level stores its
 * registers before its restore.
 *
 * This test case performs the following actions:
 *
 * - Enter a chain of 8 nested register windows. Load a pattern into each local
 *   register and each input register except I6 and I7 of each level. The chain
 *   uses more register windows than the processor provides. Store the
 *   registers of each level before its restore. Run this through the register
 *   check, once for each value inverted, then once unchanged.
 *
 *   - Check that each level of the chain has the values which it loaded before
 *     its restore.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Enter a chain of 8 nested register windows. Load a pattern into each local
 *   register and each input register except I6 and I7 of each level. Load a
 *   pattern into each global register, each output register except O6, Y, and
 *   the integer condition codes. Execute the window flush trap in the
 *   innermost level. Store the global and the output registers, Y, and the
 *   PSR. Copy the save area of each other level. Store the registers of each
 *   level before its restore. Run this through the register check, once for
 *   each value inverted, then once unchanged.
 *
 *   - Check that each level of the chain has the values which it loaded before
 *     its restore.
 *
 *   - Check that the window flush trap wrote the registers of each level
 *     except the innermost level to the save area of the level.
 *
 *   - Check that the window flush trap preserved the global and the output
 *     registers, Y, and the integer condition codes.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Enter a chain of 8 nested register windows. Load a pattern into each local
 *   register and each input register except I6 and I7 of each level. Resume a
 *   worker task of higher priority on the current processor in the innermost
 *   level. The worker uses more register windows than the processor provides
 *   and suspends itself. Store the registers of each level before its restore.
 *   Run this through the register check, once for each value inverted, then
 *   once unchanged.
 *
 *   - Check that each level of the chain has the values which it loaded before
 *     its restore.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Enter a chain of 8 nested register windows. Load a pattern into each local
 *   register and each input register except I6 and I7 of each level. Disable
 *   interrupts, raise an interrupt, and enable interrupts in the innermost
 *   level. The interrupt handler uses more register windows than the processor
 *   provides and counts its call. Wait for the count. Store the registers of
 *   each level before its restore. Run this through the register check, once
 *   for each value inverted, then once unchanged.
 *
 *   - Check that each level of the chain has the values which it loaded before
 *     its restore.
 *
 *   - Check that the interrupt handler ran once.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Enter a chain of 8 nested register windows. Load a pattern into each local
 *   register and each input register except I6 and I7 of each level. Execute
 *   an illegal instruction in the innermost level. Store the register windows
 *   of the exception frame. Continue after the trapping instruction. Store the
 *   registers of each level before its restore. Run this through the register
 *   check, once for each value inverted, then once unchanged.
 *
 *   - Check that each level of the chain has the values which it loaded before
 *     its restore.
 *
 *   - Check that the exception frame holds the register window of each level
 *     except the outermost level.
 *
 *   - Check that each run with a changed value flagged exactly the checks of
 *     that value, and that each run recorded every check.
 *
 * - Disable interrupts. Write ones to all bits of the WIM and read it back.
 *   Restore the WIM and enable interrupts. The WIM implements one bit for each
 *   register window, so the bits which read back as one count the register
 *   windows of the processor. Run this through the register check, once with
 *   the expected count inverted, then once unchanged.
 *
 *   - Check that SPARC_NUMBER_OF_REGISTER_WINDOWS is equal to the number of
 *     register windows which the processor implements.
 *
 *   - Check that the run with the changed value flagged exactly the check of
 *     that value, and that each run recorded the check.
 *
 * @{
 */

#define WORD UINT64_C( 0xffffffff )

#define LEVELS 8

#define REGISTERS 14

#define GROUPS 4

#define PER_GROUP 28

RTEMS_STATIC_ASSERT( LEVELS *REGISTERS == GROUPS * PER_GROUP, groups );

/*
 * The chain loads each value from the table of initial values.  A value sits
 * at the byte offset eight times its index plus four, since SPARC is
 * big-endian.  Level one is the outermost level of the chain.
 */
uint64_t windows_initial[ LEVELS * REGISTERS ];

/* The registers of each level before its restore */
uint32_t windows_actual[ LEVELS * REGISTERS ];

/* The save area of each level after the window flush trap */
uint32_t windows_memory[ LEVELS * REGISTERS ];

/* The register windows of each level in the exception frame */
static uint32_t windows_frame[ LEVELS * REGISTERS ];

/* The stack pointer of each level */
uint32_t windows_sp[ LEVELS ];

uint64_t windows_flush_initial[ 14 ];

uint32_t windows_flush_actual[ 14 ];

volatile uint32_t windows_isr_count;

static rtems_id windows_worker;

static CallWithinISRRequest windows_isr_request;

static __attribute__(( __noinline__ )) uint32_t
WindowsRecurse( uint32_t depth )
{
  volatile uint32_t value;

  value = depth;

  if ( depth > 0 ) {
    (void) WindowsRecurse( depth - 1 );
  }

  return value;
}

static void WindowsWorker( rtems_task_argument arg )
{
  (void) arg;

  while ( true ) {
    (void) WindowsRecurse( 2 * LEVELS );
    SuspendSelf();
  }
}

void WindowsSwitch( void );

void WindowsSwitch( void )
{
  ResumeTask( windows_worker );
}

static void WindowsInterrupt( void *arg )
{
  (void) arg;
  (void) WindowsRecurse( 2 * LEVELS );
  ++windows_isr_count;
}

void WindowsRaise( void );

void WindowsRaise( void )
{
  windows_isr_request.handler = WindowsInterrupt;
  CallWithinISRSubmit( &windows_isr_request );
}

static void WindowsFatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  CPU_Exception_frame *frame;
  size_t               window;

  (void) arg;

  if ( source != RTEMS_FATAL_SOURCE_EXCEPTION ) {
    return;
  }

  SetFatalHandler( NULL, NULL );
  frame = (CPU_Exception_frame *) code;

  /* The trap happens in the innermost level */
  for ( window = 0; window < LEVELS - 1; ++window ) {
    uint32_t *level;
    size_t    i;

    level = &windows_frame[ ( LEVELS - 1 - window ) * REGISTERS ];

    for ( i = 0; i < 8; ++i ) {
      level[ i ] = frame->windows[ window ].local[ i ];
    }

    for ( i = 0; i < 6; ++i ) {
      level[ 8 + i ] = frame->windows[ window ].input[ i ];
    }
  }

  /* Continue after the trapping instruction */
  frame->pc = frame->npc;
  frame->npc = frame->pc + 4;
  _CPU_Exception_resume( frame );
}

static void WindowsNoneScenario( void )
{
  __asm__ volatile( "save %%sp, -96, %%sp\n"
                    "set windows_initial + 0, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 0]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 112, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 4]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 224, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 8]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 336, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 12]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 448, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 16]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 560, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 20]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 672, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 24]\n"
                    "save %%sp, -160, %%sp\n"
                    "set windows_initial + 784, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 28]\n"
                    "set windows_actual + 392, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 336, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 280, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 224, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 168, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 112, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 56, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 0, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
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
                      "o5" );
}

static void WindowsFlushScenario( void )
{
  __asm__ volatile( "save %%sp, -96, %%sp\n"
                    "set windows_initial + 0, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 0]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 112, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 4]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 224, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 8]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 336, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 12]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 448, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 16]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 560, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 20]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 672, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 24]\n"
                    "save %%sp, -160, %%sp\n"
                    "set windows_initial + 784, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 28]\n"
                    "set windows_flush_initial, %%g1\n"
                    "ld [%%g1 + 100], %%g2\n"
                    "wr %%g2, 0, %%y\n"
                    "rd %%psr, %%g2\n"
                    "set 0x00f00000, %%g3\n"
                    "andn %%g2, %%g3, %%g2\n"
                    "ld [%%g1 + 108], %%g4\n"
                    "and %%g4, %%g3, %%g4\n"
                    "or %%g2, %%g4, %%g2\n"
                    "wr %%g2, 0, %%psr\n"
                    "nop\n"
                    "nop\n"
                    "nop\n"
                    "ld [%%g1 + 44], %%o0\n"
                    "ld [%%g1 + 52], %%o1\n"
                    "ld [%%g1 + 60], %%o2\n"
                    "ld [%%g1 + 68], %%o3\n"
                    "ld [%%g1 + 76], %%o4\n"
                    "ld [%%g1 + 84], %%o5\n"
                    "ld [%%g1 + 92], %%o7\n"
                    "ld [%%g1 + 12], %%g2\n"
                    "ld [%%g1 + 20], %%g3\n"
                    "ld [%%g1 + 28], %%g4\n"
                    "ld [%%g1 + 36], %%g5\n"
                    "ld [%%g1 + 4], %%g1\n"
                    "ta 3\n"
                    "st %%g1, [%%sp + 64]\n"
                    "st %%g2, [%%sp + 68]\n"
                    "st %%g3, [%%sp + 72]\n"
                    "st %%g4, [%%sp + 76]\n"
                    "st %%g5, [%%sp + 80]\n"
                    "st %%o0, [%%sp + 84]\n"
                    "st %%o1, [%%sp + 88]\n"
                    "st %%o2, [%%sp + 92]\n"
                    "st %%o3, [%%sp + 96]\n"
                    "st %%o4, [%%sp + 100]\n"
                    "st %%o5, [%%sp + 104]\n"
                    "st %%o7, [%%sp + 108]\n"
                    "rd %%y, %%g1\n"
                    "st %%g1, [%%sp + 112]\n"
                    "rd %%psr, %%g1\n"
                    "st %%g1, [%%sp + 116]\n"
                    "set windows_flush_actual, %%g1\n"
                    "ld [%%sp + 64], %%g2\n"
                    "st %%g2, [%%g1 + 0]\n"
                    "ld [%%sp + 68], %%g2\n"
                    "st %%g2, [%%g1 + 4]\n"
                    "ld [%%sp + 72], %%g2\n"
                    "st %%g2, [%%g1 + 8]\n"
                    "ld [%%sp + 76], %%g2\n"
                    "st %%g2, [%%g1 + 12]\n"
                    "ld [%%sp + 80], %%g2\n"
                    "st %%g2, [%%g1 + 16]\n"
                    "ld [%%sp + 84], %%g2\n"
                    "st %%g2, [%%g1 + 20]\n"
                    "ld [%%sp + 88], %%g2\n"
                    "st %%g2, [%%g1 + 24]\n"
                    "ld [%%sp + 92], %%g2\n"
                    "st %%g2, [%%g1 + 28]\n"
                    "ld [%%sp + 96], %%g2\n"
                    "st %%g2, [%%g1 + 32]\n"
                    "ld [%%sp + 100], %%g2\n"
                    "st %%g2, [%%g1 + 36]\n"
                    "ld [%%sp + 104], %%g2\n"
                    "st %%g2, [%%g1 + 40]\n"
                    "ld [%%sp + 108], %%g2\n"
                    "st %%g2, [%%g1 + 44]\n"
                    "ld [%%sp + 112], %%g2\n"
                    "st %%g2, [%%g1 + 48]\n"
                    "ld [%%sp + 116], %%g2\n"
                    "st %%g2, [%%g1 + 52]\n"
                    "set windows_sp, %%g1\n"
                    "set windows_memory, %%g4\n"
                    "mov 7, %%g5\n"
                    "1:\n"
                    "ld [%%g1], %%g2\n"
                    "ld [%%g2 + 0], %%g3\n"
                    "st %%g3, [%%g4 + 0]\n"
                    "ld [%%g2 + 4], %%g3\n"
                    "st %%g3, [%%g4 + 4]\n"
                    "ld [%%g2 + 8], %%g3\n"
                    "st %%g3, [%%g4 + 8]\n"
                    "ld [%%g2 + 12], %%g3\n"
                    "st %%g3, [%%g4 + 12]\n"
                    "ld [%%g2 + 16], %%g3\n"
                    "st %%g3, [%%g4 + 16]\n"
                    "ld [%%g2 + 20], %%g3\n"
                    "st %%g3, [%%g4 + 20]\n"
                    "ld [%%g2 + 24], %%g3\n"
                    "st %%g3, [%%g4 + 24]\n"
                    "ld [%%g2 + 28], %%g3\n"
                    "st %%g3, [%%g4 + 28]\n"
                    "ld [%%g2 + 32], %%g3\n"
                    "st %%g3, [%%g4 + 32]\n"
                    "ld [%%g2 + 36], %%g3\n"
                    "st %%g3, [%%g4 + 36]\n"
                    "ld [%%g2 + 40], %%g3\n"
                    "st %%g3, [%%g4 + 40]\n"
                    "ld [%%g2 + 44], %%g3\n"
                    "st %%g3, [%%g4 + 44]\n"
                    "ld [%%g2 + 48], %%g3\n"
                    "st %%g3, [%%g4 + 48]\n"
                    "ld [%%g2 + 52], %%g3\n"
                    "st %%g3, [%%g4 + 52]\n"
                    "add %%g1, 4, %%g1\n"
                    "add %%g4, 56, %%g4\n"
                    "subcc %%g5, 1, %%g5\n"
                    "bne 1b\n"
                    "nop\n"
                    "set windows_actual + 392, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 336, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 280, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 224, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 168, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 112, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 56, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 0, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
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
                      "o5" );
}

static void WindowsSwitchScenario( void )
{
  __asm__ volatile( "save %%sp, -96, %%sp\n"
                    "set windows_initial + 0, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 0]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 112, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 4]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 224, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 8]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 336, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 12]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 448, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 16]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 560, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 20]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 672, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 24]\n"
                    "save %%sp, -160, %%sp\n"
                    "set windows_initial + 784, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 28]\n"
                    "call WindowsSwitch\n"
                    "nop\n"
                    "set windows_actual + 392, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 336, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 280, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 224, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 168, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 112, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 56, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 0, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
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
                      "o5" );
}

static void WindowsInterruptScenario( void )
{
  __asm__ volatile( "save %%sp, -96, %%sp\n"
                    "set windows_initial + 0, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 0]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 112, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 4]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 224, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 8]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 336, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 12]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 448, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 16]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 560, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 20]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 672, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 24]\n"
                    "save %%sp, -160, %%sp\n"
                    "set windows_initial + 784, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 28]\n"
                    "ta 9\n"
                    "st %%g1, [%%sp + 64]\n"
                    "call WindowsRaise\n"
                    "nop\n"
                    "ld [%%sp + 64], %%g1\n"
                    "ta 10\n"
                    "set windows_isr_count, %%g2\n"
                    "set 10000000, %%g3\n"
                    "1:\n"
                    "ld [%%g2], %%g4\n"
                    "cmp %%g4, 0\n"
                    "bne 2f\n"
                    "nop\n"
                    "subcc %%g3, 1, %%g3\n"
                    "bne 1b\n"
                    "nop\n"
                    "2:\n"
                    "set windows_actual + 392, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 336, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 280, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 224, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 168, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 112, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 56, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 0, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
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
                      "o5" );
}

static void WindowsExceptionScenario( void )
{
  SetFatalHandler( WindowsFatal, NULL );
  __asm__ volatile( "save %%sp, -96, %%sp\n"
                    "set windows_initial + 0, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 0]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 112, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 4]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 224, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 8]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 336, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 12]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 448, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 16]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 560, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 20]\n"
                    "save %%sp, -96, %%sp\n"
                    "set windows_initial + 672, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 24]\n"
                    "save %%sp, -160, %%sp\n"
                    "set windows_initial + 784, %%g1\n"
                    "ld [%%g1 + 4], %%l0\n"
                    "ld [%%g1 + 12], %%l1\n"
                    "ld [%%g1 + 20], %%l2\n"
                    "ld [%%g1 + 28], %%l3\n"
                    "ld [%%g1 + 36], %%l4\n"
                    "ld [%%g1 + 44], %%l5\n"
                    "ld [%%g1 + 52], %%l6\n"
                    "ld [%%g1 + 60], %%l7\n"
                    "ld [%%g1 + 68], %%i0\n"
                    "ld [%%g1 + 76], %%i1\n"
                    "ld [%%g1 + 84], %%i2\n"
                    "ld [%%g1 + 92], %%i3\n"
                    "ld [%%g1 + 100], %%i4\n"
                    "ld [%%g1 + 108], %%i5\n"
                    "set windows_sp, %%g1\n"
                    "st %%sp, [%%g1 + 28]\n"
                    ".globl sparc_windows_exception_label\n"
                    "sparc_windows_exception_label:\n"
                    "unimp 0\n"
                    "set windows_actual + 392, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 336, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 280, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 224, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 168, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 112, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 56, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
                    "restore\n"
                    "set windows_actual + 0, %%g1\n"
                    "st %%l0, [%%g1 + 0]\n"
                    "st %%l1, [%%g1 + 4]\n"
                    "st %%l2, [%%g1 + 8]\n"
                    "st %%l3, [%%g1 + 12]\n"
                    "st %%l4, [%%g1 + 16]\n"
                    "st %%l5, [%%g1 + 20]\n"
                    "st %%l6, [%%g1 + 24]\n"
                    "st %%l7, [%%g1 + 28]\n"
                    "st %%i0, [%%g1 + 32]\n"
                    "st %%i1, [%%g1 + 36]\n"
                    "st %%i2, [%%g1 + 40]\n"
                    "st %%i3, [%%g1 + 44]\n"
                    "st %%i4, [%%g1 + 48]\n"
                    "st %%i5, [%%g1 + 52]\n"
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
                      "o5" );
}

static const RegisterCheckSource windows_sources_0[ PER_GROUP ] = {
  { "level 1 l0", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 l1", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 l2", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 l3", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 l4", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 l5", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 l6", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 l7", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 i0", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 i1", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 i2", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 i3", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 i4", REGISTER_CHECK_INITIAL, WORD },
  { "level 1 i5", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 l0", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 l1", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 l2", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 l3", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 l4", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 l5", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 l6", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 l7", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 i0", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 i1", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 i2", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 i3", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 i4", REGISTER_CHECK_INITIAL, WORD },
  { "level 2 i5", REGISTER_CHECK_INITIAL, WORD }
};

static const RegisterCheckSource windows_sources_1[ PER_GROUP ] = {
  { "level 3 l0", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 l1", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 l2", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 l3", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 l4", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 l5", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 l6", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 l7", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 i0", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 i1", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 i2", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 i3", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 i4", REGISTER_CHECK_INITIAL, WORD },
  { "level 3 i5", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 l0", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 l1", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 l2", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 l3", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 l4", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 l5", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 l6", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 l7", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 i0", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 i1", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 i2", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 i3", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 i4", REGISTER_CHECK_INITIAL, WORD },
  { "level 4 i5", REGISTER_CHECK_INITIAL, WORD }
};

static const RegisterCheckSource windows_sources_2[ PER_GROUP ] = {
  { "level 5 l0", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 l1", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 l2", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 l3", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 l4", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 l5", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 l6", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 l7", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 i0", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 i1", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 i2", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 i3", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 i4", REGISTER_CHECK_INITIAL, WORD },
  { "level 5 i5", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 l0", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 l1", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 l2", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 l3", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 l4", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 l5", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 l6", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 l7", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 i0", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 i1", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 i2", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 i3", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 i4", REGISTER_CHECK_INITIAL, WORD },
  { "level 6 i5", REGISTER_CHECK_INITIAL, WORD }
};

static const RegisterCheckSource windows_sources_3[ PER_GROUP ] = {
  { "level 7 l0", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 l1", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 l2", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 l3", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 l4", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 l5", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 l6", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 l7", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 i0", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 i1", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 i2", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 i3", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 i4", REGISTER_CHECK_INITIAL, WORD },
  { "level 7 i5", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 l0", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 l1", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 l2", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 l3", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 l4", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 l5", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 l6", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 l7", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 i0", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 i1", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 i2", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 i3", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 i4", REGISTER_CHECK_INITIAL, WORD },
  { "level 8 i5", REGISTER_CHECK_INITIAL, WORD }
};

static const uint64_t windows_patterns[ LEVELS * REGISTERS ] = {
  0xa5010010,
  0xa5010111,
  0xa5010212,
  0xa5010313,
  0xa5010414,
  0xa5010515,
  0xa5010616,
  0xa5010717,
  0xa5010818,
  0xa5010919,
  0xa5010a1a,
  0xa5010b1b,
  0xa5010c1c,
  0xa5010d1d,
  0xa5020020,
  0xa5020121,
  0xa5020222,
  0xa5020323,
  0xa5020424,
  0xa5020525,
  0xa5020626,
  0xa5020727,
  0xa5020828,
  0xa5020929,
  0xa5020a2a,
  0xa5020b2b,
  0xa5020c2c,
  0xa5020d2d,
  0xa5030030,
  0xa5030131,
  0xa5030232,
  0xa5030333,
  0xa5030434,
  0xa5030535,
  0xa5030636,
  0xa5030737,
  0xa5030838,
  0xa5030939,
  0xa5030a3a,
  0xa5030b3b,
  0xa5030c3c,
  0xa5030d3d,
  0xa5040040,
  0xa5040141,
  0xa5040242,
  0xa5040343,
  0xa5040444,
  0xa5040545,
  0xa5040646,
  0xa5040747,
  0xa5040848,
  0xa5040949,
  0xa5040a4a,
  0xa5040b4b,
  0xa5040c4c,
  0xa5040d4d,
  0xa5050050,
  0xa5050151,
  0xa5050252,
  0xa5050353,
  0xa5050454,
  0xa5050555,
  0xa5050656,
  0xa5050757,
  0xa5050858,
  0xa5050959,
  0xa5050a5a,
  0xa5050b5b,
  0xa5050c5c,
  0xa5050d5d,
  0xa5060060,
  0xa5060161,
  0xa5060262,
  0xa5060363,
  0xa5060464,
  0xa5060565,
  0xa5060666,
  0xa5060767,
  0xa5060868,
  0xa5060969,
  0xa5060a6a,
  0xa5060b6b,
  0xa5060c6c,
  0xa5060d6d,
  0xa5070070,
  0xa5070171,
  0xa5070272,
  0xa5070373,
  0xa5070474,
  0xa5070575,
  0xa5070676,
  0xa5070777,
  0xa5070878,
  0xa5070979,
  0xa5070a7a,
  0xa5070b7b,
  0xa5070c7c,
  0xa5070d7d,
  0xa5080080,
  0xa5080181,
  0xa5080282,
  0xa5080383,
  0xa5080484,
  0xa5080585,
  0xa5080686,
  0xa5080787,
  0xa5080888,
  0xa5080989,
  0xa5080a8a,
  0xa5080b8b,
  0xa5080c8c,
  0xa5080d8d
};

static const RegisterCheckSlot windows_slots_none_0[] = {
  { "level 1 l0", 0, WORD },
  { "level 1 l1", 1, WORD },
  { "level 1 l2", 2, WORD },
  { "level 1 l3", 3, WORD },
  { "level 1 l4", 4, WORD },
  { "level 1 l5", 5, WORD },
  { "level 1 l6", 6, WORD },
  { "level 1 l7", 7, WORD },
  { "level 1 i0", 8, WORD },
  { "level 1 i1", 9, WORD },
  { "level 1 i2", 10, WORD },
  { "level 1 i3", 11, WORD },
  { "level 1 i4", 12, WORD },
  { "level 1 i5", 13, WORD },
  { "level 2 l0", 14, WORD },
  { "level 2 l1", 15, WORD },
  { "level 2 l2", 16, WORD },
  { "level 2 l3", 17, WORD },
  { "level 2 l4", 18, WORD },
  { "level 2 l5", 19, WORD },
  { "level 2 l6", 20, WORD },
  { "level 2 l7", 21, WORD },
  { "level 2 i0", 22, WORD },
  { "level 2 i1", 23, WORD },
  { "level 2 i2", 24, WORD },
  { "level 2 i3", 25, WORD },
  { "level 2 i4", 26, WORD },
  { "level 2 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_none_1[] = {
  { "level 3 l0", 0, WORD },
  { "level 3 l1", 1, WORD },
  { "level 3 l2", 2, WORD },
  { "level 3 l3", 3, WORD },
  { "level 3 l4", 4, WORD },
  { "level 3 l5", 5, WORD },
  { "level 3 l6", 6, WORD },
  { "level 3 l7", 7, WORD },
  { "level 3 i0", 8, WORD },
  { "level 3 i1", 9, WORD },
  { "level 3 i2", 10, WORD },
  { "level 3 i3", 11, WORD },
  { "level 3 i4", 12, WORD },
  { "level 3 i5", 13, WORD },
  { "level 4 l0", 14, WORD },
  { "level 4 l1", 15, WORD },
  { "level 4 l2", 16, WORD },
  { "level 4 l3", 17, WORD },
  { "level 4 l4", 18, WORD },
  { "level 4 l5", 19, WORD },
  { "level 4 l6", 20, WORD },
  { "level 4 l7", 21, WORD },
  { "level 4 i0", 22, WORD },
  { "level 4 i1", 23, WORD },
  { "level 4 i2", 24, WORD },
  { "level 4 i3", 25, WORD },
  { "level 4 i4", 26, WORD },
  { "level 4 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_none_2[] = {
  { "level 5 l0", 0, WORD },
  { "level 5 l1", 1, WORD },
  { "level 5 l2", 2, WORD },
  { "level 5 l3", 3, WORD },
  { "level 5 l4", 4, WORD },
  { "level 5 l5", 5, WORD },
  { "level 5 l6", 6, WORD },
  { "level 5 l7", 7, WORD },
  { "level 5 i0", 8, WORD },
  { "level 5 i1", 9, WORD },
  { "level 5 i2", 10, WORD },
  { "level 5 i3", 11, WORD },
  { "level 5 i4", 12, WORD },
  { "level 5 i5", 13, WORD },
  { "level 6 l0", 14, WORD },
  { "level 6 l1", 15, WORD },
  { "level 6 l2", 16, WORD },
  { "level 6 l3", 17, WORD },
  { "level 6 l4", 18, WORD },
  { "level 6 l5", 19, WORD },
  { "level 6 l6", 20, WORD },
  { "level 6 l7", 21, WORD },
  { "level 6 i0", 22, WORD },
  { "level 6 i1", 23, WORD },
  { "level 6 i2", 24, WORD },
  { "level 6 i3", 25, WORD },
  { "level 6 i4", 26, WORD },
  { "level 6 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_none_3[] = {
  { "level 7 l0", 0, WORD },
  { "level 7 l1", 1, WORD },
  { "level 7 l2", 2, WORD },
  { "level 7 l3", 3, WORD },
  { "level 7 l4", 4, WORD },
  { "level 7 l5", 5, WORD },
  { "level 7 l6", 6, WORD },
  { "level 7 l7", 7, WORD },
  { "level 7 i0", 8, WORD },
  { "level 7 i1", 9, WORD },
  { "level 7 i2", 10, WORD },
  { "level 7 i3", 11, WORD },
  { "level 7 i4", 12, WORD },
  { "level 7 i5", 13, WORD },
  { "level 8 l0", 14, WORD },
  { "level 8 l1", 15, WORD },
  { "level 8 l2", 16, WORD },
  { "level 8 l3", 17, WORD },
  { "level 8 l4", 18, WORD },
  { "level 8 l5", 19, WORD },
  { "level 8 l6", 20, WORD },
  { "level 8 l7", 21, WORD },
  { "level 8 i0", 22, WORD },
  { "level 8 i1", 23, WORD },
  { "level 8 i2", 24, WORD },
  { "level 8 i3", 25, WORD },
  { "level 8 i4", 26, WORD },
  { "level 8 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_flush_0[] = {
  { "level 1 l0", 0, WORD },
  { "level 1 l1", 1, WORD },
  { "level 1 l2", 2, WORD },
  { "level 1 l3", 3, WORD },
  { "level 1 l4", 4, WORD },
  { "level 1 l5", 5, WORD },
  { "level 1 l6", 6, WORD },
  { "level 1 l7", 7, WORD },
  { "level 1 i0", 8, WORD },
  { "level 1 i1", 9, WORD },
  { "level 1 i2", 10, WORD },
  { "level 1 i3", 11, WORD },
  { "level 1 i4", 12, WORD },
  { "level 1 i5", 13, WORD },
  { "level 2 l0", 14, WORD },
  { "level 2 l1", 15, WORD },
  { "level 2 l2", 16, WORD },
  { "level 2 l3", 17, WORD },
  { "level 2 l4", 18, WORD },
  { "level 2 l5", 19, WORD },
  { "level 2 l6", 20, WORD },
  { "level 2 l7", 21, WORD },
  { "level 2 i0", 22, WORD },
  { "level 2 i1", 23, WORD },
  { "level 2 i2", 24, WORD },
  { "level 2 i3", 25, WORD },
  { "level 2 i4", 26, WORD },
  { "level 2 i5", 27, WORD },
  { "level 1 l0 memory", 0, WORD },
  { "level 1 l1 memory", 1, WORD },
  { "level 1 l2 memory", 2, WORD },
  { "level 1 l3 memory", 3, WORD },
  { "level 1 l4 memory", 4, WORD },
  { "level 1 l5 memory", 5, WORD },
  { "level 1 l6 memory", 6, WORD },
  { "level 1 l7 memory", 7, WORD },
  { "level 1 i0 memory", 8, WORD },
  { "level 1 i1 memory", 9, WORD },
  { "level 1 i2 memory", 10, WORD },
  { "level 1 i3 memory", 11, WORD },
  { "level 1 i4 memory", 12, WORD },
  { "level 1 i5 memory", 13, WORD },
  { "level 2 l0 memory", 14, WORD },
  { "level 2 l1 memory", 15, WORD },
  { "level 2 l2 memory", 16, WORD },
  { "level 2 l3 memory", 17, WORD },
  { "level 2 l4 memory", 18, WORD },
  { "level 2 l5 memory", 19, WORD },
  { "level 2 l6 memory", 20, WORD },
  { "level 2 l7 memory", 21, WORD },
  { "level 2 i0 memory", 22, WORD },
  { "level 2 i1 memory", 23, WORD },
  { "level 2 i2 memory", 24, WORD },
  { "level 2 i3 memory", 25, WORD },
  { "level 2 i4 memory", 26, WORD },
  { "level 2 i5 memory", 27, WORD }
};

static const RegisterCheckSlot windows_slots_flush_1[] = {
  { "level 3 l0", 0, WORD },
  { "level 3 l1", 1, WORD },
  { "level 3 l2", 2, WORD },
  { "level 3 l3", 3, WORD },
  { "level 3 l4", 4, WORD },
  { "level 3 l5", 5, WORD },
  { "level 3 l6", 6, WORD },
  { "level 3 l7", 7, WORD },
  { "level 3 i0", 8, WORD },
  { "level 3 i1", 9, WORD },
  { "level 3 i2", 10, WORD },
  { "level 3 i3", 11, WORD },
  { "level 3 i4", 12, WORD },
  { "level 3 i5", 13, WORD },
  { "level 4 l0", 14, WORD },
  { "level 4 l1", 15, WORD },
  { "level 4 l2", 16, WORD },
  { "level 4 l3", 17, WORD },
  { "level 4 l4", 18, WORD },
  { "level 4 l5", 19, WORD },
  { "level 4 l6", 20, WORD },
  { "level 4 l7", 21, WORD },
  { "level 4 i0", 22, WORD },
  { "level 4 i1", 23, WORD },
  { "level 4 i2", 24, WORD },
  { "level 4 i3", 25, WORD },
  { "level 4 i4", 26, WORD },
  { "level 4 i5", 27, WORD },
  { "level 3 l0 memory", 0, WORD },
  { "level 3 l1 memory", 1, WORD },
  { "level 3 l2 memory", 2, WORD },
  { "level 3 l3 memory", 3, WORD },
  { "level 3 l4 memory", 4, WORD },
  { "level 3 l5 memory", 5, WORD },
  { "level 3 l6 memory", 6, WORD },
  { "level 3 l7 memory", 7, WORD },
  { "level 3 i0 memory", 8, WORD },
  { "level 3 i1 memory", 9, WORD },
  { "level 3 i2 memory", 10, WORD },
  { "level 3 i3 memory", 11, WORD },
  { "level 3 i4 memory", 12, WORD },
  { "level 3 i5 memory", 13, WORD },
  { "level 4 l0 memory", 14, WORD },
  { "level 4 l1 memory", 15, WORD },
  { "level 4 l2 memory", 16, WORD },
  { "level 4 l3 memory", 17, WORD },
  { "level 4 l4 memory", 18, WORD },
  { "level 4 l5 memory", 19, WORD },
  { "level 4 l6 memory", 20, WORD },
  { "level 4 l7 memory", 21, WORD },
  { "level 4 i0 memory", 22, WORD },
  { "level 4 i1 memory", 23, WORD },
  { "level 4 i2 memory", 24, WORD },
  { "level 4 i3 memory", 25, WORD },
  { "level 4 i4 memory", 26, WORD },
  { "level 4 i5 memory", 27, WORD }
};

static const RegisterCheckSlot windows_slots_flush_2[] = {
  { "level 5 l0", 0, WORD },
  { "level 5 l1", 1, WORD },
  { "level 5 l2", 2, WORD },
  { "level 5 l3", 3, WORD },
  { "level 5 l4", 4, WORD },
  { "level 5 l5", 5, WORD },
  { "level 5 l6", 6, WORD },
  { "level 5 l7", 7, WORD },
  { "level 5 i0", 8, WORD },
  { "level 5 i1", 9, WORD },
  { "level 5 i2", 10, WORD },
  { "level 5 i3", 11, WORD },
  { "level 5 i4", 12, WORD },
  { "level 5 i5", 13, WORD },
  { "level 6 l0", 14, WORD },
  { "level 6 l1", 15, WORD },
  { "level 6 l2", 16, WORD },
  { "level 6 l3", 17, WORD },
  { "level 6 l4", 18, WORD },
  { "level 6 l5", 19, WORD },
  { "level 6 l6", 20, WORD },
  { "level 6 l7", 21, WORD },
  { "level 6 i0", 22, WORD },
  { "level 6 i1", 23, WORD },
  { "level 6 i2", 24, WORD },
  { "level 6 i3", 25, WORD },
  { "level 6 i4", 26, WORD },
  { "level 6 i5", 27, WORD },
  { "level 5 l0 memory", 0, WORD },
  { "level 5 l1 memory", 1, WORD },
  { "level 5 l2 memory", 2, WORD },
  { "level 5 l3 memory", 3, WORD },
  { "level 5 l4 memory", 4, WORD },
  { "level 5 l5 memory", 5, WORD },
  { "level 5 l6 memory", 6, WORD },
  { "level 5 l7 memory", 7, WORD },
  { "level 5 i0 memory", 8, WORD },
  { "level 5 i1 memory", 9, WORD },
  { "level 5 i2 memory", 10, WORD },
  { "level 5 i3 memory", 11, WORD },
  { "level 5 i4 memory", 12, WORD },
  { "level 5 i5 memory", 13, WORD },
  { "level 6 l0 memory", 14, WORD },
  { "level 6 l1 memory", 15, WORD },
  { "level 6 l2 memory", 16, WORD },
  { "level 6 l3 memory", 17, WORD },
  { "level 6 l4 memory", 18, WORD },
  { "level 6 l5 memory", 19, WORD },
  { "level 6 l6 memory", 20, WORD },
  { "level 6 l7 memory", 21, WORD },
  { "level 6 i0 memory", 22, WORD },
  { "level 6 i1 memory", 23, WORD },
  { "level 6 i2 memory", 24, WORD },
  { "level 6 i3 memory", 25, WORD },
  { "level 6 i4 memory", 26, WORD },
  { "level 6 i5 memory", 27, WORD }
};

static const RegisterCheckSlot windows_slots_flush_3[] = {
  { "level 7 l0", 0, WORD },
  { "level 7 l1", 1, WORD },
  { "level 7 l2", 2, WORD },
  { "level 7 l3", 3, WORD },
  { "level 7 l4", 4, WORD },
  { "level 7 l5", 5, WORD },
  { "level 7 l6", 6, WORD },
  { "level 7 l7", 7, WORD },
  { "level 7 i0", 8, WORD },
  { "level 7 i1", 9, WORD },
  { "level 7 i2", 10, WORD },
  { "level 7 i3", 11, WORD },
  { "level 7 i4", 12, WORD },
  { "level 7 i5", 13, WORD },
  { "level 8 l0", 14, WORD },
  { "level 8 l1", 15, WORD },
  { "level 8 l2", 16, WORD },
  { "level 8 l3", 17, WORD },
  { "level 8 l4", 18, WORD },
  { "level 8 l5", 19, WORD },
  { "level 8 l6", 20, WORD },
  { "level 8 l7", 21, WORD },
  { "level 8 i0", 22, WORD },
  { "level 8 i1", 23, WORD },
  { "level 8 i2", 24, WORD },
  { "level 8 i3", 25, WORD },
  { "level 8 i4", 26, WORD },
  { "level 8 i5", 27, WORD },
  { "level 7 l0 memory", 0, WORD },
  { "level 7 l1 memory", 1, WORD },
  { "level 7 l2 memory", 2, WORD },
  { "level 7 l3 memory", 3, WORD },
  { "level 7 l4 memory", 4, WORD },
  { "level 7 l5 memory", 5, WORD },
  { "level 7 l6 memory", 6, WORD },
  { "level 7 l7 memory", 7, WORD },
  { "level 7 i0 memory", 8, WORD },
  { "level 7 i1 memory", 9, WORD },
  { "level 7 i2 memory", 10, WORD },
  { "level 7 i3 memory", 11, WORD },
  { "level 7 i4 memory", 12, WORD },
  { "level 7 i5 memory", 13, WORD }
};

static const RegisterCheckSlot windows_slots_switch_0[] = {
  { "level 1 l0", 0, WORD },
  { "level 1 l1", 1, WORD },
  { "level 1 l2", 2, WORD },
  { "level 1 l3", 3, WORD },
  { "level 1 l4", 4, WORD },
  { "level 1 l5", 5, WORD },
  { "level 1 l6", 6, WORD },
  { "level 1 l7", 7, WORD },
  { "level 1 i0", 8, WORD },
  { "level 1 i1", 9, WORD },
  { "level 1 i2", 10, WORD },
  { "level 1 i3", 11, WORD },
  { "level 1 i4", 12, WORD },
  { "level 1 i5", 13, WORD },
  { "level 2 l0", 14, WORD },
  { "level 2 l1", 15, WORD },
  { "level 2 l2", 16, WORD },
  { "level 2 l3", 17, WORD },
  { "level 2 l4", 18, WORD },
  { "level 2 l5", 19, WORD },
  { "level 2 l6", 20, WORD },
  { "level 2 l7", 21, WORD },
  { "level 2 i0", 22, WORD },
  { "level 2 i1", 23, WORD },
  { "level 2 i2", 24, WORD },
  { "level 2 i3", 25, WORD },
  { "level 2 i4", 26, WORD },
  { "level 2 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_switch_1[] = {
  { "level 3 l0", 0, WORD },
  { "level 3 l1", 1, WORD },
  { "level 3 l2", 2, WORD },
  { "level 3 l3", 3, WORD },
  { "level 3 l4", 4, WORD },
  { "level 3 l5", 5, WORD },
  { "level 3 l6", 6, WORD },
  { "level 3 l7", 7, WORD },
  { "level 3 i0", 8, WORD },
  { "level 3 i1", 9, WORD },
  { "level 3 i2", 10, WORD },
  { "level 3 i3", 11, WORD },
  { "level 3 i4", 12, WORD },
  { "level 3 i5", 13, WORD },
  { "level 4 l0", 14, WORD },
  { "level 4 l1", 15, WORD },
  { "level 4 l2", 16, WORD },
  { "level 4 l3", 17, WORD },
  { "level 4 l4", 18, WORD },
  { "level 4 l5", 19, WORD },
  { "level 4 l6", 20, WORD },
  { "level 4 l7", 21, WORD },
  { "level 4 i0", 22, WORD },
  { "level 4 i1", 23, WORD },
  { "level 4 i2", 24, WORD },
  { "level 4 i3", 25, WORD },
  { "level 4 i4", 26, WORD },
  { "level 4 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_switch_2[] = {
  { "level 5 l0", 0, WORD },
  { "level 5 l1", 1, WORD },
  { "level 5 l2", 2, WORD },
  { "level 5 l3", 3, WORD },
  { "level 5 l4", 4, WORD },
  { "level 5 l5", 5, WORD },
  { "level 5 l6", 6, WORD },
  { "level 5 l7", 7, WORD },
  { "level 5 i0", 8, WORD },
  { "level 5 i1", 9, WORD },
  { "level 5 i2", 10, WORD },
  { "level 5 i3", 11, WORD },
  { "level 5 i4", 12, WORD },
  { "level 5 i5", 13, WORD },
  { "level 6 l0", 14, WORD },
  { "level 6 l1", 15, WORD },
  { "level 6 l2", 16, WORD },
  { "level 6 l3", 17, WORD },
  { "level 6 l4", 18, WORD },
  { "level 6 l5", 19, WORD },
  { "level 6 l6", 20, WORD },
  { "level 6 l7", 21, WORD },
  { "level 6 i0", 22, WORD },
  { "level 6 i1", 23, WORD },
  { "level 6 i2", 24, WORD },
  { "level 6 i3", 25, WORD },
  { "level 6 i4", 26, WORD },
  { "level 6 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_switch_3[] = {
  { "level 7 l0", 0, WORD },
  { "level 7 l1", 1, WORD },
  { "level 7 l2", 2, WORD },
  { "level 7 l3", 3, WORD },
  { "level 7 l4", 4, WORD },
  { "level 7 l5", 5, WORD },
  { "level 7 l6", 6, WORD },
  { "level 7 l7", 7, WORD },
  { "level 7 i0", 8, WORD },
  { "level 7 i1", 9, WORD },
  { "level 7 i2", 10, WORD },
  { "level 7 i3", 11, WORD },
  { "level 7 i4", 12, WORD },
  { "level 7 i5", 13, WORD },
  { "level 8 l0", 14, WORD },
  { "level 8 l1", 15, WORD },
  { "level 8 l2", 16, WORD },
  { "level 8 l3", 17, WORD },
  { "level 8 l4", 18, WORD },
  { "level 8 l5", 19, WORD },
  { "level 8 l6", 20, WORD },
  { "level 8 l7", 21, WORD },
  { "level 8 i0", 22, WORD },
  { "level 8 i1", 23, WORD },
  { "level 8 i2", 24, WORD },
  { "level 8 i3", 25, WORD },
  { "level 8 i4", 26, WORD },
  { "level 8 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_isr_0[] = {
  { "level 1 l0", 0, WORD },
  { "level 1 l1", 1, WORD },
  { "level 1 l2", 2, WORD },
  { "level 1 l3", 3, WORD },
  { "level 1 l4", 4, WORD },
  { "level 1 l5", 5, WORD },
  { "level 1 l6", 6, WORD },
  { "level 1 l7", 7, WORD },
  { "level 1 i0", 8, WORD },
  { "level 1 i1", 9, WORD },
  { "level 1 i2", 10, WORD },
  { "level 1 i3", 11, WORD },
  { "level 1 i4", 12, WORD },
  { "level 1 i5", 13, WORD },
  { "level 2 l0", 14, WORD },
  { "level 2 l1", 15, WORD },
  { "level 2 l2", 16, WORD },
  { "level 2 l3", 17, WORD },
  { "level 2 l4", 18, WORD },
  { "level 2 l5", 19, WORD },
  { "level 2 l6", 20, WORD },
  { "level 2 l7", 21, WORD },
  { "level 2 i0", 22, WORD },
  { "level 2 i1", 23, WORD },
  { "level 2 i2", 24, WORD },
  { "level 2 i3", 25, WORD },
  { "level 2 i4", 26, WORD },
  { "level 2 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_isr_1[] = {
  { "level 3 l0", 0, WORD },
  { "level 3 l1", 1, WORD },
  { "level 3 l2", 2, WORD },
  { "level 3 l3", 3, WORD },
  { "level 3 l4", 4, WORD },
  { "level 3 l5", 5, WORD },
  { "level 3 l6", 6, WORD },
  { "level 3 l7", 7, WORD },
  { "level 3 i0", 8, WORD },
  { "level 3 i1", 9, WORD },
  { "level 3 i2", 10, WORD },
  { "level 3 i3", 11, WORD },
  { "level 3 i4", 12, WORD },
  { "level 3 i5", 13, WORD },
  { "level 4 l0", 14, WORD },
  { "level 4 l1", 15, WORD },
  { "level 4 l2", 16, WORD },
  { "level 4 l3", 17, WORD },
  { "level 4 l4", 18, WORD },
  { "level 4 l5", 19, WORD },
  { "level 4 l6", 20, WORD },
  { "level 4 l7", 21, WORD },
  { "level 4 i0", 22, WORD },
  { "level 4 i1", 23, WORD },
  { "level 4 i2", 24, WORD },
  { "level 4 i3", 25, WORD },
  { "level 4 i4", 26, WORD },
  { "level 4 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_isr_2[] = {
  { "level 5 l0", 0, WORD },
  { "level 5 l1", 1, WORD },
  { "level 5 l2", 2, WORD },
  { "level 5 l3", 3, WORD },
  { "level 5 l4", 4, WORD },
  { "level 5 l5", 5, WORD },
  { "level 5 l6", 6, WORD },
  { "level 5 l7", 7, WORD },
  { "level 5 i0", 8, WORD },
  { "level 5 i1", 9, WORD },
  { "level 5 i2", 10, WORD },
  { "level 5 i3", 11, WORD },
  { "level 5 i4", 12, WORD },
  { "level 5 i5", 13, WORD },
  { "level 6 l0", 14, WORD },
  { "level 6 l1", 15, WORD },
  { "level 6 l2", 16, WORD },
  { "level 6 l3", 17, WORD },
  { "level 6 l4", 18, WORD },
  { "level 6 l5", 19, WORD },
  { "level 6 l6", 20, WORD },
  { "level 6 l7", 21, WORD },
  { "level 6 i0", 22, WORD },
  { "level 6 i1", 23, WORD },
  { "level 6 i2", 24, WORD },
  { "level 6 i3", 25, WORD },
  { "level 6 i4", 26, WORD },
  { "level 6 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_isr_3[] = {
  { "level 7 l0", 0, WORD },
  { "level 7 l1", 1, WORD },
  { "level 7 l2", 2, WORD },
  { "level 7 l3", 3, WORD },
  { "level 7 l4", 4, WORD },
  { "level 7 l5", 5, WORD },
  { "level 7 l6", 6, WORD },
  { "level 7 l7", 7, WORD },
  { "level 7 i0", 8, WORD },
  { "level 7 i1", 9, WORD },
  { "level 7 i2", 10, WORD },
  { "level 7 i3", 11, WORD },
  { "level 7 i4", 12, WORD },
  { "level 7 i5", 13, WORD },
  { "level 8 l0", 14, WORD },
  { "level 8 l1", 15, WORD },
  { "level 8 l2", 16, WORD },
  { "level 8 l3", 17, WORD },
  { "level 8 l4", 18, WORD },
  { "level 8 l5", 19, WORD },
  { "level 8 l6", 20, WORD },
  { "level 8 l7", 21, WORD },
  { "level 8 i0", 22, WORD },
  { "level 8 i1", 23, WORD },
  { "level 8 i2", 24, WORD },
  { "level 8 i3", 25, WORD },
  { "level 8 i4", 26, WORD },
  { "level 8 i5", 27, WORD }
};

static const RegisterCheckSlot windows_slots_exception_0[] = {
  { "level 1 l0", 0, WORD },
  { "level 1 l1", 1, WORD },
  { "level 1 l2", 2, WORD },
  { "level 1 l3", 3, WORD },
  { "level 1 l4", 4, WORD },
  { "level 1 l5", 5, WORD },
  { "level 1 l6", 6, WORD },
  { "level 1 l7", 7, WORD },
  { "level 1 i0", 8, WORD },
  { "level 1 i1", 9, WORD },
  { "level 1 i2", 10, WORD },
  { "level 1 i3", 11, WORD },
  { "level 1 i4", 12, WORD },
  { "level 1 i5", 13, WORD },
  { "level 2 l0", 14, WORD },
  { "level 2 l1", 15, WORD },
  { "level 2 l2", 16, WORD },
  { "level 2 l3", 17, WORD },
  { "level 2 l4", 18, WORD },
  { "level 2 l5", 19, WORD },
  { "level 2 l6", 20, WORD },
  { "level 2 l7", 21, WORD },
  { "level 2 i0", 22, WORD },
  { "level 2 i1", 23, WORD },
  { "level 2 i2", 24, WORD },
  { "level 2 i3", 25, WORD },
  { "level 2 i4", 26, WORD },
  { "level 2 i5", 27, WORD },
  { "level 2 l0 frame", 14, WORD },
  { "level 2 l1 frame", 15, WORD },
  { "level 2 l2 frame", 16, WORD },
  { "level 2 l3 frame", 17, WORD },
  { "level 2 l4 frame", 18, WORD },
  { "level 2 l5 frame", 19, WORD },
  { "level 2 l6 frame", 20, WORD },
  { "level 2 l7 frame", 21, WORD },
  { "level 2 i0 frame", 22, WORD },
  { "level 2 i1 frame", 23, WORD },
  { "level 2 i2 frame", 24, WORD },
  { "level 2 i3 frame", 25, WORD },
  { "level 2 i4 frame", 26, WORD },
  { "level 2 i5 frame", 27, WORD }
};

static const RegisterCheckSlot windows_slots_exception_1[] = {
  { "level 3 l0", 0, WORD },
  { "level 3 l1", 1, WORD },
  { "level 3 l2", 2, WORD },
  { "level 3 l3", 3, WORD },
  { "level 3 l4", 4, WORD },
  { "level 3 l5", 5, WORD },
  { "level 3 l6", 6, WORD },
  { "level 3 l7", 7, WORD },
  { "level 3 i0", 8, WORD },
  { "level 3 i1", 9, WORD },
  { "level 3 i2", 10, WORD },
  { "level 3 i3", 11, WORD },
  { "level 3 i4", 12, WORD },
  { "level 3 i5", 13, WORD },
  { "level 4 l0", 14, WORD },
  { "level 4 l1", 15, WORD },
  { "level 4 l2", 16, WORD },
  { "level 4 l3", 17, WORD },
  { "level 4 l4", 18, WORD },
  { "level 4 l5", 19, WORD },
  { "level 4 l6", 20, WORD },
  { "level 4 l7", 21, WORD },
  { "level 4 i0", 22, WORD },
  { "level 4 i1", 23, WORD },
  { "level 4 i2", 24, WORD },
  { "level 4 i3", 25, WORD },
  { "level 4 i4", 26, WORD },
  { "level 4 i5", 27, WORD },
  { "level 3 l0 frame", 0, WORD },
  { "level 3 l1 frame", 1, WORD },
  { "level 3 l2 frame", 2, WORD },
  { "level 3 l3 frame", 3, WORD },
  { "level 3 l4 frame", 4, WORD },
  { "level 3 l5 frame", 5, WORD },
  { "level 3 l6 frame", 6, WORD },
  { "level 3 l7 frame", 7, WORD },
  { "level 3 i0 frame", 8, WORD },
  { "level 3 i1 frame", 9, WORD },
  { "level 3 i2 frame", 10, WORD },
  { "level 3 i3 frame", 11, WORD },
  { "level 3 i4 frame", 12, WORD },
  { "level 3 i5 frame", 13, WORD },
  { "level 4 l0 frame", 14, WORD },
  { "level 4 l1 frame", 15, WORD },
  { "level 4 l2 frame", 16, WORD },
  { "level 4 l3 frame", 17, WORD },
  { "level 4 l4 frame", 18, WORD },
  { "level 4 l5 frame", 19, WORD },
  { "level 4 l6 frame", 20, WORD },
  { "level 4 l7 frame", 21, WORD },
  { "level 4 i0 frame", 22, WORD },
  { "level 4 i1 frame", 23, WORD },
  { "level 4 i2 frame", 24, WORD },
  { "level 4 i3 frame", 25, WORD },
  { "level 4 i4 frame", 26, WORD },
  { "level 4 i5 frame", 27, WORD }
};

static const RegisterCheckSlot windows_slots_exception_2[] = {
  { "level 5 l0", 0, WORD },
  { "level 5 l1", 1, WORD },
  { "level 5 l2", 2, WORD },
  { "level 5 l3", 3, WORD },
  { "level 5 l4", 4, WORD },
  { "level 5 l5", 5, WORD },
  { "level 5 l6", 6, WORD },
  { "level 5 l7", 7, WORD },
  { "level 5 i0", 8, WORD },
  { "level 5 i1", 9, WORD },
  { "level 5 i2", 10, WORD },
  { "level 5 i3", 11, WORD },
  { "level 5 i4", 12, WORD },
  { "level 5 i5", 13, WORD },
  { "level 6 l0", 14, WORD },
  { "level 6 l1", 15, WORD },
  { "level 6 l2", 16, WORD },
  { "level 6 l3", 17, WORD },
  { "level 6 l4", 18, WORD },
  { "level 6 l5", 19, WORD },
  { "level 6 l6", 20, WORD },
  { "level 6 l7", 21, WORD },
  { "level 6 i0", 22, WORD },
  { "level 6 i1", 23, WORD },
  { "level 6 i2", 24, WORD },
  { "level 6 i3", 25, WORD },
  { "level 6 i4", 26, WORD },
  { "level 6 i5", 27, WORD },
  { "level 5 l0 frame", 0, WORD },
  { "level 5 l1 frame", 1, WORD },
  { "level 5 l2 frame", 2, WORD },
  { "level 5 l3 frame", 3, WORD },
  { "level 5 l4 frame", 4, WORD },
  { "level 5 l5 frame", 5, WORD },
  { "level 5 l6 frame", 6, WORD },
  { "level 5 l7 frame", 7, WORD },
  { "level 5 i0 frame", 8, WORD },
  { "level 5 i1 frame", 9, WORD },
  { "level 5 i2 frame", 10, WORD },
  { "level 5 i3 frame", 11, WORD },
  { "level 5 i4 frame", 12, WORD },
  { "level 5 i5 frame", 13, WORD },
  { "level 6 l0 frame", 14, WORD },
  { "level 6 l1 frame", 15, WORD },
  { "level 6 l2 frame", 16, WORD },
  { "level 6 l3 frame", 17, WORD },
  { "level 6 l4 frame", 18, WORD },
  { "level 6 l5 frame", 19, WORD },
  { "level 6 l6 frame", 20, WORD },
  { "level 6 l7 frame", 21, WORD },
  { "level 6 i0 frame", 22, WORD },
  { "level 6 i1 frame", 23, WORD },
  { "level 6 i2 frame", 24, WORD },
  { "level 6 i3 frame", 25, WORD },
  { "level 6 i4 frame", 26, WORD },
  { "level 6 i5 frame", 27, WORD }
};

static const RegisterCheckSlot windows_slots_exception_3[] = {
  { "level 7 l0", 0, WORD },
  { "level 7 l1", 1, WORD },
  { "level 7 l2", 2, WORD },
  { "level 7 l3", 3, WORD },
  { "level 7 l4", 4, WORD },
  { "level 7 l5", 5, WORD },
  { "level 7 l6", 6, WORD },
  { "level 7 l7", 7, WORD },
  { "level 7 i0", 8, WORD },
  { "level 7 i1", 9, WORD },
  { "level 7 i2", 10, WORD },
  { "level 7 i3", 11, WORD },
  { "level 7 i4", 12, WORD },
  { "level 7 i5", 13, WORD },
  { "level 8 l0", 14, WORD },
  { "level 8 l1", 15, WORD },
  { "level 8 l2", 16, WORD },
  { "level 8 l3", 17, WORD },
  { "level 8 l4", 18, WORD },
  { "level 8 l5", 19, WORD },
  { "level 8 l6", 20, WORD },
  { "level 8 l7", 21, WORD },
  { "level 8 i0", 22, WORD },
  { "level 8 i1", 23, WORD },
  { "level 8 i2", 24, WORD },
  { "level 8 i3", 25, WORD },
  { "level 8 i4", 26, WORD },
  { "level 8 i5", 27, WORD },
  { "level 7 l0 frame", 0, WORD },
  { "level 7 l1 frame", 1, WORD },
  { "level 7 l2 frame", 2, WORD },
  { "level 7 l3 frame", 3, WORD },
  { "level 7 l4 frame", 4, WORD },
  { "level 7 l5 frame", 5, WORD },
  { "level 7 l6 frame", 6, WORD },
  { "level 7 l7 frame", 7, WORD },
  { "level 7 i0 frame", 8, WORD },
  { "level 7 i1 frame", 9, WORD },
  { "level 7 i2 frame", 10, WORD },
  { "level 7 i3 frame", 11, WORD },
  { "level 7 i4 frame", 12, WORD },
  { "level 7 i5 frame", 13, WORD },
  { "level 8 l0 frame", 14, WORD },
  { "level 8 l1 frame", 15, WORD },
  { "level 8 l2 frame", 16, WORD },
  { "level 8 l3 frame", 17, WORD },
  { "level 8 l4 frame", 18, WORD },
  { "level 8 l5 frame", 19, WORD },
  { "level 8 l6 frame", 20, WORD },
  { "level 8 l7 frame", 21, WORD },
  { "level 8 i0 frame", 22, WORD },
  { "level 8 i1 frame", 23, WORD },
  { "level 8 i2 frame", 24, WORD },
  { "level 8 i3 frame", 25, WORD },
  { "level 8 i4 frame", 26, WORD },
  { "level 8 i5 frame", 27, WORD }
};

typedef struct {
  void ( *scenario )( void );
  size_t group;
  bool   memory;
  bool   frame;
} WindowsArg;

static void WindowsRun( RegisterCheck *self, void *arg )
{
  const WindowsArg *windows;
  size_t            base;
  size_t            slot;
  size_t            i;

  windows = arg;
  memset( windows_actual, 0, sizeof( windows_actual ) );
  memset( windows_memory, 0, sizeof( windows_memory ) );
  memset( windows_frame, 0, sizeof( windows_frame ) );
  ( *windows->scenario )();
  base = windows->group * PER_GROUP;
  slot = 0;

  for ( i = base; i < base + PER_GROUP; ++i ) {
    RegisterCheckRecord(
      self,
      slot,
      windows_actual[ i ],
      windows_patterns[ i ]
    );
    ++slot;
  }

  /* The window flush trap writes no save area of the innermost level */
  if ( windows->memory ) {
    for ( i = base; i < base + PER_GROUP; ++i ) {
      if ( i < ( LEVELS - 1 ) * REGISTERS ) {
        RegisterCheckRecord(
          self,
          slot,
          windows_memory[ i ],
          windows_patterns[ i ]
        );
        ++slot;
      }
    }
  }

  /* The exception frame holds no register window of the outermost level */
  if ( windows->frame ) {
    for ( i = base; i < base + PER_GROUP; ++i ) {
      if ( i >= REGISTERS ) {
        RegisterCheckRecord(
          self,
          slot,
          windows_frame[ i ],
          windows_patterns[ i ]
        );
        ++slot;
      }
    }
  }
}

static const WindowsArg windows_arg_none_0 = {
  .scenario = WindowsNoneScenario,
  .group = 0,
  .memory = false,
  .frame = false
};

static const WindowsArg windows_arg_none_1 = {
  .scenario = WindowsNoneScenario,
  .group = 1,
  .memory = false,
  .frame = false
};

static const WindowsArg windows_arg_none_2 = {
  .scenario = WindowsNoneScenario,
  .group = 2,
  .memory = false,
  .frame = false
};

static const WindowsArg windows_arg_none_3 = {
  .scenario = WindowsNoneScenario,
  .group = 3,
  .memory = false,
  .frame = false
};

static RegisterCheck windows_checks_none[ GROUPS ] = {
  { .sources = windows_sources_0,
    .source_count = PER_GROUP,
    .slots = windows_slots_none_0,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_none_0 ),
    .initial = &windows_initial[ 0 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_none_0 ) },
  { .sources = windows_sources_1,
    .source_count = PER_GROUP,
    .slots = windows_slots_none_1,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_none_1 ),
    .initial = &windows_initial[ 1 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_none_1 ) },
  { .sources = windows_sources_2,
    .source_count = PER_GROUP,
    .slots = windows_slots_none_2,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_none_2 ),
    .initial = &windows_initial[ 2 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_none_2 ) },
  { .sources = windows_sources_3,
    .source_count = PER_GROUP,
    .slots = windows_slots_none_3,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_none_3 ),
    .initial = &windows_initial[ 3 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_none_3 ) }
};

static const WindowsArg windows_arg_flush_0 = {
  .scenario = WindowsFlushScenario,
  .group = 0,
  .memory = true,
  .frame = false
};

static const WindowsArg windows_arg_flush_1 = {
  .scenario = WindowsFlushScenario,
  .group = 1,
  .memory = true,
  .frame = false
};

static const WindowsArg windows_arg_flush_2 = {
  .scenario = WindowsFlushScenario,
  .group = 2,
  .memory = true,
  .frame = false
};

static const WindowsArg windows_arg_flush_3 = {
  .scenario = WindowsFlushScenario,
  .group = 3,
  .memory = true,
  .frame = false
};

static RegisterCheck windows_checks_flush[ GROUPS ] = {
  { .sources = windows_sources_0,
    .source_count = PER_GROUP,
    .slots = windows_slots_flush_0,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_flush_0 ),
    .initial = &windows_initial[ 0 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_flush_0 ) },
  { .sources = windows_sources_1,
    .source_count = PER_GROUP,
    .slots = windows_slots_flush_1,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_flush_1 ),
    .initial = &windows_initial[ 1 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_flush_1 ) },
  { .sources = windows_sources_2,
    .source_count = PER_GROUP,
    .slots = windows_slots_flush_2,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_flush_2 ),
    .initial = &windows_initial[ 2 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_flush_2 ) },
  { .sources = windows_sources_3,
    .source_count = PER_GROUP,
    .slots = windows_slots_flush_3,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_flush_3 ),
    .initial = &windows_initial[ 3 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_flush_3 ) }
};

static const WindowsArg windows_arg_switch_0 = {
  .scenario = WindowsSwitchScenario,
  .group = 0,
  .memory = false,
  .frame = false
};

static const WindowsArg windows_arg_switch_1 = {
  .scenario = WindowsSwitchScenario,
  .group = 1,
  .memory = false,
  .frame = false
};

static const WindowsArg windows_arg_switch_2 = {
  .scenario = WindowsSwitchScenario,
  .group = 2,
  .memory = false,
  .frame = false
};

static const WindowsArg windows_arg_switch_3 = {
  .scenario = WindowsSwitchScenario,
  .group = 3,
  .memory = false,
  .frame = false
};

static RegisterCheck windows_checks_switch[ GROUPS ] = {
  { .sources = windows_sources_0,
    .source_count = PER_GROUP,
    .slots = windows_slots_switch_0,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_switch_0 ),
    .initial = &windows_initial[ 0 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_switch_0 ) },
  { .sources = windows_sources_1,
    .source_count = PER_GROUP,
    .slots = windows_slots_switch_1,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_switch_1 ),
    .initial = &windows_initial[ 1 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_switch_1 ) },
  { .sources = windows_sources_2,
    .source_count = PER_GROUP,
    .slots = windows_slots_switch_2,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_switch_2 ),
    .initial = &windows_initial[ 2 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_switch_2 ) },
  { .sources = windows_sources_3,
    .source_count = PER_GROUP,
    .slots = windows_slots_switch_3,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_switch_3 ),
    .initial = &windows_initial[ 3 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_switch_3 ) }
};

static const WindowsArg windows_arg_isr_0 = {
  .scenario = WindowsInterruptScenario,
  .group = 0,
  .memory = false,
  .frame = false
};

static const WindowsArg windows_arg_isr_1 = {
  .scenario = WindowsInterruptScenario,
  .group = 1,
  .memory = false,
  .frame = false
};

static const WindowsArg windows_arg_isr_2 = {
  .scenario = WindowsInterruptScenario,
  .group = 2,
  .memory = false,
  .frame = false
};

static const WindowsArg windows_arg_isr_3 = {
  .scenario = WindowsInterruptScenario,
  .group = 3,
  .memory = false,
  .frame = false
};

static RegisterCheck windows_checks_isr[ GROUPS ] = {
  { .sources = windows_sources_0,
    .source_count = PER_GROUP,
    .slots = windows_slots_isr_0,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_isr_0 ),
    .initial = &windows_initial[ 0 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_isr_0 ) },
  { .sources = windows_sources_1,
    .source_count = PER_GROUP,
    .slots = windows_slots_isr_1,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_isr_1 ),
    .initial = &windows_initial[ 1 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_isr_1 ) },
  { .sources = windows_sources_2,
    .source_count = PER_GROUP,
    .slots = windows_slots_isr_2,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_isr_2 ),
    .initial = &windows_initial[ 2 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_isr_2 ) },
  { .sources = windows_sources_3,
    .source_count = PER_GROUP,
    .slots = windows_slots_isr_3,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_isr_3 ),
    .initial = &windows_initial[ 3 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_isr_3 ) }
};

static const WindowsArg windows_arg_exception_0 = {
  .scenario = WindowsExceptionScenario,
  .group = 0,
  .memory = false,
  .frame = true
};

static const WindowsArg windows_arg_exception_1 = {
  .scenario = WindowsExceptionScenario,
  .group = 1,
  .memory = false,
  .frame = true
};

static const WindowsArg windows_arg_exception_2 = {
  .scenario = WindowsExceptionScenario,
  .group = 2,
  .memory = false,
  .frame = true
};

static const WindowsArg windows_arg_exception_3 = {
  .scenario = WindowsExceptionScenario,
  .group = 3,
  .memory = false,
  .frame = true
};

static RegisterCheck windows_checks_exception[ GROUPS ] = {
  { .sources = windows_sources_0,
    .source_count = PER_GROUP,
    .slots = windows_slots_exception_0,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_exception_0 ),
    .initial = &windows_initial[ 0 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_exception_0 ) },
  { .sources = windows_sources_1,
    .source_count = PER_GROUP,
    .slots = windows_slots_exception_1,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_exception_1 ),
    .initial = &windows_initial[ 1 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_exception_1 ) },
  { .sources = windows_sources_2,
    .source_count = PER_GROUP,
    .slots = windows_slots_exception_2,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_exception_2 ),
    .initial = &windows_initial[ 2 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_exception_2 ) },
  { .sources = windows_sources_3,
    .source_count = PER_GROUP,
    .slots = windows_slots_exception_3,
    .slot_count = RTEMS_ARRAY_SIZE( windows_slots_exception_3 ),
    .initial = &windows_initial[ 3 * PER_GROUP ],
    .run = WindowsRun,
    .arg = RTEMS_DECONST( WindowsArg *, &windows_arg_exception_3 ) }
};

static const RegisterCheckSource windows_flush_sources[] = {
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
  { "y", REGISTER_CHECK_INITIAL, WORD },
  { "icc", REGISTER_CHECK_INITIAL, UINT64_C( 0x00f00000 ) }
};

static const RegisterCheckSlot windows_flush_slots[] = {
  { "g1", 0, WORD },
  { "g2", 1, WORD },
  { "g3", 2, WORD },
  { "g4", 3, WORD },
  { "g5", 4, WORD },
  { "o0", 5, WORD },
  { "o1", 6, WORD },
  { "o2", 7, WORD },
  { "o3", 8, WORD },
  { "o4", 9, WORD },
  { "o5", 10, WORD },
  { "o7", 11, WORD },
  { "y", 12, WORD },
  { "icc", 13, UINT64_C( 0x00f00000 ) }
};

static const uint64_t windows_flush_patterns[] = {
  0x3c000000,
  0x3c010101,
  0x3c020202,
  0x3c030303,
  0x3c040404,
  0x3c050505,
  0x3c060606,
  0x3c070707,
  0x3c080808,
  0x3c090909,
  0x3c0a0a0a,
  0x3c0b0b0b,
  0x3c0c0c0c,
  0x00500000
};

static void WindowsFlushRun( RegisterCheck *self, void *arg )
{
  size_t i;

  (void) arg;
  memset( windows_flush_actual, 0, sizeof( windows_flush_actual ) );
  WindowsFlushScenario();

  for ( i = 0; i < RTEMS_ARRAY_SIZE( windows_flush_patterns ); ++i ) {
    RegisterCheckRecord(
      self,
      i,
      windows_flush_actual[ i ],
      windows_flush_patterns[ i ]
    );
  }
}

static RegisterCheck windows_flush_check = {
  .sources = windows_flush_sources,
  .source_count = RTEMS_ARRAY_SIZE( windows_flush_sources ),
  .slots = windows_flush_slots,
  .slot_count = RTEMS_ARRAY_SIZE( windows_flush_slots ),
  .initial = windows_flush_initial,
  .run = WindowsFlushRun
};

static const RegisterCheckSource windows_count_sources[] = {
  { "count", REGISTER_CHECK_EXPECTED, WORD }
};

static const RegisterCheckSlot windows_count_slots[] = {
  { "count", 0, WORD }
};

static uint64_t windows_count_initial[ 1 ];

static void WindowsInterruptCountRun( RegisterCheck *self, void *arg )
{
  (void) arg;
  windows_isr_count = 0;
  WindowsInterruptScenario();
  RegisterCheckRecord( self, 0, windows_isr_count, 1 );
}

static RegisterCheck windows_isr_check = {
  .sources = windows_count_sources,
  .source_count = 1,
  .slots = windows_count_slots,
  .slot_count = 1,
  .initial = windows_count_initial,
  .run = WindowsInterruptCountRun
};

static const RegisterCheckSource windows_number_sources[] = {
  { "windows", REGISTER_CHECK_EXPECTED, WORD }
};

static const RegisterCheckSlot windows_number_slots[] = {
  { "windows", 0, WORD }
};

static uint64_t windows_number_initial[ 1 ];

static void NumberOfWindowsRun( RegisterCheck *self, void *arg )
{
  rtems_interrupt_level level;
  uint32_t              wim;
  uint32_t              all;

  (void) arg;
  rtems_interrupt_local_disable( level );
  __asm__ volatile( "rd %%wim, %0\n"
                    "wr %%g0, -1, %%wim\n"
                    "nop\n"
                    "nop\n"
                    "nop\n"
                    "rd %%wim, %1\n"
                    "wr %0, 0, %%wim\n"
                    "nop\n"
                    "nop\n"
                    "nop\n"
                    : "=&r"( wim ), "=&r"( all ) );
  rtems_interrupt_local_enable( level );
  (void) wim;

  /* The WIM implements one bit for each register window */
  RegisterCheckRecord(
    self,
    0,
    SPARC_NUMBER_OF_REGISTER_WINDOWS,
    (uint32_t) __builtin_popcount( all )
  );
}

static RegisterCheck windows_number_check = {
  .sources = windows_number_sources,
  .source_count = RTEMS_ARRAY_SIZE( windows_number_sources ),
  .slots = windows_number_slots,
  .slot_count = RTEMS_ARRAY_SIZE( windows_number_slots ),
  .initial = windows_number_initial,
  .run = NumberOfWindowsRun
};

static void WindowsSetup( void )
{
  memcpy( windows_initial, windows_patterns, sizeof( windows_patterns ) );
  memcpy(
    windows_flush_initial,
    windows_flush_patterns,
    sizeof( windows_flush_patterns )
  );
}

static void WindowsRunAll( RegisterCheck *checks )
{
  size_t g;

  WindowsSetup();

  for ( g = 0; g < GROUPS; ++g ) {
    RegisterCheckRun( &checks[ g ] );
  }
}

static void WindowsVerify( RegisterCheck *checks, size_t first, size_t last )
{
  size_t g;

  for ( g = 0; g < GROUPS; ++g ) {
    size_t slot;

    for (
      slot = first; slot < last && slot < checks[ g ].slot_count; ++slot
    ) {
      RegisterCheckVerify( &checks[ g ], slot );
    }
  }
}

static void WindowsReport( RegisterCheck *checks )
{
  size_t g;

  for ( g = 0; g < GROUPS; ++g ) {
    RegisterCheckReport( &checks[ g ] );
  }
}

/**
 * @brief Enter a chain of 8 nested register windows. Load a pattern into each
 *   local register and each input register except I6 and I7 of each level. The
 *   chain uses more register windows than the processor provides. Store the
 *   registers of each level before its restore. Run this through the register
 *   check, once for each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValWindows_Action_0( void )
{
  WindowsRunAll( windows_checks_none );

  /*
   * Check that each level of the chain has the values which it loaded before
   * its restore.
   */
  WindowsVerify( windows_checks_none, 0, PER_GROUP );

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  WindowsReport( windows_checks_none );
}

/**
 * @brief Enter a chain of 8 nested register windows. Load a pattern into each
 *   local register and each input register except I6 and I7 of each level.
 *   Load a pattern into each global register, each output register except O6,
 *   Y, and the integer condition codes. Execute the window flush trap in the
 *   innermost level. Store the global and the output registers, Y, and the
 *   PSR. Copy the save area of each other level. Store the registers of each
 *   level before its restore. Run this through the register check, once for
 *   each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValWindows_Action_1( void )
{
  WindowsRunAll( windows_checks_flush );
  RegisterCheckRun( &windows_flush_check );

  /*
   * Check that each level of the chain has the values which it loaded before
   * its restore.
   */
  WindowsVerify( windows_checks_flush, 0, PER_GROUP );

  /*
   * Check that the window flush trap wrote the registers of each level except
   * the innermost level to the save area of the level.
   */
  WindowsVerify( windows_checks_flush, PER_GROUP, 2 * PER_GROUP );

  /*
   * Check that the window flush trap preserved the global and the output
   * registers, Y, and the integer condition codes.
   */
  for ( size_t i = 0; i < windows_flush_check.slot_count; ++i ) {
    RegisterCheckVerify( &windows_flush_check, i );
  }

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  WindowsReport( windows_checks_flush );
  RegisterCheckReport( &windows_flush_check );
}

/**
 * @brief Enter a chain of 8 nested register windows. Load a pattern into each
 *   local register and each input register except I6 and I7 of each level.
 *   Resume a worker task of higher priority on the current processor in the
 *   innermost level. The worker uses more register windows than the processor
 *   provides and suspends itself. Store the registers of each level before its
 *   restore. Run this through the register check, once for each value
 *   inverted, then once unchanged.
 */
static void ScoreCpuSparcValWindows_Action_2( void )
{
  SetSelfPriority( PRIO_NORMAL );
  windows_worker = CreateTask( "WORK", PRIO_HIGH );
  #if defined( RTEMS_SMP )
  SetAffinityOne( windows_worker, rtems_scheduler_get_processor() );
  #endif
  StartTask( windows_worker, WindowsWorker, NULL );
  WindowsRunAll( windows_checks_switch );
  DeleteTask( windows_worker );
  RestoreRunnerPriority();

  /*
   * Check that each level of the chain has the values which it loaded before
   * its restore.
   */
  WindowsVerify( windows_checks_switch, 0, PER_GROUP );

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  WindowsReport( windows_checks_switch );
}

/**
 * @brief Enter a chain of 8 nested register windows. Load a pattern into each
 *   local register and each input register except I6 and I7 of each level.
 *   Disable interrupts, raise an interrupt, and enable interrupts in the
 *   innermost level. The interrupt handler uses more register windows than the
 *   processor provides and counts its call. Wait for the count. Store the
 *   registers of each level before its restore. Run this through the register
 *   check, once for each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValWindows_Action_3( void )
{
  WindowsRunAll( windows_checks_isr );
  RegisterCheckRun( &windows_isr_check );

  /*
   * Check that each level of the chain has the values which it loaded before
   * its restore.
   */
  WindowsVerify( windows_checks_isr, 0, PER_GROUP );

  /*
   * Check that the interrupt handler ran once.
   */
  RegisterCheckVerify( &windows_isr_check, 0 );

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  WindowsReport( windows_checks_isr );
  RegisterCheckReport( &windows_isr_check );
}

/**
 * @brief Enter a chain of 8 nested register windows. Load a pattern into each
 *   local register and each input register except I6 and I7 of each level.
 *   Execute an illegal instruction in the innermost level. Store the register
 *   windows of the exception frame. Continue after the trapping instruction.
 *   Store the registers of each level before its restore. Run this through the
 *   register check, once for each value inverted, then once unchanged.
 */
static void ScoreCpuSparcValWindows_Action_4( void )
{
  WindowsRunAll( windows_checks_exception );

  /*
   * Check that each level of the chain has the values which it loaded before
   * its restore.
   */
  WindowsVerify( windows_checks_exception, 0, PER_GROUP );

  /*
   * Check that the exception frame holds the register window of each level
   * except the outermost level.
   */
  WindowsVerify( windows_checks_exception, PER_GROUP, 2 * PER_GROUP );

  /*
   * Check that each run with a changed value flagged exactly the checks of
   * that value, and that each run recorded every check.
   */
  WindowsReport( windows_checks_exception );
}

/**
 * @brief Disable interrupts. Write ones to all bits of the WIM and read it
 *   back. Restore the WIM and enable interrupts. The WIM implements one bit
 *   for each register window, so the bits which read back as one count the
 *   register windows of the processor. Run this through the register check,
 *   once with the expected count inverted, then once unchanged.
 */
static void ScoreCpuSparcValWindows_Action_5( void )
{
  RegisterCheckRun( &windows_number_check );

  /*
   * Check that SPARC_NUMBER_OF_REGISTER_WINDOWS is equal to the number of
   * register windows which the processor implements.
   */
  RegisterCheckVerify( &windows_number_check, 0 );

  /*
   * Check that the run with the changed value flagged exactly the check of
   * that value, and that each run recorded the check.
   */
  RegisterCheckReport( &windows_number_check );
}

/**
 * @fn void T_case_body_ScoreCpuSparcValWindows( void )
 */
T_TEST_CASE( ScoreCpuSparcValWindows )
{
  ScoreCpuSparcValWindows_Action_0();
  ScoreCpuSparcValWindows_Action_1();
  ScoreCpuSparcValWindows_Action_2();
  ScoreCpuSparcValWindows_Action_3();
  ScoreCpuSparcValWindows_Action_4();
  ScoreCpuSparcValWindows_Action_5();
}

/** @} */
