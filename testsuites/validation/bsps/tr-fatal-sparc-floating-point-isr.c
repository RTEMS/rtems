/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuSparcValFatalFloatingPointIsr
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

#include <rtems/score/interr.h>

#include "tr-fatal-sparc-floating-point-isr.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuSparcValFatalFloatingPointIsr \
 *   spec:/score/cpu/sparc/val/fatal-floating-point-isr
 *
 * @ingroup TestsuitesBspsFatalSparcFloatingPointIsr
 *
 * @brief Tests a fatal error.
 *
 * This test case performs the following actions:
 *
 * - The idle task body FatalFloatingPointIsrIdleBody() calls a handler within
 *   an interrupt. The handler executes a floating-point instruction.
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the expected fatal code is present.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/cpu/sparc/val/fatal-floating-point-isr
 *   test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreCpuSparcValFatalFloatingPointIsr_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreCpuSparcValFatalFloatingPointIsr_Run() parameter.
   */
  rtems_fatal_code code;
} ScoreCpuSparcValFatalFloatingPointIsr_Context;

static ScoreCpuSparcValFatalFloatingPointIsr_Context
  ScoreCpuSparcValFatalFloatingPointIsr_Instance;

static void FloatingPointWithinIsr( void *arg )
{
  (void) arg;
  __asm__ volatile( "fmovs %%f0, %%f0"
                    :
                    :
                    : "f0" );
}

void *FatalFloatingPointIsrIdleBody( uintptr_t ignored )
{
  (void) ignored;
  CallWithinISR( FloatingPointWithinIsr, NULL );
  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, 1 );
}

static T_fixture ScoreCpuSparcValFatalFloatingPointIsr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreCpuSparcValFatalFloatingPointIsr_Instance
};

/**
 * @brief The idle task body FatalFloatingPointIsrIdleBody() calls a handler
 *   within an interrupt. The handler executes a floating-point instruction.
 */
static void ScoreCpuSparcValFatalFloatingPointIsr_Action_0(
  ScoreCpuSparcValFatalFloatingPointIsr_Context *ctx
)
{
  /* Nothing to do */

  /*
   * Check that the expected fatal source is present.
   */
  T_step_eq_int( 0, ctx->source, INTERNAL_ERROR_CORE );

  /*
   * Check that the expected fatal code is present.
   */
  T_step_eq_ulong(
    1,
    ctx->code,
    INTERNAL_ERROR_ILLEGAL_USE_OF_FLOATING_POINT_UNIT
  );
}

void ScoreCpuSparcValFatalFloatingPointIsr_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  ScoreCpuSparcValFatalFloatingPointIsr_Context *ctx;

  ctx = &ScoreCpuSparcValFatalFloatingPointIsr_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin(
    "ScoreCpuSparcValFatalFloatingPointIsr",
    &ScoreCpuSparcValFatalFloatingPointIsr_Fixture
  );

  T_plan( 2 );

  ScoreCpuSparcValFatalFloatingPointIsr_Action_0( ctx );

  T_case_end();
}

/** @} */
