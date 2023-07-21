/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemReqFlush
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

#include "tr-tq-flush-fifo.h"
#include "tr-tq-flush-priority-inherit.h"
#include "tr-tq-flush-priority.h"
#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSemReqFlush spec:/rtems/sem/req/flush
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSemReqFlush_Pre_Class_Counting,
  RtemsSemReqFlush_Pre_Class_Simple,
  RtemsSemReqFlush_Pre_Class_Binary,
  RtemsSemReqFlush_Pre_Class_PrioCeiling,
  RtemsSemReqFlush_Pre_Class_PrioInherit,
  RtemsSemReqFlush_Pre_Class_MrsP,
  RtemsSemReqFlush_Pre_Class_NA
} RtemsSemReqFlush_Pre_Class;

typedef enum {
  RtemsSemReqFlush_Pre_Discipline_FIFO,
  RtemsSemReqFlush_Pre_Discipline_Priority,
  RtemsSemReqFlush_Pre_Discipline_NA
} RtemsSemReqFlush_Pre_Discipline;

typedef enum {
  RtemsSemReqFlush_Pre_Id_Valid,
  RtemsSemReqFlush_Pre_Id_Invalid,
  RtemsSemReqFlush_Pre_Id_NA
} RtemsSemReqFlush_Pre_Id;

typedef enum {
  RtemsSemReqFlush_Post_Action_InvId,
  RtemsSemReqFlush_Post_Action_NotDef,
  RtemsSemReqFlush_Post_Action_FlushFIFO,
  RtemsSemReqFlush_Post_Action_FlushPriority,
  RtemsSemReqFlush_Post_Action_FlushPriorityCeiling,
  RtemsSemReqFlush_Post_Action_FlushPriorityInherit,
  RtemsSemReqFlush_Post_Action_NA
} RtemsSemReqFlush_Post_Action;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Class_NA : 1;
  uint8_t Pre_Discipline_NA : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Post_Action : 3;
} RtemsSemReqFlush_Entry;

/**
 * @brief Test context for spec:/rtems/sem/req/flush test case.
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

  /**
   * @brief This member specifies if the initial count of the semaphore.
   */
  uint32_t initial_count;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 3 ];

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
    RtemsSemReqFlush_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSemReqFlush_Context;

static RtemsSemReqFlush_Context
  RtemsSemReqFlush_Instance;

static const char * const RtemsSemReqFlush_PreDesc_Class[] = {
  "Counting",
  "Simple",
  "Binary",
  "PrioCeiling",
  "PrioInherit",
  "MrsP",
  "NA"
};

