/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibReqFutexWake
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
#include <rtems.h>
#include <sys/lock.h>

#include "tr-tq-flush-fifo.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup NewlibReqFutexWake spec:/newlib/req/futex-wake
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  NewlibReqFutexWake_Pre_Count_NegativeOrZero,
  NewlibReqFutexWake_Pre_Count_Positive,
  NewlibReqFutexWake_Pre_Count_NA
} NewlibReqFutexWake_Pre_Count;

typedef enum {
  NewlibReqFutexWake_Post_Result_Count,
  NewlibReqFutexWake_Post_Result_NA
} NewlibReqFutexWake_Post_Result;

typedef enum {
  NewlibReqFutexWake_Post_Flush_No,
  NewlibReqFutexWake_Post_Flush_Yes,
  NewlibReqFutexWake_Post_Flush_NA
} NewlibReqFutexWake_Post_Flush;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Count_NA : 1;
  uint8_t Post_Result : 1;
  uint8_t Post_Flush : 2;
} NewlibReqFutexWake_Entry;

/**
 * @brief Test context for spec:/newlib/req/futex-wake test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQContext tq_ctx;

  /**
   * @brief This member provides the futex object.
   */
  struct _Futex_Control futex;

  /**
   * @brief This member provides the futex state.
   */
  int state;

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
    NewlibReqFutexWake_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} NewlibReqFutexWake_Context;

static NewlibReqFutexWake_Context
  NewlibReqFutexWake_Instance;

static const char * const NewlibReqFutexWake_PreDesc_Count[] = {
  "NegativeOrZero",
  "Positive",
  "NA"
};

static const char * const * const NewlibReqFutexWake_PreDesc[] = {
  NewlibReqFutexWake_PreDesc_Count,
  NULL
};

typedef NewlibReqFutexWake_Context Context;

static Context *ToContext( TQContext *tq_ctx )
{
  return RTEMS_CONTAINER_OF( tq_ctx, Context, tq_ctx );
}

static Status_Control Enqueue( TQContext *tq_ctx, TQWait wait )
{
  Context *ctx;
  int      count;
  int      eno;

  ctx = ToContext( tq_ctx );

  count = _Futex_Wake( &ctx->futex, -1 );
  T_eq_int( count, 0 );

  count = _Futex_Wake( &ctx->futex, 0 );
  T_eq_int( count, 0 );

  eno = _Futex_Wait( &ctx->futex, &ctx->state, 0 );
  T_eq_int( eno, 0 );

  return STATUS_BUILD( 0, eno );
}

static uint32_t Flush( TQContext *tq_ctx, uint32_t thread_count, bool all )
{
  Context *ctx;
  int      count;

  (void) thread_count;

  ctx = ToContext( tq_ctx );

  if ( all ) {
    count = _Futex_Wake( &ctx->futex, INT_MAX );
  } else {
    count = _Futex_Wake( &ctx->futex, 1 );
  }

  return (uint32_t) count;
}

static void NewlibReqFutexWake_Pre_Count_Prepare(
  NewlibReqFutexWake_Context  *ctx,
  NewlibReqFutexWake_Pre_Count state
)
{
  switch ( state ) {
    case NewlibReqFutexWake_Pre_Count_NegativeOrZero: {
      /*
       * While the ``count`` parameter is less or equal to than zero.
       */
      /* This state is prepared by Enqueue() */
      break;
    }

    case NewlibReqFutexWake_Pre_Count_Positive: {
      /*
       * While the ``count`` parameter is greater than zero.
       */
      /* This state is prepared by Flush() */
      break;
    }

    case NewlibReqFutexWake_Pre_Count_NA:
      break;
  }
}

static void NewlibReqFutexWake_Post_Result_Check(
  NewlibReqFutexWake_Context    *ctx,
  NewlibReqFutexWake_Post_Result state
)
{
  switch ( state ) {
    case NewlibReqFutexWake_Post_Result_Count: {
      /*
       * The return status of _Futex_Wake() shall be the count of threads
       * extracted from the thread queue of the futex object.
       */
      /* This result is checked by Flush() */
      break;
    }

    case NewlibReqFutexWake_Post_Result_NA:
      break;
  }
}

