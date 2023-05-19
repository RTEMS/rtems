/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMessageReqFlushPending
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
 * @defgroup RtemsMessageReqFlushPending spec:/rtems/message/req/flush-pending
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsMessageReqFlushPending_Pre_Count_Valid,
  RtemsMessageReqFlushPending_Pre_Count_Null,
  RtemsMessageReqFlushPending_Pre_Count_NA
} RtemsMessageReqFlushPending_Pre_Count;

typedef enum {
  RtemsMessageReqFlushPending_Pre_Id_Valid,
  RtemsMessageReqFlushPending_Pre_Id_Invalid,
  RtemsMessageReqFlushPending_Pre_Id_NA
} RtemsMessageReqFlushPending_Pre_Id;

typedef enum {
  RtemsMessageReqFlushPending_Pre_MsgQueue_Empty,
  RtemsMessageReqFlushPending_Pre_MsgQueue_Several,
  RtemsMessageReqFlushPending_Pre_MsgQueue_NA
} RtemsMessageReqFlushPending_Pre_MsgQueue;

typedef enum {
  RtemsMessageReqFlushPending_Pre_Receivers_Waiting,
  RtemsMessageReqFlushPending_Pre_Receivers_None,
  RtemsMessageReqFlushPending_Pre_Receivers_NA
} RtemsMessageReqFlushPending_Pre_Receivers;

typedef enum {
  RtemsMessageReqFlushPending_Pre_Directive_Flush,
  RtemsMessageReqFlushPending_Pre_Directive_Pending,
  RtemsMessageReqFlushPending_Pre_Directive_NA
} RtemsMessageReqFlushPending_Pre_Directive;

typedef enum {
  RtemsMessageReqFlushPending_Pre_Storage_Nop,
  RtemsMessageReqFlushPending_Pre_Storage_NA
} RtemsMessageReqFlushPending_Pre_Storage;

typedef enum {
  RtemsMessageReqFlushPending_Post_Status_Ok,
  RtemsMessageReqFlushPending_Post_Status_InvId,
  RtemsMessageReqFlushPending_Post_Status_InvAddr,
  RtemsMessageReqFlushPending_Post_Status_NA
} RtemsMessageReqFlushPending_Post_Status;

typedef enum {
  RtemsMessageReqFlushPending_Post_Count_Zero,
  RtemsMessageReqFlushPending_Post_Count_Set,
  RtemsMessageReqFlushPending_Post_Count_Nop,
  RtemsMessageReqFlushPending_Post_Count_NA
} RtemsMessageReqFlushPending_Post_Count;

typedef enum {
  RtemsMessageReqFlushPending_Post_MsgQueue_Empty,
  RtemsMessageReqFlushPending_Post_MsgQueue_Nop,
  RtemsMessageReqFlushPending_Post_MsgQueue_NA
} RtemsMessageReqFlushPending_Post_MsgQueue;

typedef enum {
  RtemsMessageReqFlushPending_Post_Receivers_Nop,
  RtemsMessageReqFlushPending_Post_Receivers_NA
} RtemsMessageReqFlushPending_Post_Receivers;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Count_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_MsgQueue_NA : 1;
  uint16_t Pre_Receivers_NA : 1;
  uint16_t Pre_Directive_NA : 1;
  uint16_t Pre_Storage_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Count : 2;
  uint16_t Post_MsgQueue : 2;
  uint16_t Post_Receivers : 1;
} RtemsMessageReqFlushPending_Entry;

#define MAXIMUM_PENDING_MESSAGES 3
#define MAXIMUM_MESSAGE_SIZE     5
#define NUMBER_OF_WORKERS        3

/**
 * @brief Test context for spec:/rtems/message/req/flush-pending test case.
 */
