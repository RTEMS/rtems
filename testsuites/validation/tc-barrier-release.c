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
  RtemsBarrierReqRelease_Post_Released_Valid,
  RtemsBarrierReqRelease_Post_Released_Unchanged,
  RtemsBarrierReqRelease_Post_Released_NA
} RtemsBarrierReqRelease_Post_Released;

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

typedef enum {
  PRIO_HIGH = 1,
  PRIO_NORMAL
} Priorities;

static void SendEvents( rtems_id id, rtems_event_set events )
{
  rtems_status_code sc;

  sc = rtems_event_send( id, events );
  T_rsc_success( sc );
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_status_code sc;
    rtems_event_set   events;

    events = 0;
    sc = rtems_event_receive(
      RTEMS_ALL_EVENTS,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    T_rsc_success( sc );

    if ( ( events & EVENT_WAIT ) != 0 ) {
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
       * The ``id`` parameter shall not be associated with a barrier.
       */
      ctx->id = 0xffffffff;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Id_Manual: {
      /*
       * The ``id`` parameter shall be associated with a manual release
       * barrier.
       */
      ctx->id = ctx->manual_release_id;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Id_Auto: {
      /*
       * The ``id`` parameter shall be associated with an automatic release
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
       * The ``released`` parameter shall reference an object of type uint32_t.
       */
      ctx->released = &ctx->released_value;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Released_Null: {
      /*
       * The ``released`` parameter shall be NULL.
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
       * The number of tasks waiting at the barrier shall be zero.
       */
      ctx->waiting_tasks = 0;
      break;
    }

    case RtemsBarrierReqRelease_Pre_Waiting_Positive: {
      /*
       * The number of tasks waiting at the barrier shall be positive.
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

static void RtemsBarrierReqRelease_Post_Released_Check(
  RtemsBarrierReqRelease_Context      *ctx,
  RtemsBarrierReqRelease_Post_Released state
)
{
  switch ( state ) {
    case RtemsBarrierReqRelease_Post_Released_Valid: {
      /*
       * The value of the variable for the number of released tasks shall equal
       * the number of tasks released by the rtems_barrier_release() call.
       */
      T_eq_u32( ctx->released_value, ctx->waiting_tasks );
      break;
    }

    case RtemsBarrierReqRelease_Post_Released_Unchanged: {
      /*
       * The value of variable for the number of released tasks shall be
       * unchanged by the rtems_barrier_release() call.
       */
      T_eq_u32( ctx->released_value, RELEASED_INVALID_VALUE );
      break;
    }

    case RtemsBarrierReqRelease_Post_Released_NA:
      break;
  }
}

static void RtemsBarrierReqRelease_Setup( RtemsBarrierReqRelease_Context *ctx )
{
  rtems_status_code   sc;
  rtems_task_priority prio;

  memset( ctx, 0, sizeof( *ctx ) );

  prio = 0;
  sc = rtems_task_set_priority( RTEMS_SELF, PRIO_NORMAL, &prio );
  T_rsc_success( sc );
  T_eq_u32( prio, PRIO_HIGH );

  sc = rtems_task_create(
    rtems_build_name( 'W', 'O', 'R', 'K' ),
    PRIO_HIGH,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_id
  );
  T_assert_rsc_success( sc );

  sc = rtems_task_start(
    ctx->worker_id,
    Worker,
    (rtems_task_argument) ctx
  );
  T_assert_rsc_success( sc );

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
  rtems_status_code   sc;
  rtems_task_priority prio;

  prio = 0;
  sc = rtems_task_set_priority( RTEMS_SELF, PRIO_HIGH, &prio );
  T_rsc_success( sc );
  T_eq_u32( prio, PRIO_NORMAL );

  if ( ctx->worker_id != 0 ) {
    sc = rtems_task_delete( ctx->worker_id );
    T_rsc_success( sc );
  }

  if ( ctx->manual_release_id != 0 ) {
    sc = rtems_barrier_delete( ctx->manual_release_id );
    T_rsc_success( sc );
  }

  if ( ctx->auto_release_id != 0 ) {
    sc = rtems_barrier_delete( ctx->auto_release_id );
    T_rsc_success( sc );
  }
}

static void RtemsBarrierReqRelease_Teardown_Wrap( void *arg )
{
  RtemsBarrierReqRelease_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsBarrierReqRelease_Teardown( ctx );
}

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

static const uint8_t RtemsBarrierReqRelease_TransitionMap[][ 2 ] = {
  {
    RtemsBarrierReqRelease_Post_Status_InvId,
    RtemsBarrierReqRelease_Post_Released_Unchanged
  }, {
    RtemsBarrierReqRelease_Post_Status_InvId,
    RtemsBarrierReqRelease_Post_Released_Unchanged
  }, {
    RtemsBarrierReqRelease_Post_Status_InvAddr,
    RtemsBarrierReqRelease_Post_Released_Unchanged
  }, {
    RtemsBarrierReqRelease_Post_Status_InvAddr,
    RtemsBarrierReqRelease_Post_Released_Unchanged
  }, {
    RtemsBarrierReqRelease_Post_Status_Ok,
    RtemsBarrierReqRelease_Post_Released_Valid
  }, {
    RtemsBarrierReqRelease_Post_Status_Ok,
    RtemsBarrierReqRelease_Post_Released_Valid
  }, {
    RtemsBarrierReqRelease_Post_Status_InvAddr,
    RtemsBarrierReqRelease_Post_Released_Unchanged
  }, {
    RtemsBarrierReqRelease_Post_Status_InvAddr,
    RtemsBarrierReqRelease_Post_Released_Unchanged
  }, {
    RtemsBarrierReqRelease_Post_Status_Ok,
    RtemsBarrierReqRelease_Post_Released_Valid
  }, {
    RtemsBarrierReqRelease_Post_Status_Ok,
    RtemsBarrierReqRelease_Post_Released_Valid
  }, {
    RtemsBarrierReqRelease_Post_Status_InvAddr,
    RtemsBarrierReqRelease_Post_Released_Unchanged
  }, {
    RtemsBarrierReqRelease_Post_Status_InvAddr,
    RtemsBarrierReqRelease_Post_Released_Unchanged
  }
};

static const struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_Released_NA : 1;
  uint8_t Pre_Waiting_NA : 1;
} RtemsBarrierReqRelease_TransitionInfo[] = {
  {
    0, 0, 0, 1
  }, {
    0, 0, 0, 1
  }, {
    0, 0, 0, 1
  }, {
    0, 0, 0, 1
  }, {
    0, 0, 0, 0
  }, {
    0, 0, 0, 0
  }, {
    0, 0, 0, 1
  }, {
    0, 0, 0, 1
  }, {
    0, 0, 0, 0
  }, {
    0, 0, 0, 0
  }, {
    0, 0, 0, 1
  }, {
    0, 0, 0, 1
  }
};

static void RtemsBarrierReqRelease_Action(
  RtemsBarrierReqRelease_Context *ctx
)
{
  ctx->status = rtems_barrier_release( ctx->id, ctx->released );
}

/**
 * @fn void T_case_body_RtemsBarrierReqRelease( void )
 */
T_TEST_CASE_FIXTURE( RtemsBarrierReqRelease, &RtemsBarrierReqRelease_Fixture )
{
  RtemsBarrierReqRelease_Context *ctx;
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsBarrierReqRelease_Pre_Id_NoObj;
    ctx->pcs[ 0 ] < RtemsBarrierReqRelease_Pre_Id_NA;
    ++ctx->pcs[ 0 ]
  ) {
    if ( RtemsBarrierReqRelease_TransitionInfo[ index ].Pre_Id_NA ) {
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
      if ( RtemsBarrierReqRelease_TransitionInfo[ index ].Pre_Released_NA ) {
        ctx->pcs[ 1 ] = RtemsBarrierReqRelease_Pre_Released_NA;
        index += ( RtemsBarrierReqRelease_Pre_Released_NA - 1 )
          * RtemsBarrierReqRelease_Pre_Waiting_NA;
      }

      for (
        ctx->pcs[ 2 ] = RtemsBarrierReqRelease_Pre_Waiting_Zero;
        ctx->pcs[ 2 ] < RtemsBarrierReqRelease_Pre_Waiting_NA;
        ++ctx->pcs[ 2 ]
      ) {
        if ( RtemsBarrierReqRelease_TransitionInfo[ index ].Pre_Waiting_NA ) {
          ctx->pcs[ 2 ] = RtemsBarrierReqRelease_Pre_Waiting_NA;
          index += ( RtemsBarrierReqRelease_Pre_Waiting_NA - 1 );
        }

        if ( RtemsBarrierReqRelease_TransitionInfo[ index ].Skip ) {
          ++index;
          continue;
        }

        RtemsBarrierReqRelease_Pre_Id_Prepare( ctx, ctx->pcs[ 0 ] );
        RtemsBarrierReqRelease_Pre_Released_Prepare( ctx, ctx->pcs[ 1 ] );
        RtemsBarrierReqRelease_Pre_Waiting_Prepare( ctx, ctx->pcs[ 2 ] );
        RtemsBarrierReqRelease_Action( ctx );
        RtemsBarrierReqRelease_Post_Status_Check(
          ctx,
          RtemsBarrierReqRelease_TransitionMap[ index ][ 0 ]
        );
        RtemsBarrierReqRelease_Post_Released_Check(
          ctx,
          RtemsBarrierReqRelease_TransitionMap[ index ][ 1 ]
        );
        ++index;
      }
    }
  }
}

/** @} */
