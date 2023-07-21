/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibReqFutexWait
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

#include "tr-tq-enqueue-fifo.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup NewlibReqFutexWait spec:/newlib/req/futex-wait
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  NewlibReqFutexWait_Pre_State_Equal,
  NewlibReqFutexWait_Pre_State_NotEqual,
  NewlibReqFutexWait_Pre_State_NA
} NewlibReqFutexWait_Pre_State;

typedef enum {
  NewlibReqFutexWait_Post_Result_Zero,
  NewlibReqFutexWait_Post_Result_EAGAIN,
  NewlibReqFutexWait_Post_Result_NA
} NewlibReqFutexWait_Post_Result;

typedef enum {
  NewlibReqFutexWait_Post_Enqueue_No,
  NewlibReqFutexWait_Post_Enqueue_Yes,
  NewlibReqFutexWait_Post_Enqueue_NA
} NewlibReqFutexWait_Post_Enqueue;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_State_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_Enqueue : 2;
} NewlibReqFutexWait_Entry;

/**
 * @brief Test context for spec:/newlib/req/futex-wait test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQContext tq_ctx;

  /**
   * @brief This member specifies the expected futex state value.
   */
  int expected_value;

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
    NewlibReqFutexWait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} NewlibReqFutexWait_Context;

static NewlibReqFutexWait_Context
  NewlibReqFutexWait_Instance;

static const char * const NewlibReqFutexWait_PreDesc_State[] = {
  "Equal",
  "NotEqual",
  "NA"
};

static const char * const * const NewlibReqFutexWait_PreDesc[] = {
  NewlibReqFutexWait_PreDesc_State,
  NULL
};

typedef NewlibReqFutexWait_Context Context;

static Context *ToContext( TQContext *tq_ctx )
{
  return RTEMS_CONTAINER_OF( tq_ctx, Context, tq_ctx );
}

static Status_Control Enqueue( TQContext *tq_ctx, TQWait wait )
{
  Context *ctx;
  int      eno;

  ctx = ToContext( tq_ctx );
  eno = _Futex_Wait( &ctx->futex, &ctx->state, ctx->expected_value );
  T_eq_int( eno, 0 );

  return STATUS_BUILD( 0, eno );
}

static void EnqueueDone( TQContext *tq_ctx )
{
  Context *ctx;
  int      count;

  ctx = ToContext( tq_ctx );
  count = _Futex_Wake( &ctx->futex, INT_MAX );
  T_eq_int( count, (int) ctx->tq_ctx.how_many );
}

static void NewlibReqFutexWait_Pre_State_Prepare(
  NewlibReqFutexWait_Context  *ctx,
  NewlibReqFutexWait_Pre_State state
)
{
  switch ( state ) {
    case NewlibReqFutexWait_Pre_State_Equal: {
      /*
       * While the expected futex state value is equal to the actual futex
       * state value.
       */
      ctx->expected_value = 0;
      break;
    }

    case NewlibReqFutexWait_Pre_State_NotEqual: {
      /*
       * While the expected futex state value is not equal to the actual futex
       * state value.
       */
      ctx->expected_value = 1;
      break;
    }

    case NewlibReqFutexWait_Pre_State_NA:
      break;
  }
}

static void NewlibReqFutexWait_Post_Result_Check(
  NewlibReqFutexWait_Context    *ctx,
  NewlibReqFutexWait_Post_Result state
)
{
  int eno;

  switch ( state ) {
    case NewlibReqFutexWait_Post_Result_Zero: {
      /*
       * The return status of _Futex_Wait() shall be zero.
       */
      /* This result is checked by Enqueue() */
      break;
    }

    case NewlibReqFutexWait_Post_Result_EAGAIN: {
      /*
       * The return status of _Futex_Wait() shall be EAGAIN.
       */
      eno = _Futex_Wait( &ctx->futex, &ctx->state, ctx->expected_value );
      T_eq_int( eno, EAGAIN );
      break;
    }

    case NewlibReqFutexWait_Post_Result_NA:
      break;
  }
}

