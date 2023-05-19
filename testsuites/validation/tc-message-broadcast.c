/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMessageReqBroadcast
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
 * @defgroup RtemsMessageReqBroadcast spec:/rtems/message/req/broadcast
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsMessageReqBroadcast_Pre_SendBuffer_Valid,
  RtemsMessageReqBroadcast_Pre_SendBuffer_Null,
  RtemsMessageReqBroadcast_Pre_SendBuffer_NA
} RtemsMessageReqBroadcast_Pre_SendBuffer;

typedef enum {
  RtemsMessageReqBroadcast_Pre_Count_Valid,
  RtemsMessageReqBroadcast_Pre_Count_Null,
  RtemsMessageReqBroadcast_Pre_Count_NA
} RtemsMessageReqBroadcast_Pre_Count;

typedef enum {
  RtemsMessageReqBroadcast_Pre_Id_Valid,
  RtemsMessageReqBroadcast_Pre_Id_Invalid,
  RtemsMessageReqBroadcast_Pre_Id_NA
} RtemsMessageReqBroadcast_Pre_Id;

typedef enum {
  RtemsMessageReqBroadcast_Pre_MsgSize_Zero,
  RtemsMessageReqBroadcast_Pre_MsgSize_SomeSize,
  RtemsMessageReqBroadcast_Pre_MsgSize_MaxSize,
  RtemsMessageReqBroadcast_Pre_MsgSize_TooLarge,
  RtemsMessageReqBroadcast_Pre_MsgSize_NA
} RtemsMessageReqBroadcast_Pre_MsgSize;

typedef enum {
  RtemsMessageReqBroadcast_Pre_MsgQueue_Empty,
  RtemsMessageReqBroadcast_Pre_MsgQueue_Several,
  RtemsMessageReqBroadcast_Pre_MsgQueue_NA
} RtemsMessageReqBroadcast_Pre_MsgQueue;

typedef enum {
  RtemsMessageReqBroadcast_Pre_Receivers_Waiting,
  RtemsMessageReqBroadcast_Pre_Receivers_None,
  RtemsMessageReqBroadcast_Pre_Receivers_NA
} RtemsMessageReqBroadcast_Pre_Receivers;

typedef enum {
  RtemsMessageReqBroadcast_Pre_Storage_Nop,
  RtemsMessageReqBroadcast_Pre_Storage_NA
} RtemsMessageReqBroadcast_Pre_Storage;

typedef enum {
  RtemsMessageReqBroadcast_Post_Status_Ok,
  RtemsMessageReqBroadcast_Post_Status_InvId,
  RtemsMessageReqBroadcast_Post_Status_InvAddr,
  RtemsMessageReqBroadcast_Post_Status_InvSize,
  RtemsMessageReqBroadcast_Post_Status_NA
} RtemsMessageReqBroadcast_Post_Status;

typedef enum {
  RtemsMessageReqBroadcast_Post_Count_Zero,
  RtemsMessageReqBroadcast_Post_Count_Set,
  RtemsMessageReqBroadcast_Post_Count_Nop,
  RtemsMessageReqBroadcast_Post_Count_NA
} RtemsMessageReqBroadcast_Post_Count;

typedef enum {
  RtemsMessageReqBroadcast_Post_MsgQueue_Nop,
  RtemsMessageReqBroadcast_Post_MsgQueue_NA
} RtemsMessageReqBroadcast_Post_MsgQueue;

typedef enum {
  RtemsMessageReqBroadcast_Post_Receivers_Unblocked,
  RtemsMessageReqBroadcast_Post_Receivers_Nop,
  RtemsMessageReqBroadcast_Post_Receivers_NA
} RtemsMessageReqBroadcast_Post_Receivers;

typedef enum {
  RtemsMessageReqBroadcast_Post_RecSize_Message,
  RtemsMessageReqBroadcast_Post_RecSize_Nop,
  RtemsMessageReqBroadcast_Post_RecSize_NA
} RtemsMessageReqBroadcast_Post_RecSize;

