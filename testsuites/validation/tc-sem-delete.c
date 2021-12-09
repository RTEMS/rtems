/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsSemReqDelete
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestCaseRtemsSemReqDelete spec:/rtems/sem/req/delete
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsSemReqDelete_Pre_Id_NoObj,
  RtemsSemReqDelete_Pre_Id_Counting,
  RtemsSemReqDelete_Pre_Id_Simple,
  RtemsSemReqDelete_Pre_Id_Binary,
  RtemsSemReqDelete_Pre_Id_PrioCeiling,
  RtemsSemReqDelete_Pre_Id_PrioInherit,
  RtemsSemReqDelete_Pre_Id_MrsP,
  RtemsSemReqDelete_Pre_Id_NA
} RtemsSemReqDelete_Pre_Id;

typedef enum {
  RtemsSemReqDelete_Pre_Discipline_FIFO,
  RtemsSemReqDelete_Pre_Discipline_Priority,
  RtemsSemReqDelete_Pre_Discipline_NA
} RtemsSemReqDelete_Pre_Discipline;

typedef enum {
  RtemsSemReqDelete_Pre_State_GtZeroOrNoOwner,
  RtemsSemReqDelete_Pre_State_Zero,
  RtemsSemReqDelete_Pre_State_Blocked,
  RtemsSemReqDelete_Pre_State_NA
} RtemsSemReqDelete_Pre_State;

typedef enum {
  RtemsSemReqDelete_Post_Status_Ok,
  RtemsSemReqDelete_Post_Status_InvId,
  RtemsSemReqDelete_Post_Status_InUse,
  RtemsSemReqDelete_Post_Status_NA
} RtemsSemReqDelete_Post_Status;

typedef enum {
  RtemsSemReqDelete_Post_Name_Valid,
  RtemsSemReqDelete_Post_Name_Invalid,
  RtemsSemReqDelete_Post_Name_NA
} RtemsSemReqDelete_Post_Name;

typedef enum {
  RtemsSemReqDelete_Post_Flush_FIFO,
  RtemsSemReqDelete_Post_Flush_Priority,
  RtemsSemReqDelete_Post_Flush_No,
  RtemsSemReqDelete_Post_Flush_NA
} RtemsSemReqDelete_Post_Flush;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Discipline_NA : 1;
  uint16_t Pre_State_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Name : 2;
  uint16_t Post_Flush : 2;
} RtemsSemReqDelete_Entry;

/**
 * @brief Test context for spec:/rtems/sem/req/delete test case.
 */
