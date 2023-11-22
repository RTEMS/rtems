/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSemReqSeizeTry
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

#include "tr-sem-seize-try.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreSemReqSeizeTry spec:/score/sem/req/seize-try
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Count_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Count : 2;
} ScoreSemReqSeizeTry_Entry;

/**
 * @brief Test context for spec:/score/sem/req/seize-try test case.
 */
typedef struct {
  /**
   * @brief This member specifies the semaphore count before the directive
   *   call.
   */
  uint32_t count_before;

  /**
   * @brief This member contains the return status of the directive call.
   */
  Status_Control status;

  /**
   * @brief This member contains the semaphore count after the directive call.
   */
  uint32_t count_after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreSemReqSeizeTry_Run() parameter.
   */
  TQSemContext *tq_ctx;

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
    ScoreSemReqSeizeTry_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreSemReqSeizeTry_Context;

static ScoreSemReqSeizeTry_Context
  ScoreSemReqSeizeTry_Instance;

static const char * const ScoreSemReqSeizeTry_PreDesc_Count[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const * const ScoreSemReqSeizeTry_PreDesc[] = {
  ScoreSemReqSeizeTry_PreDesc_Count,
  NULL
};

typedef ScoreSemReqSeizeTry_Context Context;

static Status_Control Status( const Context *ctx, Status_Control status )
{
  return TQConvertStatus( &ctx->tq_ctx->base, status );
}

static void ScoreSemReqSeizeTry_Pre_Count_Prepare(
  ScoreSemReqSeizeTry_Context  *ctx,
  ScoreSemReqSeizeTry_Pre_Count state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeTry_Pre_Count_Zero: {
      /*
       * While the count of the semaphore is zero.
       */
      ctx->count_before = 0;
      break;
    }

    case ScoreSemReqSeizeTry_Pre_Count_Positive: {
      /*
       * While the count of the semaphore is greater than zero.
       */
      ctx->count_before = 1;
      break;
    }

    case ScoreSemReqSeizeTry_Pre_Count_NA:
      break;
  }
}

static void ScoreSemReqSeizeTry_Post_Status_Check(
  ScoreSemReqSeizeTry_Context    *ctx,
  ScoreSemReqSeizeTry_Post_Status state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeTry_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      T_eq_int( ctx->status, Status( ctx, STATUS_SUCCESSFUL ) );
      break;
    }

    case ScoreSemReqSeizeTry_Post_Status_Unsat: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_UNSATISFIED.
       */
      T_eq_int( ctx->status, Status( ctx, STATUS_UNSATISFIED ) );
      break;
    }

    case ScoreSemReqSeizeTry_Post_Status_NA:
      break;
  }
}

static void ScoreSemReqSeizeTry_Post_Count_Check(
  ScoreSemReqSeizeTry_Context   *ctx,
  ScoreSemReqSeizeTry_Post_Count state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeTry_Post_Count_Nop: {
      /*
       * The count of the semaphore shall not be modified.
       */
      T_eq_u32( ctx->count_after, ctx->count_before );
      break;
    }

    case ScoreSemReqSeizeTry_Post_Count_MinusOne: {
      /*
       * The count of the semaphore shall be decremented by one.
       */
      T_eq_u32( ctx->count_after, ctx->count_before - 1 );
      break;
    }

    case ScoreSemReqSeizeTry_Post_Count_NA:
      break;
  }
}

static void ScoreSemReqSeizeTry_Action( ScoreSemReqSeizeTry_Context *ctx )
{
  TQSemSetCount( ctx->tq_ctx, ctx->count_before );
  ctx->status = TQEnqueue( &ctx->tq_ctx->base, TQ_NO_WAIT );
  ctx->count_after = TQSemGetCount( ctx->tq_ctx );
}

static const ScoreSemReqSeizeTry_Entry
ScoreSemReqSeizeTry_Entries[] = {
  { 0, 0, ScoreSemReqSeizeTry_Post_Status_Unsat,
    ScoreSemReqSeizeTry_Post_Count_Nop },
  { 0, 0, ScoreSemReqSeizeTry_Post_Status_Ok,
    ScoreSemReqSeizeTry_Post_Count_MinusOne }
};

static const uint8_t
ScoreSemReqSeizeTry_Map[] = {
  0, 1
};

static size_t ScoreSemReqSeizeTry_Scope( void *arg, char *buf, size_t n )
{
  ScoreSemReqSeizeTry_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreSemReqSeizeTry_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreSemReqSeizeTry_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreSemReqSeizeTry_Scope,
  .initial_context = &ScoreSemReqSeizeTry_Instance
};

static inline ScoreSemReqSeizeTry_Entry ScoreSemReqSeizeTry_PopEntry(
  ScoreSemReqSeizeTry_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreSemReqSeizeTry_Entries[
    ScoreSemReqSeizeTry_Map[ index ]
  ];
}

static void ScoreSemReqSeizeTry_TestVariant( ScoreSemReqSeizeTry_Context *ctx )
{
  ScoreSemReqSeizeTry_Pre_Count_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreSemReqSeizeTry_Action( ctx );
  ScoreSemReqSeizeTry_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  ScoreSemReqSeizeTry_Post_Count_Check( ctx, ctx->Map.entry.Post_Count );
}

static T_fixture_node ScoreSemReqSeizeTry_Node;

static T_remark ScoreSemReqSeizeTry_Remark = {
  .next = NULL,
  .remark = "ScoreSemReqSeizeTry"
};

void ScoreSemReqSeizeTry_Run( TQSemContext *tq_ctx )
{
  ScoreSemReqSeizeTry_Context *ctx;

  ctx = &ScoreSemReqSeizeTry_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreSemReqSeizeTry_Node,
    &ScoreSemReqSeizeTry_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreSemReqSeizeTry_Pre_Count_Zero;
    ctx->Map.pcs[ 0 ] < ScoreSemReqSeizeTry_Pre_Count_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = ScoreSemReqSeizeTry_PopEntry( ctx );
    ScoreSemReqSeizeTry_TestVariant( ctx );
  }

  T_add_remark( &ScoreSemReqSeizeTry_Remark );
  T_pop_fixture();
}

/** @} */
