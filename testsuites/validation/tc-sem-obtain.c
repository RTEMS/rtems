/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemReqObtain
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

#include "tr-mtx-seize-try.h"
#include "tr-mtx-seize-wait.h"
#include "tr-sem-seize-try.h"
#include "tr-sem-seize-wait.h"
#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSemReqObtain spec:/rtems/sem/req/obtain
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSemReqObtain_Pre_Class_Counting,
  RtemsSemReqObtain_Pre_Class_Simple,
  RtemsSemReqObtain_Pre_Class_Binary,
  RtemsSemReqObtain_Pre_Class_PrioCeiling,
  RtemsSemReqObtain_Pre_Class_PrioInherit,
  RtemsSemReqObtain_Pre_Class_MrsP,
  RtemsSemReqObtain_Pre_Class_NA
} RtemsSemReqObtain_Pre_Class;

typedef enum {
  RtemsSemReqObtain_Pre_Discipline_FIFO,
  RtemsSemReqObtain_Pre_Discipline_Priority,
  RtemsSemReqObtain_Pre_Discipline_NA
} RtemsSemReqObtain_Pre_Discipline;

typedef enum {
  RtemsSemReqObtain_Pre_Id_Valid,
  RtemsSemReqObtain_Pre_Id_Invalid,
  RtemsSemReqObtain_Pre_Id_NA
} RtemsSemReqObtain_Pre_Id;

typedef enum {
  RtemsSemReqObtain_Pre_Wait_No,
  RtemsSemReqObtain_Pre_Wait_Timeout,
  RtemsSemReqObtain_Pre_Wait_Forever,
  RtemsSemReqObtain_Pre_Wait_NA
} RtemsSemReqObtain_Pre_Wait;

typedef enum {
  RtemsSemReqObtain_Post_Action_InvId,
  RtemsSemReqObtain_Post_Action_SemSeizeTry,
  RtemsSemReqObtain_Post_Action_SemSeizeWait,
  RtemsSemReqObtain_Post_Action_MtxSeizeTry,
  RtemsSemReqObtain_Post_Action_MtxSeizeWait,
  RtemsSemReqObtain_Post_Action_InheritMtxSeizeTry,
  RtemsSemReqObtain_Post_Action_InheritMtxSeizeWait,
  RtemsSemReqObtain_Post_Action_CeilingMtxSeizeTry,
  RtemsSemReqObtain_Post_Action_CeilingMtxSeizeWait,
  RtemsSemReqObtain_Post_Action_MrsPMtxSeizeTry,
  RtemsSemReqObtain_Post_Action_MrsPMtxSeizeWait,
  RtemsSemReqObtain_Post_Action_NA
} RtemsSemReqObtain_Post_Action;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Class_NA : 1;
  uint16_t Pre_Discipline_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Wait_NA : 1;
  uint16_t Post_Action : 4;
} RtemsSemReqObtain_Entry;

/**
 * @brief Test context for spec:/rtems/sem/req/obtain test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  union {
    TQContext tq_ctx;
    TQMtxContext tq_mtx_ctx;
    TQSemContext tq_sem_ctx;
  };

  /**
   * @brief This member specifies if the attribute set of the semaphore.
   */
  rtems_attribute attribute_set;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    RtemsSemReqObtain_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSemReqObtain_Context;

static RtemsSemReqObtain_Context
  RtemsSemReqObtain_Instance;

static const char * const RtemsSemReqObtain_PreDesc_Class[] = {
  "Counting",
  "Simple",
  "Binary",
  "PrioCeiling",
  "PrioInherit",
  "MrsP",
  "NA"
};

static const char * const RtemsSemReqObtain_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const RtemsSemReqObtain_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsSemReqObtain_PreDesc_Wait[] = {
  "No",
  "Timeout",
  "Forever",
  "NA"
};

