/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsBarrierReqGetNumberWaiting
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsBarrierReqGetNumberWaiting \
 *   spec:/rtems/barrier/req/get-number-waiting
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsBarrierReqGetNumberWaiting_Pre_Id_NoObj,
  RtemsBarrierReqGetNumberWaiting_Pre_Id_Barrier,
  RtemsBarrierReqGetNumberWaiting_Pre_Id_NA
} RtemsBarrierReqGetNumberWaiting_Pre_Id;

typedef enum {
  RtemsBarrierReqGetNumberWaiting_Pre_Waiting_Valid,
  RtemsBarrierReqGetNumberWaiting_Pre_Waiting_Null,
  RtemsBarrierReqGetNumberWaiting_Pre_Waiting_NA
} RtemsBarrierReqGetNumberWaiting_Pre_Waiting;

typedef enum {
  RtemsBarrierReqGetNumberWaiting_Post_Status_Ok,
  RtemsBarrierReqGetNumberWaiting_Post_Status_InvId,
  RtemsBarrierReqGetNumberWaiting_Post_Status_InvAddr,
  RtemsBarrierReqGetNumberWaiting_Post_Status_NA
} RtemsBarrierReqGetNumberWaiting_Post_Status;

typedef enum {
  RtemsBarrierReqGetNumberWaiting_Post_Waiting_Waiting,
  RtemsBarrierReqGetNumberWaiting_Post_Waiting_Nop,
  RtemsBarrierReqGetNumberWaiting_Post_Waiting_NA
} RtemsBarrierReqGetNumberWaiting_Post_Waiting;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_Waiting_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Waiting : 2;
} RtemsBarrierReqGetNumberWaiting_Entry;

/**
 * @brief Test context for spec:/rtems/barrier/req/get-number-waiting test
 *   case.
 */
typedef struct {
  rtems_id barrier_id;

  uint32_t waiting_obj;

  rtems_id id;

  uint32_t *waiting;

  rtems_status_code status;

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
    RtemsBarrierReqGetNumberWaiting_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsBarrierReqGetNumberWaiting_Context;

static RtemsBarrierReqGetNumberWaiting_Context
  RtemsBarrierReqGetNumberWaiting_Instance;

static const char * const RtemsBarrierReqGetNumberWaiting_PreDesc_Id[] = {
  "NoObj",
  "Barrier",
  "NA"
};

static const char * const RtemsBarrierReqGetNumberWaiting_PreDesc_Waiting[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsBarrierReqGetNumberWaiting_PreDesc[] = {
  RtemsBarrierReqGetNumberWaiting_PreDesc_Id,
  RtemsBarrierReqGetNumberWaiting_PreDesc_Waiting,
  NULL
};

static void RtemsBarrierReqGetNumberWaiting_Pre_Id_Prepare(
  RtemsBarrierReqGetNumberWaiting_Context *ctx,
  RtemsBarrierReqGetNumberWaiting_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsBarrierReqGetNumberWaiting_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is not associated with a barrier.
       */
      ctx->id = 0xffffffff;
      break;
    }

    case RtemsBarrierReqGetNumberWaiting_Pre_Id_Barrier: {
      /*
       * While the ``id`` parameter is associated with a barrier.
       */
      ctx->id = ctx->barrier_id;
      break;
    }

    case RtemsBarrierReqGetNumberWaiting_Pre_Id_NA:
      break;
  }
}

static void RtemsBarrierReqGetNumberWaiting_Pre_Waiting_Prepare(
  RtemsBarrierReqGetNumberWaiting_Context    *ctx,
  RtemsBarrierReqGetNumberWaiting_Pre_Waiting state
)
{
  switch ( state ) {
    case RtemsBarrierReqGetNumberWaiting_Pre_Waiting_Valid: {
      /*
       * While the ``waiting`` parameter references an object of type uint32_t.
       */
      ctx->waiting = &ctx->waiting_obj;
      break;
    }

    case RtemsBarrierReqGetNumberWaiting_Pre_Waiting_Null: {
      /*
       * While the ``waiting`` parameter is equal to NULL.
       */
      ctx->waiting = NULL;
      break;
    }

    case RtemsBarrierReqGetNumberWaiting_Pre_Waiting_NA:
      break;
  }
}

static void RtemsBarrierReqGetNumberWaiting_Post_Status_Check(
  RtemsBarrierReqGetNumberWaiting_Context    *ctx,
  RtemsBarrierReqGetNumberWaiting_Post_Status state
)
{
  switch ( state ) {
    case RtemsBarrierReqGetNumberWaiting_Post_Status_Ok: {
      /*
       * The return status of rtems_barrier_get_number_waiting() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsBarrierReqGetNumberWaiting_Post_Status_InvId: {
      /*
       * The return status of rtems_barrier_get_number_waiting() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsBarrierReqGetNumberWaiting_Post_Status_InvAddr: {
      /*
       * The return status of rtems_barrier_get_number_waiting() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsBarrierReqGetNumberWaiting_Post_Status_NA:
      break;
  }
}

static void RtemsBarrierReqGetNumberWaiting_Post_Waiting_Check(
  RtemsBarrierReqGetNumberWaiting_Context     *ctx,
  RtemsBarrierReqGetNumberWaiting_Post_Waiting state
)
{
  switch ( state ) {
    case RtemsBarrierReqGetNumberWaiting_Post_Waiting_Waiting: {
      /*
       * The object referenced by the ``id`` parameter shall be set to the
       * number of tasks waiting at the barrier at some time point during the
       * directive call.
       */
      T_eq_u32( ctx->waiting_obj, 0 );
      break;
    }

    case RtemsBarrierReqGetNumberWaiting_Post_Waiting_Nop: {
      /*
       * The object referenced by the ``id`` parameter shall be not be modified
       * by the directive call.
       */
      T_eq_u32( ctx->waiting_obj, 0xffffffff );
      break;
    }

    case RtemsBarrierReqGetNumberWaiting_Post_Waiting_NA:
      break;
  }
}

