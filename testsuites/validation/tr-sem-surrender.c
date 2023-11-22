/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSemReqSurrender
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

#include "tr-sem-surrender.h"
#include "tr-tq-surrender.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreSemReqSurrender spec:/score/sem/req/surrender
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Variant_NA : 1;
  uint16_t Pre_Discipline_NA : 1;
  uint16_t Pre_Count_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Surrender : 2;
  uint16_t Post_Count : 3;
} ScoreSemReqSurrender_Entry;

/**
 * @brief Test context for spec:/score/sem/req/surrender test case.
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
   * @brief If this member is true, then there shall be threads blocked on the
   *   semaphore.
   */
  bool blocked;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreSemReqSurrender_Run() parameter.
   */
  TQSemContext *tq_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    ScoreSemReqSurrender_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreSemReqSurrender_Context;

static ScoreSemReqSurrender_Context
  ScoreSemReqSurrender_Instance;

static const char * const ScoreSemReqSurrender_PreDesc_Variant[] = {
  "Binary",
  "Counting",
  "NA"
};

static const char * const ScoreSemReqSurrender_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const ScoreSemReqSurrender_PreDesc_Count[] = {
  "LessMax",
  "Max",
  "Blocked",
  "NA"
};

static const char * const * const ScoreSemReqSurrender_PreDesc[] = {
  ScoreSemReqSurrender_PreDesc_Variant,
  ScoreSemReqSurrender_PreDesc_Discipline,
  ScoreSemReqSurrender_PreDesc_Count,
  NULL
};

typedef ScoreSemReqSurrender_Context Context;

static Status_Control Status( const Context *ctx, Status_Control status )
{
  return TQConvertStatus( &ctx->tq_ctx->base, status );
}

