/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMessageReqReceive
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
#include <rtems/score/statesimpl.h>

#include "tr-tq-enqueue-fifo.h"
#include "tr-tq-enqueue-priority.h"
#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsMessageReqReceive spec:/rtems/message/req/receive
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsMessageReqReceive_Pre_Buffer_Valid,
  RtemsMessageReqReceive_Pre_Buffer_Null,
  RtemsMessageReqReceive_Pre_Buffer_NA
} RtemsMessageReqReceive_Pre_Buffer;

typedef enum {
  RtemsMessageReqReceive_Pre_Size_Valid,
  RtemsMessageReqReceive_Pre_Size_Null,
  RtemsMessageReqReceive_Pre_Size_NA
} RtemsMessageReqReceive_Pre_Size;

typedef enum {
  RtemsMessageReqReceive_Pre_Id_Valid,
  RtemsMessageReqReceive_Pre_Id_Invalid,
  RtemsMessageReqReceive_Pre_Id_NA
} RtemsMessageReqReceive_Pre_Id;

typedef enum {
  RtemsMessageReqReceive_Pre_DuringWait_Nop,
  RtemsMessageReqReceive_Pre_DuringWait_Deleted,
  RtemsMessageReqReceive_Pre_DuringWait_NA
} RtemsMessageReqReceive_Pre_DuringWait;

typedef enum {
  RtemsMessageReqReceive_Pre_TaskQueue_Fifo,
  RtemsMessageReqReceive_Pre_TaskQueue_Priority,
  RtemsMessageReqReceive_Pre_TaskQueue_NA
} RtemsMessageReqReceive_Pre_TaskQueue;

typedef enum {
  RtemsMessageReqReceive_Pre_Wait_No,
  RtemsMessageReqReceive_Pre_Wait_Timeout,
  RtemsMessageReqReceive_Pre_Wait_Forever,
  RtemsMessageReqReceive_Pre_Wait_NA
} RtemsMessageReqReceive_Pre_Wait;

typedef enum {
  RtemsMessageReqReceive_Pre_MsgQueue_Empty,
  RtemsMessageReqReceive_Pre_MsgQueue_One,
  RtemsMessageReqReceive_Pre_MsgQueue_Several,
  RtemsMessageReqReceive_Pre_MsgQueue_NA
} RtemsMessageReqReceive_Pre_MsgQueue;

typedef enum {
  RtemsMessageReqReceive_Pre_Storage_Nop,
  RtemsMessageReqReceive_Pre_Storage_NA
} RtemsMessageReqReceive_Pre_Storage;

typedef enum {
  RtemsMessageReqReceive_Post_Status_Ok,
  RtemsMessageReqReceive_Post_Status_InvId,
  RtemsMessageReqReceive_Post_Status_InvAddr,
  RtemsMessageReqReceive_Post_Status_Unsat,
  RtemsMessageReqReceive_Post_Status_Timeout,
  RtemsMessageReqReceive_Post_Status_Deleted,
  RtemsMessageReqReceive_Post_Status_NA
} RtemsMessageReqReceive_Post_Status;

typedef enum {
  RtemsMessageReqReceive_Post_Delay_None,
  RtemsMessageReqReceive_Post_Delay_Ticks,
  RtemsMessageReqReceive_Post_Delay_Forever,
  RtemsMessageReqReceive_Post_Delay_NA
} RtemsMessageReqReceive_Post_Delay;

typedef enum {
  RtemsMessageReqReceive_Post_Size_First,
  RtemsMessageReqReceive_Post_Size_Nop,
  RtemsMessageReqReceive_Post_Size_NA
} RtemsMessageReqReceive_Post_Size;

typedef enum {
  RtemsMessageReqReceive_Post_Msg_First,
  RtemsMessageReqReceive_Post_Msg_Nop,
  RtemsMessageReqReceive_Post_Msg_NA
} RtemsMessageReqReceive_Post_Msg;

typedef enum {
  RtemsMessageReqReceive_Post_MsgQueue_Empty,
  RtemsMessageReqReceive_Post_MsgQueue_OneLess,
  RtemsMessageReqReceive_Post_MsgQueue_Nop,
  RtemsMessageReqReceive_Post_MsgQueue_NA
} RtemsMessageReqReceive_Post_MsgQueue;

typedef enum {
  RtemsMessageReqReceive_Post_Tasks_Fifo,
  RtemsMessageReqReceive_Post_Tasks_Priority,
  RtemsMessageReqReceive_Post_Tasks_NA
} RtemsMessageReqReceive_Post_Tasks;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Buffer_NA : 1;
  uint32_t Pre_Size_NA : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_DuringWait_NA : 1;
  uint32_t Pre_TaskQueue_NA : 1;
  uint32_t Pre_Wait_NA : 1;
  uint32_t Pre_MsgQueue_NA : 1;
  uint32_t Pre_Storage_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Delay : 2;
  uint32_t Post_Size : 2;
  uint32_t Post_Msg : 2;
  uint32_t Post_MsgQueue : 2;
  uint32_t Post_Tasks : 2;
} RtemsMessageReqReceive_Entry;

