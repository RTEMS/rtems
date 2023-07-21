/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMessageReqUrgentSend
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
 * @defgroup RtemsMessageReqUrgentSend spec:/rtems/message/req/urgent-send
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsMessageReqUrgentSend_Pre_Buffer_Valid,
  RtemsMessageReqUrgentSend_Pre_Buffer_Null,
  RtemsMessageReqUrgentSend_Pre_Buffer_NA
} RtemsMessageReqUrgentSend_Pre_Buffer;

typedef enum {
  RtemsMessageReqUrgentSend_Pre_Id_Valid,
  RtemsMessageReqUrgentSend_Pre_Id_Invalid,
  RtemsMessageReqUrgentSend_Pre_Id_NA
} RtemsMessageReqUrgentSend_Pre_Id;

typedef enum {
  RtemsMessageReqUrgentSend_Pre_Size_Zero,
  RtemsMessageReqUrgentSend_Pre_Size_SomeSize,
  RtemsMessageReqUrgentSend_Pre_Size_MaxSize,
  RtemsMessageReqUrgentSend_Pre_Size_TooLarge,
  RtemsMessageReqUrgentSend_Pre_Size_NA
} RtemsMessageReqUrgentSend_Pre_Size;

typedef enum {
  RtemsMessageReqUrgentSend_Pre_MsgQueue_Empty,
  RtemsMessageReqUrgentSend_Pre_MsgQueue_One,
  RtemsMessageReqUrgentSend_Pre_MsgQueue_Several,
  RtemsMessageReqUrgentSend_Pre_MsgQueue_Full,
  RtemsMessageReqUrgentSend_Pre_MsgQueue_NA
} RtemsMessageReqUrgentSend_Pre_MsgQueue;

typedef enum {
  RtemsMessageReqUrgentSend_Pre_Receiver_Waiting,
  RtemsMessageReqUrgentSend_Pre_Receiver_No,
  RtemsMessageReqUrgentSend_Pre_Receiver_NA
} RtemsMessageReqUrgentSend_Pre_Receiver;

typedef enum {
  RtemsMessageReqUrgentSend_Pre_Directive_Send,
  RtemsMessageReqUrgentSend_Pre_Directive_Urgent,
  RtemsMessageReqUrgentSend_Pre_Directive_NA
} RtemsMessageReqUrgentSend_Pre_Directive;

typedef enum {
  RtemsMessageReqUrgentSend_Pre_Storage_Nop,
  RtemsMessageReqUrgentSend_Pre_Storage_NA
} RtemsMessageReqUrgentSend_Pre_Storage;

typedef enum {
  RtemsMessageReqUrgentSend_Post_Status_Ok,
  RtemsMessageReqUrgentSend_Post_Status_InvId,
  RtemsMessageReqUrgentSend_Post_Status_InvAddr,
  RtemsMessageReqUrgentSend_Post_Status_InvSize,
  RtemsMessageReqUrgentSend_Post_Status_TooMany,
  RtemsMessageReqUrgentSend_Post_Status_NA
} RtemsMessageReqUrgentSend_Post_Status;

typedef enum {
  RtemsMessageReqUrgentSend_Post_MsgQueue_Empty,
  RtemsMessageReqUrgentSend_Post_MsgQueue_One,
  RtemsMessageReqUrgentSend_Post_MsgQueue_Prepend,
  RtemsMessageReqUrgentSend_Post_MsgQueue_Append,
  RtemsMessageReqUrgentSend_Post_MsgQueue_Nop,
  RtemsMessageReqUrgentSend_Post_MsgQueue_NA
} RtemsMessageReqUrgentSend_Post_MsgQueue;

typedef enum {
  RtemsMessageReqUrgentSend_Post_Receiver_GotMsg,
  RtemsMessageReqUrgentSend_Post_Receiver_Waiting,
  RtemsMessageReqUrgentSend_Post_Receiver_NA
} RtemsMessageReqUrgentSend_Post_Receiver;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Buffer_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Size_NA : 1;
  uint16_t Pre_MsgQueue_NA : 1;
  uint16_t Pre_Receiver_NA : 1;
  uint16_t Pre_Directive_NA : 1;
  uint16_t Pre_Storage_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_MsgQueue : 3;
  uint16_t Post_Receiver : 2;
} RtemsMessageReqUrgentSend_Entry;

#define MAXIMUM_PENDING_MESSAGES 3
#define MAXIMUM_MESSAGE_SIZE 5

