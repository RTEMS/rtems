/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsPartValPerf
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include <rtems.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsPartValPerf spec:/rtems/part/val/perf
 *
 * @ingroup TestsuitesPerformanceNoClock0
 *
 * @brief This test case provides a context to run @ref RTEMSAPIClassicPart
 *   performance tests.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/part/val/perf test case.
 */
typedef struct {
  /**
   * @brief This member provides a partition with exactly one free buffer.
   */
  rtems_id part_one;

  /**
   * @brief This member provides a partition with exactly 100 free buffers.
   */
  rtems_id part_many;

  /**
   * @brief This member provides a status code.
   */
  rtems_status_code status;

  /**
   * @brief This member provides a buffer pointer.
   */
  void *buffer;

  /**
   * @brief This member provides a second buffer pointer.
   */
  void *buffer_2;

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
} RtemsPartValPerf_Context;

static RtemsPartValPerf_Context
  RtemsPartValPerf_Instance;

static void RtemsPartValPerf_Setup_Context( RtemsPartValPerf_Context *ctx )
{
  T_measure_runtime_config config;

  memset( &config, 0, sizeof( config ) );
  config.sample_count = 100;
  ctx->request.arg = ctx;
  ctx->request.flags = T_MEASURE_RUNTIME_REPORT_SAMPLES;
  ctx->context = T_measure_runtime_create( &config );
  T_assert_not_null( ctx->context );
}

/**
 * @brief Creates the test partition.
 */
static void RtemsPartValPerf_Setup( RtemsPartValPerf_Context *ctx )
{
  rtems_status_code sc;
  size_t            size;
  void             *area;
  uintptr_t         length;

  size = 2 * sizeof( uintptr_t );

  area = T_malloc( size );
  sc = rtems_partition_create(
    rtems_build_name( ' ', 'O', 'N', 'E' ),
    area,
    size,
    size,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->part_one
  );
  T_assert_rsc_success( sc );

  length = 100 * size;
  area = T_malloc( length );
  sc = rtems_partition_create(
    rtems_build_name( 'M', 'A', 'N', 'Y' ),
    area,
    length,
    size,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->part_many
  );
  T_assert_rsc_success( sc );
}

static void RtemsPartValPerf_Setup_Wrap( void *arg )
{
  RtemsPartValPerf_Context *ctx;

  ctx = arg;
  RtemsPartValPerf_Setup_Context( ctx );
  RtemsPartValPerf_Setup( ctx );
}

/**
 * @brief Deletes the test partition.
 */
static void RtemsPartValPerf_Teardown( RtemsPartValPerf_Context *ctx )
{
  rtems_status_code sc;

  if ( ctx->part_one != 0 ) {
    sc = rtems_partition_delete( ctx->part_one );
    T_rsc_success( sc );
  }

  if ( ctx->part_many != 0 ) {
    sc = rtems_partition_delete( ctx->part_many );
    T_rsc_success( sc );
  }
}

static void RtemsPartValPerf_Teardown_Wrap( void *arg )
{
  RtemsPartValPerf_Context *ctx;

  ctx = arg;
  RtemsPartValPerf_Teardown( ctx );
}

static T_fixture RtemsPartValPerf_Fixture = {
  .setup = RtemsPartValPerf_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsPartValPerf_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsPartValPerf_Instance
};

/**
 * @defgroup RtemsPartReqPerfGetBuffer spec:/rtems/part/req/perf-get-buffer
 *
 * @{
 */

/**
 * @brief Get a buffer.
 */
static void RtemsPartReqPerfGetBuffer_Body( RtemsPartValPerf_Context *ctx )
{
  ctx->status = rtems_partition_get_buffer( ctx->part_many, &ctx->buffer );
}

static void RtemsPartReqPerfGetBuffer_Body_Wrap( void *arg )
{
  RtemsPartValPerf_Context *ctx;

  ctx = arg;
  RtemsPartReqPerfGetBuffer_Body( ctx );
}

/**
 * @brief Return the buffer.
 */
static bool RtemsPartReqPerfGetBuffer_Teardown(
  RtemsPartValPerf_Context *ctx,
  uint32_t                  tic,
  uint32_t                  toc
)
{
  rtems_status_code sc;

  T_quiet_rsc_success( ctx->status );

  sc = rtems_partition_return_buffer( ctx->part_many, ctx->buffer );
  T_quiet_rsc_success( sc );

  return tic == toc;
}

