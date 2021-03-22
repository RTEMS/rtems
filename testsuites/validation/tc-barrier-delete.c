/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsBarrierReqDelete
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

#include "tc-support.h"

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestCaseRtemsBarrierReqDelete spec:/rtems/barrier/req/delete
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 *
 * @{
 */

typedef enum {
  RtemsBarrierReqDelete_Pre_Id_NoObj,
  RtemsBarrierReqDelete_Pre_Id_Barrier,
  RtemsBarrierReqDelete_Pre_Id_NA
} RtemsBarrierReqDelete_Pre_Id;

typedef enum {
  RtemsBarrierReqDelete_Post_Status_Ok,
  RtemsBarrierReqDelete_Post_Status_InvId,
  RtemsBarrierReqDelete_Post_Status_NA
} RtemsBarrierReqDelete_Post_Status;

typedef enum {
  RtemsBarrierReqDelete_Post_Name_Valid,
  RtemsBarrierReqDelete_Post_Name_Invalid,
  RtemsBarrierReqDelete_Post_Name_NA
} RtemsBarrierReqDelete_Post_Name;

typedef enum {
  RtemsBarrierReqDelete_Post_Flush_Yes,
  RtemsBarrierReqDelete_Post_Flush_No,
  RtemsBarrierReqDelete_Post_Flush_NA
} RtemsBarrierReqDelete_Post_Flush;

/**
 * @brief Test context for spec:/rtems/barrier/req/delete test case.
 */
typedef struct {
  rtems_id worker_id;

  rtems_id barrier_id;

  uint32_t wait_done;

  uint32_t wait_expected;

  rtems_id id;

  rtems_status_code status;

  /**
   * @brief This member defines the pre-condition states for the next action.
   */
  size_t pcs[ 1 ];

  /**
   * @brief This member indicates if the test action loop is currently
   *   executed.
   */
  bool in_action_loop;
} RtemsBarrierReqDelete_Context;

static RtemsBarrierReqDelete_Context
  RtemsBarrierReqDelete_Instance;

static const char * const RtemsBarrierReqDelete_PreDesc_Id[] = {
  "NoObj",
  "Barrier",
  "NA"
};

static const char * const * const RtemsBarrierReqDelete_PreDesc[] = {
  RtemsBarrierReqDelete_PreDesc_Id,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

typedef RtemsBarrierReqDelete_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_status_code   sc;
    rtems_task_priority prio;

    T_eq_u32( ctx->barrier_id, 0 );

    sc = rtems_barrier_create(
      NAME,
      RTEMS_DEFAULT_ATTRIBUTES,
      0,
      &ctx->barrier_id
    );
    T_rsc_success( sc );

    sc = rtems_barrier_wait(
      ctx->barrier_id,
      RTEMS_NO_TIMEOUT
    );
    T_rsc( sc, RTEMS_OBJECT_WAS_DELETED );

    ++ctx->wait_done;

    prio = SetSelfPriority( PRIO_LOW );
    T_eq_u32( prio, PRIO_HIGH );
  }
}

static void RtemsBarrierReqDelete_Pre_Id_Prepare(
  RtemsBarrierReqDelete_Context *ctx,
  RtemsBarrierReqDelete_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsBarrierReqDelete_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is not associated with a barrier.
       */
      ctx->id = 0;
      break;
    }

    case RtemsBarrierReqDelete_Pre_Id_Barrier: {
      /*
       * While the ``id`` parameter is associated with a barrier.
       */
      ctx->id = ctx->barrier_id;
      break;
    }

    case RtemsBarrierReqDelete_Pre_Id_NA:
      break;
  }
}

