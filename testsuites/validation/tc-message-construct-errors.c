/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsMessageReqConstructErrors
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestCaseRtemsMessageReqConstructErrors \
 *   spec:/rtems/message/req/construct-errors
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 *
 * @{
 */

typedef enum {
  RtemsMessageReqConstructErrors_Pre_Config_Valid,
  RtemsMessageReqConstructErrors_Pre_Config_Null,
  RtemsMessageReqConstructErrors_Pre_Config_NA
} RtemsMessageReqConstructErrors_Pre_Config;

typedef enum {
  RtemsMessageReqConstructErrors_Pre_Name_Valid,
  RtemsMessageReqConstructErrors_Pre_Name_Invalid,
  RtemsMessageReqConstructErrors_Pre_Name_NA
} RtemsMessageReqConstructErrors_Pre_Name;

typedef enum {
  RtemsMessageReqConstructErrors_Pre_Id_Id,
  RtemsMessageReqConstructErrors_Pre_Id_Null,
  RtemsMessageReqConstructErrors_Pre_Id_NA
} RtemsMessageReqConstructErrors_Pre_Id;

typedef enum {
  RtemsMessageReqConstructErrors_Pre_MaxPending_Valid,
  RtemsMessageReqConstructErrors_Pre_MaxPending_Zero,
  RtemsMessageReqConstructErrors_Pre_MaxPending_Big,
  RtemsMessageReqConstructErrors_Pre_MaxPending_NA
} RtemsMessageReqConstructErrors_Pre_MaxPending;

typedef enum {
  RtemsMessageReqConstructErrors_Pre_MaxSize_Valid,
  RtemsMessageReqConstructErrors_Pre_MaxSize_Zero,
  RtemsMessageReqConstructErrors_Pre_MaxSize_Big,
  RtemsMessageReqConstructErrors_Pre_MaxSize_NA
} RtemsMessageReqConstructErrors_Pre_MaxSize;

typedef enum {
  RtemsMessageReqConstructErrors_Pre_Free_Yes,
  RtemsMessageReqConstructErrors_Pre_Free_No,
  RtemsMessageReqConstructErrors_Pre_Free_NA
} RtemsMessageReqConstructErrors_Pre_Free;

typedef enum {
  RtemsMessageReqConstructErrors_Pre_Area_Valid,
  RtemsMessageReqConstructErrors_Pre_Area_Null,
  RtemsMessageReqConstructErrors_Pre_Area_NA
} RtemsMessageReqConstructErrors_Pre_Area;

typedef enum {
  RtemsMessageReqConstructErrors_Pre_AreaSize_Valid,
  RtemsMessageReqConstructErrors_Pre_AreaSize_Invalid,
  RtemsMessageReqConstructErrors_Pre_AreaSize_NA
} RtemsMessageReqConstructErrors_Pre_AreaSize;

typedef enum {
  RtemsMessageReqConstructErrors_Post_Status_Ok,
  RtemsMessageReqConstructErrors_Post_Status_InvAddr,
  RtemsMessageReqConstructErrors_Post_Status_InvName,
  RtemsMessageReqConstructErrors_Post_Status_InvNum,
  RtemsMessageReqConstructErrors_Post_Status_InvSize,
  RtemsMessageReqConstructErrors_Post_Status_TooMany,
  RtemsMessageReqConstructErrors_Post_Status_Unsat,
  RtemsMessageReqConstructErrors_Post_Status_NA
} RtemsMessageReqConstructErrors_Post_Status;

typedef enum {
  RtemsMessageReqConstructErrors_Post_Name_Valid,
  RtemsMessageReqConstructErrors_Post_Name_Invalid,
  RtemsMessageReqConstructErrors_Post_Name_NA
} RtemsMessageReqConstructErrors_Post_Name;

typedef enum {
  RtemsMessageReqConstructErrors_Post_IdVar_Set,
  RtemsMessageReqConstructErrors_Post_IdVar_Nop,
  RtemsMessageReqConstructErrors_Post_IdVar_NA
} RtemsMessageReqConstructErrors_Post_IdVar;

/**
 * @brief Test context for spec:/rtems/message/req/construct-errors test case.
 */
typedef struct {
  rtems_status_code status;

  const rtems_message_queue_config *config;

  rtems_message_queue_config config_value;

  rtems_id *id;

  rtems_id id_value;

  void *seized_objects;

  /**
   * @brief This member defines the pre-condition states for the next action.
   */
  size_t pcs[ 8 ];

  /**
   * @brief This member indicates if the test action loop is currently
   *   executed.
   */
  bool in_action_loop;
} RtemsMessageReqConstructErrors_Context;

static RtemsMessageReqConstructErrors_Context
  RtemsMessageReqConstructErrors_Instance;

static const char * const RtemsMessageReqConstructErrors_PreDesc_Config[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqConstructErrors_PreDesc_Name[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsMessageReqConstructErrors_PreDesc_Id[] = {
  "Id",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqConstructErrors_PreDesc_MaxPending[] = {
  "Valid",
  "Zero",
  "Big",
  "NA"
};

static const char * const RtemsMessageReqConstructErrors_PreDesc_MaxSize[] = {
  "Valid",
  "Zero",
  "Big",
  "NA"
};

static const char * const RtemsMessageReqConstructErrors_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsMessageReqConstructErrors_PreDesc_Area[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqConstructErrors_PreDesc_AreaSize[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const * const RtemsMessageReqConstructErrors_PreDesc[] = {
  RtemsMessageReqConstructErrors_PreDesc_Config,
  RtemsMessageReqConstructErrors_PreDesc_Name,
  RtemsMessageReqConstructErrors_PreDesc_Id,
  RtemsMessageReqConstructErrors_PreDesc_MaxPending,
  RtemsMessageReqConstructErrors_PreDesc_MaxSize,
  RtemsMessageReqConstructErrors_PreDesc_Free,
  RtemsMessageReqConstructErrors_PreDesc_Area,
  RtemsMessageReqConstructErrors_PreDesc_AreaSize,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

#define INVALID_ID 0xffffffff

#define MAX_MESSAGE_QUEUES 4

#define MAX_PENDING_MESSAGES 1

#define MAX_MESSAGE_SIZE 1

static RTEMS_MESSAGE_QUEUE_BUFFER( MAX_MESSAGE_SIZE )
  buffers_to_seize[ MAX_MESSAGE_QUEUES ][ MAX_PENDING_MESSAGES ];

static RTEMS_MESSAGE_QUEUE_BUFFER( MAX_MESSAGE_SIZE )
  buffers[ MAX_PENDING_MESSAGES ];

static rtems_status_code Create( void *arg, uint32_t *id )
{
  rtems_message_queue_config config;
  size_t                    *i;

  i = arg;
  T_quiet_lt_sz( *i, MAX_MESSAGE_QUEUES );

  memset( &config, 0, sizeof( config ) );
  config.name = rtems_build_name( 'S', 'I', 'Z', 'E' );
  config.maximum_pending_messages = MAX_PENDING_MESSAGES;
  config.maximum_message_size = MAX_MESSAGE_SIZE;
  config.storage_size = sizeof( buffers_to_seize[ *i ] );
  config.storage_area = buffers_to_seize[ *i ];
  config.attributes = RTEMS_DEFAULT_ATTRIBUTES;

  ++(*i);

  return rtems_message_queue_construct( &config, id );
}

static void RtemsMessageReqConstructErrors_Pre_Config_Prepare(
  RtemsMessageReqConstructErrors_Context   *ctx,
  RtemsMessageReqConstructErrors_Pre_Config state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_Config_Valid: {
      /*
       * While the ``config`` parameter references an object of type
       * rtems_message_queue_config.
       */
      ctx->config = &ctx->config_value;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Config_Null: {
      /*
       * While the ``config`` parameter is NULL.
       */
      ctx->config = NULL;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Config_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Pre_Name_Prepare(
  RtemsMessageReqConstructErrors_Context *ctx,
  RtemsMessageReqConstructErrors_Pre_Name state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_Name_Valid: {
      /*
       * While the name of the message queue configuration is valid.
       */
      ctx->config_value.name = NAME;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Name_Invalid: {
      /*
       * While the name of the message queue configuration is invalid.
       */
      ctx->config_value.name = 0;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Name_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Pre_Id_Prepare(
  RtemsMessageReqConstructErrors_Context *ctx,
  RtemsMessageReqConstructErrors_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_Id_Id: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Id_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Pre_MaxPending_Prepare(
  RtemsMessageReqConstructErrors_Context       *ctx,
  RtemsMessageReqConstructErrors_Pre_MaxPending state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_MaxPending_Valid: {
      /*
       * While the maximum number of pending messages of the message queue
       * configuration is valid.
       */
      ctx->config_value.maximum_pending_messages = MAX_PENDING_MESSAGES;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxPending_Zero: {
      /*
       * While the maximum number of pending messages of the message queue
       * configuration is zero.
       */
      ctx->config_value.maximum_pending_messages = 0;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxPending_Big: {
      /*
       * While the maximum number of pending messages of the message queue
       * configuration is big enough so that a calculation to get the message
       * buffer storage area size overflows.
       */
      ctx->config_value.maximum_pending_messages = UINT32_MAX;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxPending_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Pre_MaxSize_Prepare(
  RtemsMessageReqConstructErrors_Context    *ctx,
  RtemsMessageReqConstructErrors_Pre_MaxSize state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_MaxSize_Valid: {
      /*
       * While the maximum message size of the message queue configuration is
       * valid.
       */
      if ( ctx->config_value.maximum_pending_messages == UINT32_MAX ) {
        /*
         * At least on 64-bit systems we need a bit of help to ensure that we
         * meet the Big state of the MaxPending pre-condition.  The following
         * message size is valid with respect to calculations involving only
         * the message size.
         */
        ctx->config_value.maximum_message_size = SIZE_MAX - sizeof( uintptr_t ) +
          1 - sizeof( CORE_message_queue_Buffer );
      } else {
        ctx->config_value.maximum_message_size = MAX_MESSAGE_SIZE;
      }
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxSize_Zero: {
      /*
       * While the maximum message size of the message queue configuration is
       * zero.
       */
      ctx->config_value.maximum_message_size = 0;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxSize_Big: {
      /*
       * While the maximum message size of the message queue configuration is
       * big enough so that a calculation to get the message buffer storage
       * area size overflows.
       */
      ctx->config_value.maximum_message_size = SIZE_MAX;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxSize_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Pre_Free_Prepare(
  RtemsMessageReqConstructErrors_Context *ctx,
  RtemsMessageReqConstructErrors_Pre_Free state
)
{
  size_t i;

  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive message queue object
       * available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Free_No: {
      /*
       * While the system has no inactive message queue object available.
       */
      i = 0;
      ctx->seized_objects = T_seize_objects( Create, &i );
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Free_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Pre_Area_Prepare(
  RtemsMessageReqConstructErrors_Context *ctx,
  RtemsMessageReqConstructErrors_Pre_Area state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_Area_Valid: {
      /*
       * While the message buffer storage area begin pointer of the message
       * queue configuration is valid.
       */
      ctx->config_value.storage_area = buffers;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Area_Null: {
      /*
       * While the message buffer storage area begin pointer of the message
       * queue configuration is NULL.
       */
      ctx->config_value.storage_area = NULL;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Area_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Pre_AreaSize_Prepare(
  RtemsMessageReqConstructErrors_Context     *ctx,
  RtemsMessageReqConstructErrors_Pre_AreaSize state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_AreaSize_Valid: {
      /*
       * While the message buffer storage area size of the message queue
       * configuration is valid.
       */
      ctx->config_value.storage_size = sizeof( buffers );
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_AreaSize_Invalid: {
      /*
       * While the message buffer storage area size of the message queue
       * configuration is invalid.
       */
      ctx->config_value.storage_size = SIZE_MAX;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_AreaSize_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Post_Status_Check(
  RtemsMessageReqConstructErrors_Context    *ctx,
  RtemsMessageReqConstructErrors_Post_Status state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Post_Status_Ok: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_InvAddr: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_InvName: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_InvNum: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_InvSize: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_TooMany: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_Unsat: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED  );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Post_Name_Check(
  RtemsMessageReqConstructErrors_Context  *ctx,
  RtemsMessageReqConstructErrors_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsMessageReqConstructErrors_Post_Name_Valid: {
      /*
       * The unique object name shall identify the message queue constructed by
       * the rtems_message_queue_construct() call.
       */
      id = 0;
      sc = rtems_message_queue_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a message queue.
       */
      sc = rtems_message_queue_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Name_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Post_IdVar_Check(
  RtemsMessageReqConstructErrors_Context   *ctx,
  RtemsMessageReqConstructErrors_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the object identifier of the constructed message queue after
       * the return of the rtems_message_queue_construct() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_message_queue_construct() shall not be accessed by the
       * rtems_message_queue_construct() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_IdVar_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Prepare(
  RtemsMessageReqConstructErrors_Context *ctx
)
{
  ctx->id_value = INVALID_ID;
  memset( &ctx->config_value, 0, sizeof( ctx->config_value ) );
}

static void RtemsMessageReqConstructErrors_Action(
  RtemsMessageReqConstructErrors_Context *ctx
)
{
  ctx->status = rtems_message_queue_construct( ctx->config, ctx->id );
}

static void RtemsMessageReqConstructErrors_Cleanup(
  RtemsMessageReqConstructErrors_Context *ctx
)
{
  if ( ctx->id_value != INVALID_ID ) {
    rtems_status_code sc;

    sc = rtems_message_queue_delete( ctx->id_value );
    T_rsc_success( sc );

    ctx->id_value = INVALID_ID;
  }

  T_surrender_objects( &ctx->seized_objects, rtems_message_queue_delete );
}

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Config_NA : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_MaxPending_NA : 1;
  uint16_t Pre_MaxSize_NA : 1;
  uint16_t Pre_Free_NA : 1;
  uint16_t Pre_Area_NA : 1;
  uint16_t Pre_AreaSize_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Name : 2;
  uint16_t Post_IdVar : 2;
} RtemsMessageReqConstructErrors_Entry;

static const RtemsMessageReqConstructErrors_Entry
RtemsMessageReqConstructErrors_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsMessageReqConstructErrors_Post_Status_InvAddr,
    RtemsMessageReqConstructErrors_Post_Name_Invalid,
    RtemsMessageReqConstructErrors_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsMessageReqConstructErrors_Post_Status_InvName,
    RtemsMessageReqConstructErrors_Post_Name_Invalid,
    RtemsMessageReqConstructErrors_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsMessageReqConstructErrors_Post_Status_InvNum,
    RtemsMessageReqConstructErrors_Post_Name_Invalid,
    RtemsMessageReqConstructErrors_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsMessageReqConstructErrors_Post_Status_InvSize,
    RtemsMessageReqConstructErrors_Post_Name_Invalid,
    RtemsMessageReqConstructErrors_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsMessageReqConstructErrors_Post_Status_TooMany,
    RtemsMessageReqConstructErrors_Post_Name_Invalid,
    RtemsMessageReqConstructErrors_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsMessageReqConstructErrors_Post_Status_Unsat,
    RtemsMessageReqConstructErrors_Post_Name_Invalid,
    RtemsMessageReqConstructErrors_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqConstructErrors_Post_Status_Ok,
    RtemsMessageReqConstructErrors_Post_Name_Valid,
    RtemsMessageReqConstructErrors_Post_IdVar_Set }
};

static const uint8_t
RtemsMessageReqConstructErrors_Map[] = {
  6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0
};

static size_t RtemsMessageReqConstructErrors_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsMessageReqConstructErrors_Context *ctx;

  ctx = arg;

  if ( ctx->in_action_loop ) {
    return T_get_scope(
      RtemsMessageReqConstructErrors_PreDesc,
      buf,
      n,
      ctx->pcs
    );
  }

  return 0;
}

static T_fixture RtemsMessageReqConstructErrors_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsMessageReqConstructErrors_Scope,
  .initial_context = &RtemsMessageReqConstructErrors_Instance
};

static inline RtemsMessageReqConstructErrors_Entry
RtemsMessageReqConstructErrors_GetEntry( size_t index )
{
  return RtemsMessageReqConstructErrors_Entries[
    RtemsMessageReqConstructErrors_Map[ index ]
  ];
}

/**
 * @fn void T_case_body_RtemsMessageReqConstructErrors( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMessageReqConstructErrors,
  &RtemsMessageReqConstructErrors_Fixture
)
{
  RtemsMessageReqConstructErrors_Context *ctx;
  RtemsMessageReqConstructErrors_Entry entry;
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsMessageReqConstructErrors_Pre_Config_Valid;
    ctx->pcs[ 0 ] < RtemsMessageReqConstructErrors_Pre_Config_NA;
    ++ctx->pcs[ 0 ]
  ) {
    entry = RtemsMessageReqConstructErrors_GetEntry( index );

    if ( entry.Pre_Config_NA ) {
      ctx->pcs[ 0 ] = RtemsMessageReqConstructErrors_Pre_Config_NA;
      index += ( RtemsMessageReqConstructErrors_Pre_Config_NA - 1 )
        * RtemsMessageReqConstructErrors_Pre_Name_NA
        * RtemsMessageReqConstructErrors_Pre_Id_NA
        * RtemsMessageReqConstructErrors_Pre_MaxPending_NA
        * RtemsMessageReqConstructErrors_Pre_MaxSize_NA
        * RtemsMessageReqConstructErrors_Pre_Free_NA
        * RtemsMessageReqConstructErrors_Pre_Area_NA
        * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
    }

    for (
      ctx->pcs[ 1 ] = RtemsMessageReqConstructErrors_Pre_Name_Valid;
      ctx->pcs[ 1 ] < RtemsMessageReqConstructErrors_Pre_Name_NA;
      ++ctx->pcs[ 1 ]
    ) {
      entry = RtemsMessageReqConstructErrors_GetEntry( index );

      if ( entry.Pre_Name_NA ) {
        ctx->pcs[ 1 ] = RtemsMessageReqConstructErrors_Pre_Name_NA;
        index += ( RtemsMessageReqConstructErrors_Pre_Name_NA - 1 )
          * RtemsMessageReqConstructErrors_Pre_Id_NA
          * RtemsMessageReqConstructErrors_Pre_MaxPending_NA
          * RtemsMessageReqConstructErrors_Pre_MaxSize_NA
          * RtemsMessageReqConstructErrors_Pre_Free_NA
          * RtemsMessageReqConstructErrors_Pre_Area_NA
          * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
      }

      for (
        ctx->pcs[ 2 ] = RtemsMessageReqConstructErrors_Pre_Id_Id;
        ctx->pcs[ 2 ] < RtemsMessageReqConstructErrors_Pre_Id_NA;
        ++ctx->pcs[ 2 ]
      ) {
        entry = RtemsMessageReqConstructErrors_GetEntry( index );

        if ( entry.Pre_Id_NA ) {
          ctx->pcs[ 2 ] = RtemsMessageReqConstructErrors_Pre_Id_NA;
          index += ( RtemsMessageReqConstructErrors_Pre_Id_NA - 1 )
            * RtemsMessageReqConstructErrors_Pre_MaxPending_NA
            * RtemsMessageReqConstructErrors_Pre_MaxSize_NA
            * RtemsMessageReqConstructErrors_Pre_Free_NA
            * RtemsMessageReqConstructErrors_Pre_Area_NA
            * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
        }

        for (
          ctx->pcs[ 3 ] = RtemsMessageReqConstructErrors_Pre_MaxPending_Valid;
          ctx->pcs[ 3 ] < RtemsMessageReqConstructErrors_Pre_MaxPending_NA;
          ++ctx->pcs[ 3 ]
        ) {
          entry = RtemsMessageReqConstructErrors_GetEntry( index );

          if ( entry.Pre_MaxPending_NA ) {
            ctx->pcs[ 3 ] = RtemsMessageReqConstructErrors_Pre_MaxPending_NA;
            index += ( RtemsMessageReqConstructErrors_Pre_MaxPending_NA - 1 )
              * RtemsMessageReqConstructErrors_Pre_MaxSize_NA
              * RtemsMessageReqConstructErrors_Pre_Free_NA
              * RtemsMessageReqConstructErrors_Pre_Area_NA
              * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
          }

          for (
            ctx->pcs[ 4 ] = RtemsMessageReqConstructErrors_Pre_MaxSize_Valid;
            ctx->pcs[ 4 ] < RtemsMessageReqConstructErrors_Pre_MaxSize_NA;
            ++ctx->pcs[ 4 ]
          ) {
            entry = RtemsMessageReqConstructErrors_GetEntry( index );

            if ( entry.Pre_MaxSize_NA ) {
              ctx->pcs[ 4 ] = RtemsMessageReqConstructErrors_Pre_MaxSize_NA;
              index += ( RtemsMessageReqConstructErrors_Pre_MaxSize_NA - 1 )
                * RtemsMessageReqConstructErrors_Pre_Free_NA
                * RtemsMessageReqConstructErrors_Pre_Area_NA
                * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
            }

            for (
              ctx->pcs[ 5 ] = RtemsMessageReqConstructErrors_Pre_Free_Yes;
              ctx->pcs[ 5 ] < RtemsMessageReqConstructErrors_Pre_Free_NA;
              ++ctx->pcs[ 5 ]
            ) {
              entry = RtemsMessageReqConstructErrors_GetEntry( index );

              if ( entry.Pre_Free_NA ) {
                ctx->pcs[ 5 ] = RtemsMessageReqConstructErrors_Pre_Free_NA;
                index += ( RtemsMessageReqConstructErrors_Pre_Free_NA - 1 )
                  * RtemsMessageReqConstructErrors_Pre_Area_NA
                  * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
              }

              for (
                ctx->pcs[ 6 ] = RtemsMessageReqConstructErrors_Pre_Area_Valid;
                ctx->pcs[ 6 ] < RtemsMessageReqConstructErrors_Pre_Area_NA;
                ++ctx->pcs[ 6 ]
              ) {
                entry = RtemsMessageReqConstructErrors_GetEntry( index );

                if ( entry.Pre_Area_NA ) {
                  ctx->pcs[ 6 ] = RtemsMessageReqConstructErrors_Pre_Area_NA;
                  index += ( RtemsMessageReqConstructErrors_Pre_Area_NA - 1 )
                    * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
                }

                for (
                  ctx->pcs[ 7 ] = RtemsMessageReqConstructErrors_Pre_AreaSize_Valid;
                  ctx->pcs[ 7 ] < RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
                  ++ctx->pcs[ 7 ]
                ) {
                  entry = RtemsMessageReqConstructErrors_GetEntry( index );

                  if ( entry.Pre_AreaSize_NA ) {
                    ctx->pcs[ 7 ] = RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
                    index += ( RtemsMessageReqConstructErrors_Pre_AreaSize_NA - 1 );
                  }

                  if ( entry.Skip ) {
                    ++index;
                    continue;
                  }

                  RtemsMessageReqConstructErrors_Prepare( ctx );
                  RtemsMessageReqConstructErrors_Pre_Config_Prepare(
                    ctx,
                    ctx->pcs[ 0 ]
                  );
                  RtemsMessageReqConstructErrors_Pre_Name_Prepare(
                    ctx,
                    ctx->pcs[ 1 ]
                  );
                  RtemsMessageReqConstructErrors_Pre_Id_Prepare(
                    ctx,
                    ctx->pcs[ 2 ]
                  );
                  RtemsMessageReqConstructErrors_Pre_MaxPending_Prepare(
                    ctx,
                    ctx->pcs[ 3 ]
                  );
                  RtemsMessageReqConstructErrors_Pre_MaxSize_Prepare(
                    ctx,
                    ctx->pcs[ 4 ]
                  );
                  RtemsMessageReqConstructErrors_Pre_Free_Prepare(
                    ctx,
                    ctx->pcs[ 5 ]
                  );
                  RtemsMessageReqConstructErrors_Pre_Area_Prepare(
                    ctx,
                    ctx->pcs[ 6 ]
                  );
                  RtemsMessageReqConstructErrors_Pre_AreaSize_Prepare(
                    ctx,
                    ctx->pcs[ 7 ]
                  );
                  RtemsMessageReqConstructErrors_Action( ctx );
                  RtemsMessageReqConstructErrors_Post_Status_Check(
                    ctx,
                    entry.Post_Status
                  );
                  RtemsMessageReqConstructErrors_Post_Name_Check(
                    ctx,
                    entry.Post_Name
                  );
                  RtemsMessageReqConstructErrors_Post_IdVar_Check(
                    ctx,
                    entry.Post_IdVar
                  );
                  RtemsMessageReqConstructErrors_Cleanup( ctx );
                  ++index;
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
