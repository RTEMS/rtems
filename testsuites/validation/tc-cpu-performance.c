/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCpuValPerf
 */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
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

#include <rtems/score/cpuimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCpuValPerf spec:/score/cpu/val/perf
 *
 * @ingroup TestsuitesPerformanceNoClock0
 *
 * @brief This test case provides a context to run CPU port performance tests.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/cpu/val/perf test case.
 */
typedef struct {
  /**
   * @brief This member references the measure runtime context.
   */
  T_measure_runtime_context *context;

  /**
   * @brief This member provides the measure runtime request.
   */
  T_measure_runtime_request request;

  /**
   * @brief This member provides an optional measurement begin time point.
   */
  T_ticks begin;

  /**
   * @brief This member provides an optional measurement end time point.
   */
  T_ticks end;
} ScoreCpuValPerf_Context;

static ScoreCpuValPerf_Context
  ScoreCpuValPerf_Instance;

static void ScoreCpuValPerf_Setup_Context( ScoreCpuValPerf_Context *ctx )
{
  T_measure_runtime_config config;

  memset( &config, 0, sizeof( config ) );
  config.sample_count = 100;
  ctx->request.arg = ctx;
  ctx->request.flags = T_MEASURE_RUNTIME_REPORT_SAMPLES;
  ctx->context = T_measure_runtime_create( &config );
  T_assert_not_null( ctx->context );
}

static void ScoreCpuValPerf_Setup_Wrap( void *arg )
{
  ScoreCpuValPerf_Context *ctx;

  ctx = arg;
  ScoreCpuValPerf_Setup_Context( ctx );
}

static T_fixture ScoreCpuValPerf_Fixture = {
  .setup = ScoreCpuValPerf_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreCpuValPerf_Instance
};

/**
 * @defgroup ScoreCpuReqPerfEmpty spec:/score/cpu/req/perf-empty
 *
 * @{
 */

/**
 * @brief Do nothing and just return.
 */
static void ScoreCpuReqPerfEmpty_Body( ScoreCpuValPerf_Context *ctx )
{
  /* No code */
}

static void ScoreCpuReqPerfEmpty_Body_Wrap( void *arg )
{
  ScoreCpuValPerf_Context *ctx;

  ctx = arg;
  ScoreCpuReqPerfEmpty_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreCpuReqPerfEmpty_Teardown(
  ScoreCpuValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  return tic == toc;
}

static bool ScoreCpuReqPerfEmpty_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreCpuValPerf_Context *ctx;

  ctx = arg;
  return ScoreCpuReqPerfEmpty_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @defgroup ScoreCpuReqPerfNops spec:/score/cpu/req/perf-nops
 *
 * @{
 */

/**
 * @brief Execute exactly 1000 no-operation instructions.
 */
static void ScoreCpuReqPerfNops_Body( ScoreCpuValPerf_Context *ctx )
{
  #define NOPS_10 \
    _CPU_Instruction_no_operation(); _CPU_Instruction_no_operation(); \
    _CPU_Instruction_no_operation(); _CPU_Instruction_no_operation(); \
    _CPU_Instruction_no_operation(); _CPU_Instruction_no_operation(); \
    _CPU_Instruction_no_operation(); _CPU_Instruction_no_operation(); \
    _CPU_Instruction_no_operation(); _CPU_Instruction_no_operation();
  #define NOPS_100 NOPS_10 NOPS_10 NOPS_10 NOPS_10 NOPS_10 NOPS_10 NOPS_10 \
    NOPS_10 NOPS_10 NOPS_10
  NOPS_100
  NOPS_100
  NOPS_100
  NOPS_100
  NOPS_100
  NOPS_100
  NOPS_100
  NOPS_100
  NOPS_100
  NOPS_100
}

static void ScoreCpuReqPerfNops_Body_Wrap( void *arg )
{
  ScoreCpuValPerf_Context *ctx;

  ctx = arg;
  ScoreCpuReqPerfNops_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreCpuReqPerfNops_Teardown(
  ScoreCpuValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  return tic == toc;
}

static bool ScoreCpuReqPerfNops_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreCpuValPerf_Context *ctx;

  ctx = arg;
  return ScoreCpuReqPerfNops_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @fn void T_case_body_ScoreCpuValPerf( void )
 */
T_TEST_CASE_FIXTURE( ScoreCpuValPerf, &ScoreCpuValPerf_Fixture )
{
  ScoreCpuValPerf_Context *ctx;

  ctx = T_fixture_context();

  ctx->request.name = "ScoreCpuReqPerfEmpty";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreCpuReqPerfEmpty_Body_Wrap;
  ctx->request.teardown = ScoreCpuReqPerfEmpty_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "ScoreCpuReqPerfNops";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreCpuReqPerfNops_Body_Wrap;
  ctx->request.teardown = ScoreCpuReqPerfNops_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
}

/** @} */