/**
 * @brief Test context for spec:/rtems/message/req/urgent-send test case.
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
   * @brief This member contains the returned status code of the receiver.
   */
  rtems_status_code receive_status;

  /**
   * @brief This member indicates whether the a receiver task should be started
   *   to receive a message.
   */
  bool is_receiver_waiting;

  /**
   * @brief This member contains the message to be sent by the action.
   */
  uint8_t send_message[ MAXIMUM_MESSAGE_SIZE ];

  /**
   * @brief This member specifies the directive to be called as action.
   *
   * This is either rtems_message_queue_send() or rtems_message_queue_urgent().
   */
  rtems_status_code (*action)( rtems_id id, const void *buffer, size_t size );

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
  size_t size_param;

  /**
   * @brief This member contains the returned status code of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains the task identifier of the worker task.
   */
  rtems_id worker_id;

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
    RtemsMessageReqUrgentSend_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMessageReqUrgentSend_Context;

static RtemsMessageReqUrgentSend_Context
  RtemsMessageReqUrgentSend_Instance;

static const char * const RtemsMessageReqUrgentSend_PreDesc_Buffer[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqUrgentSend_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsMessageReqUrgentSend_PreDesc_Size[] = {
  "Zero",
  "SomeSize",
  "MaxSize",
  "TooLarge",
  "NA"
};

static const char * const RtemsMessageReqUrgentSend_PreDesc_MsgQueue[] = {
  "Empty",
  "One",
  "Several",
  "Full",
  "NA"
};

static const char * const RtemsMessageReqUrgentSend_PreDesc_Receiver[] = {
  "Waiting",
  "No",
  "NA"
};

static const char * const RtemsMessageReqUrgentSend_PreDesc_Directive[] = {
  "Send",
  "Urgent",
  "NA"
};

static const char * const RtemsMessageReqUrgentSend_PreDesc_Storage[] = {
  "Nop",
  "NA"
};

static const char * const * const RtemsMessageReqUrgentSend_PreDesc[] = {
  RtemsMessageReqUrgentSend_PreDesc_Buffer,
  RtemsMessageReqUrgentSend_PreDesc_Id,
  RtemsMessageReqUrgentSend_PreDesc_Size,
  RtemsMessageReqUrgentSend_PreDesc_MsgQueue,
  RtemsMessageReqUrgentSend_PreDesc_Receiver,
  RtemsMessageReqUrgentSend_PreDesc_Directive,
  RtemsMessageReqUrgentSend_PreDesc_Storage,
  NULL
};

typedef RtemsMessageReqUrgentSend_Context Context;
static const uint32_t MAGIC = 0xA66FE31; /* an arbitrary number */
static const rtems_interval TIMEOUT_TICKS = 1;
static const rtems_event_set EVENT_RECEIVE = RTEMS_EVENT_17;

static void Receive( Context *ctx )
{
  ctx->receive_status = rtems_message_queue_receive(
    ctx->message_queue_id,
    ctx->receive_buffer,
    &ctx->receive_size,
    RTEMS_WAIT,
    TIMEOUT_TICKS
  );
}

static void WorkerTask( rtems_task_argument argument )
{
  Context *ctx = (Context *) argument;

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_RECEIVE ) != 0 ) {
      Receive( ctx );
    }
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
  T_rsc( status, RTEMS_UNSATISFIED  );
}

static void CheckForFirstMessage(
  Context *ctx,
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  (void) ctx;
  T_rsc_success( status );
  T_eq_u32( message_size, 1 );
  T_eq_u8( message_buffer[0], 0 );
}

static void CheckForSecondMessage(
  Context *ctx,
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  (void) ctx;
  T_rsc_success( status );
  T_eq_u32( message_size, 3 );
  T_eq_u8( message_buffer[0], 1 );
  T_eq_u8( message_buffer[1], 1 );
  T_eq_u8( message_buffer[2], 1 );
}

static void CheckForThirdMessage(
  Context *ctx,
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  (void) ctx;
  T_rsc_success( status );
  T_eq_u32( message_size, 5 );
  T_eq_u8( message_buffer[0], 2 );
  T_eq_u8( message_buffer[1], 2 );
  T_eq_u8( message_buffer[2], 2 );
  T_eq_u8( message_buffer[3], 2 );
  T_eq_u8( message_buffer[4], 2 );
}