#define MAXIMUM_PENDING_MESSAGES 3
#define MAXIMUM_MESSAGE_SIZE 5

/**
 * @brief Test context for spec:/rtems/message/req/receive test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQContext tq_ctx;

  /**
   * @brief This member specifies the attribute set of the message queue.
   */
  rtems_attribute attribute_set;

  /**
   * @brief This member is used as storage area for the message queue.
   */
  RTEMS_MESSAGE_QUEUE_BUFFER( MAXIMUM_MESSAGE_SIZE )
    storage_area[ MAXIMUM_PENDING_MESSAGES];

  /**
   * @brief This member contains always the same arbitrary number ``magic``.
   *
   * It is used for run-time type checking.
   */
  uint32_t magic;

  /**
   * @brief This member contains a number which is sent as next message.
   */
  uint8_t send_msg_counter;

  /**
   * @brief This member contains a buffer to receive messages from the queue.
   */
  uint8_t receive_buffer[ MAXIMUM_MESSAGE_SIZE ];

  /**
   * @brief This member contains a buffer to receive the messages size.
   */
  size_t receive_size;

  /**
   * @brief This member specifies the ``id`` parameter for the action.
   */
  rtems_id id_param;

  /**
   * @brief This member specifies the ``buffer`` parameter for the action.
   */
  void *buffer_param;

  /**
   * @brief This member specifies the ``size`` parameter for the action.
   */
  size_t *size_param;

  /**
   * @brief This member specifies the ``option_set`` parameter for the action.
   */
  rtems_option option_set_param;

  /**
   * @brief This member specifies the ``timeout`` parameter for the action.
   */
  rtems_interval timeout_param;

  /**
   * @brief This member contains the returned status code of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains the duration of the action in ticks.
   */
  uint32_t action_duration;

  /**
   * @brief This member contains the task identifier of the main task.
   */
  rtems_id task_id;

  /**
   * @brief This member contains the task identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains a pointer to a function which is executed
   *   while the worker is waiting to receive a message (`delete(), nop()``).
   */
  void (*concurrent_activity)( void *ctx_in );

  /**
   * @brief This member contains a pointer to a function which is executed to
   *   check that the action has not changed the content of the message queue.
   */
  void (*check_msgq_unchanged)( void *ctx_in );

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 8 ];

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
    RtemsMessageReqReceive_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMessageReqReceive_Context;

static RtemsMessageReqReceive_Context
  RtemsMessageReqReceive_Instance;

