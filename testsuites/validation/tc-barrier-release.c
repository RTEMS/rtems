/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsBarrierReqRelease
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
 * @defgroup RTEMSTestCaseRtemsBarrierReqRelease \
 *   spec:/rtems/barrier/req/release
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 *
 * @{
 */

typedef enum {
  RtemsBarrierReqRelease_Pre_Id_NoObj,
  RtemsBarrierReqRelease_Pre_Id_Manual,
  RtemsBarrierReqRelease_Pre_Id_Auto,
  RtemsBarrierReqRelease_Pre_Id_NA
} RtemsBarrierReqRelease_Pre_Id;

typedef enum {
  RtemsBarrierReqRelease_Pre_Released_Valid,
  RtemsBarrierReqRelease_Pre_Released_Null,
  RtemsBarrierReqRelease_Pre_Released_NA
} RtemsBarrierReqRelease_Pre_Released;

typedef enum {
  RtemsBarrierReqRelease_Pre_Waiting_Zero,
  RtemsBarrierReqRelease_Pre_Waiting_Positive,
  RtemsBarrierReqRelease_Pre_Waiting_NA
} RtemsBarrierReqRelease_Pre_Waiting;

typedef enum {
  RtemsBarrierReqRelease_Post_Status_Ok,
  RtemsBarrierReqRelease_Post_Status_InvId,
  RtemsBarrierReqRelease_Post_Status_InvAddr,
  RtemsBarrierReqRelease_Post_Status_NA
} RtemsBarrierReqRelease_Post_Status;

typedef enum {
  RtemsBarrierReqRelease_Post_ReleasedVar_Set,
  RtemsBarrierReqRelease_Post_ReleasedVar_Nop,
  RtemsBarrierReqRelease_Post_ReleasedVar_NA
} RtemsBarrierReqRelease_Post_ReleasedVar;

/**
 * @brief Test context for spec:/rtems/barrier/req/release test case.
 */
typedef struct {
  rtems_id worker_id;

  rtems_id manual_release_id;

  rtems_id auto_release_id;

  uint32_t waiting_tasks;

  uint32_t released_value;

  rtems_id id;

  uint32_t *released;

  rtems_status_code status;

  /**
   * @brief This member defines the pre-condition states for the next action.
   */
  size_t pcs[ 3 ];

  /**
   * @brief This member indicates if the test action loop is currently
   *   executed.
   */
  bool in_action_loop;
} RtemsBarrierReqRelease_Context;

static RtemsBarrierReqRelease_Context
  RtemsBarrierReqRelease_Instance;

static const char * const RtemsBarrierReqRelease_PreDesc_Id[] = {
  "NoObj",
  "Manual",
  "Auto",
  "NA"
};

static const char * const RtemsBarrierReqRelease_PreDesc_Released[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsBarrierReqRelease_PreDesc_Waiting[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const * const RtemsBarrierReqRelease_PreDesc[] = {
  RtemsBarrierReqRelease_PreDesc_Id,
  RtemsBarrierReqRelease_PreDesc_Released,
  RtemsBarrierReqRelease_PreDesc_Waiting,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

#define EVENT_WAIT RTEMS_EVENT_0

#define RELEASED_INVALID_VALUE 0xffffffff

typedef RtemsBarrierReqRelease_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_WAIT ) != 0 ) {
      rtems_status_code sc;

      sc = rtems_barrier_wait( ctx->id, RTEMS_NO_TIMEOUT );
      T_rsc_success( sc );
    }
  }
}