static void RtemsBarrierReqDelete_Post_Status_Check(
  RtemsBarrierReqDelete_Context    *ctx,
  RtemsBarrierReqDelete_Post_Status state
)
{
  switch ( state ) {
    case RtemsBarrierReqDelete_Post_Status_Ok: {
      /*
       * The return status of rtems_barrier_delete() shall be RTEMS_SUCCESSFUL.
       */
      ctx->barrier_id = 0;
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsBarrierReqDelete_Post_Status_InvId: {
      /*
       * The return status of rtems_barrier_delete() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsBarrierReqDelete_Post_Status_NA:
      break;
  }
}

static void RtemsBarrierReqDelete_Post_Name_Check(
  RtemsBarrierReqDelete_Context  *ctx,
  RtemsBarrierReqDelete_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsBarrierReqDelete_Post_Name_Valid: {
      /*
       * The unique object name shall identify a barrier.
       */
      id = 0;
      sc = rtems_barrier_ident( NAME, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->barrier_id );
      break;
    }

    case RtemsBarrierReqDelete_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a barrier.
       */
      sc = rtems_barrier_ident( NAME, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsBarrierReqDelete_Post_Name_NA:
      break;
  }
}

static void RtemsBarrierReqDelete_Post_Flush_Check(
  RtemsBarrierReqDelete_Context   *ctx,
  RtemsBarrierReqDelete_Post_Flush state
)
{
  switch ( state ) {
    case RtemsBarrierReqDelete_Post_Flush_Yes: {
      /*
       * Tasks waiting at the barrier shall be unblocked.
       */
      ++ctx->wait_expected;
      T_eq_u32( ctx->wait_done, ctx->wait_expected );
      break;
    }

    case RtemsBarrierReqDelete_Post_Flush_No: {
      /*
       * Tasks waiting at the barrier shall remain blocked.
       */
      T_eq_u32( ctx->wait_done, ctx->wait_expected );
      break;
    }

    case RtemsBarrierReqDelete_Post_Flush_NA:
      break;
  }
}

static void RtemsBarrierReqDelete_Setup( RtemsBarrierReqDelete_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_LOW );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsBarrierReqDelete_Setup_Wrap( void *arg )
{
  RtemsBarrierReqDelete_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsBarrierReqDelete_Setup( ctx );
}

static void RtemsBarrierReqDelete_Teardown(
  RtemsBarrierReqDelete_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void RtemsBarrierReqDelete_Teardown_Wrap( void *arg )
{
  RtemsBarrierReqDelete_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsBarrierReqDelete_Teardown( ctx );
}

static void RtemsBarrierReqDelete_Prepare( RtemsBarrierReqDelete_Context *ctx )
{
  rtems_task_priority prio;

  prio = SetPriority( ctx->worker_id, PRIO_HIGH );
  T_true( prio == PRIO_LOW || prio == PRIO_HIGH );
}

static void RtemsBarrierReqDelete_Action( RtemsBarrierReqDelete_Context *ctx )
{
  ctx->status = rtems_barrier_delete( ctx->id );
}

static void RtemsBarrierReqDelete_Cleanup( RtemsBarrierReqDelete_Context *ctx )
{
  if ( ctx->barrier_id != 0 ) {
    rtems_status_code sc;

    sc = rtems_barrier_delete( ctx->barrier_id );
    T_rsc_success( sc );

    ++ctx->wait_expected;
    T_eq_u32( ctx->wait_done, ctx->wait_expected );

    ctx->barrier_id = 0;
  }
}

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Name : 2;
  uint8_t Post_Flush : 2;
} RtemsBarrierReqDelete_Entry;

static const RtemsBarrierReqDelete_Entry
RtemsBarrierReqDelete_Entries[] = {
  { 0, 0, RtemsBarrierReqDelete_Post_Status_InvId,
    RtemsBarrierReqDelete_Post_Name_Valid, RtemsBarrierReqDelete_Post_Flush_No },
  { 0, 0, RtemsBarrierReqDelete_Post_Status_Ok,
    RtemsBarrierReqDelete_Post_Name_Invalid,
    RtemsBarrierReqDelete_Post_Flush_Yes }
};

static const uint8_t
RtemsBarrierReqDelete_Map[] = {
  0, 1
};

static size_t RtemsBarrierReqDelete_Scope( void *arg, char *buf, size_t n )
{
  RtemsBarrierReqDelete_Context *ctx;

  ctx = arg;

  if ( ctx->in_action_loop ) {
    return T_get_scope( RtemsBarrierReqDelete_PreDesc, buf, n, ctx->pcs );
  }

  return 0;
}

static T_fixture RtemsBarrierReqDelete_Fixture = {
  .setup = RtemsBarrierReqDelete_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsBarrierReqDelete_Teardown_Wrap,
  .scope = RtemsBarrierReqDelete_Scope,
  .initial_context = &RtemsBarrierReqDelete_Instance
};

static inline RtemsBarrierReqDelete_Entry RtemsBarrierReqDelete_GetEntry(
  size_t index
)
{
  return RtemsBarrierReqDelete_Entries[
    RtemsBarrierReqDelete_Map[ index ]
  ];
}

/**
 * @fn void T_case_body_RtemsBarrierReqDelete( void )
 */
T_TEST_CASE_FIXTURE( RtemsBarrierReqDelete, &RtemsBarrierReqDelete_Fixture )
{
  RtemsBarrierReqDelete_Context *ctx;
  RtemsBarrierReqDelete_Entry entry;
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsBarrierReqDelete_Pre_Id_NoObj;
    ctx->pcs[ 0 ] < RtemsBarrierReqDelete_Pre_Id_NA;
    ++ctx->pcs[ 0 ]
  ) {
    entry = RtemsBarrierReqDelete_GetEntry( index );

    if ( entry.Pre_Id_NA ) {
      ctx->pcs[ 0 ] = RtemsBarrierReqDelete_Pre_Id_NA;
      index += ( RtemsBarrierReqDelete_Pre_Id_NA - 1 );
    }

    if ( entry.Skip ) {
      ++index;
      continue;
    }

    RtemsBarrierReqDelete_Prepare( ctx );
    RtemsBarrierReqDelete_Pre_Id_Prepare( ctx, ctx->pcs[ 0 ] );
    RtemsBarrierReqDelete_Action( ctx );
    RtemsBarrierReqDelete_Post_Status_Check( ctx, entry.Post_Status );
    RtemsBarrierReqDelete_Post_Name_Check( ctx, entry.Post_Name );
    RtemsBarrierReqDelete_Post_Flush_Check( ctx, entry.Post_Flush );
    RtemsBarrierReqDelete_Cleanup( ctx );
    ++index;
  }
}

/** @} */