static void NewlibReqFutexWake_Post_Flush_Check(
  NewlibReqFutexWake_Context   *ctx,
  NewlibReqFutexWake_Post_Flush state
)
{
  switch ( state ) {
    case NewlibReqFutexWake_Post_Flush_No: {
      /*
       * No thread shall be extracted from the thread queue of the futex
       * object.
       */
      /* This state is checked by Enqueue() */
      break;
    }

    case NewlibReqFutexWake_Post_Flush_Yes: {
      /*
       * The first count threads specified by the ``count`` parameter shall be
       * extracted from the thread queue of the futex object in FIFO order.
       */
      ScoreTqReqFlushFifo_Run( &ctx->tq_ctx, true );
      break;
    }

    case NewlibReqFutexWake_Post_Flush_NA:
      break;
  }
}

static void NewlibReqFutexWake_Setup( NewlibReqFutexWake_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.discipline = TQ_FIFO;
  ctx->tq_ctx.wait = TQ_WAIT_FOREVER;
  ctx->tq_ctx.enqueue_prepare = TQDoNothing;
  ctx->tq_ctx.enqueue = Enqueue;
  ctx->tq_ctx.enqueue_done = TQDoNothing;
  ctx->tq_ctx.flush = Flush;
  ctx->tq_ctx.surrender = TQDoNothingSuccessfully;
  ctx->tq_ctx.convert_status = TQConvertStatusPOSIX;
  TQInitialize( &ctx->tq_ctx );
}

static void NewlibReqFutexWake_Setup_Wrap( void *arg )
{
  NewlibReqFutexWake_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  NewlibReqFutexWake_Setup( ctx );
}

static void NewlibReqFutexWake_Teardown( NewlibReqFutexWake_Context *ctx )
{
  TQDestroy( &ctx->tq_ctx );
}

static void NewlibReqFutexWake_Teardown_Wrap( void *arg )
{
  NewlibReqFutexWake_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  NewlibReqFutexWake_Teardown( ctx );
}

static void NewlibReqFutexWake_Prepare( NewlibReqFutexWake_Context *ctx )
{
  _Futex_Initialize( &ctx->futex );
}

static void NewlibReqFutexWake_Action( NewlibReqFutexWake_Context *ctx )
{
  /* The action is performed in the ``Flush`` post-condition ``All`` state. */
}

static void NewlibReqFutexWake_Cleanup( NewlibReqFutexWake_Context *ctx )
{
  _Futex_Destroy( &ctx->futex );
}

static const NewlibReqFutexWake_Entry
NewlibReqFutexWake_Entries[] = {
  { 0, 0, NewlibReqFutexWake_Post_Result_Count,
    NewlibReqFutexWake_Post_Flush_No },
  { 0, 0, NewlibReqFutexWake_Post_Result_Count,
    NewlibReqFutexWake_Post_Flush_Yes }
};

static const uint8_t
NewlibReqFutexWake_Map[] = {
  0, 1
};

static size_t NewlibReqFutexWake_Scope( void *arg, char *buf, size_t n )
{
  NewlibReqFutexWake_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( NewlibReqFutexWake_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture NewlibReqFutexWake_Fixture = {
  .setup = NewlibReqFutexWake_Setup_Wrap,
  .stop = NULL,
  .teardown = NewlibReqFutexWake_Teardown_Wrap,
  .scope = NewlibReqFutexWake_Scope,
  .initial_context = &NewlibReqFutexWake_Instance
};

static inline NewlibReqFutexWake_Entry NewlibReqFutexWake_PopEntry(
  NewlibReqFutexWake_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return NewlibReqFutexWake_Entries[
    NewlibReqFutexWake_Map[ index ]
  ];
}

static void NewlibReqFutexWake_TestVariant( NewlibReqFutexWake_Context *ctx )
{
  NewlibReqFutexWake_Pre_Count_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  NewlibReqFutexWake_Action( ctx );
  NewlibReqFutexWake_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  NewlibReqFutexWake_Post_Flush_Check( ctx, ctx->Map.entry.Post_Flush );
}

/**
 * @fn void T_case_body_NewlibReqFutexWake( void )
 */
T_TEST_CASE_FIXTURE( NewlibReqFutexWake, &NewlibReqFutexWake_Fixture )
{
  NewlibReqFutexWake_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = NewlibReqFutexWake_Pre_Count_NegativeOrZero;
    ctx->Map.pcs[ 0 ] < NewlibReqFutexWake_Pre_Count_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = NewlibReqFutexWake_PopEntry( ctx );
    NewlibReqFutexWake_Prepare( ctx );
    NewlibReqFutexWake_TestVariant( ctx );
    NewlibReqFutexWake_Cleanup( ctx );
  }
}

/** @} */
