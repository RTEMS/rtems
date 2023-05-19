/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMessageReqDelete
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
#include <string.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsMessageReqDelete spec:/rtems/message/req/delete
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsMessageReqDelete_Pre_Id_NoObj,
  RtemsMessageReqDelete_Pre_Id_MsgQueue,
  RtemsMessageReqDelete_Pre_Id_NA
} RtemsMessageReqDelete_Pre_Id;

typedef enum {
  RtemsMessageReqDelete_Post_Status_Ok,
  RtemsMessageReqDelete_Post_Status_InvId,
  RtemsMessageReqDelete_Post_Status_NA
} RtemsMessageReqDelete_Post_Status;

typedef enum {
  RtemsMessageReqDelete_Post_Name_Valid,
  RtemsMessageReqDelete_Post_Name_Invalid,
  RtemsMessageReqDelete_Post_Name_NA
} RtemsMessageReqDelete_Post_Name;

typedef enum {
  RtemsMessageReqDelete_Post_Flush_Yes,
  RtemsMessageReqDelete_Post_Flush_No,
  RtemsMessageReqDelete_Post_Flush_NA
} RtemsMessageReqDelete_Post_Flush;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Name : 2;
  uint8_t Post_Flush : 2;
} RtemsMessageReqDelete_Entry;

/**
 * @brief Test context for spec:/rtems/message/req/delete test case.
 */
typedef struct {
  rtems_id worker_id;

  rtems_id message_queue_id;

  uint32_t wait_done;

  uint32_t wait_expected;

  rtems_id id;

  rtems_status_code status;

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
    RtemsMessageReqDelete_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMessageReqDelete_Context;

static RtemsMessageReqDelete_Context
  RtemsMessageReqDelete_Instance;

static const char * const RtemsMessageReqDelete_PreDesc_Id[] = {
  "NoObj",
  "MsgQueue",
  "NA"
};

static const char * const * const RtemsMessageReqDelete_PreDesc[] = {
  RtemsMessageReqDelete_PreDesc_Id,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

#define MAX_PENDING_MESSAGES 1

#define MAX_MESSAGE_SIZE 1

typedef RtemsMessageReqDelete_Context Context;

static RTEMS_MESSAGE_QUEUE_BUFFER( MAX_MESSAGE_SIZE )
  buffers[ MAX_PENDING_MESSAGES ];

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_status_code          sc;
    rtems_message_queue_config config;
    char                       buffer[ MAX_MESSAGE_SIZE ];
    size_t                     size;
    rtems_task_priority        prio;

    memset( &config, 0, sizeof( config ) );
    config.name = NAME;
    config.maximum_pending_messages = MAX_PENDING_MESSAGES;
    config.maximum_message_size = MAX_MESSAGE_SIZE;
    config.storage_size = sizeof( buffers );
    config.storage_area = buffers;
    config.attributes = RTEMS_DEFAULT_ATTRIBUTES;

    T_eq_u32( ctx->message_queue_id, 0 );

    sc = rtems_message_queue_construct( &config, &ctx->message_queue_id );
    T_rsc_success( sc );

    size = SIZE_MAX;
    sc = rtems_message_queue_receive(
      ctx->message_queue_id,
      buffer,
      &size,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    T_rsc( sc, RTEMS_OBJECT_WAS_DELETED );
    T_eq_sz( size, SIZE_MAX );

    ++ctx->wait_done;

    prio = SetSelfPriority( PRIO_LOW );
    T_eq_u32( prio, PRIO_HIGH );
  }
}

static void RtemsMessageReqDelete_Pre_Id_Prepare(
  RtemsMessageReqDelete_Context *ctx,
  RtemsMessageReqDelete_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsMessageReqDelete_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is not associated with a message queue.
       */
      ctx->id = 0;
      break;
    }

    case RtemsMessageReqDelete_Pre_Id_MsgQueue: {
      /*
       * While the ``id`` parameter is associated with a message queue.
       */
      ctx->id = ctx->message_queue_id;
      break;
    }

    case RtemsMessageReqDelete_Pre_Id_NA:
      break;
  }
}

static void RtemsMessageReqDelete_Post_Status_Check(
  RtemsMessageReqDelete_Context    *ctx,
  RtemsMessageReqDelete_Post_Status state
)
{
  switch ( state ) {
    case RtemsMessageReqDelete_Post_Status_Ok: {
      /*
       * The return status of rtems_message_queue_delete() shall be
       * RTEMS_SUCCESSFUL.
       */
      ctx->message_queue_id = 0;
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsMessageReqDelete_Post_Status_InvId: {
      /*
       * The return status of rtems_message_queue_delete() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsMessageReqDelete_Post_Status_NA:
      break;
  }
}

static void RtemsMessageReqDelete_Post_Name_Check(
  RtemsMessageReqDelete_Context  *ctx,
  RtemsMessageReqDelete_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsMessageReqDelete_Post_Name_Valid: {
      /*
       * The unique object name shall identify a message queue.
       */
      id = 0;
      sc = rtems_message_queue_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->message_queue_id );
      break;
    }

    case RtemsMessageReqDelete_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a message queue.
       */
      sc = rtems_message_queue_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsMessageReqDelete_Post_Name_NA:
      break;
  }
}