typedef enum {
  RtemsMessageReqBroadcast_Post_RecBuffer_Message,
  RtemsMessageReqBroadcast_Post_RecBuffer_Nop,
  RtemsMessageReqBroadcast_Post_RecBuffer_NA
} RtemsMessageReqBroadcast_Post_RecBuffer;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_SendBuffer_NA : 1;
  uint32_t Pre_Count_NA : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_MsgSize_NA : 1;
  uint32_t Pre_MsgQueue_NA : 1;
  uint32_t Pre_Receivers_NA : 1;
  uint32_t Pre_Storage_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Count : 2;
  uint32_t Post_MsgQueue : 1;
  uint32_t Post_Receivers : 2;
  uint32_t Post_RecSize : 2;
  uint32_t Post_RecBuffer : 2;
} RtemsMessageReqBroadcast_Entry;

#define MAXIMUM_PENDING_MESSAGES 3
#define MAXIMUM_MESSAGE_SIZE     5
#define NUMBER_OF_WORKERS        3

/**
 * @brief Test context for spec:/rtems/message/req/broadcast test case.
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
   * @brief This member contains a buffer to receive messages from the queue.
   */
  uint8_t receive_buffer[ NUMBER_OF_WORKERS ][ MAXIMUM_MESSAGE_SIZE ];

  /**
   * @brief This member contains several buffers to receive a messages size.
   */
  size_t receive_size[ NUMBER_OF_WORKERS ];

  /**
   * @brief This member contains the returned status codes of the receivers.
   */
  rtems_status_code receive_status[ NUMBER_OF_WORKERS ];

  /**
   * @brief This member specifies the ``id`` parameter of the action.
   */
  rtems_id id_param;

  /**
   * @brief This member specifies the ``buffer`` parameter of the action.
   */
  const void *buffer_param;

  /**
   * @brief This member specifies the ``size`` parameter of the action.
   */
  size_t size_param;

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
    size_t pcs[ 7 ];

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
    RtemsMessageReqBroadcast_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMessageReqBroadcast_Context;

static RtemsMessageReqBroadcast_Context
  RtemsMessageReqBroadcast_Instance;