static const char * const * const RtemsSemReqObtain_PreDesc[] = {
  RtemsSemReqObtain_PreDesc_Class,
  RtemsSemReqObtain_PreDesc_Discipline,
  RtemsSemReqObtain_PreDesc_Id,
  RtemsSemReqObtain_PreDesc_Wait,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

typedef RtemsSemReqObtain_Context Context;

static void RtemsSemReqObtain_Pre_Class_Prepare(
  RtemsSemReqObtain_Context  *ctx,
  RtemsSemReqObtain_Pre_Class state
)
{
  switch ( state ) {
    case RtemsSemReqObtain_Pre_Class_Counting: {
      /*
       * While the semaphore object is a counting semaphore.
       */
      ctx->attribute_set |= RTEMS_COUNTING_SEMAPHORE;
      break;
    }

    case RtemsSemReqObtain_Pre_Class_Simple: {
      /*
       * While the semaphore object is a simple binary semaphore.
       */
      ctx->attribute_set |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
      break;
    }

    case RtemsSemReqObtain_Pre_Class_Binary: {
      /*
       * While the semaphore object is a binary semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE;
      break;
    }

    case RtemsSemReqObtain_Pre_Class_PrioCeiling: {
      /*
       * While the semaphore object is a priority ceiling semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING;
      break;
    }

    case RtemsSemReqObtain_Pre_Class_PrioInherit: {
      /*
       * While the semaphore object is a priority inheritance semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY;
      break;
    }

    case RtemsSemReqObtain_Pre_Class_MrsP: {
      /*
       * While the semaphore object is a MrsP semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE |
        RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
      break;
    }

    case RtemsSemReqObtain_Pre_Class_NA:
      break;
  }
}

static void RtemsSemReqObtain_Pre_Discipline_Prepare(
  RtemsSemReqObtain_Context       *ctx,
  RtemsSemReqObtain_Pre_Discipline state
)
{
  switch ( state ) {
    case RtemsSemReqObtain_Pre_Discipline_FIFO: {
      /*
       * While the semaphore uses the FIFO task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_FIFO;
      ctx->tq_ctx.discipline = TQ_FIFO;
      break;
    }

    case RtemsSemReqObtain_Pre_Discipline_Priority: {
      /*
       * While the semaphore uses the priority task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_PRIORITY;
      ctx->tq_ctx.discipline = TQ_PRIORITY;
      break;
    }

    case RtemsSemReqObtain_Pre_Discipline_NA:
      break;
  }
}

static void RtemsSemReqObtain_Pre_Id_Prepare(
  RtemsSemReqObtain_Context *ctx,
  RtemsSemReqObtain_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSemReqObtain_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is associated with the semaphore.
       */
      /* Nothing to prepare */
      break;
    }

    case RtemsSemReqObtain_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a semaphore.
       */
      /* Nothing to prepare */
      break;
    }

    case RtemsSemReqObtain_Pre_Id_NA:
      break;
  }
}

static void RtemsSemReqObtain_Pre_Wait_Prepare(
  RtemsSemReqObtain_Context *ctx,
  RtemsSemReqObtain_Pre_Wait state
)
{
  switch ( state ) {
    case RtemsSemReqObtain_Pre_Wait_No: {
      /*
       * While the ``option_set`` parameter indicates the RTEMS_NO_WAIT option.
       */
      ctx->tq_ctx.wait = TQ_NO_WAIT;
      break;
    }

    case RtemsSemReqObtain_Pre_Wait_Timeout: {
      /*
       * While the ``option_set`` parameter indicates the RTEMS_WAIT option,
       * while the ``timeout`` parameter is not equal to RTEMS_NO_TIMEOUT.
       */
      ctx->tq_ctx.wait = TQ_WAIT_TIMED;
      break;
    }

    case RtemsSemReqObtain_Pre_Wait_Forever: {
      /*
       * While the ``option_set`` parameter indicates the RTEMS_WAIT option,
       * while the ``timeout`` parameter is equal to RTEMS_NO_TIMEOUT.
       */
      ctx->tq_ctx.wait = TQ_WAIT_FOREVER;
      break;
    }

    case RtemsSemReqObtain_Pre_Wait_NA:
      break;
  }
}

