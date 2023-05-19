/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CReqFlsl
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

#include <limits.h>
#include <strings.h>

#include <rtems/test.h>

/**
 * @defgroup CReqFlsl spec:/c/req/flsl
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CReqFlsl_Pre_Value_Zero,
  CReqFlsl_Pre_Value_NonZero,
  CReqFlsl_Pre_Value_NA
} CReqFlsl_Pre_Value;

typedef enum {
  CReqFlsl_Post_Result_Zero,
  CReqFlsl_Post_Result_LastBitSet,
  CReqFlsl_Post_Result_NA
} CReqFlsl_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Value_NA : 1;
  uint8_t Post_Result : 2;
} CReqFlsl_Entry;

/**
 * @brief Test context for spec:/c/req/flsl test case.
 */
typedef struct {
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
    CReqFlsl_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CReqFlsl_Context;

static CReqFlsl_Context
  CReqFlsl_Instance;

static const char * const CReqFlsl_PreDesc_Value[] = {
  "Zero",
  "NonZero",
  "NA"
};

static const char * const * const CReqFlsl_PreDesc[] = {
  CReqFlsl_PreDesc_Value,
  NULL
};

static void CReqFlsl_Pre_Value_Prepare(
  CReqFlsl_Context  *ctx,
  CReqFlsl_Pre_Value state
)
{
  switch ( state ) {
    case CReqFlsl_Pre_Value_Zero: {
      /*
       * While the parameter value is equal to zero.
       */
      /* Nothing to prepare */
      break;
    }

    case CReqFlsl_Pre_Value_NonZero: {
      /*
       * While the parameter value is not equal to zero.
       */
      /* Nothing to prepare */
      break;
    }

    case CReqFlsl_Pre_Value_NA:
      break;
  }
}

static void CReqFlsl_Post_Result_Check(
  CReqFlsl_Context    *ctx,
  CReqFlsl_Post_Result state
)
{
  int    expected_result;
  long   value;
  size_t i;

  switch ( state ) {
    case CReqFlsl_Post_Result_Zero: {
      /*
       * The return value of flsl() shall be equal to zero.
       */
      T_eq_int( flsl( 0 ), 0 );
      break;
    }

    case CReqFlsl_Post_Result_LastBitSet: {
      /*
       * The return value of flsl() shall be equal to the index of the
       * most-significant bit set in the parameter value.
       */
      expected_result = 1;
      value = 1;

      for ( i = 0; i < sizeof( long ) * CHAR_BIT; ++i ) {
        T_eq_int( flsl( value ), expected_result );
        ++expected_result;
        value <<= 1;
      }
      break;
    }

    case CReqFlsl_Post_Result_NA:
      break;
  }
}

static void CReqFlsl_Action( CReqFlsl_Context *ctx )
{
  /* The action is performed in the post-condition states */
}

static const CReqFlsl_Entry
CReqFlsl_Entries[] = {
  { 0, 0, CReqFlsl_Post_Result_Zero },
  { 0, 0, CReqFlsl_Post_Result_LastBitSet }
};

static const uint8_t
CReqFlsl_Map[] = {
  0, 1
};

static size_t CReqFlsl_Scope( void *arg, char *buf, size_t n )
{
  CReqFlsl_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CReqFlsl_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CReqFlsl_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CReqFlsl_Scope,
  .initial_context = &CReqFlsl_Instance
};

static inline CReqFlsl_Entry CReqFlsl_PopEntry( CReqFlsl_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CReqFlsl_Entries[
    CReqFlsl_Map[ index ]
  ];
}

static void CReqFlsl_TestVariant( CReqFlsl_Context *ctx )
{
  CReqFlsl_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CReqFlsl_Action( ctx );
  CReqFlsl_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CReqFlsl( void )
 */
T_TEST_CASE_FIXTURE( CReqFlsl, &CReqFlsl_Fixture )
{
  CReqFlsl_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CReqFlsl_Pre_Value_Zero;
    ctx->Map.pcs[ 0 ] < CReqFlsl_Pre_Value_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CReqFlsl_PopEntry( ctx );
    CReqFlsl_TestVariant( ctx );
  }
}

/** @} */