static void CheckForSendMessage(
  Context *ctx,
  rtems_status_code status,
  uint8_t *message_buffer,
  size_t message_size
)
{
  size_t i;
  T_rsc_success( status );
  T_eq_u32( message_size, ctx->size_param );
  for ( i = 0; i < ctx->size_param; ++i ) {
    T_eq_u8( message_buffer[i], ctx->send_message[i] );
  }
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
{}

static void CheckForOneMessageInQueue( void *ctx_in )
{
  Context *ctx = ctx_in;
  T_assert_eq_u32( ctx->magic, MAGIC ); /* Run-time type check */
  PopMessage( ctx, CheckForFirstMessage );
}

static void CheckForSeveralMessagesInQueue( void *ctx_in )
{
  Context *ctx = ctx_in;
  T_assert_eq_u32( ctx->magic, MAGIC ); /* Run-time type check */
  PopMessage( ctx, CheckForFirstMessage );
  PopMessage( ctx, CheckForSecondMessage );
}

static void CheckForAllMessagesInQueue( void *ctx_in )
{
  Context *ctx = ctx_in;
  T_assert_eq_u32( ctx->magic, MAGIC ); /* Run-time type check */
  PopMessage( ctx, CheckForFirstMessage );
  PopMessage( ctx, CheckForSecondMessage );
  PopMessage( ctx, CheckForThirdMessage );
}

static void SendMsg( Context *ctx )
{
  rtems_status_code status;
  uint8_t msg[ MAXIMUM_MESSAGE_SIZE ];

  memset( msg, ctx->send_msg_counter, MAXIMUM_MESSAGE_SIZE );
  status = rtems_message_queue_send(
    ctx->message_queue_id,
    msg,
    ( ctx->send_msg_counter * 2 ) % MAXIMUM_MESSAGE_SIZE + 1
  );
  T_rsc_success( status );
  ++ctx->send_msg_counter;
}

static void RtemsMessageReqUrgentSend_Pre_Buffer_Prepare(
  RtemsMessageReqUrgentSend_Context   *ctx,
  RtemsMessageReqUrgentSend_Pre_Buffer state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Pre_Buffer_Valid: {
      /*
       * While the ``buffer`` parameter references a memory area where the
       * message to be sent is stored.
       */
      uint8_t i;
      for ( i = 0; i < MAXIMUM_MESSAGE_SIZE; ++i ) {
        ctx->send_message[i] = 42 + i;
      }
      ctx->buffer_param = &ctx->send_message;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Buffer_Null: {
      /*
       * While the ``buffer`` parameter is NULL.
       */
      ctx->buffer_param = NULL;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Buffer_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Pre_Id_Prepare(
  RtemsMessageReqUrgentSend_Context *ctx,
  RtemsMessageReqUrgentSend_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->message_queue_id;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Id_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Pre_Size_Prepare(
  RtemsMessageReqUrgentSend_Context *ctx,
  RtemsMessageReqUrgentSend_Pre_Size state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Pre_Size_Zero: {
      /*
       * While the ``size`` parameter is 0.
       */
      ctx->size_param = 0;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Size_SomeSize: {
      /*
       * While the ``size`` parameter has a value between 0 and the maximum
       * message size.
       */
      ctx->size_param = MAXIMUM_MESSAGE_SIZE / 2 + 1;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Size_MaxSize: {
      /*
       * While the ``size`` parameter has a value of the maximum message size.
       */
      ctx->size_param = MAXIMUM_MESSAGE_SIZE;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Size_TooLarge: {
      /*
       * While the ``size`` parameter has a value greater than the maximum
       * message size.
       */
      ctx->size_param = MAXIMUM_MESSAGE_SIZE + 1;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Size_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Pre_MsgQueue_Prepare(
  RtemsMessageReqUrgentSend_Context     *ctx,
  RtemsMessageReqUrgentSend_Pre_MsgQueue state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Pre_MsgQueue_Empty: {
      /*
       * While there is no message in the message queue.
       */
      /* Message queue is already empty. */
      ctx->check_msgq_unchanged = CheckForNoMessageInQueue;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_MsgQueue_One: {
      /*
       * While there is exactly one message in the message queue.
       */
      SendMsg( ctx );
      ctx->check_msgq_unchanged = CheckForOneMessageInQueue;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_MsgQueue_Several: {
      /*
       * While there are more than one and less than maximum pending messages
       * in the message queue.
       */
      SendMsg( ctx );
      SendMsg( ctx );
      ctx->check_msgq_unchanged = CheckForSeveralMessagesInQueue;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_MsgQueue_Full: {
      /*
       * While there are maximum pending messages in the message queue.
       */
      SendMsg( ctx );
      SendMsg( ctx );
      SendMsg( ctx );
      ctx->check_msgq_unchanged = CheckForAllMessagesInQueue;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_MsgQueue_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Pre_Receiver_Prepare(
  RtemsMessageReqUrgentSend_Context     *ctx,
  RtemsMessageReqUrgentSend_Pre_Receiver state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Pre_Receiver_Waiting: {
      /*
       * While a receiver is waiting to receive a message.
       */
      ctx->is_receiver_waiting = true;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Receiver_No: {
      /*
       * While no receiver is waiting to receive a message.
       */
      ctx->is_receiver_waiting = false;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Receiver_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Pre_Directive_Prepare(
  RtemsMessageReqUrgentSend_Context      *ctx,
  RtemsMessageReqUrgentSend_Pre_Directive state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Pre_Directive_Send: {
      /*
       * While the directive rtems_message_queue_send() is called.
       */
      ctx->action = rtems_message_queue_send;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Directive_Urgent: {
      /*
       * While the directive rtems_message_queue_urgent() is called.
       */
      ctx->action = rtems_message_queue_urgent;
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Directive_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Pre_Storage_Prepare(
  RtemsMessageReqUrgentSend_Context    *ctx,
  RtemsMessageReqUrgentSend_Pre_Storage state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Pre_Storage_Nop: {
      /*
       * While the memory area to which a pointer is provided as member
       * storage_area of type rtems_message_queue_config when the message queue
       * is constructed by rtems_message_queue_construct() is altered only by
       * the RTEMS operating system.
       */
      /* Only a requirement text. */
      break;
    }

    case RtemsMessageReqUrgentSend_Pre_Storage_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Post_Status_Check(
  RtemsMessageReqUrgentSend_Context    *ctx,
  RtemsMessageReqUrgentSend_Post_Status state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Post_Status_Ok: {
      /*
       * The return status of the called directive (rtems_message_queue_send()
       * or rtems_message_queue_urgent()) shall be RTEMS_SUCCESSFUL
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_Status_InvId: {
      /*
       * The return status of the called directive (rtems_message_queue_send()
       * or rtems_message_queue_urgent()) shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_Status_InvAddr: {
      /*
       * The return status of the called directive (rtems_message_queue_send()
       * or rtems_message_queue_urgent()) shall be RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_Status_InvSize: {
      /*
       * The return status of the called directive (rtems_message_queue_send()
       * or rtems_message_queue_urgent()) shall be RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_Status_TooMany: {
      /*
       * The return status of the called directive (rtems_message_queue_send()
       * or rtems_message_queue_urgent()) shall be RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_Status_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Post_MsgQueue_Check(
  RtemsMessageReqUrgentSend_Context      *ctx,
  RtemsMessageReqUrgentSend_Post_MsgQueue state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Post_MsgQueue_Empty: {
      /*
       * The message queue shall be empty after the return of the
       * rtems_message_queue_send() or rtems_message_queue_urgent() call.
       */
      PopMessage( ctx, CheckForNoMessage );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_MsgQueue_One: {
      /*
       * The message queue shall contain only the send message after the return
       * of the rtems_message_queue_send() or rtems_message_queue_urgent()
       * call.
       */
      PopMessage( ctx, CheckForSendMessage );
      PopMessage( ctx, CheckForNoMessage );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_MsgQueue_Prepend: {
      /*
       * The message queue shall contain the message send by the last call to
       * rtems_message_queue_urgent() as first message followed by all the
       * messages which were in the message queue before that call (in the same
       * order and each message with the same content and size).
       */
      PopMessage( ctx, CheckForSendMessage );
      ctx->check_msgq_unchanged( ctx );
      PopMessage( ctx, CheckForNoMessage );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_MsgQueue_Append: {
      /*
       * The message queue shall contain the message send by the last call to
       * rtems_message_queue_send() as last message preceded by all the
       * messages which were in the message queue before that call (in the same
       * order and each message with the same content and size).
       */
      ctx->check_msgq_unchanged( ctx );
      PopMessage( ctx, CheckForSendMessage );
      PopMessage( ctx, CheckForNoMessage );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_MsgQueue_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past call to
       * rtems_message_queue_send() or rtems_message_queue_urgent() shall not
       * be accessed by that call (see also Nop).
       */
      ctx->check_msgq_unchanged( ctx );
      PopMessage( ctx, CheckForNoMessage );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_MsgQueue_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Post_Receiver_Check(
  RtemsMessageReqUrgentSend_Context      *ctx,
  RtemsMessageReqUrgentSend_Post_Receiver state
)
{
  switch ( state ) {
    case RtemsMessageReqUrgentSend_Post_Receiver_GotMsg: {
      /*
       * The receiver shall receive the message send by the last call to the
       * rtems_message_queue_send() or rtems_message_queue_urgent() directive.
       */
      CheckForSendMessage(
        ctx,
        ctx->receive_status,
        ctx->receive_buffer,
        ctx->receive_size
      );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_Receiver_Waiting: {
      /*
       * The receiver shall still wait to receive a message after the last call
       * to the rtems_message_queue_send() or rtems_message_queue_urgent()
       * directive.
       */
      T_rsc( ctx->receive_status, RTEMS_TIMEOUT );
      break;
    }

    case RtemsMessageReqUrgentSend_Post_Receiver_NA:
      break;
  }
}

static void RtemsMessageReqUrgentSend_Setup(
  RtemsMessageReqUrgentSend_Context *ctx
)
{
  ctx->magic   = MAGIC;

  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, WorkerTask, ctx );
}

static void RtemsMessageReqUrgentSend_Setup_Wrap( void *arg )
{
  RtemsMessageReqUrgentSend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqUrgentSend_Setup( ctx );
}

static void RtemsMessageReqUrgentSend_Teardown(
  RtemsMessageReqUrgentSend_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void RtemsMessageReqUrgentSend_Teardown_Wrap( void *arg )
{
  RtemsMessageReqUrgentSend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqUrgentSend_Teardown( ctx );
}

static void RtemsMessageReqUrgentSend_Prepare(
  RtemsMessageReqUrgentSend_Context *ctx
)
{
  rtems_status_code status;

  ctx->send_msg_counter = 0;

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
}

static void RtemsMessageReqUrgentSend_Action(
  RtemsMessageReqUrgentSend_Context *ctx
)
{
  if ( ctx->is_receiver_waiting ) {
    SendEvents( ctx->worker_id, EVENT_RECEIVE );
  }

  ctx->status = (ctx->action)(
    ctx->id_param,
    ctx->buffer_param,
    ctx->size_param
  );

  if ( ctx->is_receiver_waiting ) {
    FinalClockTick();
  }
}

static void RtemsMessageReqUrgentSend_Cleanup(
  RtemsMessageReqUrgentSend_Context *ctx
)
{
  T_rsc_success( rtems_message_queue_delete( ctx->message_queue_id ) );
}

static const RtemsMessageReqUrgentSend_Entry
RtemsMessageReqUrgentSend_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_NA,
    RtemsMessageReqUrgentSend_Post_MsgQueue_NA,
    RtemsMessageReqUrgentSend_Post_Receiver_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_InvAddr,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Nop,
    RtemsMessageReqUrgentSend_Post_Receiver_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_InvId,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Nop,
    RtemsMessageReqUrgentSend_Post_Receiver_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_InvAddr,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Nop,
    RtemsMessageReqUrgentSend_Post_Receiver_Waiting },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_InvSize,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Nop,
    RtemsMessageReqUrgentSend_Post_Receiver_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_InvId,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Nop,
    RtemsMessageReqUrgentSend_Post_Receiver_Waiting },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_Ok,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Empty,
    RtemsMessageReqUrgentSend_Post_Receiver_GotMsg },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_Ok,
    RtemsMessageReqUrgentSend_Post_MsgQueue_One,
    RtemsMessageReqUrgentSend_Post_Receiver_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_Ok,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Append,
    RtemsMessageReqUrgentSend_Post_Receiver_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_Ok,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Prepend,
    RtemsMessageReqUrgentSend_Post_Receiver_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_TooMany,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Nop,
    RtemsMessageReqUrgentSend_Post_Receiver_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqUrgentSend_Post_Status_InvSize,
    RtemsMessageReqUrgentSend_Post_MsgQueue_Nop,
    RtemsMessageReqUrgentSend_Post_Receiver_Waiting }
};

static const uint8_t
RtemsMessageReqUrgentSend_Map[] = {
  6, 6, 7, 7, 0, 0, 8, 9, 0, 0, 8, 9, 0, 0, 10, 10, 6, 6, 7, 7, 0, 0, 8, 9, 0,
  0, 8, 9, 0, 0, 10, 10, 6, 6, 7, 7, 0, 0, 8, 9, 0, 0, 8, 9, 0, 0, 10, 10, 11,
  11, 4, 4, 0, 0, 4, 4, 0, 0, 4, 4, 0, 0, 4, 4, 5, 5, 2, 2, 0, 0, 2, 2, 0, 0,
  2, 2, 0, 0, 2, 2, 5, 5, 2, 2, 0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 2, 2, 5, 5, 2, 2,
  0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 2, 2, 5, 5, 2, 2, 0, 0, 2, 2, 0, 0, 2, 2, 0, 0,
  2, 2, 3, 3, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 3, 1, 1, 0, 0, 1, 1,
  0, 0, 1, 1, 0, 0, 1, 1, 3, 3, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 3,
  1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 3, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
  0, 0, 1, 1, 3, 3, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 3, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 3, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1
};

static size_t RtemsMessageReqUrgentSend_Scope( void *arg, char *buf, size_t n )
{
  RtemsMessageReqUrgentSend_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsMessageReqUrgentSend_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsMessageReqUrgentSend_Fixture = {
  .setup = RtemsMessageReqUrgentSend_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMessageReqUrgentSend_Teardown_Wrap,
  .scope = RtemsMessageReqUrgentSend_Scope,
  .initial_context = &RtemsMessageReqUrgentSend_Instance
};

static inline RtemsMessageReqUrgentSend_Entry
RtemsMessageReqUrgentSend_PopEntry( RtemsMessageReqUrgentSend_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMessageReqUrgentSend_Entries[
    RtemsMessageReqUrgentSend_Map[ index ]
  ];
}

static void RtemsMessageReqUrgentSend_TestVariant(
  RtemsMessageReqUrgentSend_Context *ctx
)
{
  RtemsMessageReqUrgentSend_Pre_Buffer_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMessageReqUrgentSend_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsMessageReqUrgentSend_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsMessageReqUrgentSend_Pre_MsgQueue_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsMessageReqUrgentSend_Pre_Receiver_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsMessageReqUrgentSend_Pre_Directive_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsMessageReqUrgentSend_Pre_Storage_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsMessageReqUrgentSend_Action( ctx );
  RtemsMessageReqUrgentSend_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsMessageReqUrgentSend_Post_MsgQueue_Check(
    ctx,
    ctx->Map.entry.Post_MsgQueue
  );
  RtemsMessageReqUrgentSend_Post_Receiver_Check(
    ctx,
    ctx->Map.entry.Post_Receiver
  );
}

/**
 * @fn void T_case_body_RtemsMessageReqUrgentSend( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMessageReqUrgentSend,
  &RtemsMessageReqUrgentSend_Fixture
)
{
  RtemsMessageReqUrgentSend_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsMessageReqUrgentSend_Pre_Buffer_Valid;
    ctx->Map.pcs[ 0 ] < RtemsMessageReqUrgentSend_Pre_Buffer_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsMessageReqUrgentSend_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsMessageReqUrgentSend_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsMessageReqUrgentSend_Pre_Size_Zero;
        ctx->Map.pcs[ 2 ] < RtemsMessageReqUrgentSend_Pre_Size_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsMessageReqUrgentSend_Pre_MsgQueue_Empty;
          ctx->Map.pcs[ 3 ] < RtemsMessageReqUrgentSend_Pre_MsgQueue_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsMessageReqUrgentSend_Pre_Receiver_Waiting;
            ctx->Map.pcs[ 4 ] < RtemsMessageReqUrgentSend_Pre_Receiver_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsMessageReqUrgentSend_Pre_Directive_Send;
              ctx->Map.pcs[ 5 ] < RtemsMessageReqUrgentSend_Pre_Directive_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = RtemsMessageReqUrgentSend_Pre_Storage_Nop;
                ctx->Map.pcs[ 6 ] < RtemsMessageReqUrgentSend_Pre_Storage_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                ctx->Map.entry = RtemsMessageReqUrgentSend_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                RtemsMessageReqUrgentSend_Prepare( ctx );
                RtemsMessageReqUrgentSend_TestVariant( ctx );
                RtemsMessageReqUrgentSend_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