static void RtemsSemReqObtain_Post_Action_Check(
  RtemsSemReqObtain_Context    *ctx,
  RtemsSemReqObtain_Post_Action state
)
{
  rtems_status_code sc;

  switch ( state ) {
    case RtemsSemReqObtain_Post_Action_InvId: {
      /*
       * The return status of rtems_semaphore_obtain() shall be
       * RTEMS_INVALID_ID.
       */
      sc = rtems_semaphore_obtain( 0xffffffff, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
      T_rsc( sc, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSemReqObtain_Post_Action_SemSeizeTry: {
      /*
       * The calling task shall try to seize the semaphore as specified by
       * spec:/score/sem/req/seize-try.
       */
      ctx->tq_sem_ctx.get_count = TQSemGetCountClassic;
      ctx->tq_sem_ctx.set_count = TQSemSetCountClassic;
      ScoreSemReqSeizeTry_Run( &ctx->tq_sem_ctx );
      break;
    }

    case RtemsSemReqObtain_Post_Action_SemSeizeWait: {
      /*
       * The calling task shall wait to seize the semaphore as specified by
       * spec:/score/sem/req/seize-wait.
       */
      ctx->tq_sem_ctx.get_count = TQSemGetCountClassic;
      ctx->tq_sem_ctx.set_count = TQSemSetCountClassic;
      ScoreSemReqSeizeWait_Run( &ctx->tq_sem_ctx );
      break;
    }

    case RtemsSemReqObtain_Post_Action_MtxSeizeTry: {
      /*
       * The calling task shall try to seize the mutex as specified by
       * spec:/score/mtx/req/seize-try where an enqueue blocks, a recursive
       * seize is allowed, and no locking protocol is used.
       */
      ctx->tq_mtx_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_NO_PROTOCOL;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_INVALID;
      ScoreMtxReqSeizeTry_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqObtain_Post_Action_MtxSeizeWait: {
      /*
       * The calling task shall wait to seize the mutex as specified by
       * spec:/score/mtx/req/seize-wait where an enqueue blocks, a recursive
       * seize is allowed, and no locking protocol is used.
       */
      ctx->tq_mtx_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_NO_PROTOCOL;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_INVALID;
      ScoreMtxReqSeizeWait_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqObtain_Post_Action_InheritMtxSeizeTry: {
      /*
       * The calling task shall try to seize the mutex as specified by
       * spec:/score/mtx/req/seize-try where an enqueue blocks, a recursive
       * seize is allowed, and a priority inheritance protocol is used.
       */
      ctx->tq_mtx_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_NO_PROTOCOL;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_INVALID;
      ScoreMtxReqSeizeTry_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqObtain_Post_Action_InheritMtxSeizeWait: {
      /*
       * The calling task shall wait to seize the mutex as specified by
       * spec:/score/mtx/req/seize-wait where an enqueue blocks, a recursive
       * seize is allowed, and a priority inheritance protocol is used.
       */
      ctx->tq_mtx_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_NO_PROTOCOL;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_INVALID;
      ScoreMtxReqSeizeWait_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqObtain_Post_Action_CeilingMtxSeizeTry: {
      /*
       * The calling task shall try to seize the mutex as specified by
       * spec:/score/mtx/req/seize-try where an enqueue blocks, a recursive
       * seize is allowed, and a priority ceiling is used.
       */
      ctx->tq_mtx_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_PRIORITY_CEILING;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_VERY_HIGH;
      ScoreMtxReqSeizeTry_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqObtain_Post_Action_CeilingMtxSeizeWait: {
      /*
       * The calling task shall wait to seize the mutex as specified by
       * spec:/score/mtx/req/seize-wait where an enqueue blocks, a recursive
       * seize is allowed, and a priority ceiling is used.
       */
      ctx->tq_mtx_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_PRIORITY_CEILING;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_VERY_HIGH;
      ScoreMtxReqSeizeWait_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqObtain_Post_Action_MrsPMtxSeizeTry: {
      /*
       * The calling task shall try to seize the mutex as specified by
       * spec:/score/mtx/req/seize-try where an enqueue is sticky, a recursive
       * seize returns an error status, and a priority ceiling is used.
       */
      #if defined(RTEMS_SMP)
      ctx->tq_mtx_ctx.base.enqueue_variant = TQ_ENQUEUE_STICKY;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_MRSP;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_DEADLOCK;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_VERY_HIGH;
      ScoreMtxReqSeizeTry_Run( &ctx->tq_mtx_ctx );
      #else
      T_unreachable();
      #endif
      break;
    }

    case RtemsSemReqObtain_Post_Action_MrsPMtxSeizeWait: {
      /*
       * The calling task shall wait to seize the mutex as specified by
       * spec:/score/mtx/req/seize-wait where an enqueue is sticky, a recursive
       * seize returns an error status, and a priority ceiling is used.
       */
      #if defined(RTEMS_SMP)
      ctx->tq_mtx_ctx.base.enqueue_variant = TQ_ENQUEUE_STICKY;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_MRSP;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_DEADLOCK;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_VERY_HIGH;
      ScoreMtxReqSeizeWait_Run( &ctx->tq_mtx_ctx );
      #else
      T_unreachable();
      #endif
      break;
    }

    case RtemsSemReqObtain_Post_Action_NA:
      break;
  }
}

static void RtemsSemReqObtain_Setup( RtemsSemReqObtain_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.deadlock = TQ_DEADLOCK_STATUS;
  ctx->tq_ctx.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_ctx.enqueue = TQEnqueueClassicSem;
  ctx->tq_ctx.surrender = TQSurrenderClassicSem;
  ctx->tq_ctx.get_owner = TQGetOwnerClassicSem;
  ctx->tq_ctx.convert_status = TQConvertStatusClassic;
  TQInitialize( &ctx->tq_ctx );
}

static void RtemsSemReqObtain_Setup_Wrap( void *arg )
{
  RtemsSemReqObtain_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqObtain_Setup( ctx );
}

static void RtemsSemReqObtain_Teardown( RtemsSemReqObtain_Context *ctx )
{
  TQDestroy( &ctx->tq_ctx );
}

static void RtemsSemReqObtain_Teardown_Wrap( void *arg )
{
  RtemsSemReqObtain_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqObtain_Teardown( ctx );
}

static void RtemsSemReqObtain_Prepare( RtemsSemReqObtain_Context *ctx )
{
  ctx->attribute_set = RTEMS_DEFAULT_ATTRIBUTES;
}

static void RtemsSemReqObtain_Action( RtemsSemReqObtain_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    NAME,
    1,
    ctx->attribute_set,
    PRIO_VERY_HIGH,
    &ctx->tq_ctx.thread_queue_id
  );
  T_rsc_success( sc );

  #if defined(RTEMS_SMP)
  if ( ( ctx->attribute_set & RTEMS_MULTIPROCESSOR_RESOURCE_SHARING ) != 0 ) {
    rtems_task_priority prio;

    sc = rtems_semaphore_set_priority(
      ctx->tq_ctx.thread_queue_id,
      SCHEDULER_B_ID,
      PRIO_VERY_HIGH,
      &prio
    );
    T_rsc_success( sc );
  }
  #endif
}

static void RtemsSemReqObtain_Cleanup( RtemsSemReqObtain_Context *ctx )
{
  rtems_status_code sc;
  sc = rtems_semaphore_delete( ctx->tq_ctx.thread_queue_id ); T_rsc_success( sc );
}

static const RtemsSemReqObtain_Entry
RtemsSemReqObtain_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_InvId },
  { 1, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_NA },
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_SemSeizeWait },
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_SemSeizeTry },
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_MtxSeizeWait },
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_MtxSeizeTry },
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_CeilingMtxSeizeWait },
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_InheritMtxSeizeWait },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_MrsPMtxSeizeWait },
#else
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_CeilingMtxSeizeWait },
#endif
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_CeilingMtxSeizeTry },
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_InheritMtxSeizeTry },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_MrsPMtxSeizeTry }
#else
  { 0, 0, 0, 0, 0, RtemsSemReqObtain_Post_Action_CeilingMtxSeizeTry }
