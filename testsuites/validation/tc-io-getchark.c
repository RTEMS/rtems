/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIoReqGetchark
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

#include <rtems/bspIo.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsIoReqGetchark spec:/rtems/io/req/getchark
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsIoReqGetchark_Pre_PollChar_Valid,
  RtemsIoReqGetchark_Pre_PollChar_Null,
  RtemsIoReqGetchark_Pre_PollChar_NA
} RtemsIoReqGetchark_Pre_PollChar;

typedef enum {
  RtemsIoReqGetchark_Post_Result_PollChar,
  RtemsIoReqGetchark_Post_Result_MinusOne,
  RtemsIoReqGetchark_Post_Result_NA
} RtemsIoReqGetchark_Post_Result;

typedef enum {
  RtemsIoReqGetchark_Post_Calls_Once,
  RtemsIoReqGetchark_Post_Calls_NA
} RtemsIoReqGetchark_Post_Calls;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_PollChar_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_Calls : 1;
} RtemsIoReqGetchark_Entry;

/**
 * @brief Test context for spec:/rtems/io/req/getchark test case.
 */
typedef struct {
  /**
   * @brief This member contains the character input count.
   */
  size_t input_count;

  /**
   * @brief This member specifies the value for BSP_poll_char.
   */
  BSP_polling_getchar_function_type poll_char;

  /**
   * @brief This member contains the return value of the getchark() call.
   */
  int result;

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
    RtemsIoReqGetchark_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIoReqGetchark_Context;

static RtemsIoReqGetchark_Context
  RtemsIoReqGetchark_Instance;

static const char * const RtemsIoReqGetchark_PreDesc_PollChar[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsIoReqGetchark_PreDesc[] = {
  RtemsIoReqGetchark_PreDesc_PollChar,
  NULL
};

typedef RtemsIoReqGetchark_Context Context;

static int PollChar( void )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->input_count;

  return 123;
}

static void RtemsIoReqGetchark_Pre_PollChar_Prepare(
  RtemsIoReqGetchark_Context     *ctx,
  RtemsIoReqGetchark_Pre_PollChar state
)
{
  switch ( state ) {
    case RtemsIoReqGetchark_Pre_PollChar_Valid: {
      /*
       * While BSP_poll_char references a function.
       */
      ctx->poll_char = PollChar;
      break;
    }

    case RtemsIoReqGetchark_Pre_PollChar_Null: {
      /*
       * While BSP_poll_char is equal to NULL.
       */
      ctx->poll_char = NULL;
      break;
    }

    case RtemsIoReqGetchark_Pre_PollChar_NA:
      break;
  }
}

static void RtemsIoReqGetchark_Post_Result_Check(
  RtemsIoReqGetchark_Context    *ctx,
  RtemsIoReqGetchark_Post_Result state
)
{
  switch ( state ) {
    case RtemsIoReqGetchark_Post_Result_PollChar: {
      /*
       * The return value of getchark() shall be the return value of the
       * function referenced by BSP_poll_char.
       */
      T_eq_int( ctx->result, 123 );
      break;
    }

    case RtemsIoReqGetchark_Post_Result_MinusOne: {
      /*
       * The return value of getchark() shall be minus one.
       */
      T_eq_int( ctx->result, -1 );
      T_eq_u32( ctx->input_count, 0 );
      break;
    }

    case RtemsIoReqGetchark_Post_Result_NA:
      break;
  }
}

static void RtemsIoReqGetchark_Post_Calls_Check(
  RtemsIoReqGetchark_Context   *ctx,
  RtemsIoReqGetchark_Post_Calls state
)
{
  switch ( state ) {
    case RtemsIoReqGetchark_Post_Calls_Once: {
      /*
       * The function referenced by BSP_poll_char shall be called exactly once
       * to get the return value for getchark().
       */
      T_eq_u32( ctx->input_count, 1 );
      break;
    }

    case RtemsIoReqGetchark_Post_Calls_NA:
      break;
  }
}

static void RtemsIoReqGetchark_Action( RtemsIoReqGetchark_Context *ctx )
{
  BSP_polling_getchar_function_type poll_char;

  ctx->input_count = 0;
  poll_char = BSP_poll_char;
  BSP_poll_char = ctx->poll_char;
  ctx->result = getchark();
  BSP_poll_char = poll_char;
}

static const RtemsIoReqGetchark_Entry
RtemsIoReqGetchark_Entries[] = {
  { 0, 0, RtemsIoReqGetchark_Post_Result_PollChar,
    RtemsIoReqGetchark_Post_Calls_Once },
  { 0, 0, RtemsIoReqGetchark_Post_Result_MinusOne,
    RtemsIoReqGetchark_Post_Calls_NA }
};

static const uint8_t
RtemsIoReqGetchark_Map[] = {
  0, 1
};

static size_t RtemsIoReqGetchark_Scope( void *arg, char *buf, size_t n )
{
  RtemsIoReqGetchark_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsIoReqGetchark_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsIoReqGetchark_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIoReqGetchark_Scope,
  .initial_context = &RtemsIoReqGetchark_Instance
};

static inline RtemsIoReqGetchark_Entry RtemsIoReqGetchark_PopEntry(
  RtemsIoReqGetchark_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIoReqGetchark_Entries[
    RtemsIoReqGetchark_Map[ index ]
  ];
}

static void RtemsIoReqGetchark_TestVariant( RtemsIoReqGetchark_Context *ctx )
{
  RtemsIoReqGetchark_Pre_PollChar_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIoReqGetchark_Action( ctx );
  RtemsIoReqGetchark_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  RtemsIoReqGetchark_Post_Calls_Check( ctx, ctx->Map.entry.Post_Calls );
}

/**
 * @fn void T_case_body_RtemsIoReqGetchark( void )
 */
T_TEST_CASE_FIXTURE( RtemsIoReqGetchark, &RtemsIoReqGetchark_Fixture )
{
  RtemsIoReqGetchark_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsIoReqGetchark_Pre_PollChar_Valid;
    ctx->Map.pcs[ 0 ] < RtemsIoReqGetchark_Pre_PollChar_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsIoReqGetchark_PopEntry( ctx );
    RtemsIoReqGetchark_TestVariant( ctx );
  }
}

/** @} */
