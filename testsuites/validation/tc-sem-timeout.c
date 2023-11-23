/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemReqTimeout
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

#include <rtems.h>
#include <string.h>

#include "tr-tq-timeout-priority-inherit.h"
#include "tr-tq-timeout.h"
#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSemReqTimeout spec:/rtems/sem/req/timeout
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSemReqTimeout_Pre_Class_Counting,
  RtemsSemReqTimeout_Pre_Class_Simple,
  RtemsSemReqTimeout_Pre_Class_Binary,
  RtemsSemReqTimeout_Pre_Class_PrioCeiling,
  RtemsSemReqTimeout_Pre_Class_PrioInherit,
  RtemsSemReqTimeout_Pre_Class_MrsP,
  RtemsSemReqTimeout_Pre_Class_NA
} RtemsSemReqTimeout_Pre_Class;

typedef enum {
  RtemsSemReqTimeout_Pre_Discipline_FIFO,
  RtemsSemReqTimeout_Pre_Discipline_Priority,
  RtemsSemReqTimeout_Pre_Discipline_NA
} RtemsSemReqTimeout_Pre_Discipline;

typedef enum {
  RtemsSemReqTimeout_Post_Action_Timeout,
  RtemsSemReqTimeout_Post_Action_TimeoutMrsP,
  RtemsSemReqTimeout_Post_Action_TimeoutPriorityInherit,
  RtemsSemReqTimeout_Post_Action_NA
} RtemsSemReqTimeout_Post_Action;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Class_NA : 1;
  uint8_t Pre_Discipline_NA : 1;
  uint8_t Post_Action : 2;
} RtemsSemReqTimeout_Entry;

/**
 * @brief Test context for spec:/rtems/sem/req/timeout test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQContext tq_ctx;

  /**
   * @brief This member specifies if the attribute set of the semaphore.
   */
  rtems_attribute attribute_set;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    RtemsSemReqTimeout_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSemReqTimeout_Context;

static RtemsSemReqTimeout_Context
  RtemsSemReqTimeout_Instance;

static const char * const RtemsSemReqTimeout_PreDesc_Class[] = {
  "Counting",
  "Simple",
  "Binary",
  "PrioCeiling",
  "PrioInherit",
  "MrsP",
  "NA"
};

static const char * const RtemsSemReqTimeout_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const * const RtemsSemReqTimeout_PreDesc[] = {
  RtemsSemReqTimeout_PreDesc_Class,
  RtemsSemReqTimeout_PreDesc_Discipline,
  NULL
};

#if defined(RTEMS_SMP)
#include "tr-tq-timeout-mrsp.h"
#endif

static void RtemsSemReqTimeout_Pre_Class_Prepare(
  RtemsSemReqTimeout_Context  *ctx,
  RtemsSemReqTimeout_Pre_Class state
)
{
  switch ( state ) {
    case RtemsSemReqTimeout_Pre_Class_Counting: {
      /*
       * While the semaphore object is a counting semaphore.
       */
      ctx->attribute_set |= RTEMS_COUNTING_SEMAPHORE;
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      break;
    }

    case RtemsSemReqTimeout_Pre_Class_Simple: {
      /*
       * While the semaphore object is a simple binary semaphore.
       */
      ctx->attribute_set |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      break;
    }

    case RtemsSemReqTimeout_Pre_Class_Binary: {
      /*
       * While the semaphore object is a binary semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE;
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      break;
    }

    case RtemsSemReqTimeout_Pre_Class_PrioCeiling: {
      /*
       * While the semaphore object is a priority ceiling semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING;
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      break;
    }

    case RtemsSemReqTimeout_Pre_Class_PrioInherit: {
      /*
       * While the semaphore object is a priority inheritance semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY;
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      break;
    }

    case RtemsSemReqTimeout_Pre_Class_MrsP: {
      /*
       * While the semaphore object is a MrsP semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE |
        RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
      #if defined(RTEMS_SMP)
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_STICKY;
      #else
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      #endif
      break;
    }

    case RtemsSemReqTimeout_Pre_Class_NA:
      break;
  }
}

static void RtemsSemReqTimeout_Pre_Discipline_Prepare(
  RtemsSemReqTimeout_Context       *ctx,
  RtemsSemReqTimeout_Pre_Discipline state
)
{
  switch ( state ) {
    case RtemsSemReqTimeout_Pre_Discipline_FIFO: {
      /*
       * While the semaphore uses the FIFO task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_FIFO;
      ctx->tq_ctx.discipline = TQ_FIFO;
      break;
    }

    case RtemsSemReqTimeout_Pre_Discipline_Priority: {
      /*
       * While the semaphore uses the priority task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_PRIORITY;
      ctx->tq_ctx.discipline = TQ_PRIORITY;
      break;
    }

    case RtemsSemReqTimeout_Pre_Discipline_NA:
      break;
  }
}

static void RtemsSemReqTimeout_Post_Action_Check(
  RtemsSemReqTimeout_Context    *ctx,
  RtemsSemReqTimeout_Post_Action state
)
{
  switch ( state ) {
    case RtemsSemReqTimeout_Post_Action_Timeout: {
      /*
       * The semaphore obtain timeout actions shall be done as specified by
       * spec:/score/tq/req/timeout.
       */
      ctx->tq_ctx.wait = TQ_WAIT_TIMED;
      ScoreTqReqTimeout_Run( &ctx->tq_ctx );
      break;
    }

    case RtemsSemReqTimeout_Post_Action_TimeoutMrsP: {
      /*
       * The semaphore obtain timeout actions shall be done as specified by
       * spec:/score/tq/req/timeout-mrsp.
       */
      #if defined(RTEMS_SMP)
      ctx->tq_ctx.wait = TQ_WAIT_TIMED;
      ScoreTqReqTimeoutMrsp_Run( &ctx->tq_ctx );
      #else
      T_unreachable();
      #endif
      break;
    }

    case RtemsSemReqTimeout_Post_Action_TimeoutPriorityInherit: {
      /*
       * The semaphore obtain timeout actions shall be done as specified by
       * spec:/score/tq/req/timeout-priority-inherit.
       */
      ctx->tq_ctx.wait = TQ_WAIT_FOREVER;
      ScoreTqReqTimeoutPriorityInherit_Run( &ctx->tq_ctx );
      break;
    }

    case RtemsSemReqTimeout_Post_Action_NA:
      break;
  }
}