static void RtemsMessageReqDelete_Post_Flush_Check(
  RtemsMessageReqDelete_Context   *ctx,
  RtemsMessageReqDelete_Post_Flush state
)
{
  switch ( state ) {
    case RtemsMessageReqDelete_Post_Flush_Yes: {
      /*
       * Tasks waiting at the message queue shall be unblocked.
       */
      ++ctx->wait_expected;
      T_eq_u32( ctx->wait_done, ctx->wait_expected );
      break;
    }

    case RtemsMessageReqDelete_Post_Flush_No: {
      /*
       * Tasks waiting at the message queue shall remain blocked.
       */
      T_eq_u32( ctx->wait_done, ctx->wait_expected );
      break;
    }

    case RtemsMessageReqDelete_Post_Flush_NA:
      break;
  }
}

static void RtemsMessageReqDelete_Setup( RtemsMessageReqDelete_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_LOW );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsMessageReqDelete_Setup_Wrap( void *arg )
{
  RtemsMessageReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqDelete_Setup( ctx );
}

static void RtemsMessageReqDelete_Teardown(
  RtemsMessageReqDelete_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void RtemsMessageReqDelete_Teardown_Wrap( void *arg )
{
  RtemsMessageReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMessageReqDelete_Teardown( ctx );
}

static void RtemsMessageReqDelete_Prepare( RtemsMessageReqDelete_Context *ctx )
{
  rtems_task_priority prio;

  prio = SetPriority( ctx->worker_id, PRIO_HIGH );
  T_true( prio == PRIO_LOW || prio == PRIO_HIGH );
}

static void RtemsMessageReqDelete_Action( RtemsMessageReqDelete_Context *ctx )
{
  ctx->status = rtems_message_queue_delete( ctx->id );
}

static void RtemsMessageReqDelete_Cleanup( RtemsMessageReqDelete_Context *ctx )
{
  if ( ctx->message_queue_id != 0 ) {
    rtems_status_code sc;

    sc = rtems_message_queue_delete( ctx->message_queue_id );
    T_rsc_success( sc );

    ++ctx->wait_expected;
    T_eq_u32( ctx->wait_done, ctx->wait_expected );

    ctx->message_queue_id = 0;
  }
}

static const RtemsMessageReqDelete_Entry
RtemsMessageReqDelete_Entries[] = {
  { 0, 0, RtemsMessageReqDelete_Post_Status_InvId,
    RtemsMessageReqDelete_Post_Name_Valid, RtemsMessageReqDelete_Post_Flush_No },
  { 0, 0, RtemsMessageReqDelete_Post_Status_Ok,
    RtemsMessageReqDelete_Post_Name_Invalid,
    RtemsMessageReqDelete_Post_Flush_Yes }
};

static const uint8_t
RtemsMessageReqDelete_Map[] = {
  0, 1
};

static size_t RtemsMessageReqDelete_Scope( void *arg, char *buf, size_t n )
{
  RtemsMessageReqDelete_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsMessageReqDelete_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsMessageReqDelete_Fixture = {
  .setup = RtemsMessageReqDelete_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMessageReqDelete_Teardown_Wrap,
  .scope = RtemsMessageReqDelete_Scope,
  .initial_context = &RtemsMessageReqDelete_Instance
};

static inline RtemsMessageReqDelete_Entry RtemsMessageReqDelete_PopEntry(
  RtemsMessageReqDelete_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMessageReqDelete_Entries[
    RtemsMessageReqDelete_Map[ index ]
  ];
}

static void RtemsMessageReqDelete_TestVariant(
  RtemsMessageReqDelete_Context *ctx
)
{
  RtemsMessageReqDelete_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMessageReqDelete_Action( ctx );
  RtemsMessageReqDelete_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsMessageReqDelete_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsMessageReqDelete_Post_Flush_Check( ctx, ctx->Map.entry.Post_Flush );
}

/**
 * @fn void T_case_body_RtemsMessageReqDelete( void )
 */
T_TEST_CASE_FIXTURE( RtemsMessageReqDelete, &RtemsMessageReqDelete_Fixture )
{
  RtemsMessageReqDelete_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsMessageReqDelete_Pre_Id_NoObj;
    ctx->Map.pcs[ 0 ] < RtemsMessageReqDelete_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsMessageReqDelete_PopEntry( ctx );
    RtemsMessageReqDelete_Prepare( ctx );
    RtemsMessageReqDelete_TestVariant( ctx );
    RtemsMessageReqDelete_Cleanup( ctx );
  }
}

/** @} */
