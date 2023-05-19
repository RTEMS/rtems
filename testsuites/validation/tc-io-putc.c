/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIoReqPutc
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
 * @defgroup RtemsIoReqPutc spec:/rtems/io/req/putc
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsIoReqPutc_Pre_Char_Nl,
  RtemsIoReqPutc_Pre_Char_Other,
  RtemsIoReqPutc_Pre_Char_NA
} RtemsIoReqPutc_Pre_Char;

typedef enum {
  RtemsIoReqPutc_Post_Output_CrNl,
  RtemsIoReqPutc_Post_Output_Other,
  RtemsIoReqPutc_Post_Output_NA
} RtemsIoReqPutc_Post_Output;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Output : 2;
} RtemsIoReqPutc_Entry;

/**
 * @brief Test context for spec:/rtems/io/req/putc test case.
 */
typedef struct {
  /**
   * @brief This member contains the character output.
   */
  int output[ 2 ];

  /**
   * @brief This member contains the character output count.
   */
  size_t output_count;

  /**
   * @brief This member specifies if the ``c`` parameter value.
   */
  char character;

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
    RtemsIoReqPutc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIoReqPutc_Context;

static RtemsIoReqPutc_Context
  RtemsIoReqPutc_Instance;

static const char * const RtemsIoReqPutc_PreDesc_Char[] = {
  "Nl",
  "Other",
  "NA"
};

static const char * const * const RtemsIoReqPutc_PreDesc[] = {
  RtemsIoReqPutc_PreDesc_Char,
  NULL
};

typedef RtemsIoReqPutc_Context Context;

static void Output( int value )
{
  Context *ctx;

  ctx = T_fixture_context();

  if ( ctx->output_count < RTEMS_ARRAY_SIZE( ctx->output ) ) {
    ctx->output[ ctx->output_count ] = value;
  }

  ++ctx->output_count;
}

static void WrongOutput( char c )
{
  (void) c;
  Output( -1 );
}

static void OutputChar( char c )
{
  BSP_output_char = WrongOutput;
  Output( (unsigned char) c );
}

static void RtemsIoReqPutc_Pre_Char_Prepare(
  RtemsIoReqPutc_Context *ctx,
  RtemsIoReqPutc_Pre_Char state
)
{
  switch ( state ) {
    case RtemsIoReqPutc_Pre_Char_Nl: {
      /*
       * While the ``c`` parameter is equal to ``NL``.
       */
      ctx->character = '\n';
      break;
    }

    case RtemsIoReqPutc_Pre_Char_Other: {
      /*
       * While the ``c`` parameter is not equal to ``NL``.
       */
      ctx->character = (char) 0xff;
      break;
    }

    case RtemsIoReqPutc_Pre_Char_NA:
      break;
  }
}

static void RtemsIoReqPutc_Post_Output_Check(
  RtemsIoReqPutc_Context    *ctx,
  RtemsIoReqPutc_Post_Output state
)
{
  switch ( state ) {
    case RtemsIoReqPutc_Post_Output_CrNl: {
      /*
       * The function referenced by BSP_output_char shall be called with a
       * ``CR`` character followed by a call with a ``NL`` character.
       */
      T_eq_int( ctx->output[ 0 ], (unsigned char) '\r' );
      T_eq_int( ctx->output[ 1 ], (unsigned char) '\n' );
      T_eq_sz( ctx->output_count, 2 );
      break;
    }

    case RtemsIoReqPutc_Post_Output_Other: {
      /*
       * The function referenced by BSP_output_char shall be called with the
       * character specified by ``c``.
       */
      T_eq_int( ctx->output[ 0 ], 0xff );
      T_eq_sz( ctx->output_count, 1 );
      break;
    }

    case RtemsIoReqPutc_Post_Output_NA:
      break;
  }
}

static void RtemsIoReqPutc_Action( RtemsIoReqPutc_Context *ctx )
{
  BSP_output_char_function_type output_char;

  ctx->output[ 0 ] = -1;
  ctx->output[ 1 ] = -1;
  ctx->output_count = 0;
  output_char = BSP_output_char;
  BSP_output_char = OutputChar;
  rtems_putc( ctx->character );
  BSP_output_char = output_char;
}

static const RtemsIoReqPutc_Entry
RtemsIoReqPutc_Entries[] = {
  { 0, 0, RtemsIoReqPutc_Post_Output_CrNl },
  { 0, 0, RtemsIoReqPutc_Post_Output_Other }
};

static const uint8_t
RtemsIoReqPutc_Map[] = {
  0, 1
};

static size_t RtemsIoReqPutc_Scope( void *arg, char *buf, size_t n )
{
  RtemsIoReqPutc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsIoReqPutc_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsIoReqPutc_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIoReqPutc_Scope,
  .initial_context = &RtemsIoReqPutc_Instance
};

static inline RtemsIoReqPutc_Entry RtemsIoReqPutc_PopEntry(
  RtemsIoReqPutc_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIoReqPutc_Entries[
    RtemsIoReqPutc_Map[ index ]
  ];
}

static void RtemsIoReqPutc_TestVariant( RtemsIoReqPutc_Context *ctx )
{
  RtemsIoReqPutc_Pre_Char_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIoReqPutc_Action( ctx );
  RtemsIoReqPutc_Post_Output_Check( ctx, ctx->Map.entry.Post_Output );
}

/**
 * @fn void T_case_body_RtemsIoReqPutc( void )
 */
T_TEST_CASE_FIXTURE( RtemsIoReqPutc, &RtemsIoReqPutc_Fixture )
{
  RtemsIoReqPutc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsIoReqPutc_Pre_Char_Nl;
    ctx->Map.pcs[ 0 ] < RtemsIoReqPutc_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsIoReqPutc_PopEntry( ctx );
    RtemsIoReqPutc_TestVariant( ctx );
  }
}

/** @} */