typedef struct {
  /**
   * @brief This member contains the worker task identifiers.
   */
  rtems_id worker_id[ 2 ];

  /**
   * @brief This member contains the worker activity counter.
   */
  uint32_t counter;

  /**
   * @brief This member contains the worker activity counter of a specific
   *   worker.
   */
  uint32_t worker_counter[ 2 ];

  /**
   * @brief This member specifies the expected rtems_semaphore_obtain() status.
   */
  rtems_status_code obtain_status;

  /**
   * @brief This member specifies if the initial count of the semaphore.
   */
  uint32_t count;

  /**
   * @brief This member specifies if the attribute set of the semaphore.
   */
  rtems_attribute attribute_set;

  /**
   * @brief This member contains the semaphore identifier.
   */
  rtems_id semaphore_id;

  /**
   * @brief If this member is true, then the ``name`` parameter shall be valid,
   *   otherwise it should be NULL.
   */
  bool valid_id;

  /**
   * @brief If this member is true, then tasks shall be blocked on the
   *   semaphore, otherwise no tasks shall be blocked on the semaphore.
   */
  bool blocked;

  /**
   * @brief This member specifies the ``name`` parameter for the
   *   rtems_semaphore_delete() call.
   */
  rtems_id id;

  /**
   * @brief This member specifies the expected rtems_semaphore_delete() status.
   */
  rtems_status_code delete_status;

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
    RtemsSemReqDelete_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSemReqDelete_Context;

static RtemsSemReqDelete_Context
  RtemsSemReqDelete_Instance;

static const char * const RtemsSemReqDelete_PreDesc_Id[] = {
  "NoObj",
  "Counting",
  "Simple",
  "Binary",
  "PrioCeiling",
  "PrioInherit",
  "MrsP",
  "NA"
};

static const char * const RtemsSemReqDelete_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const RtemsSemReqDelete_PreDesc_State[] = {
  "GtZeroOrNoOwner",
  "Zero",
  "Blocked",
  "NA"
};

static const char * const * const RtemsSemReqDelete_PreDesc[] = {
  RtemsSemReqDelete_PreDesc_Id,
  RtemsSemReqDelete_PreDesc_Discipline,
  RtemsSemReqDelete_PreDesc_State,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

#define EVENT_OBTAIN RTEMS_EVENT_0

typedef RtemsSemReqDelete_Context Context;

static void WakeUp( Context *ctx, size_t index )
{
  SendEvents( ctx->worker_id[ index ], RTEMS_EVENT_0 );
}

static void Worker( rtems_task_argument arg, size_t index )
{
  Context *ctx;

  ctx = (Context *) arg;

  /*
   * In order to test the flush in FIFO order, we have to use the no-preempt
   * mode.
   */
  SetMode( RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK );

  while ( true ) {
    rtems_status_code sc;
    rtems_event_set   events;
    uint32_t          counter;

    events = ReceiveAnyEvents();
    T_eq_u32( events, RTEMS_EVENT_0 );

    sc = rtems_semaphore_obtain(
      ctx->semaphore_id,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    T_rsc( sc, ctx->obtain_status );

    counter = ctx->counter;
    ++counter;
    ctx->counter = counter;
    ctx->worker_counter[ index ] = counter;

    if ( sc == RTEMS_SUCCESSFUL ) {
      sc = rtems_semaphore_release( ctx->semaphore_id );
      T_rsc_success( sc );
    }
  }
}

static void WorkerZero( rtems_task_argument arg )
{
  Worker( arg, 0 );
}

static void WorkerOne( rtems_task_argument arg )
{
  Worker( arg, 1 );
}

static void RtemsSemReqDelete_Pre_Id_Prepare(
  RtemsSemReqDelete_Context *ctx,
  RtemsSemReqDelete_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSemReqDelete_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is not associated with a semaphore.
       */
      ctx->valid_id = false;
      break;
    }

    case RtemsSemReqDelete_Pre_Id_Counting: {
      /*
       * While the ``id`` parameter is associated with a counting semaphore.
       */
      ctx->attribute_set |= RTEMS_COUNTING_SEMAPHORE;
      ctx->obtain_status = RTEMS_OBJECT_WAS_DELETED;
      break;
    }

    case RtemsSemReqDelete_Pre_Id_Simple: {
      /*
       * While the ``id`` parameter is associated with a simple binary
       * semaphore.
       */
      ctx->attribute_set |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
      ctx->obtain_status = RTEMS_OBJECT_WAS_DELETED;
      break;
    }

    case RtemsSemReqDelete_Pre_Id_Binary: {
      /*
       * While the ``id`` parameter is associated with a binary semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE;
      break;
    }

    case RtemsSemReqDelete_Pre_Id_PrioCeiling: {
      /*
       * While the ``id`` parameter is associated with a priority ceiling
       * semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING;
      break;
    }

    case RtemsSemReqDelete_Pre_Id_PrioInherit: {
      /*
       * While the ``id`` parameter is associated with a priority inheritance
       * semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY;
      break;
    }

    case RtemsSemReqDelete_Pre_Id_MrsP: {
      /*
       * While the ``id`` parameter is associated with a MrsP semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE |
        RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
      break;
    }

    case RtemsSemReqDelete_Pre_Id_NA:
      break;
  }
}

static void RtemsSemReqDelete_Pre_Discipline_Prepare(
  RtemsSemReqDelete_Context       *ctx,
  RtemsSemReqDelete_Pre_Discipline state
)
{
  switch ( state ) {
    case RtemsSemReqDelete_Pre_Discipline_FIFO: {
      /*
       * While the semaphore uses the FIFO task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_FIFO;
      break;
    }

    case RtemsSemReqDelete_Pre_Discipline_Priority: {
      /*
       * While the semaphore uses the priority task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_PRIORITY;
      break;
    }

    case RtemsSemReqDelete_Pre_Discipline_NA:
      break;
  }
}

static void RtemsSemReqDelete_Pre_State_Prepare(
  RtemsSemReqDelete_Context  *ctx,
  RtemsSemReqDelete_Pre_State state
)
{
  switch ( state ) {
    case RtemsSemReqDelete_Pre_State_GtZeroOrNoOwner: {
      /*
       * While the semaphore has a count greater than zero or no owner.
       */
      ctx->blocked = false;
      ctx->count = 1;
      break;
    }

    case RtemsSemReqDelete_Pre_State_Zero: {
      /*
       * While the semaphore has a count of zero or an owner.
       */
      ctx->blocked = false;
      ctx->count = 0;
      break;
    }

    case RtemsSemReqDelete_Pre_State_Blocked: {
      /*
       * While the semaphore has tasks blocked on the semaphore.
       */
      ctx->blocked = true;
      ctx->count = 0;
      break;
    }

    case RtemsSemReqDelete_Pre_State_NA:
      break;
  }
}

static void RtemsSemReqDelete_Post_Status_Check(
  RtemsSemReqDelete_Context    *ctx,
  RtemsSemReqDelete_Post_Status state
)
{
  switch ( state ) {
    case RtemsSemReqDelete_Post_Status_Ok: {
      /*
       * The return status of rtems_semaphore_delete() shall be
       * RTEMS_SUCCESSFUL.
       */
      ctx->semaphore_id = 0;
      T_rsc_success( ctx->delete_status );
      break;
    }

    case RtemsSemReqDelete_Post_Status_InvId: {
      /*
       * The return status of rtems_semaphore_delete() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->delete_status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSemReqDelete_Post_Status_InUse: {
      /*
       * The return status of rtems_semaphore_delete() shall be
       * RTEMS_RESOURCE_IN_USE.
       */
      T_rsc( ctx->delete_status, RTEMS_RESOURCE_IN_USE );
      break;
    }

    case RtemsSemReqDelete_Post_Status_NA:
      break;
  }
}

static void RtemsSemReqDelete_Post_Name_Check(
  RtemsSemReqDelete_Context  *ctx,
  RtemsSemReqDelete_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsSemReqDelete_Post_Name_Valid: {
      /*
       * The unique object name shall identify a semaphore.
       */
      id = 0;
      sc = rtems_semaphore_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->semaphore_id );
      break;
    }

    case RtemsSemReqDelete_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a semaphore.
       */
      sc = rtems_semaphore_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsSemReqDelete_Post_Name_NA:
      break;
  }
}