static void RtemsSemReqTimeout_Setup( RtemsSemReqTimeout_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_ctx.enqueue = TQEnqueueClassicSem;
  ctx->tq_ctx.surrender = TQSurrenderClassicSem;
  ctx->tq_ctx.convert_status = TQConvertStatusClassic;
  TQInitialize( &ctx->tq_ctx );
}

static void RtemsSemReqTimeout_Setup_Wrap( void *arg )
{
  RtemsSemReqTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqTimeout_Setup( ctx );
}

static void RtemsSemReqTimeout_Teardown( RtemsSemReqTimeout_Context *ctx )
{
  TQDestroy( &ctx->tq_ctx );
}

static void RtemsSemReqTimeout_Teardown_Wrap( void *arg )
{
  RtemsSemReqTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqTimeout_Teardown( ctx );
}

static void RtemsSemReqTimeout_Prepare( RtemsSemReqTimeout_Context *ctx )
{
  ctx->attribute_set = RTEMS_DEFAULT_ATTRIBUTES;
  ctx->tq_ctx.thread_queue_id = 0;
}

static void RtemsSemReqTimeout_Action( RtemsSemReqTimeout_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    OBJECT_NAME,
    1,
    ctx->attribute_set,
    PRIO_HIGH,
    &ctx->tq_ctx.thread_queue_id
  );
  T_rsc_success( sc );
}

static void RtemsSemReqTimeout_Cleanup( RtemsSemReqTimeout_Context *ctx )
{
  if ( ctx->tq_ctx.thread_queue_id != 0 ) {
    rtems_status_code sc;

    sc = rtems_semaphore_delete( ctx->tq_ctx.thread_queue_id );
    T_rsc_success( sc );
  }
}

static const RtemsSemReqTimeout_Entry
RtemsSemReqTimeout_Entries[] = {
  { 0, 0, 0, RtemsSemReqTimeout_Post_Action_Timeout },
  { 1, 0, 0, RtemsSemReqTimeout_Post_Action_NA },
  { 0, 0, 0, RtemsSemReqTimeout_Post_Action_TimeoutPriorityInherit },
#if defined(RTEMS_SMP)
  { 0, 0, 0, RtemsSemReqTimeout_Post_Action_TimeoutMrsP }
#else
  { 0, 0, 0, RtemsSemReqTimeout_Post_Action_Timeout }
#endif
};

static const uint8_t
RtemsSemReqTimeout_Map[] = {
  0, 0, 0, 0, 0, 0, 1, 0, 1, 2, 1, 3
};

static size_t RtemsSemReqTimeout_Scope( void *arg, char *buf, size_t n )
{
  RtemsSemReqTimeout_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSemReqTimeout_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSemReqTimeout_Fixture = {
  .setup = RtemsSemReqTimeout_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemReqTimeout_Teardown_Wrap,
  .scope = RtemsSemReqTimeout_Scope,
  .initial_context = &RtemsSemReqTimeout_Instance
};

static inline RtemsSemReqTimeout_Entry RtemsSemReqTimeout_PopEntry(
  RtemsSemReqTimeout_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSemReqTimeout_Entries[
    RtemsSemReqTimeout_Map[ index ]
  ];
}

static void RtemsSemReqTimeout_TestVariant( RtemsSemReqTimeout_Context *ctx )
{
  RtemsSemReqTimeout_Pre_Class_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSemReqTimeout_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSemReqTimeout_Action( ctx );
  RtemsSemReqTimeout_Post_Action_Check( ctx, ctx->Map.entry.Post_Action );
}

/**
 * @fn void T_case_body_RtemsSemReqTimeout( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemReqTimeout, &RtemsSemReqTimeout_Fixture )
{
  RtemsSemReqTimeout_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSemReqTimeout_Pre_Class_Counting;
    ctx->Map.pcs[ 0 ] < RtemsSemReqTimeout_Pre_Class_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSemReqTimeout_Pre_Discipline_FIFO;
      ctx->Map.pcs[ 1 ] < RtemsSemReqTimeout_Pre_Discipline_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsSemReqTimeout_PopEntry( ctx );

      if ( ctx->Map.entry.Skip ) {
        continue;
      }

      RtemsSemReqTimeout_Prepare( ctx );
      RtemsSemReqTimeout_TestVariant( ctx );
      RtemsSemReqTimeout_Cleanup( ctx );
    }
  }
}

/** @} */
