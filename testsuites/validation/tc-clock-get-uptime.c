/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsClockReqGetUptime
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

#include <rtems/test.h>

/**
 * @defgroup RtemsClockReqGetUptime spec:/rtems/clock/req/get-uptime
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsClockReqGetUptime_Pre_Uptime_Valid,
  RtemsClockReqGetUptime_Pre_Uptime_Null,
  RtemsClockReqGetUptime_Pre_Uptime_NA
} RtemsClockReqGetUptime_Pre_Uptime;

typedef enum {
  RtemsClockReqGetUptime_Post_Status_Ok,
  RtemsClockReqGetUptime_Post_Status_InvAddr,
  RtemsClockReqGetUptime_Post_Status_NA
} RtemsClockReqGetUptime_Post_Status;

typedef enum {
  RtemsClockReqGetUptime_Post_Uptime_Set,
  RtemsClockReqGetUptime_Post_Uptime_Unchanged,
  RtemsClockReqGetUptime_Post_Uptime_NA
} RtemsClockReqGetUptime_Post_Uptime;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Uptime_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Uptime : 2;
} RtemsClockReqGetUptime_Entry;

/**
 * @brief Test context for spec:/rtems/clock/req/get-uptime test case.
 */
typedef struct {
  rtems_status_code status;

  struct timespec *uptime;

  struct timespec uptime_value;

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
    RtemsClockReqGetUptime_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsClockReqGetUptime_Context;

static RtemsClockReqGetUptime_Context
  RtemsClockReqGetUptime_Instance;

static const char * const RtemsClockReqGetUptime_PreDesc_Uptime[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsClockReqGetUptime_PreDesc[] = {
  RtemsClockReqGetUptime_PreDesc_Uptime,
  NULL
};

static void RtemsClockReqGetUptime_Pre_Uptime_Prepare(
  RtemsClockReqGetUptime_Context   *ctx,
  RtemsClockReqGetUptime_Pre_Uptime state
)
{
  switch ( state ) {
    case RtemsClockReqGetUptime_Pre_Uptime_Valid: {
      /*
       * While the ``uptime`` parameter references an object of type struct
       * timespec.
       */
      ctx->uptime = &ctx->uptime_value;
      break;
    }

    case RtemsClockReqGetUptime_Pre_Uptime_Null: {
      /*
       * While the ``uptime`` parameter is NULL.
       */
      ctx->uptime = NULL;
      break;
    }

    case RtemsClockReqGetUptime_Pre_Uptime_NA:
      break;
  }
}

static void RtemsClockReqGetUptime_Post_Status_Check(
  RtemsClockReqGetUptime_Context    *ctx,
  RtemsClockReqGetUptime_Post_Status state
)
{
  switch ( state ) {
    case RtemsClockReqGetUptime_Post_Status_Ok: {
      /*
       * The return status of rtems_clock_get_uptime() shall be
       * RTEMS_SUCCESSFUL
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsClockReqGetUptime_Post_Status_InvAddr: {
      /*
       * The return status of rtems_clock_get_uptime() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsClockReqGetUptime_Post_Status_NA:
      break;
  }
}

static void RtemsClockReqGetUptime_Post_Uptime_Check(
  RtemsClockReqGetUptime_Context    *ctx,
  RtemsClockReqGetUptime_Post_Uptime state
)
{
  switch ( state ) {
    case RtemsClockReqGetUptime_Post_Uptime_Set: {
      /*
       * The value of the object referenced by the ``uptime`` parameter shall
       * be set to seconds and nanoseconds elapsed since a point in time during
       * the system initialization and a point in time during the call of
       * rtems_clock_get_uptime() using CLOCK_MONOTONIC as result of the
       * rtems_clock_get_uptime() call.
       */
      T_eq_ptr( ctx->uptime, &ctx->uptime_value );
      T_ge_i64( ctx->uptime_value.tv_sec, 0LL );
      T_ge_long( ctx->uptime_value.tv_nsec, 0L );
      T_lt_long( ctx->uptime_value.tv_nsec, 1000000000L );
      break;
    }

    case RtemsClockReqGetUptime_Post_Uptime_Unchanged: {
      /*
       * Objects referenced by the ``uptime`` parameter in calls to
       * rtems_clock_get_uptime() shall not be modified by the
       * rtems_clock_get_uptime() call.
       */
      T_null( ctx->uptime );
      break;
    }

    case RtemsClockReqGetUptime_Post_Uptime_NA:
      break;
  }
}

static void RtemsClockReqGetUptime_Setup( RtemsClockReqGetUptime_Context *ctx )
{
  ctx->uptime_value.tv_sec  = -1;
  ctx->uptime_value.tv_nsec = -1;
}

static void RtemsClockReqGetUptime_Setup_Wrap( void *arg )
{
  RtemsClockReqGetUptime_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsClockReqGetUptime_Setup( ctx );
}

static void RtemsClockReqGetUptime_Action(
  RtemsClockReqGetUptime_Context *ctx
)
{
  ctx->status = rtems_clock_get_uptime( ctx->uptime );
}

static const RtemsClockReqGetUptime_Entry
RtemsClockReqGetUptime_Entries[] = {
  { 0, 0, RtemsClockReqGetUptime_Post_Status_Ok,
    RtemsClockReqGetUptime_Post_Uptime_Set },
  { 0, 0, RtemsClockReqGetUptime_Post_Status_InvAddr,
    RtemsClockReqGetUptime_Post_Uptime_Unchanged }
};

static const uint8_t
RtemsClockReqGetUptime_Map[] = {
  0, 1
};

static size_t RtemsClockReqGetUptime_Scope( void *arg, char *buf, size_t n )
{
  RtemsClockReqGetUptime_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsClockReqGetUptime_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsClockReqGetUptime_Fixture = {
  .setup = RtemsClockReqGetUptime_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsClockReqGetUptime_Scope,
  .initial_context = &RtemsClockReqGetUptime_Instance
};

static inline RtemsClockReqGetUptime_Entry RtemsClockReqGetUptime_PopEntry(
  RtemsClockReqGetUptime_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsClockReqGetUptime_Entries[
    RtemsClockReqGetUptime_Map[ index ]
  ];
}

static void RtemsClockReqGetUptime_TestVariant(
  RtemsClockReqGetUptime_Context *ctx
)
{
  RtemsClockReqGetUptime_Pre_Uptime_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsClockReqGetUptime_Action( ctx );
  RtemsClockReqGetUptime_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsClockReqGetUptime_Post_Uptime_Check( ctx, ctx->Map.entry.Post_Uptime );
}

/**
 * @fn void T_case_body_RtemsClockReqGetUptime( void )
 */
T_TEST_CASE_FIXTURE( RtemsClockReqGetUptime, &RtemsClockReqGetUptime_Fixture )
{
  RtemsClockReqGetUptime_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsClockReqGetUptime_Pre_Uptime_Valid;
    ctx->Map.pcs[ 0 ] < RtemsClockReqGetUptime_Pre_Uptime_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsClockReqGetUptime_PopEntry( ctx );
    RtemsClockReqGetUptime_TestVariant( ctx );
  }
}

/** @} */