static void RtemsSemReqDelete_Post_Flush_Check(
  RtemsSemReqDelete_Context   *ctx,
  RtemsSemReqDelete_Post_Flush state
)
{
  switch ( state ) {
    case RtemsSemReqDelete_Post_Flush_FIFO: {
      /*
       * Tasks waiting at the semaphore shall be unblocked in FIFO order.
       */
      T_eq_u32( ctx->worker_counter[ 0 ], 1 );
      T_eq_u32( ctx->worker_counter[ 1 ], 2 );
      break;
    }

    case RtemsSemReqDelete_Post_Flush_Priority: {
      /*
       * Tasks waiting at the semaphore shall be unblocked in priority order.
       */
      T_eq_u32( ctx->worker_counter[ 0 ], 2 );
      T_eq_u32( ctx->worker_counter[ 1 ], 1 );
      break;
    }

    case RtemsSemReqDelete_Post_Flush_No: {
      /*
       * Tasks waiting at the semaphore shall remain blocked.
       */
      T_eq_u32( ctx->worker_counter[ 0 ], 0 );
      T_eq_u32( ctx->worker_counter[ 1 ], 0 );
      break;
    }

    case RtemsSemReqDelete_Post_Flush_NA:
      break;
  }
}

static void RtemsSemReqDelete_Setup( RtemsSemReqDelete_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id[ 0 ] = CreateTask( "WRK0", PRIO_HIGH );
  StartTask( ctx->worker_id[ 0 ], WorkerZero, ctx );
  ctx->worker_id[ 1 ] = CreateTask( "WRK1", PRIO_VERY_HIGH );
  StartTask( ctx->worker_id[ 1 ], WorkerOne, ctx );
}

static void RtemsSemReqDelete_Setup_Wrap( void *arg )
{
  RtemsSemReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqDelete_Setup( ctx );
}

static void RtemsSemReqDelete_Teardown( RtemsSemReqDelete_Context *ctx )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->worker_id ); ++i ) {
    DeleteTask( ctx->worker_id[ i ] );
  }

  RestoreRunnerPriority();
}

static void RtemsSemReqDelete_Teardown_Wrap( void *arg )
{
  RtemsSemReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqDelete_Teardown( ctx );
}

static void RtemsSemReqDelete_Prepare( RtemsSemReqDelete_Context *ctx )
{
  ctx->counter = 0;
  ctx->worker_counter[ 0 ] = 0;
  ctx->worker_counter[ 1 ] = 0;
  ctx->attribute_set = RTEMS_DEFAULT_ATTRIBUTES;
  ctx->valid_id = true;
  ctx->obtain_status = RTEMS_SUCCESSFUL;
}