static bool RtemsPartReqPerfGetBuffer_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsPartValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return RtemsPartReqPerfGetBuffer_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup RtemsPartReqPerfGetNoBuffer \
 *   spec:/rtems/part/req/perf-get-no-buffer
 *
 * @{
 */

/**
 * @brief Get the buffer.
 */
static void RtemsPartReqPerfGetNoBuffer_Prepare(
  RtemsPartValPerf_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_partition_get_buffer( ctx->part_one, &ctx->buffer_2 );
  T_rsc_success( sc );
}

/**
 * @brief Try to get a buffer.
 */
static void RtemsPartReqPerfGetNoBuffer_Body( RtemsPartValPerf_Context *ctx )
{
  ctx->status = rtems_partition_get_buffer( ctx->part_one, &ctx->buffer );
}

static void RtemsPartReqPerfGetNoBuffer_Body_Wrap( void *arg )
{
  RtemsPartValPerf_Context *ctx;

  ctx = arg;
  RtemsPartReqPerfGetNoBuffer_Body( ctx );
}

/**
 * @brief Check the status code.
 */
static bool RtemsPartReqPerfGetNoBuffer_Teardown(
  RtemsPartValPerf_Context *ctx,
  uint32_t                  tic,
  uint32_t                  toc
)
{
  T_quiet_rsc( ctx->status, RTEMS_UNSATISFIED );

  return tic == toc;
}

static bool RtemsPartReqPerfGetNoBuffer_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsPartValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return RtemsPartReqPerfGetNoBuffer_Teardown( ctx, tic, toc );
}

/**
 * @brief Return the buffer.
 */
static void RtemsPartReqPerfGetNoBuffer_Cleanup(
  RtemsPartValPerf_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_partition_return_buffer( ctx->part_one, ctx->buffer_2 );
  T_rsc_success( sc );
}

/** @} */

/**
 * @defgroup RtemsPartReqPerfReturnBuffer \
 *   spec:/rtems/part/req/perf-return-buffer
 *
 * @{
 */

/**
 * @brief Get the buffer.
 */
static void RtemsPartReqPerfReturnBuffer_Setup( RtemsPartValPerf_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_partition_get_buffer( ctx->part_many, &ctx->buffer );
  T_quiet_rsc_success( sc );
}

static void RtemsPartReqPerfReturnBuffer_Setup_Wrap( void *arg )
{
  RtemsPartValPerf_Context *ctx;

  ctx = arg;
  RtemsPartReqPerfReturnBuffer_Setup( ctx );
}

/**
 * @brief Return the buffer.
 */
static void RtemsPartReqPerfReturnBuffer_Body( RtemsPartValPerf_Context *ctx )
{
  ctx->status = rtems_partition_return_buffer( ctx->part_many, ctx->buffer );
}

static void RtemsPartReqPerfReturnBuffer_Body_Wrap( void *arg )
{
  RtemsPartValPerf_Context *ctx;

  ctx = arg;
  RtemsPartReqPerfReturnBuffer_Body( ctx );
}

/**
 * @brief Check the status code.
 */
static bool RtemsPartReqPerfReturnBuffer_Teardown(
  RtemsPartValPerf_Context *ctx,
  uint32_t                  tic,
  uint32_t                  toc
)
{
  T_quiet_rsc( ctx->status, RTEMS_SUCCESSFUL );

  return tic == toc;
}

static bool RtemsPartReqPerfReturnBuffer_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsPartValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return RtemsPartReqPerfReturnBuffer_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @fn void T_case_body_RtemsPartValPerf( void )
 */
T_TEST_CASE_FIXTURE( RtemsPartValPerf, &RtemsPartValPerf_Fixture )
{
  RtemsPartValPerf_Context *ctx;

  ctx = T_fixture_context();

  ctx->request.name = "RtemsPartReqPerfGetBuffer";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsPartReqPerfGetBuffer_Body_Wrap;
  ctx->request.teardown = RtemsPartReqPerfGetBuffer_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  RtemsPartReqPerfGetNoBuffer_Prepare( ctx );
  ctx->request.name = "RtemsPartReqPerfGetNoBuffer";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsPartReqPerfGetNoBuffer_Body_Wrap;
  ctx->request.teardown = RtemsPartReqPerfGetNoBuffer_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsPartReqPerfGetNoBuffer_Cleanup( ctx );

  ctx->request.name = "RtemsPartReqPerfReturnBuffer";
  ctx->request.setup = RtemsPartReqPerfReturnBuffer_Setup_Wrap;
  ctx->request.body = RtemsPartReqPerfReturnBuffer_Body_Wrap;
  ctx->request.teardown = RtemsPartReqPerfReturnBuffer_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
}

/** @} */