static void ScoreSemReqSurrender_Pre_Variant_Prepare(
  ScoreSemReqSurrender_Context    *ctx,
  ScoreSemReqSurrender_Pre_Variant state
)
{
  switch ( state ) {
    case ScoreSemReqSurrender_Pre_Variant_Binary: {
      /*
       * Where the semaphore is a binary semaphore.
       */
      if ( ctx->tq_ctx->variant != TQ_SEM_BINARY ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreSemReqSurrender_Pre_Variant_Counting: {
      /*
       * Where the semaphore is a counting semaphore.
       */
      if ( ctx->tq_ctx->variant != TQ_SEM_COUNTING ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreSemReqSurrender_Pre_Variant_NA:
      break;
  }
}

static void ScoreSemReqSurrender_Pre_Discipline_Prepare(
  ScoreSemReqSurrender_Context       *ctx,
  ScoreSemReqSurrender_Pre_Discipline state
)
{
  switch ( state ) {
    case ScoreSemReqSurrender_Pre_Discipline_FIFO: {
      /*
       * Where the thread queue of the semaphore uses the FIFO discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_FIFO ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreSemReqSurrender_Pre_Discipline_Priority: {
      /*
       * Where the thread queue of the semaphore uses the priority discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_PRIORITY ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreSemReqSurrender_Pre_Discipline_NA:
      break;
  }
}

static void ScoreSemReqSurrender_Pre_Count_Prepare(
  ScoreSemReqSurrender_Context  *ctx,
  ScoreSemReqSurrender_Pre_Count state
)
{
  switch ( state ) {
    case ScoreSemReqSurrender_Pre_Count_LessMax: {
      /*
       * While the count of the semaphore is less than the maximum count.
       */
      ctx->blocked = false;

      if ( ctx->tq_ctx->variant == TQ_SEM_BINARY ) {
        ctx->count_before = 0;
      } else {
        ctx->count_before = UINT32_MAX - 1;
      }
      break;
    }

    case ScoreSemReqSurrender_Pre_Count_Max: {
      /*
       * While the count of the semaphore is equal to the maximum count.
       */
      ctx->blocked = false;

      if ( ctx->tq_ctx->variant == TQ_SEM_BINARY ) {
        ctx->count_before = 1;
      } else {
        ctx->count_before = UINT32_MAX;
      }
      break;
    }

    case ScoreSemReqSurrender_Pre_Count_Blocked: {
      /*
       * While the semaphore has threads blocked on the semaphore.
       */
      ctx->blocked = true;
      ctx->count_before = 0;
      break;
    }

    case ScoreSemReqSurrender_Pre_Count_NA:
      break;
  }
}

static void ScoreSemReqSurrender_Post_Status_Check(
  ScoreSemReqSurrender_Context    *ctx,
  ScoreSemReqSurrender_Post_Status state
)
{
  switch ( state ) {
    case ScoreSemReqSurrender_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      T_eq_int( ctx->status, Status( ctx, STATUS_SUCCESSFUL ) );
      break;
    }

    case ScoreSemReqSurrender_Post_Status_MaxCountExceeded: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_MAXIMUM_COUNT_EXCEEDED.
       */
      T_eq_int( ctx->status, Status( ctx, STATUS_MAXIMUM_COUNT_EXCEEDED ) );
      break;
    }

    case ScoreSemReqSurrender_Post_Status_NA:
      break;
  }
}

static void ScoreSemReqSurrender_Post_Surrender_Check(
  ScoreSemReqSurrender_Context       *ctx,
  ScoreSemReqSurrender_Post_Surrender state
)
{
  switch ( state ) {
    case ScoreSemReqSurrender_Post_Surrender_FIFO: {
      /*
       * The thread queue of the semaphore shall be surrendered in FIFO order.
       */
      ScoreTqReqSurrender_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreSemReqSurrender_Post_Surrender_Priority: {
      /*
       * The thread queue of the semaphore shall be surrendered in priority
       * order.
       */
      ScoreTqReqSurrender_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreSemReqSurrender_Post_Surrender_NA:
      break;
  }
}

static void ScoreSemReqSurrender_Post_Count_Check(
  ScoreSemReqSurrender_Context   *ctx,
  ScoreSemReqSurrender_Post_Count state
)
{
  switch ( state ) {
    case ScoreSemReqSurrender_Post_Count_Zero: {
      /*
       * The count of the semaphore shall be zero.
       */
      T_eq_u32( ctx->count_after, 0 );
      break;
    }

    case ScoreSemReqSurrender_Post_Count_One: {
      /*
       * The count of the semaphore shall be one.
       */
      T_eq_u32( ctx->count_after, 1 );
      break;
    }

    case ScoreSemReqSurrender_Post_Count_PlusOne: {
      /*
       * The count of the semaphore shall be incremented by one.
       */
      T_eq_u32( ctx->count_after, ctx->count_before + 1 );
      break;
    }

    case ScoreSemReqSurrender_Post_Count_Nop: {
      /*
       * The count of the semaphore shall not be modified.
       */
      T_eq_u32( ctx->count_after, ctx->count_before );
      break;
    }

    case ScoreSemReqSurrender_Post_Count_NA:
      break;
  }
}

static void ScoreSemReqSurrender_Setup( ScoreSemReqSurrender_Context *ctx )
{
  ctx->tq_ctx->base.wait = TQ_WAIT_FOREVER;
  TQReset( &ctx->tq_ctx->base );
}

static void ScoreSemReqSurrender_Setup_Wrap( void *arg )
{
  ScoreSemReqSurrender_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreSemReqSurrender_Setup( ctx );
}

static void ScoreSemReqSurrender_Action( ScoreSemReqSurrender_Context *ctx )
{
  TQSemSetCount( ctx->tq_ctx, ctx->count_before );

  if ( ctx->blocked ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
  }

  ctx->status = TQSurrender( &ctx->tq_ctx->base );
  ctx->count_after = TQSemGetCount( ctx->tq_ctx );
  TQSemSetCount( ctx->tq_ctx, 1 );
}

static const ScoreSemReqSurrender_Entry
ScoreSemReqSurrender_Entries[] = {
  { 0, 0, 0, 0, ScoreSemReqSurrender_Post_Status_Ok,
    ScoreSemReqSurrender_Post_Surrender_NA, ScoreSemReqSurrender_Post_Count_One },
  { 0, 0, 0, 0, ScoreSemReqSurrender_Post_Status_Ok,
    ScoreSemReqSurrender_Post_Surrender_FIFO,
    ScoreSemReqSurrender_Post_Count_Zero },
  { 0, 0, 0, 0, ScoreSemReqSurrender_Post_Status_Ok,
    ScoreSemReqSurrender_Post_Surrender_Priority,
    ScoreSemReqSurrender_Post_Count_Zero },
  { 0, 0, 0, 0, ScoreSemReqSurrender_Post_Status_Ok,
    ScoreSemReqSurrender_Post_Surrender_NA,
    ScoreSemReqSurrender_Post_Count_PlusOne },
  { 0, 0, 0, 0, ScoreSemReqSurrender_Post_Status_MaxCountExceeded,
    ScoreSemReqSurrender_Post_Surrender_NA, ScoreSemReqSurrender_Post_Count_Nop }
};

static const uint8_t
ScoreSemReqSurrender_Map[] = {
  0, 0, 1, 0, 0, 2, 3, 4, 1, 3, 4, 2
};

static size_t ScoreSemReqSurrender_Scope( void *arg, char *buf, size_t n )
{
  ScoreSemReqSurrender_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreSemReqSurrender_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreSemReqSurrender_Fixture = {
  .setup = ScoreSemReqSurrender_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreSemReqSurrender_Scope,
  .initial_context = &ScoreSemReqSurrender_Instance
};

static const uint8_t ScoreSemReqSurrender_Weights[] = {
  6, 3, 1
};

static void ScoreSemReqSurrender_Skip(
  ScoreSemReqSurrender_Context *ctx,
  size_t                        index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreSemReqSurrender_Pre_Discipline_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreSemReqSurrender_Pre_Count_NA - 1;
      break;
  }
}

static inline ScoreSemReqSurrender_Entry ScoreSemReqSurrender_PopEntry(
  ScoreSemReqSurrender_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += ScoreSemReqSurrender_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreSemReqSurrender_Entries[
    ScoreSemReqSurrender_Map[ index ]
  ];
}

static void ScoreSemReqSurrender_TestVariant(
  ScoreSemReqSurrender_Context *ctx
)
{
  ScoreSemReqSurrender_Pre_Variant_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreSemReqSurrender_Skip( ctx, 0 );
    return;
  }

  ScoreSemReqSurrender_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreSemReqSurrender_Skip( ctx, 1 );
    return;
  }

  ScoreSemReqSurrender_Pre_Count_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreSemReqSurrender_Action( ctx );
  ScoreSemReqSurrender_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  ScoreSemReqSurrender_Post_Surrender_Check(
    ctx,
    ctx->Map.entry.Post_Surrender
  );
  ScoreSemReqSurrender_Post_Count_Check( ctx, ctx->Map.entry.Post_Count );
}

static T_fixture_node ScoreSemReqSurrender_Node;

static T_remark ScoreSemReqSurrender_Remark = {
  .next = NULL,
  .remark = "ScoreSemReqSurrender"
};

void ScoreSemReqSurrender_Run( TQSemContext *tq_ctx )
{
  ScoreSemReqSurrender_Context *ctx;

  ctx = &ScoreSemReqSurrender_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreSemReqSurrender_Node,
    &ScoreSemReqSurrender_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreSemReqSurrender_Pre_Variant_Binary;
    ctx->Map.pcs[ 0 ] < ScoreSemReqSurrender_Pre_Variant_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreSemReqSurrender_Pre_Discipline_FIFO;
      ctx->Map.pcs[ 1 ] < ScoreSemReqSurrender_Pre_Discipline_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreSemReqSurrender_Pre_Count_LessMax;
        ctx->Map.pcs[ 2 ] < ScoreSemReqSurrender_Pre_Count_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreSemReqSurrender_PopEntry( ctx );
        ScoreSemReqSurrender_TestVariant( ctx );
      }
    }
  }

  T_add_remark( &ScoreSemReqSurrender_Remark );
  T_pop_fixture();
}

/** @} */
