/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreMsgqUnitMsgq
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
#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/coremsgimpl.h>

#include "../validation/tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreMsgqUnitMsgq spec:/score/msgq/unit/msgq
 *
 * @ingroup TestsuitesUnitNoClock0
 *
 * @brief Unit tests for the Message Queue Handler.
 *
 * Parts of the files ``cpukit/score/src/coremsginsert.c``,
 * ``cpukit/score/src/coremsgseize.c``, and
 * ``cpukit/score/src/coremsgsubmit.c`` are only executed by the POSIX API.
 * Currently, the pre-qualified subset of RTEMS does not contain the POSIX API.
 * This test exercises the code parts otherwise only reached by the POSIX API
 * to achieve full code coverage.
 *
 * This test case performs the following actions:
 *
 * - Use _CORE_message_queue_Insert_message() to insert two messages into a
 *   message queue and use the POSIX message priority to define their order in
 *   the queue.
 *
 *   - Check that _CORE_message_queue_Submit() was executed successfully.
 *
 *   - Check that the messages are in the right order in the message queue.
 *
 * - Submit three messages into a message queue which can only store two and
 *   have the third submit() blocked till a seize() occurs.
 *
 *   - Check that the third _CORE_message_queue_Submit() did actually block
 *     till there was room for the message in the message queue.
 *
 * - Submit messages in the queue from within an ISR.
 *
 *   - Check that the first two messages were successfully send.
 *
 *   - Check that trying to send the third message from ISR when the message
 *     queue was full was rejected.
 *
 * @{
 */

#define MAXIMUM_PENDING_MESSAGES 2
#define MAXIMUM_MESSAGE_SIZE 3

static void WorkerTask( rtems_task_argument argument );

/**
 * @brief Test context for spec:/score/msgq/unit/msgq test case.
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
   * @brief This member contains the task identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member indicated whether the worker task is currently sending
   *   a message (``true``) or whether it is waiting to receive an event
   *   (``false``).
   */
  bool is_worker_working;

  /**
   * @brief This member contains the returned status code of the SendMessage()
   *   function.
   */
  rtems_status_code send_status;
} ScoreMsgqUnitMsgq_Context;

static ScoreMsgqUnitMsgq_Context
  ScoreMsgqUnitMsgq_Instance;

#define EVENT_SEND RTEMS_EVENT_17
#define MESSAGE_CONTENT_LOW { 1, 2, 3 }
#define MESSAGE_CONTENT_HIGH { 4, 5 }
#define MESSAGE_PRIORITY_LOW 5
#define MESSAGE_PRIORITY_HIGH 7
#define DO_WAIT true

typedef ScoreMsgqUnitMsgq_Context Context;

/*
 * This is a code fragment from rtems_message_queue_send() with the
 * specialty that it uses a POSIX priority and the sender
 * task will wait in case the queue is full.
 */
static rtems_status_code SubmitMessage(
  rtems_id id,
  uint8_t *message,
  size_t message_size,
  unsigned int posix_piority
)
{
  rtems_status_code status;
  Thread_queue_Context queue_context;
  Message_queue_Control *the_message_queue;

  T_assert_lt_uint( posix_piority, MQ_PRIO_MAX );

  the_message_queue = _Message_queue_Get(
    id,
    &queue_context
  );
  T_assert_not_null( the_message_queue );

  /* The next two calls are from _POSIX_Message_queue_Send_support() */
  _Thread_queue_Context_set_enqueue_callout(
    &queue_context,
    _Thread_queue_Enqueue_do_nothing_extra
  );
  _Thread_queue_Context_set_timeout_argument( &queue_context, NULL, true );

  _CORE_message_queue_Acquire_critical(
    &the_message_queue->message_queue,
    &queue_context
  );

  status = _CORE_message_queue_Submit(
    &the_message_queue->message_queue,
    _Thread_Executing,
    message,
    message_size,
    (CORE_message_queue_Submit_types) ( posix_piority * -1 ),
    DO_WAIT,
    &queue_context
  );

  return _Status_Get( status );
}

static rtems_status_code ReceiveMessage(
  rtems_id id,
  void *buffer,
  size_t *size
)
{
  return rtems_message_queue_receive(
    id,
    buffer,
    size,
    RTEMS_LOCAL | RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );
}

static rtems_status_code ReceiveOneMessages( Context *ctx )
{
  uint8_t message_buffer[ MAXIMUM_MESSAGE_SIZE ];
  size_t message_size;

  return ReceiveMessage(
    ctx->message_queue_id,
    &message_buffer,
    &message_size
  );
}

static void SendMessage( Context *ctx )
{
  uint8_t message[] = { 100, 101, 102 };
  ctx->send_status = SubmitMessage(
    ctx->message_queue_id,
    message,
    sizeof( message ),
    MESSAGE_PRIORITY_LOW
  );
}

static void WorkerTask( rtems_task_argument argument )
{
  Context *ctx = (Context *) argument;

  while ( true ) {
    ctx->is_worker_working = false;
    ReceiveAnyEvents();
    ctx->is_worker_working = true;
    SendMessage( ctx );
    T_assert_rsc_success( ctx->send_status );
  }
}

static void WorkerSendMessage( Context *ctx )
{
  SendEvents( ctx->worker_id, EVENT_SEND );
}

static void ScoreMsgqUnitMsgq_Setup( ScoreMsgqUnitMsgq_Context *ctx )
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

  SetSelfPriority( PRIO_NORMAL );

  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, WorkerTask, ctx );
}

static void ScoreMsgqUnitMsgq_Setup_Wrap( void *arg )
{
  ScoreMsgqUnitMsgq_Context *ctx;

  ctx = arg;
  ScoreMsgqUnitMsgq_Setup( ctx );
}