static void RtemsSemReqDelete_Action( RtemsSemReqDelete_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    NAME,
    ctx->count,
    ctx->attribute_set,
    PRIO_ULTRA_HIGH,
    &ctx->semaphore_id
  );
  T_rsc_success( sc );

  if ( ctx->blocked ) {
    WakeUp( ctx, 0 );
    WakeUp( ctx, 1 );
  }

  if ( ctx->valid_id ) {
    ctx->id = ctx->semaphore_id;
  } else {
    ctx->id = 0;
  }

  ctx->delete_status = rtems_semaphore_delete( ctx->id );
}

static void RtemsSemReqDelete_Cleanup( RtemsSemReqDelete_Context *ctx )
{
  if ( ctx->semaphore_id != 0 ) {
    rtems_status_code sc;

    if ( ctx->count == 0 ) {
      sc = rtems_semaphore_release( ctx->semaphore_id );
      T_rsc_success( sc );
    }

    sc = rtems_semaphore_delete( ctx->semaphore_id );
    T_rsc_success( sc );

    ctx->semaphore_id = 0;
  }
}

static const RtemsSemReqDelete_Entry
RtemsSemReqDelete_Entries[] = {
  { 0, 0, 0, 0, RtemsSemReqDelete_Post_Status_Ok,
    RtemsSemReqDelete_Post_Name_Invalid, RtemsSemReqDelete_Post_Flush_No },
  { 0, 0, 0, 0, RtemsSemReqDelete_Post_Status_InUse,
    RtemsSemReqDelete_Post_Name_Valid, RtemsSemReqDelete_Post_Flush_No },
  { 1, 0, 0, 0, RtemsSemReqDelete_Post_Status_NA,
    RtemsSemReqDelete_Post_Name_NA, RtemsSemReqDelete_Post_Flush_NA },
  { 0, 0, 0, 0, RtemsSemReqDelete_Post_Status_InvId,
    RtemsSemReqDelete_Post_Name_Valid, RtemsSemReqDelete_Post_Flush_No },
  { 0, 0, 0, 0, RtemsSemReqDelete_Post_Status_Ok,
    RtemsSemReqDelete_Post_Name_Invalid, RtemsSemReqDelete_Post_Flush_FIFO },
  { 0, 0, 0, 0, RtemsSemReqDelete_Post_Status_Ok,
    RtemsSemReqDelete_Post_Name_Invalid, RtemsSemReqDelete_Post_Flush_Priority }
};

static const uint8_t
RtemsSemReqDelete_Map[] = {
  3, 3, 3, 3, 3, 3, 0, 0, 4, 0, 0, 5, 0, 0, 4, 0, 0, 5, 0, 1, 1, 0, 1, 1, 2, 2,
  2, 0, 1, 1, 2, 2, 2, 0, 1, 1, 2, 2, 2, 0, 1, 1
};

static size_t RtemsSemReqDelete_Scope( void *arg, char *buf, size_t n )
{
  RtemsSemReqDelete_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSemReqDelete_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSemReqDelete_Fixture = {
  .setup = RtemsSemReqDelete_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemReqDelete_Teardown_Wrap,
  .scope = RtemsSemReqDelete_Scope,
  .initial_context = &RtemsSemReqDelete_Instance
};

static inline RtemsSemReqDelete_Entry RtemsSemReqDelete_PopEntry(
  RtemsSemReqDelete_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSemReqDelete_Entries[
    RtemsSemReqDelete_Map[ index ]
  ];
}

static void RtemsSemReqDelete_TestVariant( RtemsSemReqDelete_Context *ctx )
{
  RtemsSemReqDelete_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSemReqDelete_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSemReqDelete_Pre_State_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSemReqDelete_Action( ctx );
  RtemsSemReqDelete_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsSemReqDelete_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsSemReqDelete_Post_Flush_Check( ctx, ctx->Map.entry.Post_Flush );
}

/**
 * @fn void T_case_body_RtemsSemReqDelete( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemReqDelete, &RtemsSemReqDelete_Fixture )
{
  RtemsSemReqDelete_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSemReqDelete_Pre_Id_NoObj;
    ctx->Map.pcs[ 0 ] < RtemsSemReqDelete_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSemReqDelete_Pre_Discipline_FIFO;
      ctx->Map.pcs[ 1 ] < RtemsSemReqDelete_Pre_Discipline_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsSemReqDelete_Pre_State_GtZeroOrNoOwner;
        ctx->Map.pcs[ 2 ] < RtemsSemReqDelete_Pre_State_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsSemReqDelete_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        RtemsSemReqDelete_Prepare( ctx );
        RtemsSemReqDelete_TestVariant( ctx );
        RtemsSemReqDelete_Cleanup( ctx );
      }
    }
  }
}

/** @} */