static const char * const RtemsMessageReqBroadcast_PreDesc_SendBuffer[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqBroadcast_PreDesc_Count[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqBroadcast_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsMessageReqBroadcast_PreDesc_MsgSize[] = {
  "Zero",
  "SomeSize",
  "MaxSize",
  "TooLarge",
  "NA"
};

static const char * const RtemsMessageReqBroadcast_PreDesc_MsgQueue[] = {
  "Empty",
  "Several",
  "NA"
};

static const char * const RtemsMessageReqBroadcast_PreDesc_Receivers[] = {
  "Waiting",
  "None",
  "NA"
};

static const char * const RtemsMessageReqBroadcast_PreDesc_Storage[] = {
  "Nop",
  "NA"
};

static const char * const * const RtemsMessageReqBroadcast_PreDesc[] = {
  RtemsMessageReqBroadcast_PreDesc_SendBuffer,
  RtemsMessageReqBroadcast_PreDesc_Count,
  RtemsMessageReqBroadcast_PreDesc_Id,
  RtemsMessageReqBroadcast_PreDesc_MsgSize,
  RtemsMessageReqBroadcast_PreDesc_MsgQueue,
  RtemsMessageReqBroadcast_PreDesc_Receivers,
  RtemsMessageReqBroadcast_PreDesc_Storage,
  NULL
};

typedef RtemsMessageReqBroadcast_Context Context;
static const rtems_interval TIMEOUT_TICKS = 1;
static const rtems_event_set EVENT_RECEIVE = RTEMS_EVENT_17;
static const uint8_t message[ MAXIMUM_MESSAGE_SIZE ] =
  { 13, 42, 99, 222, 101 };
static const uint8_t queued_message[] = { 200, 201, 202 };

static void Receive( Context *ctx, size_t worker_index )
{
  ctx->receive_status[worker_index] = rtems_message_queue_receive(
    ctx->message_queue_id,
    ctx->receive_buffer[worker_index],
    &ctx->receive_size[worker_index],
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

static void CheckForMessage(
  Context *ctx,
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  T_rsc_success( status );
  T_eq_u32( message_size, ctx->size_param );
  T_eq_mem( message_buffer, message, ctx->size_param );
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
  PopMessage( ctx, CheckForQueuedMessage );
  PopMessage( ctx, CheckForQueuedMessage );
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

static void RtemsMessageReqBroadcast_Pre_SendBuffer_Prepare(
  RtemsMessageReqBroadcast_Context       *ctx,
  RtemsMessageReqBroadcast_Pre_SendBuffer state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Pre_SendBuffer_Valid: {
      /*
       * While the ``buffer`` parameter references a memory area where the
       * message to be sent is stored.
       */
      ctx->buffer_param = &message;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_SendBuffer_Null: {
      /*
       * While the ``buffer`` parameter is NULL.
       */
      ctx->buffer_param = NULL;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_SendBuffer_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Pre_Count_Prepare(
  RtemsMessageReqBroadcast_Context  *ctx,
  RtemsMessageReqBroadcast_Pre_Count state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Pre_Count_Valid: {
      /*
       * While the ``count`` parameter references an ``uint32_t`` object.
       */
      ctx->count_param = &ctx->count;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_Count_Null: {
      /*
       * While the ``count`` parameter is NULL.
       */
      ctx->count_param = NULL;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_Count_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Pre_Id_Prepare(
  RtemsMessageReqBroadcast_Context *ctx,
  RtemsMessageReqBroadcast_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->message_queue_id;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_Id_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Pre_MsgSize_Prepare(
  RtemsMessageReqBroadcast_Context    *ctx,
  RtemsMessageReqBroadcast_Pre_MsgSize state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Pre_MsgSize_Zero: {
      /*
       * While the ``size`` parameter is 0.
       */
      ctx->size_param = 0;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_MsgSize_SomeSize: {
      /*
       * While the ``size`` parameter has a value between 0 and the maximum
       * message size.
       */
      ctx->size_param = MAXIMUM_MESSAGE_SIZE / 2 + 1;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_MsgSize_MaxSize: {
      /*
       * While the ``size`` parameter has a value of the maximum message size.
       */
      ctx->size_param = MAXIMUM_MESSAGE_SIZE;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_MsgSize_TooLarge: {
      /*
       * While the ``size`` parameter has a value greater than the maximum
       * message size.
       */
      ctx->size_param = MAXIMUM_MESSAGE_SIZE + 1;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_MsgSize_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Pre_MsgQueue_Prepare(
  RtemsMessageReqBroadcast_Context     *ctx,
  RtemsMessageReqBroadcast_Pre_MsgQueue state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Pre_MsgQueue_Empty: {
      /*
       * While there is no message in the message queue.
       */
      /* Message queue is already empty. */
      ctx->check_msgq_unchanged = CheckForNoMessageInQueue;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_MsgQueue_Several: {
      /*
       * While there are messages in the message queue.
       */
      SendMsg( ctx );
      SendMsg( ctx );
      ctx->check_msgq_unchanged = CheckForSeveralMessagesInQueue;
      break;
    }

    case RtemsMessageReqBroadcast_Pre_MsgQueue_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Pre_Receivers_Prepare(
  RtemsMessageReqBroadcast_Context      *ctx,
  RtemsMessageReqBroadcast_Pre_Receivers state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Pre_Receivers_Waiting: {
      /*
       * While one or more receivers are waiting to receive a message.
       */
      size_t i;
      for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
        SendEvents( ctx->worker_id[i], EVENT_RECEIVE );
      }
      break;
    }

    case RtemsMessageReqBroadcast_Pre_Receivers_None: {
      /*
       * While no receiver is waiting to receive a message.
       */
      /* There is already no receiver waiting. */
      break;
    }

    case RtemsMessageReqBroadcast_Pre_Receivers_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Pre_Storage_Prepare(
  RtemsMessageReqBroadcast_Context    *ctx,
  RtemsMessageReqBroadcast_Pre_Storage state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Pre_Storage_Nop: {
      /*
       * While the memory area to which a pointer is provided as member
       * storage_area of type rtems_message_queue_config when the message queue
       * is constructed by rtems_message_queue_construct() is altered only by
       * the RTEMS operating system.
       */
      /* Only a requirement text. */
      break;
    }

    case RtemsMessageReqBroadcast_Pre_Storage_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Post_Status_Check(
  RtemsMessageReqBroadcast_Context    *ctx,
  RtemsMessageReqBroadcast_Post_Status state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Post_Status_Ok: {
      /*
       * The return status of rtems_message_queue_broadcast() shall be
       * RTEMS_SUCCESSFUL
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsMessageReqBroadcast_Post_Status_InvId: {
      /*
       * The return status of rtems_message_queue_broadcast() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsMessageReqBroadcast_Post_Status_InvAddr: {
      /*
       * The return status of rtems_message_queue_broadcast() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsMessageReqBroadcast_Post_Status_InvSize: {
      /*
       * The return status of rtems_message_queue_broadcast() shall be
       * RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsMessageReqBroadcast_Post_Status_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Post_Count_Check(
  RtemsMessageReqBroadcast_Context   *ctx,
  RtemsMessageReqBroadcast_Post_Count state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Post_Count_Zero: {
      /*
       * The value of the object referenced by the ``count`` parameter shall be
       * set to 0 after the return of the rtems_message_queue_broadcast() call.
       */
      T_eq_u32( ctx->count, 0 );
      break;
    }

    case RtemsMessageReqBroadcast_Post_Count_Set: {
      /*
       * The value of the object referenced by the ``count`` parameter shall be
       * set to the number of tasks unblocked (see unblock) by the call to
       * directive rtems_message_queue_broadcast() after the return of the
       * rtems_message_queue_broadcast() call.
       */
      T_eq_u32( ctx->count, NUMBER_OF_WORKERS );
      break;
    }

    case RtemsMessageReqBroadcast_Post_Count_Nop: {
      /*
       * The value of the object referenced by the ``count`` parameter in past
       * call to rtems_message_queue_broadcast() shall not be accessed by the
       * rtems_message_queue_broadcast() call (see also Nop).
       */
      T_eq_u32( ctx->count, UINT8_MAX );
      break;
    }

    case RtemsMessageReqBroadcast_Post_Count_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Post_MsgQueue_Check(
  RtemsMessageReqBroadcast_Context      *ctx,
  RtemsMessageReqBroadcast_Post_MsgQueue state
)
{
  switch ( state ) {
    case RtemsMessageReqBroadcast_Post_MsgQueue_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in the past call to
       * rtems_message_queue_broadcast() shall not be accessed by that call
       * (see also Nop).
       */
      ctx->check_msgq_unchanged( ctx );
      break;
    }

    case RtemsMessageReqBroadcast_Post_MsgQueue_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Post_Receivers_Check(
  RtemsMessageReqBroadcast_Context       *ctx,
  RtemsMessageReqBroadcast_Post_Receivers state
)
{
  size_t i;

  switch ( state ) {
    case RtemsMessageReqBroadcast_Post_Receivers_Unblocked: {
      /*
       * The call to the rtems_message_queue_broadcast() directive shall
       * unblock all receivers waiting for a message at the message queue.
       *
       * Note: Currently, rtems_message_queue_broadcast() unblocks receivers in
       * a none-atomic way. Meaning, it will not only unblock those receivers
       * it finds waiting at the queue when rtems_message_queue_broadcast() is
       * invoked but also any new receivers which start waiting for messages
       * after rtems_message_queue_broadcast() is invoked and before it
       * returns. This may lead to infinite unblocking loops.
       */
      for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
        T_rsc_success( ctx->receive_status[i] );
      }
      break;
    }

    case RtemsMessageReqBroadcast_Post_Receivers_Nop: {
      /*
       * The receivers waiting for a message at the message queue shall not be
       * affected by the call to the rtems_message_queue_broadcast() directive.
       */
      for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
        T_rsc( ctx->receive_status[i], RTEMS_TIMEOUT );
      }
      break;
    }

    case RtemsMessageReqBroadcast_Post_Receivers_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Post_RecSize_Check(
  RtemsMessageReqBroadcast_Context     *ctx,
  RtemsMessageReqBroadcast_Post_RecSize state
)
{
  size_t i;

  switch ( state ) {
    case RtemsMessageReqBroadcast_Post_RecSize_Message: {
      /*
       * The values of the objects referenced by the ``size`` parameter in all
       * calls to rtems_message_queue_receive() which are unblocked (see
       * unblock) by the rtems_message_queue_broadcast() call shall be set to
       * the same value as provided by parameter ``size`` of the
       * rtems_message_queue_broadcast() call after the return of the
       * rtems_message_queue_broadcast() call.
       */
      for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
        CheckForMessage(
          ctx,
          ctx->receive_status[i],
          ctx->receive_buffer[i],
          ctx->receive_size[i]
        );
      }
      break;
    }

    case RtemsMessageReqBroadcast_Post_RecSize_Nop: {
      /*
       * Objects referenced by the ``size`` parameter in past calls to
       * rtems_message_queue_receive() shall not be accessed by the
       * rtems_message_queue_broadcast() call (see also Nop).
       */
      for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
        T_eq_sz( ctx->receive_size[i], SIZE_MAX );
      }
      break;
    }

    case RtemsMessageReqBroadcast_Post_RecSize_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Post_RecBuffer_Check(
  RtemsMessageReqBroadcast_Context       *ctx,
  RtemsMessageReqBroadcast_Post_RecBuffer state
)
{
  size_t w, i;

  switch ( state ) {
    case RtemsMessageReqBroadcast_Post_RecBuffer_Message: {
      /*
       * Bytes 0 till ``size`` - 1 of the object referenced by the ``buffer``
       * parameter in all calls to rtems_message_queue_receive() which are
       * unblocked (see unblock) by the rtems_message_queue_broadcast() call
       * shall be set to the same values as bytes 0 till ``size`` - 1 of the
       * object referenced by parameter ``buffer`` of the
       * rtems_message_queue_broadcast() call after the return of the
       * rtems_message_queue_receive() call.
       */
      for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
        CheckForMessage(
          ctx,
          ctx->receive_status[i],
          ctx->receive_buffer[i],
          ctx->receive_size[i]
        );
      }
      break;
    }

    case RtemsMessageReqBroadcast_Post_RecBuffer_Nop: {
      /*
       * Objects referenced by the ``buffer`` parameter in past calls to
       * rtems_message_queue_receive() shall not be accessed by the
       * rtems_message_queue_broadcast() call (see also Nop).
       */
      for ( w = 0; w < NUMBER_OF_WORKERS; ++w ) {
        for ( i = 0; i < MAXIMUM_MESSAGE_SIZE; ++i ) {
          T_eq_u8( ctx->receive_buffer[w][i], UINT8_MAX );
        }
      }
      break;
    }

    case RtemsMessageReqBroadcast_Post_RecBuffer_NA:
      break;
  }
}

static void RtemsMessageReqBroadcast_Setup(
  RtemsMessageReqBroadcast_Context *ctx
)
{
  size_t i;
  SetSelfPriority( PRIO_NORMAL );

  for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
    ctx->worker_id[i] = CreateTask( "WORK", PRIO_HIGH );
    StartTask( ctx->worker_id[i], WorkerTask, ctx );
  }
}

static void RtemsMessageReqBroadcast_Setup_Wrap( void *arg )
{
  RtemsMessageReqBroadcast_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqBroadcast_Setup( ctx );
}

static void RtemsMessageReqBroadcast_Teardown(
  RtemsMessageReqBroadcast_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
    DeleteTask( ctx->worker_id[i] );
  }
  RestoreRunnerPriority();
}

static void RtemsMessageReqBroadcast_Teardown_Wrap( void *arg )
{
  RtemsMessageReqBroadcast_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqBroadcast_Teardown( ctx );
}

static void RtemsMessageReqBroadcast_Prepare(
  RtemsMessageReqBroadcast_Context *ctx
)
{
  rtems_status_code status;
  size_t i;

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
  for ( i = 0; i < NUMBER_OF_WORKERS; ++i ) {
      ctx->receive_size[i] = SIZE_MAX;
      memset( ctx->receive_buffer[i], UINT8_MAX, MAXIMUM_MESSAGE_SIZE );
  }
}

static void RtemsMessageReqBroadcast_Action(
  RtemsMessageReqBroadcast_Context *ctx
)
{
  ctx->status = rtems_message_queue_broadcast(
    ctx->id_param,
    ctx->buffer_param,
    ctx->size_param,
    ctx->count_param
  );

  FinalClockTick();
}

static void RtemsMessageReqBroadcast_Cleanup(
  RtemsMessageReqBroadcast_Context *ctx
)
{
  T_rsc_success( rtems_message_queue_delete( ctx->message_queue_id ) );
}

static const RtemsMessageReqBroadcast_Entry
RtemsMessageReqBroadcast_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqBroadcast_Post_Status_InvAddr,
    RtemsMessageReqBroadcast_Post_Count_Nop,
    RtemsMessageReqBroadcast_Post_MsgQueue_Nop,
    RtemsMessageReqBroadcast_Post_Receivers_NA,
    RtemsMessageReqBroadcast_Post_RecSize_NA,
    RtemsMessageReqBroadcast_Post_RecBuffer_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqBroadcast_Post_Status_NA,
    RtemsMessageReqBroadcast_Post_Count_NA,
    RtemsMessageReqBroadcast_Post_MsgQueue_NA,
    RtemsMessageReqBroadcast_Post_Receivers_NA,
    RtemsMessageReqBroadcast_Post_RecSize_NA,
    RtemsMessageReqBroadcast_Post_RecBuffer_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqBroadcast_Post_Status_InvAddr,
    RtemsMessageReqBroadcast_Post_Count_Nop,
    RtemsMessageReqBroadcast_Post_MsgQueue_Nop,
    RtemsMessageReqBroadcast_Post_Receivers_Nop,
    RtemsMessageReqBroadcast_Post_RecSize_Nop,
    RtemsMessageReqBroadcast_Post_RecBuffer_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqBroadcast_Post_Status_InvId,
    RtemsMessageReqBroadcast_Post_Count_Nop,
    RtemsMessageReqBroadcast_Post_MsgQueue_Nop,
    RtemsMessageReqBroadcast_Post_Receivers_NA,
    RtemsMessageReqBroadcast_Post_RecSize_NA,
    RtemsMessageReqBroadcast_Post_RecBuffer_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqBroadcast_Post_Status_Ok,
    RtemsMessageReqBroadcast_Post_Count_Zero,
    RtemsMessageReqBroadcast_Post_MsgQueue_Nop,
    RtemsMessageReqBroadcast_Post_Receivers_NA,
    RtemsMessageReqBroadcast_Post_RecSize_NA,
    RtemsMessageReqBroadcast_Post_RecBuffer_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqBroadcast_Post_Status_InvId,
    RtemsMessageReqBroadcast_Post_Count_Nop,
    RtemsMessageReqBroadcast_Post_MsgQueue_Nop,
    RtemsMessageReqBroadcast_Post_Receivers_Nop,
    RtemsMessageReqBroadcast_Post_RecSize_Nop,
    RtemsMessageReqBroadcast_Post_RecBuffer_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqBroadcast_Post_Status_Ok,
    RtemsMessageReqBroadcast_Post_Count_Set,
    RtemsMessageReqBroadcast_Post_MsgQueue_Nop,
    RtemsMessageReqBroadcast_Post_Receivers_Unblocked,
    RtemsMessageReqBroadcast_Post_RecSize_Message,
    RtemsMessageReqBroadcast_Post_RecBuffer_Message },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqBroadcast_Post_Status_InvSize,
    RtemsMessageReqBroadcast_Post_Count_Nop,
    RtemsMessageReqBroadcast_Post_MsgQueue_Nop,
    RtemsMessageReqBroadcast_Post_Receivers_NA,
    RtemsMessageReqBroadcast_Post_RecSize_NA,
    RtemsMessageReqBroadcast_Post_RecBuffer_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqBroadcast_Post_Status_InvSize,
    RtemsMessageReqBroadcast_Post_Count_Nop,
    RtemsMessageReqBroadcast_Post_MsgQueue_Nop,
    RtemsMessageReqBroadcast_Post_Receivers_Nop,
    RtemsMessageReqBroadcast_Post_RecSize_Nop,
    RtemsMessageReqBroadcast_Post_RecBuffer_Nop }
};

static const uint8_t
RtemsMessageReqBroadcast_Map[] = {
  6, 4, 1, 4, 6, 4, 1, 4, 6, 4, 1, 4, 8, 7, 1, 7, 5, 3, 1, 3, 5, 3, 1, 3, 5, 3,
  1, 3, 5, 3, 1, 3, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0,
  2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0,
  1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0,
  2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0
};

static size_t RtemsMessageReqBroadcast_Scope( void *arg, char *buf, size_t n )
{
  RtemsMessageReqBroadcast_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsMessageReqBroadcast_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsMessageReqBroadcast_Fixture = {
  .setup = RtemsMessageReqBroadcast_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMessageReqBroadcast_Teardown_Wrap,
  .scope = RtemsMessageReqBroadcast_Scope,
  .initial_context = &RtemsMessageReqBroadcast_Instance
};

static inline RtemsMessageReqBroadcast_Entry RtemsMessageReqBroadcast_PopEntry(
  RtemsMessageReqBroadcast_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMessageReqBroadcast_Entries[
    RtemsMessageReqBroadcast_Map[ index ]
  ];
}

static void RtemsMessageReqBroadcast_TestVariant(
  RtemsMessageReqBroadcast_Context *ctx
)
{
  RtemsMessageReqBroadcast_Pre_SendBuffer_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMessageReqBroadcast_Pre_Count_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsMessageReqBroadcast_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsMessageReqBroadcast_Pre_MsgSize_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsMessageReqBroadcast_Pre_MsgQueue_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsMessageReqBroadcast_Pre_Receivers_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsMessageReqBroadcast_Pre_Storage_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsMessageReqBroadcast_Action( ctx );
  RtemsMessageReqBroadcast_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsMessageReqBroadcast_Post_Count_Check( ctx, ctx->Map.entry.Post_Count );
  RtemsMessageReqBroadcast_Post_MsgQueue_Check(
    ctx,
    ctx->Map.entry.Post_MsgQueue
  );
  RtemsMessageReqBroadcast_Post_Receivers_Check(
    ctx,
    ctx->Map.entry.Post_Receivers
  );
  RtemsMessageReqBroadcast_Post_RecSize_Check(
    ctx,
    ctx->Map.entry.Post_RecSize
  );
  RtemsMessageReqBroadcast_Post_RecBuffer_Check(
    ctx,
    ctx->Map.entry.Post_RecBuffer
  );
}

/**
 * @fn void T_case_body_RtemsMessageReqBroadcast( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMessageReqBroadcast,
  &RtemsMessageReqBroadcast_Fixture
)
{
  RtemsMessageReqBroadcast_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsMessageReqBroadcast_Pre_SendBuffer_Valid;
    ctx->Map.pcs[ 0 ] < RtemsMessageReqBroadcast_Pre_SendBuffer_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsMessageReqBroadcast_Pre_Count_Valid;
      ctx->Map.pcs[ 1 ] < RtemsMessageReqBroadcast_Pre_Count_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsMessageReqBroadcast_Pre_Id_Valid;
        ctx->Map.pcs[ 2 ] < RtemsMessageReqBroadcast_Pre_Id_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsMessageReqBroadcast_Pre_MsgSize_Zero;
          ctx->Map.pcs[ 3 ] < RtemsMessageReqBroadcast_Pre_MsgSize_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsMessageReqBroadcast_Pre_MsgQueue_Empty;
            ctx->Map.pcs[ 4 ] < RtemsMessageReqBroadcast_Pre_MsgQueue_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsMessageReqBroadcast_Pre_Receivers_Waiting;
              ctx->Map.pcs[ 5 ] < RtemsMessageReqBroadcast_Pre_Receivers_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = RtemsMessageReqBroadcast_Pre_Storage_Nop;
                ctx->Map.pcs[ 6 ] < RtemsMessageReqBroadcast_Pre_Storage_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                ctx->Map.entry = RtemsMessageReqBroadcast_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                RtemsMessageReqBroadcast_Prepare( ctx );
                RtemsMessageReqBroadcast_TestVariant( ctx );
                RtemsMessageReqBroadcast_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