static void ScoreMsgqUnitMsgq_Teardown( ScoreMsgqUnitMsgq_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
  T_rsc_success( rtems_message_queue_delete( ctx->message_queue_id ) );
}

static void ScoreMsgqUnitMsgq_Teardown_Wrap( void *arg )
{
  ScoreMsgqUnitMsgq_Context *ctx;

  ctx = arg;
  ScoreMsgqUnitMsgq_Teardown( ctx );
}

static T_fixture ScoreMsgqUnitMsgq_Fixture = {
  .setup = ScoreMsgqUnitMsgq_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreMsgqUnitMsgq_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &ScoreMsgqUnitMsgq_Instance
};

/**
 * @brief Use _CORE_message_queue_Insert_message() to insert two messages into
 *   a message queue and use the POSIX message priority to define their order
 *   in the queue.
 */
static void ScoreMsgqUnitMsgq_Action_0( ScoreMsgqUnitMsgq_Context *ctx )
{
  rtems_status_code status_submit_low;
  rtems_status_code status_submit_high;
  rtems_status_code status_receive_low;
  rtems_status_code status_receive_high;
  uint8_t message_low[] = MESSAGE_CONTENT_LOW;
  uint8_t message_high[] = MESSAGE_CONTENT_HIGH;
  uint8_t message_buffer_low[ MAXIMUM_MESSAGE_SIZE ];
  uint8_t message_buffer_high[ MAXIMUM_MESSAGE_SIZE ];
  size_t message_size_low;
  size_t message_size_high;

  status_submit_low = SubmitMessage(
    ctx->message_queue_id,
    message_low,
    sizeof( message_low ),
    MESSAGE_PRIORITY_LOW
  );

  status_submit_high = SubmitMessage(
    ctx->message_queue_id,
    message_high,
    sizeof( message_high ),
    MESSAGE_PRIORITY_HIGH
  );

  status_receive_high = ReceiveMessage(
    ctx->message_queue_id,
    &message_buffer_high,
    &message_size_high
  );

  status_receive_low = ReceiveMessage(
    ctx->message_queue_id,
    &message_buffer_low,
    &message_size_low
  );

  /*
   * Check that _CORE_message_queue_Submit() was executed successfully.
   */
  T_rsc_success( status_submit_low );
  T_rsc_success( status_submit_high );

  /*
   * Check that the messages are in the right order in the message queue.
   */
  T_rsc_success( status_receive_high );
  T_eq_sz( message_size_high, sizeof( message_high ) );
  T_eq_mem( message_buffer_high, message_high, message_size_high );

  T_rsc_success( status_receive_low );
  T_eq_sz( message_size_low, sizeof( message_low ) );
  T_eq_mem( message_buffer_low, message_low, message_size_low );
}

/**
 * @brief Submit three messages into a message queue which can only store two
 *   and have the third submit() blocked till a seize() occurs.
 */
static void ScoreMsgqUnitMsgq_Action_1( ScoreMsgqUnitMsgq_Context *ctx )
{
  bool is_worker_blocked_after_third_send;
  bool is_worker_blocked_after_first_receive;

  WorkerSendMessage( ctx );
  WorkerSendMessage( ctx );
  WorkerSendMessage( ctx );
  is_worker_blocked_after_third_send = ctx->is_worker_working;

  T_rsc_success( ReceiveOneMessages( ctx ) );
  is_worker_blocked_after_first_receive = ctx->is_worker_working;

  T_rsc_success( ReceiveOneMessages( ctx ) );
  T_rsc_success( ReceiveOneMessages( ctx ) );

  /*
   * Check that the third _CORE_message_queue_Submit() did actually block till
   * there was room for the message in the message queue.
   */
  T_true( is_worker_blocked_after_third_send );
  T_true( !is_worker_blocked_after_first_receive );
}

/**
 * @brief Submit messages in the queue from within an ISR.
 */
static void ScoreMsgqUnitMsgq_Action_2( ScoreMsgqUnitMsgq_Context *ctx )
{
  rtems_status_code status_send_first_message;
  rtems_status_code status_send_second_message;
  rtems_status_code status_send_third_message;

  CallWithinISR( ( void (*)(void*) ) SendMessage, ctx );
  status_send_first_message = ctx->send_status;
  CallWithinISR( ( void (*)(void*) ) SendMessage, ctx );
  status_send_second_message = ctx->send_status;
  CallWithinISR( ( void (*)(void*) ) SendMessage, ctx );
  status_send_third_message = ctx->send_status;

  T_rsc_success( ReceiveOneMessages( ctx ) );
  T_rsc_success( ReceiveOneMessages( ctx ) );

  /*
   * Check that the first two messages were successfully send.
   */
  T_assert_rsc_success( status_send_first_message );
  T_assert_rsc_success( status_send_second_message );

  /*
   * Check that trying to send the third message from ISR when the message
   * queue was full was rejected.
   */
  T_rsc( status_send_third_message, STATUS_CLASSIC_INTERNAL_ERROR );
}

/**
 * @fn void T_case_body_ScoreMsgqUnitMsgq( void )
 */
T_TEST_CASE_FIXTURE( ScoreMsgqUnitMsgq, &ScoreMsgqUnitMsgq_Fixture )
{
  ScoreMsgqUnitMsgq_Context *ctx;

  ctx = T_fixture_context();

  ScoreMsgqUnitMsgq_Action_0( ctx );
  ScoreMsgqUnitMsgq_Action_1( ctx );
  ScoreMsgqUnitMsgq_Action_2( ctx );
}

/** @} */