static void RtemsBarrierReqRelease_Pre_Id_Prepare(
  RtemsBarrierReqRelease_Context *ctx,
  RtemsBarrierReqRelease_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsBarrierReqRelease_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is not associated with a barrier.
       */
      ctx->id = 0xffffffff;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Id_Manual: {
      /*
       * While the ``id`` parameter is associated with a manual release
       * barrier.
       */
      ctx->id = ctx->manual_release_id;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Id_Auto: {
      /*
       * While the ``id`` parameter is associated with an automatic release
       * barrier.
       */
      ctx->id = ctx->auto_release_id;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Id_NA:
      break;
  }
}

static void RtemsBarrierReqRelease_Pre_Released_Prepare(
  RtemsBarrierReqRelease_Context     *ctx,
  RtemsBarrierReqRelease_Pre_Released state
)
{
  ctx->released_value = RELEASED_INVALID_VALUE;

  switch ( state ) {
    case RtemsBarrierReqRelease_Pre_Released_Valid: {
      /*
       * While the ``released`` parameter references an object of type
       * uint32_t.
       */
      ctx->released = &ctx->released_value;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Released_Null: {
      /*
       * While the ``released`` parameter is NULL.
       */
      ctx->released = NULL;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Released_NA:
      break;
  }
}

static void RtemsBarrierReqRelease_Pre_Waiting_Prepare(
  RtemsBarrierReqRelease_Context    *ctx,
  RtemsBarrierReqRelease_Pre_Waiting state
)
{
  switch ( state ) {
    case RtemsBarrierReqRelease_Pre_Waiting_Zero: {
      /*
       * While the number of tasks waiting at the barrier is zero.
       */
      ctx->waiting_tasks = 0;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Waiting_Positive: {
      /*
       * While the number of tasks waiting at the barrier is positive.
       */
      ctx->waiting_tasks = 1;
      SendEvents( ctx->worker_id, EVENT_WAIT );
      break;
    }

    case RtemsBarrierReqRelease_Pre_Waiting_NA:
      break;
  }
}

static void RtemsBarrierReqRelease_Post_Status_Check(
  RtemsBarrierReqRelease_Context    *ctx,
  RtemsBarrierReqRelease_Post_Status state
)
{
  switch ( state ) {
    case RtemsBarrierReqRelease_Post_Status_Ok: {
      /*
       * The return status of rtems_barrier_release() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsBarrierReqRelease_Post_Status_InvId: {
      /*
       * The return status of rtems_barrier_release() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsBarrierReqRelease_Post_Status_InvAddr: {
      /*
       * The return status of rtems_barrier_release() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsBarrierReqRelease_Post_Status_NA:
      break;
  }
}

static void RtemsBarrierReqRelease_Post_ReleasedVar_Check(
  RtemsBarrierReqRelease_Context         *ctx,
  RtemsBarrierReqRelease_Post_ReleasedVar state
)
{
  switch ( state ) {
    case RtemsBarrierReqRelease_Post_ReleasedVar_Set: {
      /*
       * The value of the object referenced by the ``released`` parameter shall
       * be set to the number of released tasks after the return of the
       * rtems_barrier_release() call.
       */
      T_eq_u32( ctx->released_value, ctx->waiting_tasks );
      break;
    }

    case RtemsBarrierReqRelease_Post_ReleasedVar_Nop: {
      /*
       * Objects referenced by the ``released`` parameter in past calls to
       * rtems_barrier_release() shall not be accessed by the
       * rtems_barrier_release() call.
       */
      T_eq_u32( ctx->released_value, RELEASED_INVALID_VALUE );
      break;
    }

    case RtemsBarrierReqRelease_Post_ReleasedVar_NA:
      break;
  }
}

static void RtemsBarrierReqRelease_Setup( RtemsBarrierReqRelease_Context *ctx )
{
  rtems_status_code sc;

  memset( ctx, 0, sizeof( *ctx ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );

  sc = rtems_barrier_create(
    NAME,
    RTEMS_BARRIER_MANUAL_RELEASE,
    0,
    &ctx->manual_release_id
  );
  T_assert_rsc_success( sc );

  sc = rtems_barrier_create(
    NAME,
    RTEMS_BARRIER_AUTOMATIC_RELEASE,
    2,
    &ctx->auto_release_id
  );
  T_assert_rsc_success( sc );
}

static void RtemsBarrierReqRelease_Setup_Wrap( void *arg )
{
  RtemsBarrierReqRelease_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsBarrierReqRelease_Setup( ctx );
}

static void RtemsBarrierReqRelease_Teardown(
  RtemsBarrierReqRelease_Context *ctx
)
{
  rtems_status_code sc;

  DeleteTask( ctx->worker_id );

  if ( ctx->manual_release_id != 0 ) {
    sc = rtems_barrier_delete( ctx->manual_release_id );
    T_rsc_success( sc );
  }

  if ( ctx->auto_release_id != 0 ) {
    sc = rtems_barrier_delete( ctx->auto_release_id );
    T_rsc_success( sc );
  }

  RestoreRunnerPriority();
}

static void RtemsBarrierReqRelease_Teardown_Wrap( void *arg )
{
  RtemsBarrierReqRelease_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsBarrierReqRelease_Teardown( ctx );
}

static void RtemsBarrierReqRelease_Action(
  RtemsBarrierReqRelease_Context *ctx
)
{
  ctx->status = rtems_barrier_release( ctx->id, ctx->released );
}

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_Released_NA : 1;
  uint8_t Pre_Waiting_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_ReleasedVar : 2;
} RtemsBarrierReqRelease_Entry;

static const RtemsBarrierReqRelease_Entry
RtemsBarrierReqRelease_Entries[] = {
  { 0, 0, 0, 1, RtemsBarrierReqRelease_Post_Status_InvAddr,
    RtemsBarrierReqRelease_Post_ReleasedVar_Nop },
  { 0, 0, 0, 0, RtemsBarrierReqRelease_Post_Status_Ok,
    RtemsBarrierReqRelease_Post_ReleasedVar_Set },
  { 0, 0, 0, 1, RtemsBarrierReqRelease_Post_Status_InvId,
    RtemsBarrierReqRelease_Post_ReleasedVar_Nop }
};

static const uint8_t
RtemsBarrierReqRelease_Map[] = {
  2, 2, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0
};

static size_t RtemsBarrierReqRelease_Scope( void *arg, char *buf, size_t n )
{
  RtemsBarrierReqRelease_Context *ctx;

  ctx = arg;

  if ( ctx->in_action_loop ) {
    return T_get_scope( RtemsBarrierReqRelease_PreDesc, buf, n, ctx->pcs );
  }

  return 0;
}

static T_fixture RtemsBarrierReqRelease_Fixture = {
  .setup = RtemsBarrierReqRelease_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsBarrierReqRelease_Teardown_Wrap,
  .scope = RtemsBarrierReqRelease_Scope,
  .initial_context = &RtemsBarrierReqRelease_Instance
};

static inline RtemsBarrierReqRelease_Entry RtemsBarrierReqRelease_GetEntry(
  size_t index
)
{
  return RtemsBarrierReqRelease_Entries[
    RtemsBarrierReqRelease_Map[ index ]
  ];
}

/**
 * @fn void T_case_body_RtemsBarrierReqRelease( void )
 */
T_TEST_CASE_FIXTURE( RtemsBarrierReqRelease, &RtemsBarrierReqRelease_Fixture )
{
  RtemsBarrierReqRelease_Context *ctx;
  RtemsBarrierReqRelease_Entry entry;
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsBarrierReqRelease_Pre_Id_NoObj;
    ctx->pcs[ 0 ] < RtemsBarrierReqRelease_Pre_Id_NA;
    ++ctx->pcs[ 0 ]
  ) {
    entry = RtemsBarrierReqRelease_GetEntry( index );

    if ( entry.Pre_Id_NA ) {
      ctx->pcs[ 0 ] = RtemsBarrierReqRelease_Pre_Id_NA;
      index += ( RtemsBarrierReqRelease_Pre_Id_NA - 1 )
        * RtemsBarrierReqRelease_Pre_Released_NA
        * RtemsBarrierReqRelease_Pre_Waiting_NA;
    }

    for (
      ctx->pcs[ 1 ] = RtemsBarrierReqRelease_Pre_Released_Valid;
      ctx->pcs[ 1 ] < RtemsBarrierReqRelease_Pre_Released_NA;
      ++ctx->pcs[ 1 ]
    ) {
      entry = RtemsBarrierReqRelease_GetEntry( index );

      if ( entry.Pre_Released_NA ) {
        ctx->pcs[ 1 ] = RtemsBarrierReqRelease_Pre_Released_NA;
        index += ( RtemsBarrierReqRelease_Pre_Released_NA - 1 )
          * RtemsBarrierReqRelease_Pre_Waiting_NA;
      }

      for (
        ctx->pcs[ 2 ] = RtemsBarrierReqRelease_Pre_Waiting_Zero;
        ctx->pcs[ 2 ] < RtemsBarrierReqRelease_Pre_Waiting_NA;
        ++ctx->pcs[ 2 ]
      ) {
        entry = RtemsBarrierReqRelease_GetEntry( index );

        if ( entry.Pre_Waiting_NA ) {
          ctx->pcs[ 2 ] = RtemsBarrierReqRelease_Pre_Waiting_NA;
          index += ( RtemsBarrierReqRelease_Pre_Waiting_NA - 1 );
        }

        if ( entry.Skip ) {
          ++index;
          continue;
        }

        RtemsBarrierReqRelease_Pre_Id_Prepare( ctx, ctx->pcs[ 0 ] );
        RtemsBarrierReqRelease_Pre_Released_Prepare( ctx, ctx->pcs[ 1 ] );
        RtemsBarrierReqRelease_Pre_Waiting_Prepare( ctx, ctx->pcs[ 2 ] );
        RtemsBarrierReqRelease_Action( ctx );
        RtemsBarrierReqRelease_Post_Status_Check( ctx, entry.Post_Status );
        RtemsBarrierReqRelease_Post_ReleasedVar_Check(
          ctx,
          entry.Post_ReleasedVar
        );
        ++index;
      }
    }
  }
}

/** @} */
