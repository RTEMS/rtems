/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsStatusReqIsSuccessful
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
 * @defgroup RtemsStatusReqIsSuccessful spec:/rtems/status/req/is-successful
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsStatusReqIsSuccessful_Pre_Status_Successful,
  RtemsStatusReqIsSuccessful_Pre_Status_Other,
  RtemsStatusReqIsSuccessful_Pre_Status_NA
} RtemsStatusReqIsSuccessful_Pre_Status;

typedef enum {
  RtemsStatusReqIsSuccessful_Post_Result_True,
  RtemsStatusReqIsSuccessful_Post_Result_False,
  RtemsStatusReqIsSuccessful_Post_Result_NA
} RtemsStatusReqIsSuccessful_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Status_NA : 1;
  uint8_t Post_Result : 2;
} RtemsStatusReqIsSuccessful_Entry;

/**
 * @brief Test context for spec:/rtems/status/req/is-successful test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the
   *   rtems_is_status_successful() call.
   */
  bool result;

  /**
   * @brief This member specifies if the ``status_code`` parameter value.
   */
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
    RtemsStatusReqIsSuccessful_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsStatusReqIsSuccessful_Context;

static RtemsStatusReqIsSuccessful_Context
  RtemsStatusReqIsSuccessful_Instance;

static const char * const RtemsStatusReqIsSuccessful_PreDesc_Status[] = {
  "Successful",
  "Other",
  "NA"
};

static const char * const * const RtemsStatusReqIsSuccessful_PreDesc[] = {
  RtemsStatusReqIsSuccessful_PreDesc_Status,
  NULL
};

static void RtemsStatusReqIsSuccessful_Pre_Status_Prepare(
  RtemsStatusReqIsSuccessful_Context   *ctx,
  RtemsStatusReqIsSuccessful_Pre_Status state
)
{
  switch ( state ) {
    case RtemsStatusReqIsSuccessful_Pre_Status_Successful: {
      /*
       * While the ``status_code`` parameter is equal to RTEMS_SUCCESSFUL.
       */
      ctx->status = RTEMS_SUCCESSFUL;
      break;
    }

    case RtemsStatusReqIsSuccessful_Pre_Status_Other: {
      /*
       * While the ``status_code`` parameter is not equal to RTEMS_SUCCESSFUL.
       */
      ctx->status = RTEMS_INVALID_ID;
      break;
    }

    case RtemsStatusReqIsSuccessful_Pre_Status_NA:
      break;
  }
}

static void RtemsStatusReqIsSuccessful_Post_Result_Check(
  RtemsStatusReqIsSuccessful_Context    *ctx,
  RtemsStatusReqIsSuccessful_Post_Result state
)
{
  switch ( state ) {
    case RtemsStatusReqIsSuccessful_Post_Result_True: {
      /*
       * The return value of rtems_is_status_successful() shall be true.
       */
      T_true( ctx->result );
      break;
    }

    case RtemsStatusReqIsSuccessful_Post_Result_False: {
      /*
       * The return value of rtems_is_status_successful() shall be false.
       */
      T_false( ctx->result );
      break;
    }

    case RtemsStatusReqIsSuccessful_Post_Result_NA:
      break;
  }
}

static void RtemsStatusReqIsSuccessful_Action(
  RtemsStatusReqIsSuccessful_Context *ctx
)
{
  ctx->result = rtems_is_status_successful( ctx->status );
}

static const RtemsStatusReqIsSuccessful_Entry
RtemsStatusReqIsSuccessful_Entries[] = {
  { 0, 0, RtemsStatusReqIsSuccessful_Post_Result_True },
  { 0, 0, RtemsStatusReqIsSuccessful_Post_Result_False }
};

static const uint8_t
RtemsStatusReqIsSuccessful_Map[] = {
  0, 1
};

static size_t RtemsStatusReqIsSuccessful_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsStatusReqIsSuccessful_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsStatusReqIsSuccessful_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsStatusReqIsSuccessful_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsStatusReqIsSuccessful_Scope,
  .initial_context = &RtemsStatusReqIsSuccessful_Instance
};

static inline RtemsStatusReqIsSuccessful_Entry
RtemsStatusReqIsSuccessful_PopEntry( RtemsStatusReqIsSuccessful_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsStatusReqIsSuccessful_Entries[
    RtemsStatusReqIsSuccessful_Map[ index ]
  ];
}

static void RtemsStatusReqIsSuccessful_TestVariant(
  RtemsStatusReqIsSuccessful_Context *ctx
)
{
  RtemsStatusReqIsSuccessful_Pre_Status_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsStatusReqIsSuccessful_Action( ctx );
  RtemsStatusReqIsSuccessful_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

/**
 * @fn void T_case_body_RtemsStatusReqIsSuccessful( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsStatusReqIsSuccessful,
  &RtemsStatusReqIsSuccessful_Fixture
)
{
  RtemsStatusReqIsSuccessful_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsStatusReqIsSuccessful_Pre_Status_Successful;
    ctx->Map.pcs[ 0 ] < RtemsStatusReqIsSuccessful_Pre_Status_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsStatusReqIsSuccessful_PopEntry( ctx );
    RtemsStatusReqIsSuccessful_TestVariant( ctx );
  }
}

/** @} */
