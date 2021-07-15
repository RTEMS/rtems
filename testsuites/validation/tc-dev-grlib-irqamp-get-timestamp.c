/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup DevGrlibReqIrqampGetTimestamp
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include <grlib/irqamp.h>

#include <rtems/test.h>

/**
 * @defgroup DevGrlibReqIrqampGetTimestamp \
 *   spec:/dev/grlib/req/irqamp-get-timestamp
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @{
 */

typedef enum {
  DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_Zero,
  DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_Positive,
  DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_NA
} DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets;

typedef enum {
  DevGrlibReqIrqampGetTimestamp_Post_Result_Registers,
  DevGrlibReqIrqampGetTimestamp_Post_Result_Null,
  DevGrlibReqIrqampGetTimestamp_Post_Result_NA
} DevGrlibReqIrqampGetTimestamp_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_NumberOfTimestampRegisterSets_NA : 1;
  uint8_t Post_Result : 2;
} DevGrlibReqIrqampGetTimestamp_Entry;

/**
 * @brief Test context for spec:/dev/grlib/req/irqamp-get-timestamp test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the
   *   irqamp_get_timestamp_registers() call.
   */
  irqamp_timestamp *result;

  /**
   * @brief This member contains the IRQ(A)MP register block.
   */
  irqamp irqamp_regs;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    DevGrlibReqIrqampGetTimestamp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} DevGrlibReqIrqampGetTimestamp_Context;

static DevGrlibReqIrqampGetTimestamp_Context
  DevGrlibReqIrqampGetTimestamp_Instance;

static const char * const DevGrlibReqIrqampGetTimestamp_PreDesc_NumberOfTimestampRegisterSets[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const * const DevGrlibReqIrqampGetTimestamp_PreDesc[] = {
  DevGrlibReqIrqampGetTimestamp_PreDesc_NumberOfTimestampRegisterSets,
  NULL
};

static void
DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_Prepare(
  DevGrlibReqIrqampGetTimestamp_Context                          *ctx,
  DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets state
)
{
  switch ( state ) {
    case DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_Zero: {
      /*
       * While the number of timestamp register sets indicated by the IRQ(A)MP
       * register block specified by the ``irqamp_regs`` parameter is zero.
       */
      ctx->irqamp_regs.itstmp[ 0 ].itstmpc = 0;
      break;
    }

    case DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_Positive: {
      /*
       * While the number of timestamp register sets indicated by the IRQ(A)MP
       * register block specified by the ``irqamp_regs`` parameter is positive.
       */
      ctx->irqamp_regs.itstmp[ 0 ].itstmpc = IRQAMP_ITSTMPC_TSTAMP( 1 );
      break;
    }

    case DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_NA:
      break;
  }
}

static void DevGrlibReqIrqampGetTimestamp_Post_Result_Check(
  DevGrlibReqIrqampGetTimestamp_Context    *ctx,
  DevGrlibReqIrqampGetTimestamp_Post_Result state
)
{
  switch ( state ) {
    case DevGrlibReqIrqampGetTimestamp_Post_Result_Registers: {
      /*
       * The return value of irqamp_get_timestamp_registers() shall be address
       * of the timestamping register block contained in the IRQ(A)MP register
       * block specified by the ``irqamp_regs`` parameter.
       */
      T_not_null( ctx->result );
      break;
    }

    case DevGrlibReqIrqampGetTimestamp_Post_Result_Null: {
      /*
       * The return value of irqamp_get_timestamp_registers() shall be false.
       */
      T_null( ctx->result );
      break;
    }

    case DevGrlibReqIrqampGetTimestamp_Post_Result_NA:
      break;
  }
}

static void DevGrlibReqIrqampGetTimestamp_Action(
  DevGrlibReqIrqampGetTimestamp_Context *ctx
)
{
  ctx->result = irqamp_get_timestamp_registers( &ctx->irqamp_regs );
}

static const DevGrlibReqIrqampGetTimestamp_Entry
DevGrlibReqIrqampGetTimestamp_Entries[] = {
  { 0, 0, DevGrlibReqIrqampGetTimestamp_Post_Result_Null },
  { 0, 0, DevGrlibReqIrqampGetTimestamp_Post_Result_Registers }
};

static const uint8_t
DevGrlibReqIrqampGetTimestamp_Map[] = {
  0, 1
};

static size_t DevGrlibReqIrqampGetTimestamp_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  DevGrlibReqIrqampGetTimestamp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      DevGrlibReqIrqampGetTimestamp_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture DevGrlibReqIrqampGetTimestamp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = DevGrlibReqIrqampGetTimestamp_Scope,
  .initial_context = &DevGrlibReqIrqampGetTimestamp_Instance
};

static inline DevGrlibReqIrqampGetTimestamp_Entry
DevGrlibReqIrqampGetTimestamp_PopEntry(
  DevGrlibReqIrqampGetTimestamp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return DevGrlibReqIrqampGetTimestamp_Entries[
    DevGrlibReqIrqampGetTimestamp_Map[ index ]
  ];
}

static void DevGrlibReqIrqampGetTimestamp_TestVariant(
  DevGrlibReqIrqampGetTimestamp_Context *ctx
)
{
  DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  DevGrlibReqIrqampGetTimestamp_Action( ctx );
  DevGrlibReqIrqampGetTimestamp_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

/**
 * @fn void T_case_body_DevGrlibReqIrqampGetTimestamp( void )
 */
T_TEST_CASE_FIXTURE(
  DevGrlibReqIrqampGetTimestamp,
  &DevGrlibReqIrqampGetTimestamp_Fixture
)
{
  DevGrlibReqIrqampGetTimestamp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_Zero;
    ctx->Map.pcs[ 0 ] < DevGrlibReqIrqampGetTimestamp_Pre_NumberOfTimestampRegisterSets_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = DevGrlibReqIrqampGetTimestamp_PopEntry( ctx );
    DevGrlibReqIrqampGetTimestamp_TestVariant( ctx );
  }
}

/** @} */
