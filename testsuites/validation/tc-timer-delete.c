/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerReqDelete
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

#include <rtems/test.h>

/**
 * @defgroup RtemsTimerReqDelete spec:/rtems/timer/req/delete
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTimerReqDelete_Pre_Id_NoObj,
  RtemsTimerReqDelete_Pre_Id_Timer,
  RtemsTimerReqDelete_Pre_Id_NA
} RtemsTimerReqDelete_Pre_Id;

typedef enum {
  RtemsTimerReqDelete_Post_Status_Ok,
  RtemsTimerReqDelete_Post_Status_InvId,
  RtemsTimerReqDelete_Post_Status_NA
} RtemsTimerReqDelete_Post_Status;

typedef enum {
  RtemsTimerReqDelete_Post_Name_Valid,
  RtemsTimerReqDelete_Post_Name_Invalid,
  RtemsTimerReqDelete_Post_Name_NA
} RtemsTimerReqDelete_Post_Name;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Name : 2;
} RtemsTimerReqDelete_Entry;

/**
 * @brief Test context for spec:/rtems/timer/req/delete test case.
 */
typedef struct {
  rtems_id timer_id;

  rtems_id id;

  rtems_status_code status;

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
    RtemsTimerReqDelete_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTimerReqDelete_Context;

static RtemsTimerReqDelete_Context
  RtemsTimerReqDelete_Instance;

static const char * const RtemsTimerReqDelete_PreDesc_Id[] = {
  "NoObj",
  "Timer",
  "NA"
};

static const char * const * const RtemsTimerReqDelete_PreDesc[] = {
  RtemsTimerReqDelete_PreDesc_Id,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

static void RtemsTimerReqDelete_Pre_Id_Prepare(
  RtemsTimerReqDelete_Context *ctx,
  RtemsTimerReqDelete_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTimerReqDelete_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is not associated with a timer.
       */
      ctx->id = 0;
      break;
    }

    case RtemsTimerReqDelete_Pre_Id_Timer: {
      /*
       * While the ``id`` parameter is associated with a timer.
       */
      ctx->id = ctx->timer_id;
      break;
    }

    case RtemsTimerReqDelete_Pre_Id_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Post_Status_Check(
  RtemsTimerReqDelete_Context    *ctx,
  RtemsTimerReqDelete_Post_Status state
)
{
  switch ( state ) {
    case RtemsTimerReqDelete_Post_Status_Ok: {
      /*
       * The return status of rtems_timer_delete() shall be RTEMS_SUCCESSFUL.
       */
      ctx->timer_id = 0;
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTimerReqDelete_Post_Status_InvId: {
      /*
       * The return status of rtems_timer_delete() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTimerReqDelete_Post_Status_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Post_Name_Check(
  RtemsTimerReqDelete_Context  *ctx,
  RtemsTimerReqDelete_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsTimerReqDelete_Post_Name_Valid: {
      /*
       * The unique object name shall identify a timer.
       */
      id = 0;
      sc = rtems_timer_ident( NAME, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->timer_id );
      break;
    }

    case RtemsTimerReqDelete_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a timer.
       */
      sc = rtems_timer_ident( NAME, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsTimerReqDelete_Post_Name_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Setup( RtemsTimerReqDelete_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
}

static void RtemsTimerReqDelete_Setup_Wrap( void *arg )
{
  RtemsTimerReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqDelete_Setup( ctx );
}

static void RtemsTimerReqDelete_Teardown( RtemsTimerReqDelete_Context *ctx )
{
  if ( ctx->timer_id != 0 ) {
    rtems_status_code sc;

    sc = rtems_timer_delete( ctx->timer_id );
    T_rsc_success( sc );
  }
}

static void RtemsTimerReqDelete_Teardown_Wrap( void *arg )
{
  RtemsTimerReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqDelete_Teardown( ctx );
}

static void RtemsTimerReqDelete_Prepare( RtemsTimerReqDelete_Context *ctx )
{
  if ( ctx->timer_id == 0 ) {
    rtems_status_code sc;

    sc = rtems_timer_create( NAME, &ctx->timer_id );
    T_rsc_success( sc );
  }
}

static void RtemsTimerReqDelete_Action( RtemsTimerReqDelete_Context *ctx )
{
  ctx->status = rtems_timer_delete( ctx->id );
}

static const RtemsTimerReqDelete_Entry
RtemsTimerReqDelete_Entries[] = {
  { 0, 0, RtemsTimerReqDelete_Post_Status_InvId,
    RtemsTimerReqDelete_Post_Name_Valid },
  { 0, 0, RtemsTimerReqDelete_Post_Status_Ok,
    RtemsTimerReqDelete_Post_Name_Invalid }
};

static const uint8_t
RtemsTimerReqDelete_Map[] = {
  0, 1
};

static size_t RtemsTimerReqDelete_Scope( void *arg, char *buf, size_t n )
{
  RtemsTimerReqDelete_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTimerReqDelete_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTimerReqDelete_Fixture = {
  .setup = RtemsTimerReqDelete_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTimerReqDelete_Teardown_Wrap,
  .scope = RtemsTimerReqDelete_Scope,
  .initial_context = &RtemsTimerReqDelete_Instance
};

static inline RtemsTimerReqDelete_Entry RtemsTimerReqDelete_PopEntry(
  RtemsTimerReqDelete_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTimerReqDelete_Entries[
    RtemsTimerReqDelete_Map[ index ]
  ];
}

static void RtemsTimerReqDelete_TestVariant( RtemsTimerReqDelete_Context *ctx )
{
  RtemsTimerReqDelete_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTimerReqDelete_Action( ctx );
  RtemsTimerReqDelete_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTimerReqDelete_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
}

/**
 * @fn void T_case_body_RtemsTimerReqDelete( void )
 */
T_TEST_CASE_FIXTURE( RtemsTimerReqDelete, &RtemsTimerReqDelete_Fixture )
{
  RtemsTimerReqDelete_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTimerReqDelete_Pre_Id_NoObj;
    ctx->Map.pcs[ 0 ] < RtemsTimerReqDelete_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsTimerReqDelete_PopEntry( ctx );
    RtemsTimerReqDelete_Prepare( ctx );
    RtemsTimerReqDelete_TestVariant( ctx );
  }
}

/** @} */