static const char * const RtemsMessageReqReceive_PreDesc_Buffer[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqReceive_PreDesc_Size[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqReceive_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsMessageReqReceive_PreDesc_DuringWait[] = {
  "Nop",
  "Deleted",
  "NA"
};

static const char * const RtemsMessageReqReceive_PreDesc_TaskQueue[] = {
  "Fifo",
  "Priority",
  "NA"
};

static const char * const RtemsMessageReqReceive_PreDesc_Wait[] = {
  "No",
  "Timeout",
  "Forever",
  "NA"
};

static const char * const RtemsMessageReqReceive_PreDesc_MsgQueue[] = {
  "Empty",
  "One",
  "Several",
  "NA"
};

static const char * const RtemsMessageReqReceive_PreDesc_Storage[] = {
  "Nop",
  "NA"
};

static const char * const * const RtemsMessageReqReceive_PreDesc[] = {
  RtemsMessageReqReceive_PreDesc_Buffer,
  RtemsMessageReqReceive_PreDesc_Size,
  RtemsMessageReqReceive_PreDesc_Id,
  RtemsMessageReqReceive_PreDesc_DuringWait,
  RtemsMessageReqReceive_PreDesc_TaskQueue,
  RtemsMessageReqReceive_PreDesc_Wait,
  RtemsMessageReqReceive_PreDesc_MsgQueue,
  RtemsMessageReqReceive_PreDesc_Storage,
  NULL
};

typedef RtemsMessageReqReceive_Context Context;
static const uint32_t        magic = 0xA55CA3D1; /* an arbitrary number */
static const rtems_interval  timeout_ticks = 3;
static const rtems_event_set wake_main_task_event = RTEMS_EVENT_17;

static void DoAction( void *ctx_in )
{
  Context *ctx = ctx_in;
  ctx->status = rtems_message_queue_receive(
    ctx->id_param,
    ctx->buffer_param,
    ctx->size_param,
    ctx->option_set_param,
    ctx->timeout_param
  );
}

static void WorkerTask( rtems_task_argument argument )
{
  Context *ctx = (Context *) argument;
  if ( ctx != NULL ) {
    T_assert_eq_u32( ctx->magic, magic ); /* Run-time type check */
    DoAction( ctx );
    T_rsc_success( rtems_event_send( ctx->task_id, wake_main_task_event ) );
  }
  T_rsc_success( rtems_task_suspend( RTEMS_SELF ) );
}

static void WorkerDoAction( void *ctx_in )
{
  rtems_status_code status;
  Context *ctx = ctx_in;
  T_assert_eq_u32( ctx->magic, magic ); /* Run-time type check */
  status = rtems_task_restart( ctx->worker_id, (rtems_task_argument) ctx );
  T_rsc_success( status );
}

static uint32_t WaitForWorker( Context *ctx )
{
  uint32_t ticks_to_wait = timeout_ticks + 1;
  rtems_status_code status;
  rtems_event_set event_set;

   for ( ; ticks_to_wait > 0; --ticks_to_wait ) {
    /* Check whether the worker finished executing the action */
    status = rtems_event_receive(
      RTEMS_PENDING_EVENTS,
      RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
      RTEMS_NO_TIMEOUT,
      &event_set
    );
    T_rsc_success( status );

    if ( ( event_set & wake_main_task_event ) == wake_main_task_event ) {
      break;
    }
    TimecounterTick();
  }

  if ( ctx->timeout_param != RTEMS_NO_TIMEOUT ) {
    /* Wait till the worker task finishes */
    status = rtems_event_receive(
      wake_main_task_event,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &event_set
    );
  T_rsc_success( status );
  }

  return timeout_ticks + 1 - ticks_to_wait;
}

static void MessageQueueSetup( Context *ctx )
{
  rtems_status_code status;
  /* Sanity check: Make sure the message queue does not exist, yet. */
  T_assert_eq_u32( ctx->tq_ctx.thread_queue_id, RTEMS_ID_NONE );
  rtems_message_queue_config config = {
    .name = rtems_build_name( 'M', 'S', 'G', 'Q' ),
    .maximum_pending_messages = MAXIMUM_PENDING_MESSAGES,
    .maximum_message_size = MAXIMUM_MESSAGE_SIZE,
    .storage_area = ctx->storage_area,
    .storage_size = sizeof( ctx->storage_area ),
    .storage_free = NULL,
    .attributes = ctx->attribute_set
  };

  status = rtems_message_queue_construct(
    &config,
    &ctx->tq_ctx.thread_queue_id
  );
  T_rsc_success( status );

  if ( ctx->id_param != RTEMS_ID_NONE ) {
    ctx->id_param = ctx->tq_ctx.thread_queue_id;
  }
}

static void MessageQueueTeardown( Context *ctx )
{
  rtems_status_code status;
  if ( ctx->tq_ctx.thread_queue_id != RTEMS_ID_NONE ) {
    status = rtems_message_queue_delete( ctx->tq_ctx.thread_queue_id );
    T_rsc_success( status );
    ctx->tq_ctx.thread_queue_id = RTEMS_ID_NONE;
  }
}

static void CheckForNoMessage(
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  (void) message_buffer;
  (void) message_size;
  T_rsc( status, RTEMS_UNSATISFIED  );
}

static void CheckForFirstMessage(
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  T_rsc_success( status );
  T_eq_u32( message_size, 1 );
  T_eq_u8( message_buffer[0], 0 );
}

static void CheckForSecondMessage(
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  T_rsc_success( status );
  T_eq_u32( message_size, 3 );
  T_eq_u8( message_buffer[0], 1 );
  T_eq_u8( message_buffer[1], 1 );
  T_eq_u8( message_buffer[2], 1 );
}

static void CheckForThirdMessage(
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  T_rsc_success( status );
  T_eq_u32( message_size, 5 );
  T_eq_u8( message_buffer[0], 2 );
  T_eq_u8( message_buffer[1], 2 );
  T_eq_u8( message_buffer[2], 2 );
  T_eq_u8( message_buffer[3], 2 );
  T_eq_u8( message_buffer[4], 2 );
}

static void PopMessage(
  Context *ctx,
  void (*check_fn)(
    rtems_status_code status,
    uint8_t *message_buffer,
    size_t message_size
  )
)
{
  rtems_status_code status;
  uint8_t message_buffer[ MAXIMUM_MESSAGE_SIZE ];
  size_t message_size;

  status = rtems_message_queue_receive(
    ctx->tq_ctx.thread_queue_id,
    &message_buffer,
    &message_size,
    RTEMS_LOCAL | RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );

 check_fn( status, message_buffer, message_size );
}

static void CheckForNoMessageInQueue( void *ctx_in )
{
  Context *ctx = ctx_in;
  T_assert_eq_u32( ctx->magic, magic ); /* Run-time type check */
  PopMessage( ctx, CheckForNoMessage );
}

static void CheckForOneMessageInQueue( void *ctx_in )
{
  Context *ctx = ctx_in;
  T_assert_eq_u32( ctx->magic, magic ); /* Run-time type check */
  PopMessage( ctx, CheckForFirstMessage );
  PopMessage( ctx, CheckForNoMessage );
}

static void CheckForSeveralMessagesInQueue( void *ctx_in )
{
  Context *ctx = ctx_in;
  T_assert_eq_u32( ctx->magic, magic ); /* Run-time type check */
  PopMessage( ctx, CheckForFirstMessage );
  PopMessage( ctx, CheckForSecondMessage );
  PopMessage( ctx, CheckForThirdMessage );
  PopMessage( ctx, CheckForNoMessage );
}

static void MessageQueueNop( void *ctx_in )
{
  (void) ctx_in;
}

static void MessageQueueDelete( void *ctx_in )
{
  Context *ctx = ctx_in;
  T_assert_eq_u32( ctx->magic, magic ); /* Run-time type check */
  MessageQueueTeardown( ctx );
}

static Context *ToContext( TQContext *tqctx )
{
  Context *ctx = RTEMS_CONTAINER_OF( tqctx, Context, tq_ctx );
  T_assert_eq_u32( ctx->magic, magic ); /* Run-time type check */
  return ctx;
}

static Status_Control ReceiveMsg( TQContext *tqctx, TQWait wait )
{
  Context *ctx = ToContext( tqctx );
  rtems_status_code status;
  rtems_option option_set;
  rtems_interval timeout;

  switch ( wait ) {
    case TQ_WAIT_FOREVER:
      option_set = RTEMS_WAIT;
      timeout = RTEMS_NO_TIMEOUT;
      break;
    case TQ_WAIT_TIMED:
      option_set = RTEMS_WAIT;
      timeout = UINT32_MAX;
      break;
    default:
      option_set = RTEMS_NO_WAIT;
      timeout = 0;
      break;
  }

  status = rtems_message_queue_receive(
    ctx->tq_ctx.thread_queue_id,
    ctx->receive_buffer,
    &ctx->receive_size,
    option_set,
    timeout
  );

  return STATUS_BUILD( status, 0 );
}

static void SendMsg( TQContext *tqctx )
{
  Context *ctx = ToContext( tqctx );
  rtems_status_code status;
  uint8_t msg[ MAXIMUM_MESSAGE_SIZE ];

  memset( msg, ctx->send_msg_counter, MAXIMUM_MESSAGE_SIZE );
  status = rtems_message_queue_send(
    ctx->tq_ctx.thread_queue_id,
    msg,
    ( ctx->send_msg_counter * 2 ) % MAXIMUM_MESSAGE_SIZE + 1
  );
  T_rsc_success( status );
  ++ctx->send_msg_counter;
}

static void EnqueuePrepare( TQContext *tqctx )
{
  Status_Control status;

  /* Check that the message queue is empty */
  status = TQEnqueue( tqctx, TQ_NO_WAIT );
  T_eq_int( status, STATUS_BUILD( RTEMS_UNSATISFIED, 0 ) );
}

static void EnqueueDone( TQContext *tqctx )
{
  uint32_t i;

  for ( i = 0; i < tqctx->how_many; ++i ) {
    SendMsg( tqctx );
  }
}

static void RtemsMessageReqReceive_Pre_Buffer_Prepare(
  RtemsMessageReqReceive_Context   *ctx,
  RtemsMessageReqReceive_Pre_Buffer state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Pre_Buffer_Valid: {
      /*
       * While the ``buffer`` parameter references a memory area able to store
       * a message up to the maximum size permitted in this message queue.
       */
      ctx->buffer_param = ctx->receive_buffer;
      break;
    }

    case RtemsMessageReqReceive_Pre_Buffer_Null: {
      /*
       * While the ``buffer`` parameter is NULL.
       */
      ctx->buffer_param = NULL;
      break;
    }

    case RtemsMessageReqReceive_Pre_Buffer_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Pre_Size_Prepare(
  RtemsMessageReqReceive_Context *ctx,
  RtemsMessageReqReceive_Pre_Size state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Pre_Size_Valid: {
      /*
       * While the ``size`` parameter references an object of type ``size_t``.
       */
      ctx->size_param = &ctx->receive_size;
      break;
    }

    case RtemsMessageReqReceive_Pre_Size_Null: {
      /*
       * While the ``size`` parameter is NULL.
       */
      ctx->size_param = NULL;
      break;
    }

    case RtemsMessageReqReceive_Pre_Size_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Pre_Id_Prepare(
  RtemsMessageReqReceive_Context *ctx,
  RtemsMessageReqReceive_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = 1;
      break;
    }

    case RtemsMessageReqReceive_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsMessageReqReceive_Pre_Id_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Pre_DuringWait_Prepare(
  RtemsMessageReqReceive_Context       *ctx,
  RtemsMessageReqReceive_Pre_DuringWait state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Pre_DuringWait_Nop: {
      /*
       * While no rtems_message_queue_delete() directive is called successfully
       * on the message queue during the time one or more tasks are waiting to
       * receive messages.
       */
      ctx->concurrent_activity = MessageQueueNop;
      break;
    }

    case RtemsMessageReqReceive_Pre_DuringWait_Deleted: {
      /*
       * While rtems_message_queue_delete() is called successfully on the
       * message queue while one or more tasks are waiting to receive messages.
       */
      ctx->concurrent_activity = MessageQueueDelete;
      break;
    }

    case RtemsMessageReqReceive_Pre_DuringWait_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Pre_TaskQueue_Prepare(
  RtemsMessageReqReceive_Context      *ctx,
  RtemsMessageReqReceive_Pre_TaskQueue state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Pre_TaskQueue_Fifo: {
      /*
       * While the member attributes of type rtems_message_queue_config
       * contains value RTEMS_FIFO when the message queue is constructed.
       *
       * Note: RTEMS_GLOBAL is not part of the space profile because no remote
       * nodes are supported.
       */
      ctx->attribute_set = RTEMS_LOCAL | RTEMS_FIFO;
      break;
    }

    case RtemsMessageReqReceive_Pre_TaskQueue_Priority: {
      /*
       * While the member attributes of type rtems_message_queue_config
       * contains value RTEMS_PRIORITY when the message queue is constructed.
       *
       * Note: RTEMS_GLOBAL is not part of the space profile because no remote
       * nodes are supported.
       */
      ctx->attribute_set = RTEMS_LOCAL | RTEMS_PRIORITY;
      break;
    }

    case RtemsMessageReqReceive_Pre_TaskQueue_NA:
      break;
  }

  MessageQueueSetup( ctx );
}

static void RtemsMessageReqReceive_Pre_Wait_Prepare(
  RtemsMessageReqReceive_Context *ctx,
  RtemsMessageReqReceive_Pre_Wait state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Pre_Wait_No: {
      /*
       * While the ``option_set`` parameter indicates the RTEMS_NO_WAIT option.
       */
      ctx->tq_ctx.wait =      TQ_NO_WAIT;
      ctx->option_set_param = RTEMS_NO_WAIT;
      ctx->timeout_param =    1; /* 0 would be RTEMS_NO_TIMEOUT */
      break;
    }

    case RtemsMessageReqReceive_Pre_Wait_Timeout: {
      /*
       * While the ``option_set`` parameter indicates the RTEMS_WAIT option,
       * while the ``timeout`` parameter is not equal to RTEMS_NO_TIMEOUT.
       */
      ctx->tq_ctx.wait =      TQ_WAIT_TIMED;
      ctx->option_set_param = RTEMS_WAIT;
      ctx->timeout_param =    timeout_ticks;
      break;
    }

    case RtemsMessageReqReceive_Pre_Wait_Forever: {
      /*
       * While the ``option_set`` parameter indicates the RTEMS_WAIT option,
       * while the ``timeout`` parameter is equal to RTEMS_NO_TIMEOUT.
       */
      ctx->tq_ctx.wait =      TQ_WAIT_FOREVER;
      ctx->option_set_param = RTEMS_WAIT;
      ctx->timeout_param =    RTEMS_NO_TIMEOUT;
      break;
    }

    case RtemsMessageReqReceive_Pre_Wait_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Pre_MsgQueue_Prepare(
  RtemsMessageReqReceive_Context     *ctx,
  RtemsMessageReqReceive_Pre_MsgQueue state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Pre_MsgQueue_Empty: {
      /*
       * While there is no message in the message queue.
       */
      /* Message queue is already empty. */
      ctx->check_msgq_unchanged = CheckForNoMessageInQueue;
      break;
    }

    case RtemsMessageReqReceive_Pre_MsgQueue_One: {
      /*
       * While there is exactly one message in the message queue.
       */
      SendMsg( &( ctx->tq_ctx ) );
      ctx->check_msgq_unchanged = CheckForOneMessageInQueue;
      break;
    }

    case RtemsMessageReqReceive_Pre_MsgQueue_Several: {
      /*
       * While there are more than one message in the message queue.
       */
      SendMsg( &( ctx->tq_ctx ) );
      SendMsg( &( ctx->tq_ctx ) );
      SendMsg( &( ctx->tq_ctx ) );
      ctx->check_msgq_unchanged = CheckForSeveralMessagesInQueue;
      break;
    }

    case RtemsMessageReqReceive_Pre_MsgQueue_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Pre_Storage_Prepare(
  RtemsMessageReqReceive_Context    *ctx,
  RtemsMessageReqReceive_Pre_Storage state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Pre_Storage_Nop: {
      /*
       * While the memory area to which a pointer is provided as member
       * storage_area of type rtems_message_queue_config when the message queue
       * is constructed by rtems_message_queue_construct() is altered only by
       * the RTEMS operating system.
       */
      /* Only a requirement text. */
      break;
    }

    case RtemsMessageReqReceive_Pre_Storage_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Post_Status_Check(
  RtemsMessageReqReceive_Context    *ctx,
  RtemsMessageReqReceive_Post_Status state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Post_Status_Ok: {
      /*
       * The return status of rtems_message_queue_receive() shall be
       * RTEMS_SUCCESSFUL
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsMessageReqReceive_Post_Status_InvId: {
      /*
       * The return status of rtems_message_queue_receive() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsMessageReqReceive_Post_Status_InvAddr: {
      /*
       * The return status of rtems_message_queue_receive() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsMessageReqReceive_Post_Status_Unsat: {
      /*
       * The return status of rtems_message_queue_receive() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED  );
      break;
    }

    case RtemsMessageReqReceive_Post_Status_Timeout: {
      /*
       * The return status of rtems_message_queue_receive() shall be
       * RTEMS_TIMEOUT.
       */
      T_rsc( ctx->status, RTEMS_TIMEOUT );
      break;
    }

    case RtemsMessageReqReceive_Post_Status_Deleted: {
      /*
       * The return status of rtems_message_queue_receive() shall be
       * RTEMS_OBJECT_WAS_DELETED.
       */
      T_rsc( ctx->status, RTEMS_OBJECT_WAS_DELETED );
      break;
    }

    case RtemsMessageReqReceive_Post_Status_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Post_Delay_Check(
  RtemsMessageReqReceive_Context   *ctx,
  RtemsMessageReqReceive_Post_Delay state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Post_Delay_None: {
      /*
       * The rtems_message_queue_receive() call shall return immediately.
       */
      T_eq_u32( ctx->action_duration, 0 );
      break;
    }

    case RtemsMessageReqReceive_Post_Delay_Ticks: {
      /*
       * The rtems_message_queue_receive() call shall return after the timeout
       * period in ticks.
       */
      T_eq_u32( ctx->action_duration, timeout_ticks );
      break;
    }

    case RtemsMessageReqReceive_Post_Delay_Forever: {
      /*
       * The rtems_message_queue_receive() call shall not return.
       */
      T_gt_u32( ctx->action_duration, timeout_ticks );
      break;
    }

    case RtemsMessageReqReceive_Post_Delay_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Post_Size_Check(
  RtemsMessageReqReceive_Context  *ctx,
  RtemsMessageReqReceive_Post_Size state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Post_Size_First: {
      /*
       * The value of the object referenced by the ``size`` parameter shall be
       * set to the size of the first message (the same value as provided by
       * parameter ``size`` of the rtems_message_queue_send() or
       * rtems_message_queue_urgent() directive which added the message to the
       * queue) after the return of the rtems_message_queue_receive() call.
       */
      CheckForFirstMessage(
        ctx->status,
        ctx->receive_buffer,
        ctx->receive_size
      );
      break;
    }

    case RtemsMessageReqReceive_Post_Size_Nop: {
      /*
       * Objects referenced by the ``size`` parameter in past calls to
       * rtems_message_queue_receive() shall not be accessed by the
       * rtems_message_queue_receive() call (see also Nop).
       */
      T_eq_sz( ctx->receive_size, SIZE_MAX );
      break;
    }

    case RtemsMessageReqReceive_Post_Size_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Post_Msg_Check(
  RtemsMessageReqReceive_Context *ctx,
  RtemsMessageReqReceive_Post_Msg state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Post_Msg_First: {
      /*
       * The bytes 0 till ``size`` - 1 of the object referenced by the
       * ``option_set`` parameter shall contain a copy of the content of the
       * first message (all bytes unchanged and in the same order as provided
       * by parameter ``buffer`` of the rtems_message_queue_send() or
       * rtems_message_queue_urgent() directive which added the message to the
       * queue) after the return of the rtems_message_queue_receive() call.
       */
      CheckForFirstMessage(
        ctx->status,
        ctx->receive_buffer,
        ctx->receive_size
      );
      break;
    }

    case RtemsMessageReqReceive_Post_Msg_Nop: {
      /*
       * Objects referenced by the ``option_set`` parameter in past calls to
       * rtems_message_queue_receive() shall not be accessed by the
       * rtems_message_queue_receive() call (see also Nop).
       */
      int i;
      for ( i = 0; i < MAXIMUM_MESSAGE_SIZE; ++i ) {
        T_eq_u8( ctx->receive_buffer[i], UINT8_MAX );
      }
      break;
    }

    case RtemsMessageReqReceive_Post_Msg_NA:
      break;
  }
}

static void RtemsMessageReqReceive_Post_MsgQueue_Check(
  RtemsMessageReqReceive_Context      *ctx,
  RtemsMessageReqReceive_Post_MsgQueue state
)
{
  switch ( state ) {
    case RtemsMessageReqReceive_Post_MsgQueue_Empty: {
      /*
       * The message queue shall be empty after the return of the
       * rtems_message_queue_receive() call.
       */
      PopMessage( ctx, CheckForNoMessage );
      break;
    }

    case RtemsMessageReqReceive_Post_MsgQueue_OneLess: {
      /*
       * The first message shall be removed from the message queue after the
       * return of the rtems_message_queue_receive() call.
       */
      PopMessage( ctx, CheckForSecondMessage );
      PopMessage( ctx, CheckForThirdMessage );
      PopMessage( ctx, CheckForNoMessage );
      break;
    }

    case RtemsMessageReqReceive_Post_MsgQueue_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_message_queue_receive() shall not be accessed by the
       * rtems_message_queue_receive() call (see also Nop).
       */
      ctx->check_msgq_unchanged( ctx );
      break;
    }

    case RtemsMessageReqReceive_Post_MsgQueue_NA:
      break;
  }

  MessageQueueTeardown( ctx );
}

static void RtemsMessageReqReceive_Post_Tasks_Check(
  RtemsMessageReqReceive_Context   *ctx,
  RtemsMessageReqReceive_Post_Tasks state
)
{
  MessageQueueSetup( ctx );

  switch ( state ) {
    case RtemsMessageReqReceive_Post_Tasks_Fifo: {
      /*
       * Where the thread queue uses the FIFO discipline, the calling thread
       * shall be enqueued in FIFO order.
       */
      ScoreTqReqEnqueueFifo_Run( &ctx->tq_ctx );
      break;
    }

    case RtemsMessageReqReceive_Post_Tasks_Priority: {
      /*
       * Where the thread queue uses the priority discipline, the calling
       * thread shall be enqueued in priority order.
       */
      ScoreTqReqEnqueuePriority_Run( &ctx->tq_ctx );
      break;
    }

    case RtemsMessageReqReceive_Post_Tasks_NA:
      break;
  }

  MessageQueueTeardown( ctx );
}

static void RtemsMessageReqReceive_Setup( RtemsMessageReqReceive_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->magic                  = magic;
  ctx->tq_ctx.enqueue         = ReceiveMsg;
  ctx->tq_ctx.surrender       = TQDoNothingSuccessfully;
  ctx->tq_ctx.convert_status  = TQConvertStatusClassic;
  ctx->tq_ctx.enqueue_prepare = EnqueuePrepare;
  ctx->tq_ctx.enqueue_done    = EnqueueDone;
  TQInitialize( &ctx->tq_ctx );

  /*
   * ctx->tq_ctx.thread_queue_id = RTEMS_ID_NONE indicates that the message
   * queue does currently not exist. A message queue is created
   * two times in a row in a single test cycle. First after the attributes
   * are set in the preconditions. That queue is used for all tests of
   * usual message queue requirements. Second a message queue is recreated
   * in the tasks post-conditions for the tests of the task queue.
   * To avoid an accidentally creation of a second
   * message queue without the first being deleted prior,
   * ctx->tq_ctx.thread_queue_id is checked for being RTEMS_ID_NONE before
   * any message queue is created - a run-time sanity check.
   */
  ctx->tq_ctx.thread_queue_id = RTEMS_ID_NONE;
  ctx->task_id                = rtems_task_self();

  /* Note: TQInitialize() will assign the "main" task priority PRIO_NORMAL */
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, WorkerTask, NULL );
}

static void RtemsMessageReqReceive_Setup_Wrap( void *arg )
{
  RtemsMessageReqReceive_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqReceive_Setup( ctx );
}

static void RtemsMessageReqReceive_Teardown(
  RtemsMessageReqReceive_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  TQDestroy( &ctx->tq_ctx );
  (void) PollAnyEvents();
}

static void RtemsMessageReqReceive_Teardown_Wrap( void *arg )
{
  RtemsMessageReqReceive_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqReceive_Teardown( ctx );
}

static void RtemsMessageReqReceive_Prepare(
  RtemsMessageReqReceive_Context *ctx
)
{
  /* Clean away pending events - happens after RTEMS_WAIT + RTEMS_NO_TIMEOUT */
  (void) PollAnyEvents();

  ctx->send_msg_counter = 0;
  ctx->receive_size     = SIZE_MAX;
  memset( ctx->receive_buffer, UINT8_MAX, MAXIMUM_MESSAGE_SIZE );
}

static void RtemsMessageReqReceive_Action(
  RtemsMessageReqReceive_Context *ctx
)
{
  WorkerDoAction( ctx );
  ctx->concurrent_activity( ctx );
  ctx->action_duration = WaitForWorker( ctx );
}

static const RtemsMessageReqReceive_Entry
RtemsMessageReqReceive_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_InvAddr,
    RtemsMessageReqReceive_Post_Delay_None,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_Nop,
    RtemsMessageReqReceive_Post_Tasks_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_InvAddr,
    RtemsMessageReqReceive_Post_Delay_None,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_NA,
    RtemsMessageReqReceive_Post_Tasks_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_InvId,
    RtemsMessageReqReceive_Post_Delay_None,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_Nop,
    RtemsMessageReqReceive_Post_Tasks_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_InvId,
    RtemsMessageReqReceive_Post_Delay_None,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_NA,
    RtemsMessageReqReceive_Post_Tasks_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_NA,
    RtemsMessageReqReceive_Post_Delay_NA, RtemsMessageReqReceive_Post_Size_NA,
    RtemsMessageReqReceive_Post_Msg_NA,
    RtemsMessageReqReceive_Post_MsgQueue_NA,
    RtemsMessageReqReceive_Post_Tasks_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_Ok,
    RtemsMessageReqReceive_Post_Delay_None,
    RtemsMessageReqReceive_Post_Size_First,
    RtemsMessageReqReceive_Post_Msg_First,
    RtemsMessageReqReceive_Post_MsgQueue_Empty,
    RtemsMessageReqReceive_Post_Tasks_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_Ok,
    RtemsMessageReqReceive_Post_Delay_None,
    RtemsMessageReqReceive_Post_Size_First,
    RtemsMessageReqReceive_Post_Msg_First,
    RtemsMessageReqReceive_Post_MsgQueue_OneLess,
    RtemsMessageReqReceive_Post_Tasks_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_Deleted,
    RtemsMessageReqReceive_Post_Delay_None,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_NA,
    RtemsMessageReqReceive_Post_Tasks_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_Unsat,
    RtemsMessageReqReceive_Post_Delay_None,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_Nop,
    RtemsMessageReqReceive_Post_Tasks_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_Timeout,
    RtemsMessageReqReceive_Post_Delay_Ticks,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_Nop,
    RtemsMessageReqReceive_Post_Tasks_Fifo },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_NA,
    RtemsMessageReqReceive_Post_Delay_Forever,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_Nop,
    RtemsMessageReqReceive_Post_Tasks_Fifo },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_Timeout,
    RtemsMessageReqReceive_Post_Delay_Ticks,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_Nop,
    RtemsMessageReqReceive_Post_Tasks_Priority },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqReceive_Post_Status_NA,
    RtemsMessageReqReceive_Post_Delay_Forever,
    RtemsMessageReqReceive_Post_Size_Nop, RtemsMessageReqReceive_Post_Msg_Nop,
    RtemsMessageReqReceive_Post_MsgQueue_Nop,
    RtemsMessageReqReceive_Post_Tasks_Priority }
};

static const uint8_t
RtemsMessageReqReceive_Map[] = {
  8, 5, 6, 9, 5, 6, 10, 5, 6, 8, 5, 6, 11, 5, 6, 12, 5, 6, 4, 4, 4, 7, 4, 4, 7,
  4, 4, 4, 4, 4, 7, 4, 4, 7, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1
};

static size_t RtemsMessageReqReceive_Scope( void *arg, char *buf, size_t n )
{
  RtemsMessageReqReceive_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsMessageReqReceive_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsMessageReqReceive_Fixture = {
  .setup = RtemsMessageReqReceive_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMessageReqReceive_Teardown_Wrap,
  .scope = RtemsMessageReqReceive_Scope,
  .initial_context = &RtemsMessageReqReceive_Instance
};

static inline RtemsMessageReqReceive_Entry RtemsMessageReqReceive_PopEntry(
  RtemsMessageReqReceive_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMessageReqReceive_Entries[
    RtemsMessageReqReceive_Map[ index ]
  ];
}

static void RtemsMessageReqReceive_TestVariant(
  RtemsMessageReqReceive_Context *ctx
)
{
  RtemsMessageReqReceive_Pre_Buffer_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMessageReqReceive_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsMessageReqReceive_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsMessageReqReceive_Pre_DuringWait_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsMessageReqReceive_Pre_TaskQueue_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsMessageReqReceive_Pre_Wait_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsMessageReqReceive_Pre_MsgQueue_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsMessageReqReceive_Pre_Storage_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  RtemsMessageReqReceive_Action( ctx );
  RtemsMessageReqReceive_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsMessageReqReceive_Post_Delay_Check( ctx, ctx->Map.entry.Post_Delay );
  RtemsMessageReqReceive_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
  RtemsMessageReqReceive_Post_Msg_Check( ctx, ctx->Map.entry.Post_Msg );
  RtemsMessageReqReceive_Post_MsgQueue_Check(
    ctx,
    ctx->Map.entry.Post_MsgQueue
  );
  RtemsMessageReqReceive_Post_Tasks_Check( ctx, ctx->Map.entry.Post_Tasks );
}

/**
 * @fn void T_case_body_RtemsMessageReqReceive( void )
 */
T_TEST_CASE_FIXTURE( RtemsMessageReqReceive, &RtemsMessageReqReceive_Fixture )
{
  RtemsMessageReqReceive_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsMessageReqReceive_Pre_Buffer_Valid;
    ctx->Map.pcs[ 0 ] < RtemsMessageReqReceive_Pre_Buffer_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsMessageReqReceive_Pre_Size_Valid;
      ctx->Map.pcs[ 1 ] < RtemsMessageReqReceive_Pre_Size_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsMessageReqReceive_Pre_Id_Valid;
        ctx->Map.pcs[ 2 ] < RtemsMessageReqReceive_Pre_Id_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsMessageReqReceive_Pre_DuringWait_Nop;
          ctx->Map.pcs[ 3 ] < RtemsMessageReqReceive_Pre_DuringWait_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsMessageReqReceive_Pre_TaskQueue_Fifo;
            ctx->Map.pcs[ 4 ] < RtemsMessageReqReceive_Pre_TaskQueue_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsMessageReqReceive_Pre_Wait_No;
              ctx->Map.pcs[ 5 ] < RtemsMessageReqReceive_Pre_Wait_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = RtemsMessageReqReceive_Pre_MsgQueue_Empty;
                ctx->Map.pcs[ 6 ] < RtemsMessageReqReceive_Pre_MsgQueue_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                for (
                  ctx->Map.pcs[ 7 ] = RtemsMessageReqReceive_Pre_Storage_Nop;
                  ctx->Map.pcs[ 7 ] < RtemsMessageReqReceive_Pre_Storage_NA;
                  ++ctx->Map.pcs[ 7 ]
                ) {
                  ctx->Map.entry = RtemsMessageReqReceive_PopEntry( ctx );

                  if ( ctx->Map.entry.Skip ) {
                    continue;
                  }

                  RtemsMessageReqReceive_Prepare( ctx );
                  RtemsMessageReqReceive_TestVariant( ctx );
                }
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