#endif
};

static const uint8_t
RtemsSemReqObtain_Map[] = {
  3, 2, 2, 0, 0, 0, 3, 2, 2, 0, 0, 0, 3, 2, 2, 0, 0, 0, 3, 2, 2, 0, 0, 0, 5, 4,
  4, 0, 0, 0, 5, 4, 4, 0, 0, 0, 1, 1, 1, 1, 1, 1, 9, 6, 6, 0, 0, 0, 1, 1, 1, 1,
  1, 1, 10, 7, 7, 0, 0, 0, 1, 1, 1, 1, 1, 1, 11, 8, 8, 0, 0, 0
};

static size_t RtemsSemReqObtain_Scope( void *arg, char *buf, size_t n )
{
  RtemsSemReqObtain_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSemReqObtain_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSemReqObtain_Fixture = {
  .setup = RtemsSemReqObtain_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemReqObtain_Teardown_Wrap,
  .scope = RtemsSemReqObtain_Scope,
  .initial_context = &RtemsSemReqObtain_Instance
};

static inline RtemsSemReqObtain_Entry RtemsSemReqObtain_PopEntry(
  RtemsSemReqObtain_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSemReqObtain_Entries[
    RtemsSemReqObtain_Map[ index ]
  ];
}

static void RtemsSemReqObtain_TestVariant( RtemsSemReqObtain_Context *ctx )
{
  RtemsSemReqObtain_Pre_Class_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSemReqObtain_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSemReqObtain_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSemReqObtain_Pre_Wait_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsSemReqObtain_Action( ctx );
  RtemsSemReqObtain_Post_Action_Check( ctx, ctx->Map.entry.Post_Action );
}

/**
 * @fn void T_case_body_RtemsSemReqObtain( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemReqObtain, &RtemsSemReqObtain_Fixture )
{
  RtemsSemReqObtain_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSemReqObtain_Pre_Class_Counting;
    ctx->Map.pcs[ 0 ] < RtemsSemReqObtain_Pre_Class_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSemReqObtain_Pre_Discipline_FIFO;
      ctx->Map.pcs[ 1 ] < RtemsSemReqObtain_Pre_Discipline_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsSemReqObtain_Pre_Id_Valid;
        ctx->Map.pcs[ 2 ] < RtemsSemReqObtain_Pre_Id_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsSemReqObtain_Pre_Wait_No;
          ctx->Map.pcs[ 3 ] < RtemsSemReqObtain_Pre_Wait_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = RtemsSemReqObtain_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsSemReqObtain_Prepare( ctx );
          RtemsSemReqObtain_TestVariant( ctx );
          RtemsSemReqObtain_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
