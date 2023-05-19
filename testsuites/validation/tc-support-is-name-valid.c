/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSupportReqIsNameValid
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
 * @defgroup RtemsSupportReqIsNameValid spec:/rtems/support/req/is-name-valid
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSupportReqIsNameValid_Pre_Name_Valid,
  RtemsSupportReqIsNameValid_Pre_Name_Invalid,
  RtemsSupportReqIsNameValid_Pre_Name_NA
} RtemsSupportReqIsNameValid_Pre_Name;

typedef enum {
  RtemsSupportReqIsNameValid_Post_Result_True,
  RtemsSupportReqIsNameValid_Post_Result_False,
  RtemsSupportReqIsNameValid_Post_Result_NA
} RtemsSupportReqIsNameValid_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Name_NA : 1;
  uint8_t Post_Result : 2;
} RtemsSupportReqIsNameValid_Entry;

/**
 * @brief Test context for spec:/rtems/support/req/is-name-valid test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the rtems_is_name_valid()
   *   call.
   */
  bool result;

  /**
   * @brief This member specifies if the ``name`` parameter value.
   */
  rtems_name name;

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
    RtemsSupportReqIsNameValid_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSupportReqIsNameValid_Context;

static RtemsSupportReqIsNameValid_Context
  RtemsSupportReqIsNameValid_Instance;

static const char * const RtemsSupportReqIsNameValid_PreDesc_Name[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const * const RtemsSupportReqIsNameValid_PreDesc[] = {
  RtemsSupportReqIsNameValid_PreDesc_Name,
  NULL
};

static void RtemsSupportReqIsNameValid_Pre_Name_Prepare(
  RtemsSupportReqIsNameValid_Context *ctx,
  RtemsSupportReqIsNameValid_Pre_Name state
)
{
  switch ( state ) {
    case RtemsSupportReqIsNameValid_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is not equal to zero.
       */
      ctx->name = 1;
      break;
    }

    case RtemsSupportReqIsNameValid_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is equal to zero.
       */
      ctx->name = 0;
      break;
    }

    case RtemsSupportReqIsNameValid_Pre_Name_NA:
      break;
  }
}

static void RtemsSupportReqIsNameValid_Post_Result_Check(
  RtemsSupportReqIsNameValid_Context    *ctx,
  RtemsSupportReqIsNameValid_Post_Result state
)
{
  switch ( state ) {
    case RtemsSupportReqIsNameValid_Post_Result_True: {
      /*
       * The return value of rtems_is_name_valid() shall be true.
       */
      T_true( ctx->result );
      break;
    }

    case RtemsSupportReqIsNameValid_Post_Result_False: {
      /*
       * The return value of rtems_is_name_valid() shall be false.
       */
      T_false( ctx->result );
      break;
    }

    case RtemsSupportReqIsNameValid_Post_Result_NA:
      break;
  }
}

static void RtemsSupportReqIsNameValid_Action(
  RtemsSupportReqIsNameValid_Context *ctx
)
{
  ctx->result = rtems_is_name_valid( ctx->name );
}

static const RtemsSupportReqIsNameValid_Entry
RtemsSupportReqIsNameValid_Entries[] = {
  { 0, 0, RtemsSupportReqIsNameValid_Post_Result_True },
  { 0, 0, RtemsSupportReqIsNameValid_Post_Result_False }
};

static const uint8_t
RtemsSupportReqIsNameValid_Map[] = {
  0, 1
};

static size_t RtemsSupportReqIsNameValid_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsSupportReqIsNameValid_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsSupportReqIsNameValid_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsSupportReqIsNameValid_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsSupportReqIsNameValid_Scope,
  .initial_context = &RtemsSupportReqIsNameValid_Instance
};

static inline RtemsSupportReqIsNameValid_Entry
RtemsSupportReqIsNameValid_PopEntry( RtemsSupportReqIsNameValid_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSupportReqIsNameValid_Entries[
    RtemsSupportReqIsNameValid_Map[ index ]
  ];
}

static void RtemsSupportReqIsNameValid_TestVariant(
  RtemsSupportReqIsNameValid_Context *ctx
)
{
  RtemsSupportReqIsNameValid_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSupportReqIsNameValid_Action( ctx );
  RtemsSupportReqIsNameValid_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

/**
 * @fn void T_case_body_RtemsSupportReqIsNameValid( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsSupportReqIsNameValid,
  &RtemsSupportReqIsNameValid_Fixture
)
{
  RtemsSupportReqIsNameValid_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSupportReqIsNameValid_Pre_Name_Valid;
    ctx->Map.pcs[ 0 ] < RtemsSupportReqIsNameValid_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsSupportReqIsNameValid_PopEntry( ctx );
    RtemsSupportReqIsNameValid_TestVariant( ctx );
  }
}

/** @} */
