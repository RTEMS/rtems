/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSignalReqSend
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSignalReqSend spec:/rtems/signal/req/send
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsSignalReqSend_Pre_Task_NoObj,
  RtemsSignalReqSend_Pre_Task_Self,
  RtemsSignalReqSend_Pre_Task_Other,
  RtemsSignalReqSend_Pre_Task_NA
} RtemsSignalReqSend_Pre_Task;

typedef enum {
  RtemsSignalReqSend_Pre_Set_Zero,
  RtemsSignalReqSend_Pre_Set_NonZero,
  RtemsSignalReqSend_Pre_Set_NA
} RtemsSignalReqSend_Pre_Set;

typedef enum {
  RtemsSignalReqSend_Pre_Handler_Invalid,
  RtemsSignalReqSend_Pre_Handler_Valid,
  RtemsSignalReqSend_Pre_Handler_NA
} RtemsSignalReqSend_Pre_Handler;

typedef enum {
  RtemsSignalReqSend_Pre_ASR_Enabled,
  RtemsSignalReqSend_Pre_ASR_Disabled,
  RtemsSignalReqSend_Pre_ASR_NA
} RtemsSignalReqSend_Pre_ASR;

typedef enum {
  RtemsSignalReqSend_Pre_Nested_Yes,
  RtemsSignalReqSend_Pre_Nested_No,
  RtemsSignalReqSend_Pre_Nested_NA
} RtemsSignalReqSend_Pre_Nested;

typedef enum {
  RtemsSignalReqSend_Post_Status_Ok,
  RtemsSignalReqSend_Post_Status_InvNum,
  RtemsSignalReqSend_Post_Status_InvId,
  RtemsSignalReqSend_Post_Status_NotDef,
  RtemsSignalReqSend_Post_Status_NA
} RtemsSignalReqSend_Post_Status;

typedef enum {
  RtemsSignalReqSend_Post_Handler_NoCall,
  RtemsSignalReqSend_Post_Handler_DuringSend,
  RtemsSignalReqSend_Post_Handler_AfterDispatch,
  RtemsSignalReqSend_Post_Handler_AfterEnable,
  RtemsSignalReqSend_Post_Handler_NA
} RtemsSignalReqSend_Post_Handler;

typedef enum {
  RtemsSignalReqSend_Post_Recursive_Yes,
  RtemsSignalReqSend_Post_Recursive_No,
  RtemsSignalReqSend_Post_Recursive_NA
} RtemsSignalReqSend_Post_Recursive;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Task_NA : 1;
  uint16_t Pre_Set_NA : 1;
  uint16_t Pre_Handler_NA : 1;
  uint16_t Pre_ASR_NA : 1;
  uint16_t Pre_Nested_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Handler : 3;
  uint16_t Post_Recursive : 2;
} RtemsSignalReqSend_Entry;

/**
 * @brief Test context for spec:/rtems/signal/req/send test case.
 */