static void NewlibReqFutexWait_Post_Enqueue_Check(
  NewlibReqFutexWait_Context     *ctx,
  NewlibReqFutexWait_Post_Enqueue state
)
{
  switch ( state ) {
    case NewlibReqFutexWait_Post_Enqueue_No: {
      /*
       * The calling thread shall not be enqueued on the thread queue of the
       * futex object.
       */
      /* The runner would block forever */
      break;
    }

    case NewlibReqFutexWait_Post_Enqueue_Yes: {
      /*
       * The calling thread shall be enqueued in FIFO order on the thread queue
       * of the futex object.
       */
      ScoreTqReqEnqueueFifo_Run( &ctx->tq_ctx );
      break;
    }

    case NewlibReqFutexWait_Post_Enqueue_NA:
      break;
  }
}

static void NewlibReqFutexWait_Setup( NewlibReqFutexWait_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.discipline = TQ_FIFO;
  ctx->tq_ctx.wait = TQ_WAIT_FOREVER;
  ctx->tq_ctx.enqueue_prepare = TQDoNothing;
  ctx->tq_ctx.enqueue = Enqueue;
  ctx->tq_ctx.enqueue_done = EnqueueDone;
  ctx->tq_ctx.surrender = TQDoNothingSuccessfully;
  ctx->tq_ctx.convert_status = TQConvertStatusPOSIX;
  TQInitialize( &ctx->tq_ctx );
}

static void NewlibReqFutexWait_Setup_Wrap( void *arg )
{
  NewlibReqFutexWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  NewlibReqFutexWait_Setup( ctx );
}

static void NewlibReqFutexWait_Teardown( NewlibReqFutexWait_Context *ctx )
{
  TQDestroy( &ctx->tq_ctx );
}

static void NewlibReqFutexWait_Teardown_Wrap( void *arg )
{
  NewlibReqFutexWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  NewlibReqFutexWait_Teardown( ctx );
}

static void NewlibReqFutexWait_Prepare( NewlibReqFutexWait_Context *ctx )
{
  _Futex_Initialize( &ctx->futex );
  ctx->state = 0;
}

static void NewlibReqFutexWait_Action( NewlibReqFutexWait_Context *ctx )
{
  /* The action is performed in the post-conditions. */
}

static void NewlibReqFutexWait_Cleanup( NewlibReqFutexWait_Context *ctx )
{
  _Futex_Destroy( &ctx->futex );
}

static const NewlibReqFutexWait_Entry
NewlibReqFutexWait_Entries[] = {
  { 0, 0, NewlibReqFutexWait_Post_Result_Zero,
    NewlibReqFutexWait_Post_Enqueue_Yes },
  { 0, 0, NewlibReqFutexWait_Post_Result_EAGAIN,
    NewlibReqFutexWait_Post_Enqueue_No }
};

static const uint8_t
NewlibReqFutexWait_Map[] = {
  0, 1
};

static size_t NewlibReqFutexWait_Scope( void *arg, char *buf, size_t n )
{
  NewlibReqFutexWait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( NewlibReqFutexWait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture NewlibReqFutexWait_Fixture = {
  .setup = NewlibReqFutexWait_Setup_Wrap,
  .stop = NULL,
  .teardown = NewlibReqFutexWait_Teardown_Wrap,
  .scope = NewlibReqFutexWait_Scope,
  .initial_context = &NewlibReqFutexWait_Instance
};

static inline NewlibReqFutexWait_Entry NewlibReqFutexWait_PopEntry(
  NewlibReqFutexWait_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return NewlibReqFutexWait_Entries[
    NewlibReqFutexWait_Map[ index ]
  ];
}

static void NewlibReqFutexWait_TestVariant( NewlibReqFutexWait_Context *ctx )
{
  NewlibReqFutexWait_Pre_State_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  NewlibReqFutexWait_Action( ctx );
  NewlibReqFutexWait_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  NewlibReqFutexWait_Post_Enqueue_Check( ctx, ctx->Map.entry.Post_Enqueue );
}

/**
 * @fn void T_case_body_NewlibReqFutexWait( void )
 */
T_TEST_CASE_FIXTURE( NewlibReqFutexWait, &NewlibReqFutexWait_Fixture )
{
  NewlibReqFutexWait_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = NewlibReqFutexWait_Pre_State_Equal;
    ctx->Map.pcs[ 0 ] < NewlibReqFutexWait_Pre_State_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = NewlibReqFutexWait_PopEntry( ctx );
    NewlibReqFutexWait_Prepare( ctx );
    NewlibReqFutexWait_TestVariant( ctx );
    NewlibReqFutexWait_Cleanup( ctx );
  }
}

/** @} */
