/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsStatusReqIsEqual
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
 * @defgroup RtemsStatusReqIsEqual spec:/rtems/status/req/is-equal
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsStatusReqIsEqual_Pre_Status_Equal,
  RtemsStatusReqIsEqual_Pre_Status_NotEqual,
  RtemsStatusReqIsEqual_Pre_Status_NA
} RtemsStatusReqIsEqual_Pre_Status;

typedef enum {
  RtemsStatusReqIsEqual_Post_Result_True,
  RtemsStatusReqIsEqual_Post_Result_False,
  RtemsStatusReqIsEqual_Post_Result_NA
} RtemsStatusReqIsEqual_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Status_NA : 1;
  uint8_t Post_Result : 2;
} RtemsStatusReqIsEqual_Entry;

/**
 * @brief Test context for spec:/rtems/status/req/is-equal test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the
   *   rtems_are_statuses_equal() call.
   */
  bool result;

  /**
   * @brief This member specifies if the ``left_status_code`` parameter value.
   */
  rtems_status_code status_0;

  /**
   * @brief This member specifies if the ``right_status_code`` parameter value.
   */
  rtems_status_code status_1;

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
    RtemsStatusReqIsEqual_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsStatusReqIsEqual_Context;

static RtemsStatusReqIsEqual_Context
  RtemsStatusReqIsEqual_Instance;

static const char * const RtemsStatusReqIsEqual_PreDesc_Status[] = {
  "Equal",
  "NotEqual",
  "NA"
};

static const char * const * const RtemsStatusReqIsEqual_PreDesc[] = {
  RtemsStatusReqIsEqual_PreDesc_Status,
  NULL
};

static void RtemsStatusReqIsEqual_Pre_Status_Prepare(
  RtemsStatusReqIsEqual_Context   *ctx,
  RtemsStatusReqIsEqual_Pre_Status state
)
{
  switch ( state ) {
    case RtemsStatusReqIsEqual_Pre_Status_Equal: {
      /*
       * While the ``left_status_code`` parameter is equal to the
       * ``right_status_code`` parameter.
       */
      ctx->status_0 = RTEMS_INVALID_NAME;
      ctx->status_1 = RTEMS_INVALID_NAME;
      break;
    }

    case RtemsStatusReqIsEqual_Pre_Status_NotEqual: {
      /*
       * While the ``left_status_code`` parameter is not equal to the
       * ``right_status_code`` parameter.
       */
      ctx->status_0 = RTEMS_SUCCESSFUL;
      ctx->status_1 = RTEMS_INVALID_ID;
      break;
    }

    case RtemsStatusReqIsEqual_Pre_Status_NA:
      break;
  }
}

static void RtemsStatusReqIsEqual_Post_Result_Check(
  RtemsStatusReqIsEqual_Context    *ctx,
  RtemsStatusReqIsEqual_Post_Result state
)
{
  switch ( state ) {
    case RtemsStatusReqIsEqual_Post_Result_True: {
      /*
       * The return value of rtems_are_statuses_equal() shall be true.
       */
      T_true( ctx->result );
      break;
    }

    case RtemsStatusReqIsEqual_Post_Result_False: {
      /*
       * The return value of rtems_are_statuses_equal() shall be false.
       */
      T_false( ctx->result );
      break;
    }

    case RtemsStatusReqIsEqual_Post_Result_NA:
      break;
  }
}

static void RtemsStatusReqIsEqual_Action( RtemsStatusReqIsEqual_Context *ctx )
{
  ctx->result = rtems_are_statuses_equal( ctx->status_0, ctx->status_1 );
}

static const RtemsStatusReqIsEqual_Entry
RtemsStatusReqIsEqual_Entries[] = {
  { 0, 0, RtemsStatusReqIsEqual_Post_Result_True },
  { 0, 0, RtemsStatusReqIsEqual_Post_Result_False }
};

static const uint8_t
RtemsStatusReqIsEqual_Map[] = {
  0, 1
};

static size_t RtemsStatusReqIsEqual_Scope( void *arg, char *buf, size_t n )
{
  RtemsStatusReqIsEqual_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsStatusReqIsEqual_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsStatusReqIsEqual_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsStatusReqIsEqual_Scope,
  .initial_context = &RtemsStatusReqIsEqual_Instance
};

static inline RtemsStatusReqIsEqual_Entry RtemsStatusReqIsEqual_PopEntry(
  RtemsStatusReqIsEqual_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsStatusReqIsEqual_Entries[
    RtemsStatusReqIsEqual_Map[ index ]
  ];
}

static void RtemsStatusReqIsEqual_TestVariant(
  RtemsStatusReqIsEqual_Context *ctx
)
{
  RtemsStatusReqIsEqual_Pre_Status_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsStatusReqIsEqual_Action( ctx );
  RtemsStatusReqIsEqual_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_RtemsStatusReqIsEqual( void )
 */
T_TEST_CASE_FIXTURE( RtemsStatusReqIsEqual, &RtemsStatusReqIsEqual_Fixture )
{
  RtemsStatusReqIsEqual_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsStatusReqIsEqual_Pre_Status_Equal;
    ctx->Map.pcs[ 0 ] < RtemsStatusReqIsEqual_Pre_Status_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsStatusReqIsEqual_PopEntry( ctx );
    RtemsStatusReqIsEqual_TestVariant( ctx );
  }
}

/** @} */