static void RtemsBarrierReqGetNumberWaiting_Setup(
  RtemsBarrierReqGetNumberWaiting_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_barrier_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    RTEMS_BARRIER_AUTOMATIC_RELEASE,
    1,
    &ctx->barrier_id
  );
  T_assert_rsc_success( sc );
}

static void RtemsBarrierReqGetNumberWaiting_Setup_Wrap( void *arg )
{
  RtemsBarrierReqGetNumberWaiting_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsBarrierReqGetNumberWaiting_Setup( ctx );
}

static void RtemsBarrierReqGetNumberWaiting_Teardown(
  RtemsBarrierReqGetNumberWaiting_Context *ctx
)
{
  rtems_status_code sc;

  if ( ctx->barrier_id != 0 ) {
    sc = rtems_barrier_delete( ctx->barrier_id );
    T_rsc_success( sc );
  }
}

static void RtemsBarrierReqGetNumberWaiting_Teardown_Wrap( void *arg )
{
  RtemsBarrierReqGetNumberWaiting_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsBarrierReqGetNumberWaiting_Teardown( ctx );
}

static void RtemsBarrierReqGetNumberWaiting_Action(
  RtemsBarrierReqGetNumberWaiting_Context *ctx
)
{
  ctx->waiting_obj = 0xffffffff;
  ctx->status = rtems_barrier_get_number_waiting( ctx->id, ctx->waiting );
}

static const RtemsBarrierReqGetNumberWaiting_Entry
RtemsBarrierReqGetNumberWaiting_Entries[] = {
  { 0, 0, 0, RtemsBarrierReqGetNumberWaiting_Post_Status_InvAddr,
    RtemsBarrierReqGetNumberWaiting_Post_Waiting_NA },
  { 0, 0, 0, RtemsBarrierReqGetNumberWaiting_Post_Status_InvId,
    RtemsBarrierReqGetNumberWaiting_Post_Waiting_Nop },
  { 0, 0, 0, RtemsBarrierReqGetNumberWaiting_Post_Status_Ok,
    RtemsBarrierReqGetNumberWaiting_Post_Waiting_Waiting }
};

static const uint8_t
RtemsBarrierReqGetNumberWaiting_Map[] = {
  1, 0, 2, 0
};

static size_t RtemsBarrierReqGetNumberWaiting_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsBarrierReqGetNumberWaiting_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsBarrierReqGetNumberWaiting_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsBarrierReqGetNumberWaiting_Fixture = {
  .setup = RtemsBarrierReqGetNumberWaiting_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsBarrierReqGetNumberWaiting_Teardown_Wrap,
  .scope = RtemsBarrierReqGetNumberWaiting_Scope,
  .initial_context = &RtemsBarrierReqGetNumberWaiting_Instance
};

static inline RtemsBarrierReqGetNumberWaiting_Entry
RtemsBarrierReqGetNumberWaiting_PopEntry(
  RtemsBarrierReqGetNumberWaiting_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsBarrierReqGetNumberWaiting_Entries[
    RtemsBarrierReqGetNumberWaiting_Map[ index ]
  ];
}

static void RtemsBarrierReqGetNumberWaiting_TestVariant(
  RtemsBarrierReqGetNumberWaiting_Context *ctx
)
{
  RtemsBarrierReqGetNumberWaiting_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsBarrierReqGetNumberWaiting_Pre_Waiting_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  RtemsBarrierReqGetNumberWaiting_Action( ctx );
  RtemsBarrierReqGetNumberWaiting_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsBarrierReqGetNumberWaiting_Post_Waiting_Check(
    ctx,
    ctx->Map.entry.Post_Waiting
  );
}

/**
 * @fn void T_case_body_RtemsBarrierReqGetNumberWaiting( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsBarrierReqGetNumberWaiting,
  &RtemsBarrierReqGetNumberWaiting_Fixture
)
{
  RtemsBarrierReqGetNumberWaiting_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsBarrierReqGetNumberWaiting_Pre_Id_NoObj;
    ctx->Map.pcs[ 0 ] < RtemsBarrierReqGetNumberWaiting_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsBarrierReqGetNumberWaiting_Pre_Waiting_Valid;
      ctx->Map.pcs[ 1 ] < RtemsBarrierReqGetNumberWaiting_Pre_Waiting_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsBarrierReqGetNumberWaiting_PopEntry( ctx );
      RtemsBarrierReqGetNumberWaiting_TestVariant( ctx );
    }
  }
}

/** @} */