static const char * const RtemsSemReqFlush_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const RtemsSemReqFlush_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const * const RtemsSemReqFlush_PreDesc[] = {
  RtemsSemReqFlush_PreDesc_Class,
  RtemsSemReqFlush_PreDesc_Discipline,
  RtemsSemReqFlush_PreDesc_Id,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

typedef RtemsSemReqFlush_Context Context;

static void EnqueuePrepare( TQContext *tq_ctx )
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(
    tq_ctx->thread_queue_id,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  T_rsc_success( sc );
}

static Status_Control Enqueue( TQContext *tq_ctx, TQWait wait )
{
  rtems_status_code sc;

  (void) wait;
  sc = rtems_semaphore_obtain(
    tq_ctx->thread_queue_id,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  T_rsc( sc, RTEMS_UNSATISFIED );

  return STATUS_BUILD( STATUS_SUCCESSFUL, 0 );
}

static uint32_t Flush( TQContext *tq_ctx, uint32_t thread_count, bool all )
{
  rtems_status_code sc;

  (void) all;

  sc = rtems_semaphore_flush( tq_ctx->thread_queue_id );
  T_rsc_success( sc );

  return thread_count;
}

static void RtemsSemReqFlush_Pre_Class_Prepare(
  RtemsSemReqFlush_Context  *ctx,
  RtemsSemReqFlush_Pre_Class state
)
{
  switch ( state ) {
    case RtemsSemReqFlush_Pre_Class_Counting: {
      /*
       * While the semaphore object is a counting semaphore.
       */
      ctx->attribute_set |= RTEMS_COUNTING_SEMAPHORE;
      ctx->initial_count = 0;
      ctx->tq_ctx.enqueue_prepare = TQDoNothing;
      ctx->tq_ctx.enqueue_done = TQDoNothing;
      break;
    }

    case RtemsSemReqFlush_Pre_Class_Simple: {
      /*
       * While the semaphore object is a simple binary semaphore.
       */
      ctx->attribute_set |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
      ctx->initial_count = 0;
      ctx->tq_ctx.enqueue_prepare = TQDoNothing;
      ctx->tq_ctx.enqueue_done = TQDoNothing;
      break;
    }

    case RtemsSemReqFlush_Pre_Class_Binary: {
      /*
       * While the semaphore object is a binary semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE;
      ctx->initial_count = 1;
      ctx->tq_ctx.enqueue_prepare = EnqueuePrepare;
      ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
      break;
    }

    case RtemsSemReqFlush_Pre_Class_PrioCeiling: {
      /*
       * While the semaphore object is a priority ceiling semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING;
      ctx->initial_count = 1;
      ctx->tq_ctx.enqueue_prepare = EnqueuePrepare;
      ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
      break;
    }

    case RtemsSemReqFlush_Pre_Class_PrioInherit: {
      /*
       * While the semaphore object is a priority inheritance semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY;
      ctx->initial_count = 1;
      ctx->tq_ctx.enqueue_prepare = EnqueuePrepare;
      ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
      break;
    }

    case RtemsSemReqFlush_Pre_Class_MrsP: {
      /*
       * While the semaphore object is a MrsP semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE |
        RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
      ctx->initial_count = 1;
      ctx->tq_ctx.enqueue_prepare = EnqueuePrepare;
      ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
      break;
    }

    case RtemsSemReqFlush_Pre_Class_NA:
      break;
  }
}

static void RtemsSemReqFlush_Pre_Discipline_Prepare(
  RtemsSemReqFlush_Context       *ctx,
  RtemsSemReqFlush_Pre_Discipline state
)
{
  switch ( state ) {
    case RtemsSemReqFlush_Pre_Discipline_FIFO: {
      /*
       * While the semaphore uses the FIFO task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_FIFO;
      ctx->tq_ctx.discipline = TQ_FIFO;
      break;
    }

    case RtemsSemReqFlush_Pre_Discipline_Priority: {
      /*
       * While the semaphore uses the priority task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_PRIORITY;
      ctx->tq_ctx.discipline = TQ_PRIORITY;
      break;
    }

    case RtemsSemReqFlush_Pre_Discipline_NA:
      break;
  }
}

static void RtemsSemReqFlush_Pre_Id_Prepare(
  RtemsSemReqFlush_Context *ctx,
  RtemsSemReqFlush_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSemReqFlush_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is associated with the semaphore.
       */
      /* Nothing to prepare */
      break;
    }

    case RtemsSemReqFlush_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a semaphore.
       */
      /* Nothing to prepare */
      break;
    }

    case RtemsSemReqFlush_Pre_Id_NA:
      break;
  }
}

static void RtemsSemReqFlush_Post_Action_Check(
  RtemsSemReqFlush_Context    *ctx,
  RtemsSemReqFlush_Post_Action state
)
{
  rtems_status_code sc;

  switch ( state ) {
    case RtemsSemReqFlush_Post_Action_InvId: {
      /*
       * The return status of rtems_semaphore_flush() shall be
       * RTEMS_INVALID_ID.
       */
      sc = rtems_semaphore_flush( 0xffffffff );
      T_rsc( sc, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSemReqFlush_Post_Action_NotDef: {
      /*
       * The return status of rtems_semaphore_flush() shall be
       * RTEMS_NOT_DEFINED.
       */
      sc = rtems_semaphore_flush( ctx->tq_ctx.thread_queue_id );
      T_rsc( sc, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsSemReqFlush_Post_Action_FlushFIFO: {
      /*
       * The calling task shall flush the semaphore as specified by
       * spec:/score/tq/req/flush-fifo.
       */
      ScoreTqReqFlushFifo_Run( &ctx->tq_ctx, false );
      break;
    }

    case RtemsSemReqFlush_Post_Action_FlushPriority: {
      /*
       * The calling task shall flush the semaphore as specified by
       * spec:/score/tq/req/flush-priority.
       */
      ScoreTqReqFlushPriority_Run( &ctx->tq_ctx, true );
      break;
    }

    case RtemsSemReqFlush_Post_Action_FlushPriorityCeiling: {
      /*
       * The calling task shall flush the semaphore as specified by
       * spec:/score/tq/req/flush-priority.
       */
      ScoreTqReqFlushPriority_Run( &ctx->tq_ctx, false );
      break;
    }

    case RtemsSemReqFlush_Post_Action_FlushPriorityInherit: {
      /*
       * The calling task shall flush the semaphore as specified by
       * spec:/score/tq/req/flush-priority-inherit.
       */
      ScoreTqReqFlushPriorityInherit_Run( &ctx->tq_ctx );
      break;
    }

    case RtemsSemReqFlush_Post_Action_NA:
      break;
  }
}

static void RtemsSemReqFlush_Setup( RtemsSemReqFlush_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.enqueue = Enqueue;
  ctx->tq_ctx.flush = Flush;
  ctx->tq_ctx.surrender = TQSurrenderClassicSem;
  ctx->tq_ctx.convert_status = TQConvertStatusClassic;
  TQInitialize( &ctx->tq_ctx );
}

static void RtemsSemReqFlush_Setup_Wrap( void *arg )
{
  RtemsSemReqFlush_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqFlush_Setup( ctx );
}

static void RtemsSemReqFlush_Teardown( RtemsSemReqFlush_Context *ctx )
{
  TQDestroy( &ctx->tq_ctx );
}

static void RtemsSemReqFlush_Teardown_Wrap( void *arg )
{
  RtemsSemReqFlush_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqFlush_Teardown( ctx );
}

static void RtemsSemReqFlush_Prepare( RtemsSemReqFlush_Context *ctx )
{
  ctx->attribute_set = RTEMS_DEFAULT_ATTRIBUTES;
}

static void RtemsSemReqFlush_Action( RtemsSemReqFlush_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    NAME,
    ctx->initial_count,
    ctx->attribute_set,
    PRIO_ULTRA_HIGH,
    &ctx->tq_ctx.thread_queue_id
  );
  T_rsc_success( sc );
}

static void RtemsSemReqFlush_Cleanup( RtemsSemReqFlush_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_semaphore_delete( ctx->tq_ctx.thread_queue_id );
  T_rsc_success( sc );
}

static const RtemsSemReqFlush_Entry
RtemsSemReqFlush_Entries[] = {
  { 0, 1, 1, 0, RtemsSemReqFlush_Post_Action_InvId },
  { 0, 0, 0, 0, RtemsSemReqFlush_Post_Action_FlushFIFO },
  { 0, 0, 0, 0, RtemsSemReqFlush_Post_Action_FlushPriority },
  { 1, 0, 0, 0, RtemsSemReqFlush_Post_Action_NA },
  { 0, 0, 0, 0, RtemsSemReqFlush_Post_Action_FlushPriorityCeiling },
  { 0, 0, 0, 0, RtemsSemReqFlush_Post_Action_FlushPriorityInherit },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, RtemsSemReqFlush_Post_Action_NotDef }
#else
  { 1, 0, 0, 0, RtemsSemReqFlush_Post_Action_NA }
#endif
};

static const uint8_t
RtemsSemReqFlush_Map[] = {
  1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 3, 0, 4, 0, 3, 0, 5, 0, 3, 0, 6, 0
};

static size_t RtemsSemReqFlush_Scope( void *arg, char *buf, size_t n )
{
  RtemsSemReqFlush_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSemReqFlush_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSemReqFlush_Fixture = {
  .setup = RtemsSemReqFlush_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemReqFlush_Teardown_Wrap,
  .scope = RtemsSemReqFlush_Scope,
  .initial_context = &RtemsSemReqFlush_Instance
};

static inline RtemsSemReqFlush_Entry RtemsSemReqFlush_PopEntry(
  RtemsSemReqFlush_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSemReqFlush_Entries[
    RtemsSemReqFlush_Map[ index ]
  ];
}

static void RtemsSemReqFlush_SetPreConditionStates(
  RtemsSemReqFlush_Context *ctx
)
{
  if ( ctx->Map.entry.Pre_Class_NA ) {
    ctx->Map.pcs[ 0 ] = RtemsSemReqFlush_Pre_Class_NA;
  } else {
    ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  }

  if ( ctx->Map.entry.Pre_Discipline_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsSemReqFlush_Pre_Discipline_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void RtemsSemReqFlush_TestVariant( RtemsSemReqFlush_Context *ctx )
{
  RtemsSemReqFlush_Pre_Class_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSemReqFlush_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSemReqFlush_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSemReqFlush_Action( ctx );
  RtemsSemReqFlush_Post_Action_Check( ctx, ctx->Map.entry.Post_Action );
}

/**
 * @fn void T_case_body_RtemsSemReqFlush( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemReqFlush, &RtemsSemReqFlush_Fixture )
{
  RtemsSemReqFlush_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsSemReqFlush_Pre_Class_Counting;
    ctx->Map.pci[ 0 ] < RtemsSemReqFlush_Pre_Class_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsSemReqFlush_Pre_Discipline_FIFO;
      ctx->Map.pci[ 1 ] < RtemsSemReqFlush_Pre_Discipline_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsSemReqFlush_Pre_Id_Valid;
        ctx->Map.pci[ 2 ] < RtemsSemReqFlush_Pre_Id_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsSemReqFlush_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        RtemsSemReqFlush_SetPreConditionStates( ctx );
        RtemsSemReqFlush_Prepare( ctx );
        RtemsSemReqFlush_TestVariant( ctx );
        RtemsSemReqFlush_Cleanup( ctx );
      }
    }
  }
}

/** @} */