typedef struct {
  rtems_id runner_id;

  rtems_id worker_id;

  rtems_asr_entry handler;

  size_t nested;

  size_t handler_calls;

  size_t calls_after_send;

  size_t calls_after_dispatch;

  size_t calls_after_enable;

  rtems_signal_set processed_signal_sets[ 2 ];

  uintptr_t stack_pointers[ 2 ];

  rtems_mode mode;

  rtems_status_code status;

  rtems_id id;

  rtems_signal_set signal_set;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    RtemsSignalReqSend_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSignalReqSend_Context;

static RtemsSignalReqSend_Context
  RtemsSignalReqSend_Instance;

static const char * const RtemsSignalReqSend_PreDesc_Task[] = {
  "NoObj",
  "Self",
  "Other",
  "NA"
};

static const char * const RtemsSignalReqSend_PreDesc_Set[] = {
  "Zero",
  "NonZero",
  "NA"
};

static const char * const RtemsSignalReqSend_PreDesc_Handler[] = {
  "Invalid",
  "Valid",
  "NA"
};

static const char * const RtemsSignalReqSend_PreDesc_ASR[] = {
  "Enabled",
  "Disabled",
  "NA"
};

static const char * const RtemsSignalReqSend_PreDesc_Nested[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsSignalReqSend_PreDesc[] = {
  RtemsSignalReqSend_PreDesc_Task,
  RtemsSignalReqSend_PreDesc_Set,
  RtemsSignalReqSend_PreDesc_Handler,
  RtemsSignalReqSend_PreDesc_ASR,
  RtemsSignalReqSend_PreDesc_Nested,
  NULL
};

#define EVENT_START RTEMS_EVENT_0

#define EVENT_SEND_DONE RTEMS_EVENT_1

#define EVENT_DO_ENABLE RTEMS_EVENT_2

#define EVENT_END RTEMS_EVENT_3

#define EVENT_WORKER_DONE RTEMS_EVENT_4

typedef RtemsSignalReqSend_Context Context;

static void WorkerDone( const Context *ctx )
{
#if defined(RTEMS_SMP)
  if ( rtems_scheduler_get_processor_maximum() > 1 ) {
    SendEvents( ctx->runner_id, EVENT_WORKER_DONE );
  }
#else
  (void) ctx;
#endif
}

static void SendEventsToWorker( const Context *ctx, rtems_event_set events )
{
  SendEvents( ctx->worker_id, events );

#if defined(RTEMS_SMP)
  if ( rtems_scheduler_get_processor_maximum() > 1 ) {
    events = ReceiveAnyEvents();
    T_eq_u32( events, EVENT_WORKER_DONE );
  }
#endif
}

static void SignalHandler( rtems_signal_set signal_set )
{
  Context *ctx;
  size_t   i;
  size_t   n;

  ctx = T_fixture_context();
  i = ctx->handler_calls;
  n = RTEMS_ARRAY_SIZE( ctx->processed_signal_sets );
  ctx->processed_signal_sets[ i % n ] = signal_set;
  ctx->stack_pointers[ i % n ] = (uintptr_t) __builtin_frame_address( 0 );
  T_lt_sz( i, n );
  ctx->handler_calls = i + 1;

  if ( ctx->nested != 0 && i == 0 ) {
    rtems_status_code sc;

    if ( ctx->id == ctx->worker_id ) {
      rtems_event_set events;

      sc = rtems_signal_catch( ctx->handler, RTEMS_NO_ASR );
      T_rsc_success( sc );

      WorkerDone( ctx );

      events = ReceiveAnyEvents();
      T_eq_u32( events, EVENT_SEND_DONE );

      WorkerDone( ctx );

      events = ReceiveAnyEvents();
      T_eq_u32( events, EVENT_DO_ENABLE );
    } else {
      sc = rtems_signal_catch( ctx->handler, RTEMS_NO_ASR );
      T_rsc_success( sc );

      ctx->status = rtems_signal_send( ctx->id, ctx->signal_set );
      ctx->calls_after_send = ctx->handler_calls;
      ctx->calls_after_dispatch = ctx->handler_calls;
    }
  }
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_status_code sc;
    rtems_event_set   events;

    events = ReceiveAnyEvents();
    T_eq_u32( events, EVENT_START );

    if ( ctx->nested != 0 ) {
      sc = rtems_signal_catch( SignalHandler, ctx->mode );
      T_rsc_success( sc );

      sc = rtems_signal_send( RTEMS_SELF, 0x600df00d );
      T_rsc_success( sc );

      WorkerDone( ctx );
    } else {
      rtems_mode mode;

      sc = rtems_task_mode( ctx->mode, RTEMS_ASR_MASK, &mode );
      T_rsc_success( sc );

      sc = rtems_signal_catch( ctx->handler, RTEMS_NO_ASR );
      T_rsc_success( sc );

      WorkerDone( ctx );

      events = ReceiveAnyEvents();
      T_eq_u32( events, EVENT_SEND_DONE );

      WorkerDone( ctx );

      events = ReceiveAnyEvents();
      T_eq_u32( events, EVENT_DO_ENABLE );

      sc = rtems_task_mode( mode, RTEMS_ASR_MASK, &mode );
      T_rsc_success( sc );

      WorkerDone( ctx );
    }

    events = ReceiveAnyEvents();
    T_eq_u32( events, EVENT_END );

    WorkerDone( ctx );
  }
}

static void RtemsSignalReqSend_Pre_Task_Prepare(
  RtemsSignalReqSend_Context *ctx,
  RtemsSignalReqSend_Pre_Task state
)
{
  switch ( state ) {
    case RtemsSignalReqSend_Pre_Task_NoObj: {
      /*
       * While the ``id`` parameter is not associated with a task.
       */
      ctx->id = 0xffffffff;
      break;
    }

    case RtemsSignalReqSend_Pre_Task_Self: {
      /*
       * While the ``id`` parameter is associated with the calling task.
       */
      ctx->id = RTEMS_SELF;
      break;
    }

    case RtemsSignalReqSend_Pre_Task_Other: {
      /*
       * While the ``id`` parameter is associated with a task other than the
       * calling task.
       */
      ctx->id = ctx->worker_id;
      break;
    }

    case RtemsSignalReqSend_Pre_Task_NA:
      break;
  }
}

static void RtemsSignalReqSend_Pre_Set_Prepare(
  RtemsSignalReqSend_Context *ctx,
  RtemsSignalReqSend_Pre_Set  state
)
{
  switch ( state ) {
    case RtemsSignalReqSend_Pre_Set_Zero: {
      /*
       * While the ``signal_set`` parameter is zero.
       */
      ctx->signal_set = 0;
      break;
    }

    case RtemsSignalReqSend_Pre_Set_NonZero: {
      /*
       * While the ``signal_set`` parameter is non-zero.
       */
      ctx->signal_set = 0xdeadbeef;
      break;
    }

    case RtemsSignalReqSend_Pre_Set_NA:
      break;
  }
}

static void RtemsSignalReqSend_Pre_Handler_Prepare(
  RtemsSignalReqSend_Context    *ctx,
  RtemsSignalReqSend_Pre_Handler state
)
{
  switch ( state ) {
    case RtemsSignalReqSend_Pre_Handler_Invalid: {
      /*
       * While the target task has no valid ASR handler installed.
       */
      ctx->handler = NULL;
      break;
    }

    case RtemsSignalReqSend_Pre_Handler_Valid: {
      /*
       * While the target task has a valid ASR handler installed.
       */
      ctx->handler = SignalHandler;
      break;
    }

    case RtemsSignalReqSend_Pre_Handler_NA:
      break;
  }
}

static void RtemsSignalReqSend_Pre_ASR_Prepare(
  RtemsSignalReqSend_Context *ctx,
  RtemsSignalReqSend_Pre_ASR  state
)
{
  switch ( state ) {
    case RtemsSignalReqSend_Pre_ASR_Enabled: {
      /*
       * While the target task has ASR processing enabled.
       */
      ctx->mode = RTEMS_DEFAULT_MODES;
      break;
    }

    case RtemsSignalReqSend_Pre_ASR_Disabled: {
      /*
       * While the target task has ASR processing disabled.
       */
      ctx->mode = RTEMS_NO_ASR;
      break;
    }

    case RtemsSignalReqSend_Pre_ASR_NA:
      break;
  }
}

static void RtemsSignalReqSend_Pre_Nested_Prepare(
  RtemsSignalReqSend_Context   *ctx,
  RtemsSignalReqSend_Pre_Nested state
)
{
  switch ( state ) {
    case RtemsSignalReqSend_Pre_Nested_Yes: {
      /*
       * While the target task processes an asynchronous signal.
       */
      ctx->nested = 1;
      break;
    }

    case RtemsSignalReqSend_Pre_Nested_No: {
      /*
       * While the target task does not process an asynchronous signal.
       */
      ctx->nested = 0;
      break;
    }

    case RtemsSignalReqSend_Pre_Nested_NA:
      break;
  }
}

static void RtemsSignalReqSend_Post_Status_Check(
  RtemsSignalReqSend_Context    *ctx,
  RtemsSignalReqSend_Post_Status state
)
{
  switch ( state ) {
    case RtemsSignalReqSend_Post_Status_Ok: {
      /*
       * The return status of rtems_signal_send() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSignalReqSend_Post_Status_InvNum: {
      /*
       * The return status of rtems_signal_send() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsSignalReqSend_Post_Status_InvId: {
      /*
       * The return status of rtems_signal_send() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSignalReqSend_Post_Status_NotDef: {
      /*
       * The return status of rtems_signal_send() shall be RTEMS_NOT_DEFINED.
       */
      T_rsc( ctx->status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsSignalReqSend_Post_Status_NA:
      break;
  }
}

static void RtemsSignalReqSend_Post_Handler_Check(
  RtemsSignalReqSend_Context     *ctx,
  RtemsSignalReqSend_Post_Handler state
)
{
  size_t expected_calls;

  expected_calls = ctx->nested;

  switch ( state ) {
    case RtemsSignalReqSend_Post_Handler_NoCall: {
      /*
       * While the ASR processing is disabled, the ASR handler shall not be
       * called.
       */
      T_eq_sz( ctx->calls_after_send, ctx->nested );
      T_eq_sz( ctx->calls_after_dispatch, ctx->nested );
      T_eq_sz( ctx->calls_after_enable, ctx->nested );
      break;
    }

    case RtemsSignalReqSend_Post_Handler_DuringSend: {
      /*
       * The ASR handler shall be called during the rtems_signal_send() call.
       */
      ++expected_calls;
      T_eq_sz( ctx->calls_after_send, ctx->nested + 1 );
      T_eq_sz( ctx->calls_after_dispatch, ctx->nested + 1 );
      T_eq_sz( ctx->calls_after_enable, ctx->nested + 1 );
      break;
    }

    case RtemsSignalReqSend_Post_Handler_AfterDispatch: {
      /*
       * When the next thread dispatch of the target task of the
       * rtems_signal_send() call takes place, the ASR handler shall be called.
       */
      ++expected_calls;
      T_eq_sz( ctx->calls_after_send, ctx->nested );
      T_eq_sz( ctx->calls_after_dispatch, ctx->nested + 1 );
      T_eq_sz( ctx->calls_after_enable, ctx->nested + 1 );
      break;
    }

    case RtemsSignalReqSend_Post_Handler_AfterEnable: {
      /*
       * When the target task of the rtems_signal_send() call re-enables ASR
       * processing, the ASR handler shall be called.
       */
      ++expected_calls;
      T_eq_sz( ctx->calls_after_send, ctx->nested );
      T_eq_sz( ctx->calls_after_dispatch, ctx->nested );
      T_eq_sz( ctx->calls_after_enable, ctx->nested + 1 );
      break;
    }

    case RtemsSignalReqSend_Post_Handler_NA:
      break;
  }

  T_eq_sz( ctx->handler_calls, expected_calls );

  if ( ctx->nested != 0 ) {
    T_eq_u32( ctx->processed_signal_sets[ 0 ], 0x600df00d );
  }

  if ( expected_calls > ctx->nested ) {
    T_eq_u32( ctx->processed_signal_sets[ ctx->nested ], 0xdeadbeef );
  }
}

static void RtemsSignalReqSend_Post_Recursive_Check(
  RtemsSignalReqSend_Context       *ctx,
  RtemsSignalReqSend_Post_Recursive state
)
{
  switch ( state ) {
    case RtemsSignalReqSend_Post_Recursive_Yes: {
      /*
       * The ASR handler shall be called recursively.
       */
      T_eq_sz( ctx->handler_calls, 2 );
      T_ne_uptr( ctx->stack_pointers[ 0 ], 0 );
      T_ne_uptr( ctx->stack_pointers[ 1 ], 0 );
      T_ne_uptr( ctx->stack_pointers[ 0 ], ctx->stack_pointers[ 1 ] );
      break;
    }

    case RtemsSignalReqSend_Post_Recursive_No: {
      /*
       * The ASR handler shall not be called recursively.
       */
      if ( ctx->handler_calls == 2 ) {
        T_ne_uptr( ctx->stack_pointers[ 0 ], 0 );
        T_ne_uptr( ctx->stack_pointers[ 1 ], 0 );
        T_eq_uptr( ctx->stack_pointers[ 0 ], ctx->stack_pointers[ 1 ] );
      }
      break;
    }

    case RtemsSignalReqSend_Post_Recursive_NA:
      break;
  }
}

static void RtemsSignalReqSend_Setup( RtemsSignalReqSend_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->runner_id = rtems_task_self();
  SetSelfPriority( PRIO_NORMAL );

  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );

  #if defined(RTEMS_SMP)
  if ( rtems_scheduler_get_processor_maximum() > 1 ) {
    rtems_status_code sc;
    rtems_id          scheduler_id;

    sc = rtems_scheduler_ident_by_processor( 1, &scheduler_id );
    T_assert_rsc_success( sc );

    sc = rtems_task_set_scheduler( ctx->worker_id, scheduler_id, PRIO_HIGH );
    T_assert_rsc_success( sc );
  }
  #endif

  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsSignalReqSend_Setup_Wrap( void *arg )
{
  RtemsSignalReqSend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSignalReqSend_Setup( ctx );
}

static void RtemsSignalReqSend_Teardown( RtemsSignalReqSend_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void RtemsSignalReqSend_Teardown_Wrap( void *arg )
{
  RtemsSignalReqSend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSignalReqSend_Teardown( ctx );
}

static void RtemsSignalReqSend_Prepare( RtemsSignalReqSend_Context *ctx )
{
  ctx->handler_calls = 0;
  ctx->calls_after_send = 0;
  ctx->calls_after_dispatch = 0;
  ctx->calls_after_enable = 0;
  memset( &ctx->processed_signal_sets, 0, sizeof( ctx->processed_signal_sets ) );
  memset( &ctx->stack_pointers, 0, sizeof( ctx->stack_pointers ) );
}

static void RtemsSignalReqSend_Action( RtemsSignalReqSend_Context *ctx )
{
  rtems_status_code sc;

  if ( ctx->id == ctx->worker_id ) {
    SendEventsToWorker( ctx, EVENT_START );

    StartDelayThreadDispatch( 1 );
    ctx->status = rtems_signal_send( ctx->id, ctx->signal_set );
    ctx->calls_after_send = ctx->handler_calls;
    StopDelayThreadDispatch( 1 );

    SendEventsToWorker( ctx, EVENT_SEND_DONE );
    ctx->calls_after_dispatch = ctx->handler_calls;
    SendEventsToWorker( ctx, EVENT_DO_ENABLE );
    ctx->calls_after_enable = ctx->handler_calls;
    SendEventsToWorker( ctx, EVENT_END );
  } else if ( ctx->nested != 0 ) {
    sc = rtems_signal_catch( SignalHandler, ctx->mode );
    T_rsc_success( sc );

    sc = rtems_signal_send( RTEMS_SELF, 0x600df00d );
    T_rsc_success( sc );

    ctx->calls_after_enable = ctx->handler_calls;
  } else {
    rtems_mode mode;

    sc = rtems_task_mode( ctx->mode, RTEMS_ASR_MASK, &mode );
    T_rsc_success( sc );

    sc = rtems_signal_catch( ctx->handler, RTEMS_NO_ASR );
    T_rsc_success( sc );

    ctx->status = rtems_signal_send( ctx->id, ctx->signal_set );
    ctx->calls_after_send = ctx->handler_calls;
    ctx->calls_after_dispatch = ctx->handler_calls;

    sc = rtems_task_mode( mode, RTEMS_ASR_MASK, &mode );
    T_rsc_success( sc );

    ctx->calls_after_enable = ctx->handler_calls;
  }
}

static const RtemsSignalReqSend_Entry
RtemsSignalReqSend_Entries[] = {
  { 0, 0, 0, 0, 0, 0, RtemsSignalReqSend_Post_Status_InvNum,
    RtemsSignalReqSend_Post_Handler_NoCall,
    RtemsSignalReqSend_Post_Recursive_No },
  { 0, 0, 0, 0, 0, 0, RtemsSignalReqSend_Post_Status_InvId,
    RtemsSignalReqSend_Post_Handler_NoCall,
    RtemsSignalReqSend_Post_Recursive_No },
  { 0, 0, 0, 0, 0, 0, RtemsSignalReqSend_Post_Status_NotDef,
    RtemsSignalReqSend_Post_Handler_NoCall,
    RtemsSignalReqSend_Post_Recursive_No },
  { 0, 0, 0, 0, 0, 0, RtemsSignalReqSend_Post_Status_Ok,
    RtemsSignalReqSend_Post_Handler_AfterEnable,
    RtemsSignalReqSend_Post_Recursive_No },
  { 0, 0, 0, 0, 0, 0, RtemsSignalReqSend_Post_Status_Ok,
    RtemsSignalReqSend_Post_Handler_DuringSend,
    RtemsSignalReqSend_Post_Recursive_Yes },
  { 0, 0, 0, 0, 0, 0, RtemsSignalReqSend_Post_Status_Ok,
    RtemsSignalReqSend_Post_Handler_DuringSend,
    RtemsSignalReqSend_Post_Recursive_No },
  { 0, 0, 0, 0, 0, 0, RtemsSignalReqSend_Post_Status_Ok,
    RtemsSignalReqSend_Post_Handler_AfterDispatch,
    RtemsSignalReqSend_Post_Recursive_Yes },
  { 0, 0, 0, 0, 0, 0, RtemsSignalReqSend_Post_Status_Ok,
    RtemsSignalReqSend_Post_Handler_AfterDispatch,
    RtemsSignalReqSend_Post_Recursive_No }
};

static const uint8_t
RtemsSignalReqSend_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2,
  2, 2, 4, 5, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 6, 7, 3, 3
};

static size_t RtemsSignalReqSend_Scope( void *arg, char *buf, size_t n )
{
  RtemsSignalReqSend_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSignalReqSend_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSignalReqSend_Fixture = {
  .setup = RtemsSignalReqSend_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSignalReqSend_Teardown_Wrap,
  .scope = RtemsSignalReqSend_Scope,
  .initial_context = &RtemsSignalReqSend_Instance
};

static inline RtemsSignalReqSend_Entry RtemsSignalReqSend_PopEntry(
  RtemsSignalReqSend_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSignalReqSend_Entries[
    RtemsSignalReqSend_Map[ index ]
  ];
}

static void RtemsSignalReqSend_TestVariant( RtemsSignalReqSend_Context *ctx )
{
  RtemsSignalReqSend_Pre_Task_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSignalReqSend_Pre_Set_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSignalReqSend_Pre_Handler_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSignalReqSend_Pre_ASR_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsSignalReqSend_Pre_Nested_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsSignalReqSend_Action( ctx );
  RtemsSignalReqSend_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsSignalReqSend_Post_Handler_Check( ctx, ctx->Map.entry.Post_Handler );
  RtemsSignalReqSend_Post_Recursive_Check(
    ctx,
    ctx->Map.entry.Post_Recursive
  );
}

/**
 * @fn void T_case_body_RtemsSignalReqSend( void )
 */
T_TEST_CASE_FIXTURE( RtemsSignalReqSend, &RtemsSignalReqSend_Fixture )
{
  RtemsSignalReqSend_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSignalReqSend_Pre_Task_NoObj;
    ctx->Map.pcs[ 0 ] < RtemsSignalReqSend_Pre_Task_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSignalReqSend_Pre_Set_Zero;
      ctx->Map.pcs[ 1 ] < RtemsSignalReqSend_Pre_Set_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsSignalReqSend_Pre_Handler_Invalid;
        ctx->Map.pcs[ 2 ] < RtemsSignalReqSend_Pre_Handler_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsSignalReqSend_Pre_ASR_Enabled;
          ctx->Map.pcs[ 3 ] < RtemsSignalReqSend_Pre_ASR_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsSignalReqSend_Pre_Nested_Yes;
            ctx->Map.pcs[ 4 ] < RtemsSignalReqSend_Pre_Nested_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = RtemsSignalReqSend_PopEntry( ctx );
            RtemsSignalReqSend_Prepare( ctx );
            RtemsSignalReqSend_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