typedef struct {
  /**
   * @brief This member contains a valid ID of a message queue.
   */
  rtems_id message_queue_id;

  /**
   * @brief This member is used as storage area for the message queue.
   */
  RTEMS_MESSAGE_QUEUE_BUFFER( MAXIMUM_MESSAGE_SIZE )
    storage_area[ MAXIMUM_PENDING_MESSAGES ];

  /**
   * @brief This member contains the returned status codes of the receivers.
   */
  rtems_status_code receive_status[ NUMBER_OF_WORKERS ];

  /**
   * @brief This member specifies the directive to be called as action.
   *
   * This is either rtems_message_queue_flush() or
   * rtems_message_queue_get_number_pending().
   */
  rtems_status_code (*action)( rtems_id id, uint32_t *count );

  /**
   * @brief This member specifies the ``id`` parameter of the action.
   */
  rtems_id id_param;

  /**
   * @brief This member specifies the ``count`` parameter of the action.
   */
  uint32_t *count_param;

  /**
   * @brief This member contains the returned status code of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains the value returned in parameter ``count`` of
   *   the action.
   */
  uint32_t count;

  /**
   * @brief This member contains the task identifiers of the worker tasks.
   */
  rtems_id worker_id[ NUMBER_OF_WORKERS ];

  /**
   * @brief This member contains a pointer to a function which is executed to
   *   check that the action has not changed the content of the message queue.
   */
  void (*check_msgq_unchanged)( void *ctx_in );

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    RtemsMessageReqFlushPending_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMessageReqFlushPending_Context;

static RtemsMessageReqFlushPending_Context
  RtemsMessageReqFlushPending_Instance;

static const char * const RtemsMessageReqFlushPending_PreDesc_Count[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqFlushPending_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsMessageReqFlushPending_PreDesc_MsgQueue[] = {
  "Empty",
  "Several",
  "NA"
};

static const char * const RtemsMessageReqFlushPending_PreDesc_Receivers[] = {
  "Waiting",
  "None",
  "NA"
};

static const char * const RtemsMessageReqFlushPending_PreDesc_Directive[] = {
  "Flush",
  "Pending",
  "NA"
};

static const char * const RtemsMessageReqFlushPending_PreDesc_Storage[] = {
  "Nop",
  "NA"
};

static const char * const * const RtemsMessageReqFlushPending_PreDesc[] = {
  RtemsMessageReqFlushPending_PreDesc_Count,
  RtemsMessageReqFlushPending_PreDesc_Id,
  RtemsMessageReqFlushPending_PreDesc_MsgQueue,
  RtemsMessageReqFlushPending_PreDesc_Receivers,
  RtemsMessageReqFlushPending_PreDesc_Directive,
  RtemsMessageReqFlushPending_PreDesc_Storage,
  NULL
};

typedef RtemsMessageReqFlushPending_Context Context;
static const uint32_t NUMBER_OF_PENDING_MESSAGES = 2;
static const rtems_interval TIMEOUT_TICKS = 1;
static const rtems_event_set EVENT_RECEIVE = RTEMS_EVENT_17;
static const uint8_t queued_message[] = { 200, 201, 202 };

static void Receive( Context *ctx, size_t worker_index )
{
  size_t size;
  uint8_t buffer[ MAXIMUM_MESSAGE_SIZE ];

  ctx->receive_status[worker_index] = rtems_message_queue_receive(
    ctx->message_queue_id,
    buffer,
    &size,
    RTEMS_WAIT,
    TIMEOUT_TICKS
  );
}

static void WorkerTask( rtems_task_argument argument )
{
  static size_t worker_number = 0;
  size_t worker_index = worker_number++;
  Context *ctx = (Context *) argument;

  while ( true ) {
    ReceiveAnyEvents();
    Receive( ctx, worker_index );
  }
}

static void CheckForNoMessage(
  Context *ctx,
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  (void) ctx;
  (void) message_buffer;
  (void) message_size;
  T_rsc( status, RTEMS_UNSATISFIED  );
}

static void CheckForQueuedMessage(
  Context *ctx,
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  (void) ctx;
  T_rsc_success( status );
  T_eq_u32( message_size, sizeof( queued_message ) );
  T_eq_mem( message_buffer, queued_message, sizeof( queued_message ) );
}

static void PopMessage(
  Context *ctx,
  void (*check_fn)(
    Context *ctx,
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
    ctx->message_queue_id,
    &message_buffer,
    &message_size,
    RTEMS_LOCAL | RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );

 check_fn( ctx, status, message_buffer, message_size );
}

static void CheckForNoMessageInQueue( void *ctx_in )
{
  Context *ctx = ctx_in;
  PopMessage( ctx, CheckForNoMessage );
}

static void CheckForSeveralMessagesInQueue( void *ctx_in )
{
  Context *ctx = ctx_in;
  uint32_t i;
  for ( i = 0; i < NUMBER_OF_PENDING_MESSAGES; ++i ) {
    PopMessage( ctx, CheckForQueuedMessage );
  }
  PopMessage( ctx, CheckForNoMessage );
}

static void SendMsg( Context *ctx )
{
  rtems_status_code status;

  status = rtems_message_queue_send(
    ctx->message_queue_id,
    queued_message,
    sizeof( queued_message )
  );
  T_rsc_success( status );
}

static void RtemsMessageReqFlushPending_Pre_Count_Prepare(
  RtemsMessageReqFlushPending_Context  *ctx,
  RtemsMessageReqFlushPending_Pre_Count state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Pre_Count_Valid: {
      /*
       * While the ``count`` parameter references an ``uint32_t`` object.
       */
      ctx->count_param = &ctx->count;
      break;
    }

    case RtemsMessageReqFlushPending_Pre_Count_Null: {
      /*
       * While the ``count`` parameter is NULL.
       */
      ctx->count_param = NULL;
      break;
    }

    case RtemsMessageReqFlushPending_Pre_Count_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Pre_Id_Prepare(
  RtemsMessageReqFlushPending_Context *ctx,
  RtemsMessageReqFlushPending_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->message_queue_id;
      break;
    }

    case RtemsMessageReqFlushPending_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsMessageReqFlushPending_Pre_Id_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Pre_MsgQueue_Prepare(
  RtemsMessageReqFlushPending_Context     *ctx,
  RtemsMessageReqFlushPending_Pre_MsgQueue state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Pre_MsgQueue_Empty: {
      /*
       * While there is no message in the message queue.
       */
      /* Message queue is already empty. */
      ctx->check_msgq_unchanged = CheckForNoMessageInQueue;
      break;
    }

    case RtemsMessageReqFlushPending_Pre_MsgQueue_Several: {
      /*
       * While there are messages in the message queue.
       */
      uint32_t i;
      for ( i = 0; i < NUMBER_OF_PENDING_MESSAGES; ++i ) {
        SendMsg( ctx );
      }
      ctx->check_msgq_unchanged = CheckForSeveralMessagesInQueue;
      break;
    }

    case RtemsMessageReqFlushPending_Pre_MsgQueue_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Pre_Receivers_Prepare(
  RtemsMessageReqFlushPending_Context      *ctx,
  RtemsMessageReqFlushPending_Pre_Receivers state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Pre_Receivers_Waiting: {
      /*
       * While one or more receivers are waiting to receive a message.
       */
      size_t i;
      for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
        SendEvents( ctx->worker_id[i], EVENT_RECEIVE );
      }
      break;
    }

    case RtemsMessageReqFlushPending_Pre_Receivers_None: {
      /*
       * While no receiver is waiting to receive a message.
       */
      /* There is already no receiver waiting. */
      break;
    }

    case RtemsMessageReqFlushPending_Pre_Receivers_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Pre_Directive_Prepare(
  RtemsMessageReqFlushPending_Context      *ctx,
  RtemsMessageReqFlushPending_Pre_Directive state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Pre_Directive_Flush: {
      /*
       * While the directive rtems_message_queue_flush() is called.
       */
      ctx->action = rtems_message_queue_flush;
      break;
    }

    case RtemsMessageReqFlushPending_Pre_Directive_Pending: {
      /*
       * While the directive rtems_message_queue_get_number_pending() is
       * called.
       */
      ctx->action = rtems_message_queue_get_number_pending;
      break;
    }

    case RtemsMessageReqFlushPending_Pre_Directive_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Pre_Storage_Prepare(
  RtemsMessageReqFlushPending_Context    *ctx,
  RtemsMessageReqFlushPending_Pre_Storage state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Pre_Storage_Nop: {
      /*
       * While the memory area to which a pointer is provided as member
       * storage_area of type rtems_message_queue_config when the message queue
       * is constructed by rtems_message_queue_construct() is altered only by
       * the RTEMS operating system.
       */
      /* Only a requirement text. */
      break;
    }

    case RtemsMessageReqFlushPending_Pre_Storage_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Post_Status_Check(
  RtemsMessageReqFlushPending_Context    *ctx,
  RtemsMessageReqFlushPending_Post_Status state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Post_Status_Ok: {
      /*
       * The return status of the called directive (rtems_message_queue_flush()
       * or rtems_message_queue_get_number_pending()) shall be RTEMS_SUCCESSFUL
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsMessageReqFlushPending_Post_Status_InvId: {
      /*
       * The return status of the called directive (rtems_message_queue_flush()
       * or rtems_message_queue_get_number_pending()) shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsMessageReqFlushPending_Post_Status_InvAddr: {
      /*
       * The return status of the called directive (rtems_message_queue_flush()
       * or rtems_message_queue_get_number_pending()) shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsMessageReqFlushPending_Post_Status_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Post_Count_Check(
  RtemsMessageReqFlushPending_Context   *ctx,
  RtemsMessageReqFlushPending_Post_Count state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Post_Count_Zero: {
      /*
       * The value of the object referenced by the ``count`` parameter shall be
       * 0 after the return of the rtems_message_queue_flush() or
       * rtems_message_queue_get_number_pending() call.
       */
      T_eq_u32( ctx->count, 0 );
      break;
    }

    case RtemsMessageReqFlushPending_Post_Count_Set: {
      /*
       * The rtems_message_queue_get_number_pending() directive shall set the
       * value of the object referenced by the ``count`` parameter to the
       * number of messages present in the message queue at a point in time
       * during the single execution of the
       * rtems_message_queue_get_number_pending() directive.
       *
       * The rtems_message_queue_flush() directive shall set the value of the
       * object referenced by the ``count`` parameter to the number of messages
       * it removed from the message queue during the single execution of the
       * rtems_message_queue_flush() directive.
       */
      T_eq_u32( ctx->count, NUMBER_OF_PENDING_MESSAGES );
      break;
    }

    case RtemsMessageReqFlushPending_Post_Count_Nop: {
      /*
       * The value of the object referenced by the ``count`` parameter in past
       * call to rtems_message_queue_flush() or
       * rtems_message_queue_get_number_pending() shall not be accessed by the
       * rtems_message_queue_flush() or
       * rtems_message_queue_get_number_pending() call (see also Nop).
       */
      T_eq_u32( ctx->count, UINT8_MAX );
      break;
    }

    case RtemsMessageReqFlushPending_Post_Count_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Post_MsgQueue_Check(
  RtemsMessageReqFlushPending_Context      *ctx,
  RtemsMessageReqFlushPending_Post_MsgQueue state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Post_MsgQueue_Empty: {
      /*
       * The message queue shall contain no messages after the last call to
       * ``id``.
       */
      PopMessage( ctx, CheckForNoMessage );
      break;
    }

    case RtemsMessageReqFlushPending_Post_MsgQueue_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in the past call to
       * rtems_message_queue_flush() or
       * rtems_message_queue_get_number_pending() shall not be changed by that
       * call (see also Nop).
       */
      ctx->check_msgq_unchanged( ctx );
      break;
    }

    case RtemsMessageReqFlushPending_Post_MsgQueue_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Post_Receivers_Check(
  RtemsMessageReqFlushPending_Context       *ctx,
  RtemsMessageReqFlushPending_Post_Receivers state
)
{
  switch ( state ) {
    case RtemsMessageReqFlushPending_Post_Receivers_Nop: {
      /*
       * The receivers waiting for a message at the message queue shall not be
       * affected by the call to the rtems_message_queue_flush() or
       * rtems_message_queue_get_number_pending() directive.
       */
      size_t i;
      for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
        T_rsc( ctx->receive_status[i], RTEMS_TIMEOUT );
      }
      break;
    }

    case RtemsMessageReqFlushPending_Post_Receivers_NA:
      break;
  }
}

static void RtemsMessageReqFlushPending_Setup(
  RtemsMessageReqFlushPending_Context *ctx
)
{
  size_t i;
  SetSelfPriority( PRIO_NORMAL );

  for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
    ctx->worker_id[i] = CreateTask( "WORK", PRIO_HIGH );
    StartTask( ctx->worker_id[i], WorkerTask, ctx );
  }
}

static void RtemsMessageReqFlushPending_Setup_Wrap( void *arg )
{
  RtemsMessageReqFlushPending_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqFlushPending_Setup( ctx );
}

static void RtemsMessageReqFlushPending_Teardown(
  RtemsMessageReqFlushPending_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
    DeleteTask( ctx->worker_id[i] );
  }
  RestoreRunnerPriority();
}

static void RtemsMessageReqFlushPending_Teardown_Wrap( void *arg )
{
  RtemsMessageReqFlushPending_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqFlushPending_Teardown( ctx );
}

static void RtemsMessageReqFlushPending_Prepare(
  RtemsMessageReqFlushPending_Context *ctx
)
{
  rtems_status_code status;

  rtems_message_queue_config config = {
    .name                     = rtems_build_name( 'M', 'S', 'G', 'Q' ),
    .maximum_pending_messages = MAXIMUM_PENDING_MESSAGES,
    .maximum_message_size     = MAXIMUM_MESSAGE_SIZE,
    .storage_area             = ctx->storage_area,
    .storage_size             = sizeof( ctx->storage_area ),
    .storage_free             = NULL,
    .attributes               = RTEMS_DEFAULT_ATTRIBUTES
  };

  status = rtems_message_queue_construct(
    &config,
    &ctx->message_queue_id
  );
  T_rsc_success( status );

  ctx->count = UINT8_MAX;
}

static void RtemsMessageReqFlushPending_Action(
  RtemsMessageReqFlushPending_Context *ctx
)
{
  ctx->status = (ctx->action)(
    ctx->id_param,
    ctx->count_param
  );

  FinalClockTick();
}

static void RtemsMessageReqFlushPending_Cleanup(
  RtemsMessageReqFlushPending_Context *ctx
)
{
  T_rsc_success( rtems_message_queue_delete( ctx->message_queue_id ) );
}

static const RtemsMessageReqFlushPending_Entry
RtemsMessageReqFlushPending_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_NA,
    RtemsMessageReqFlushPending_Post_Count_NA,
    RtemsMessageReqFlushPending_Post_MsgQueue_NA,
    RtemsMessageReqFlushPending_Post_Receivers_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_InvAddr,
    RtemsMessageReqFlushPending_Post_Count_Nop,
    RtemsMessageReqFlushPending_Post_MsgQueue_Nop,
    RtemsMessageReqFlushPending_Post_Receivers_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_InvId,
    RtemsMessageReqFlushPending_Post_Count_Nop,
    RtemsMessageReqFlushPending_Post_MsgQueue_Nop,
    RtemsMessageReqFlushPending_Post_Receivers_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_InvAddr,
    RtemsMessageReqFlushPending_Post_Count_Nop,
    RtemsMessageReqFlushPending_Post_MsgQueue_Nop,
    RtemsMessageReqFlushPending_Post_Receivers_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_InvId,
    RtemsMessageReqFlushPending_Post_Count_Nop,
    RtemsMessageReqFlushPending_Post_MsgQueue_Nop,
    RtemsMessageReqFlushPending_Post_Receivers_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_Ok,
    RtemsMessageReqFlushPending_Post_Count_Zero,
    RtemsMessageReqFlushPending_Post_MsgQueue_Empty,
    RtemsMessageReqFlushPending_Post_Receivers_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_Ok,
    RtemsMessageReqFlushPending_Post_Count_Zero,
    RtemsMessageReqFlushPending_Post_MsgQueue_Nop,
    RtemsMessageReqFlushPending_Post_Receivers_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_Ok,
    RtemsMessageReqFlushPending_Post_Count_Zero,
    RtemsMessageReqFlushPending_Post_MsgQueue_Empty,
    RtemsMessageReqFlushPending_Post_Receivers_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_Ok,
    RtemsMessageReqFlushPending_Post_Count_Zero,
    RtemsMessageReqFlushPending_Post_MsgQueue_Nop,
    RtemsMessageReqFlushPending_Post_Receivers_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_Ok,
    RtemsMessageReqFlushPending_Post_Count_Set,
    RtemsMessageReqFlushPending_Post_MsgQueue_Empty,
    RtemsMessageReqFlushPending_Post_Receivers_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqFlushPending_Post_Status_Ok,
    RtemsMessageReqFlushPending_Post_Count_Set,
    RtemsMessageReqFlushPending_Post_MsgQueue_Nop,
    RtemsMessageReqFlushPending_Post_Receivers_NA }
};

static const uint8_t
RtemsMessageReqFlushPending_Map[] = {
  5, 6, 7, 8, 0, 0, 9, 10, 4, 4, 2, 2, 0, 0, 2, 2, 3, 3, 1, 1, 0, 0, 1, 1, 3,
  3, 1, 1, 0, 0, 1, 1
};

static size_t RtemsMessageReqFlushPending_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsMessageReqFlushPending_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsMessageReqFlushPending_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsMessageReqFlushPending_Fixture = {
  .setup = RtemsMessageReqFlushPending_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMessageReqFlushPending_Teardown_Wrap,
  .scope = RtemsMessageReqFlushPending_Scope,
  .initial_context = &RtemsMessageReqFlushPending_Instance
};

static inline RtemsMessageReqFlushPending_Entry
RtemsMessageReqFlushPending_PopEntry(
  RtemsMessageReqFlushPending_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMessageReqFlushPending_Entries[
    RtemsMessageReqFlushPending_Map[ index ]
  ];
}

static void RtemsMessageReqFlushPending_TestVariant(
  RtemsMessageReqFlushPending_Context *ctx
)
{
  RtemsMessageReqFlushPending_Pre_Count_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMessageReqFlushPending_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsMessageReqFlushPending_Pre_MsgQueue_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsMessageReqFlushPending_Pre_Receivers_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsMessageReqFlushPending_Pre_Directive_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsMessageReqFlushPending_Pre_Storage_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsMessageReqFlushPending_Action( ctx );
  RtemsMessageReqFlushPending_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsMessageReqFlushPending_Post_Count_Check(
    ctx,
    ctx->Map.entry.Post_Count
  );
  RtemsMessageReqFlushPending_Post_MsgQueue_Check(
    ctx,
    ctx->Map.entry.Post_MsgQueue
  );
  RtemsMessageReqFlushPending_Post_Receivers_Check(
    ctx,
    ctx->Map.entry.Post_Receivers
  );
}

/**
 * @fn void T_case_body_RtemsMessageReqFlushPending( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMessageReqFlushPending,
  &RtemsMessageReqFlushPending_Fixture
)
{
  RtemsMessageReqFlushPending_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsMessageReqFlushPending_Pre_Count_Valid;
    ctx->Map.pcs[ 0 ] < RtemsMessageReqFlushPending_Pre_Count_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsMessageReqFlushPending_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsMessageReqFlushPending_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsMessageReqFlushPending_Pre_MsgQueue_Empty;
        ctx->Map.pcs[ 2 ] < RtemsMessageReqFlushPending_Pre_MsgQueue_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsMessageReqFlushPending_Pre_Receivers_Waiting;
          ctx->Map.pcs[ 3 ] < RtemsMessageReqFlushPending_Pre_Receivers_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsMessageReqFlushPending_Pre_Directive_Flush;
            ctx->Map.pcs[ 4 ] < RtemsMessageReqFlushPending_Pre_Directive_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsMessageReqFlushPending_Pre_Storage_Nop;
              ctx->Map.pcs[ 5 ] < RtemsMessageReqFlushPending_Pre_Storage_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              ctx->Map.entry = RtemsMessageReqFlushPending_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              RtemsMessageReqFlushPending_Prepare( ctx );
              RtemsMessageReqFlushPending_TestVariant( ctx );
              RtemsMessageReqFlushPending_Cleanup( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
