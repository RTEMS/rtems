/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMessageReqConstruct
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH & Co. KG
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
 * @defgroup RtemsMessageReqConstruct spec:/rtems/message/req/construct
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsMessageReqConstruct_Pre_Config_Valid,
  RtemsMessageReqConstruct_Pre_Config_Null,
  RtemsMessageReqConstruct_Pre_Config_NA
} RtemsMessageReqConstruct_Pre_Config;

typedef enum {
  RtemsMessageReqConstruct_Pre_Name_Valid,
  RtemsMessageReqConstruct_Pre_Name_Invalid,
  RtemsMessageReqConstruct_Pre_Name_NA
} RtemsMessageReqConstruct_Pre_Name;

typedef enum {
  RtemsMessageReqConstruct_Pre_Id_Id,
  RtemsMessageReqConstruct_Pre_Id_Null,
  RtemsMessageReqConstruct_Pre_Id_NA
} RtemsMessageReqConstruct_Pre_Id;

typedef enum {
  RtemsMessageReqConstruct_Pre_MaxPending_Valid,
  RtemsMessageReqConstruct_Pre_MaxPending_Zero,
  RtemsMessageReqConstruct_Pre_MaxPending_Big,
  RtemsMessageReqConstruct_Pre_MaxPending_NA
} RtemsMessageReqConstruct_Pre_MaxPending;

typedef enum {
  RtemsMessageReqConstruct_Pre_MaxSize_Valid,
  RtemsMessageReqConstruct_Pre_MaxSize_Zero,
  RtemsMessageReqConstruct_Pre_MaxSize_Big,
  RtemsMessageReqConstruct_Pre_MaxSize_NA
} RtemsMessageReqConstruct_Pre_MaxSize;

typedef enum {
  RtemsMessageReqConstruct_Pre_Free_Yes,
  RtemsMessageReqConstruct_Pre_Free_No,
  RtemsMessageReqConstruct_Pre_Free_NA
} RtemsMessageReqConstruct_Pre_Free;

typedef enum {
  RtemsMessageReqConstruct_Pre_Area_Valid,
  RtemsMessageReqConstruct_Pre_Area_Null,
  RtemsMessageReqConstruct_Pre_Area_NA
} RtemsMessageReqConstruct_Pre_Area;

typedef enum {
  RtemsMessageReqConstruct_Pre_AreaSize_Valid,
  RtemsMessageReqConstruct_Pre_AreaSize_Invalid,
  RtemsMessageReqConstruct_Pre_AreaSize_NA
} RtemsMessageReqConstruct_Pre_AreaSize;

typedef enum {
  RtemsMessageReqConstruct_Pre_StorageFree_Null,
  RtemsMessageReqConstruct_Pre_StorageFree_Handler,
  RtemsMessageReqConstruct_Pre_StorageFree_NA
} RtemsMessageReqConstruct_Pre_StorageFree;

typedef enum {
  RtemsMessageReqConstruct_Post_Status_Ok,
  RtemsMessageReqConstruct_Post_Status_InvAddr,
  RtemsMessageReqConstruct_Post_Status_InvName,
  RtemsMessageReqConstruct_Post_Status_InvNum,
  RtemsMessageReqConstruct_Post_Status_InvSize,
  RtemsMessageReqConstruct_Post_Status_TooMany,
  RtemsMessageReqConstruct_Post_Status_Unsat,
  RtemsMessageReqConstruct_Post_Status_NA
} RtemsMessageReqConstruct_Post_Status;

typedef enum {
  RtemsMessageReqConstruct_Post_Name_Valid,
  RtemsMessageReqConstruct_Post_Name_Invalid,
  RtemsMessageReqConstruct_Post_Name_NA
} RtemsMessageReqConstruct_Post_Name;

typedef enum {
  RtemsMessageReqConstruct_Post_IdObj_Set,
  RtemsMessageReqConstruct_Post_IdObj_Nop,
  RtemsMessageReqConstruct_Post_IdObj_NA
} RtemsMessageReqConstruct_Post_IdObj;

typedef enum {
  RtemsMessageReqConstruct_Post_StorageFree_Free,
  RtemsMessageReqConstruct_Post_StorageFree_Nop,
  RtemsMessageReqConstruct_Post_StorageFree_NA
} RtemsMessageReqConstruct_Post_StorageFree;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Config_NA : 1;
  uint32_t Pre_Name_NA : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_MaxPending_NA : 1;
  uint32_t Pre_MaxSize_NA : 1;
  uint32_t Pre_Free_NA : 1;
  uint32_t Pre_Area_NA : 1;
  uint32_t Pre_AreaSize_NA : 1;
  uint32_t Pre_StorageFree_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Name : 2;
  uint32_t Post_IdObj : 2;
  uint32_t Post_StorageFree : 2;
} RtemsMessageReqConstruct_Entry;

/**
 * @brief Test context for spec:/rtems/message/req/construct test case.
 */
typedef struct {
  rtems_status_code status;

  const rtems_message_queue_config *config;

  rtems_message_queue_config config_obj;

  rtems_id *id;

  rtems_id id_obj;

  void *seized_objects;

  uint32_t storage_free_counter;

  uint32_t expected_storage_free_counter;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 9 ];

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
    RtemsMessageReqConstruct_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMessageReqConstruct_Context;

static RtemsMessageReqConstruct_Context
  RtemsMessageReqConstruct_Instance;

static const char * const RtemsMessageReqConstruct_PreDesc_Config[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqConstruct_PreDesc_Name[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsMessageReqConstruct_PreDesc_Id[] = {
  "Id",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqConstruct_PreDesc_MaxPending[] = {
  "Valid",
  "Zero",
  "Big",
  "NA"
};

static const char * const RtemsMessageReqConstruct_PreDesc_MaxSize[] = {
  "Valid",
  "Zero",
  "Big",
  "NA"
};

static const char * const RtemsMessageReqConstruct_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsMessageReqConstruct_PreDesc_Area[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqConstruct_PreDesc_AreaSize[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsMessageReqConstruct_PreDesc_StorageFree[] = {
  "Null",
  "Handler",
  "NA"
};

static const char * const * const RtemsMessageReqConstruct_PreDesc[] = {
  RtemsMessageReqConstruct_PreDesc_Config,
  RtemsMessageReqConstruct_PreDesc_Name,
  RtemsMessageReqConstruct_PreDesc_Id,
  RtemsMessageReqConstruct_PreDesc_MaxPending,
  RtemsMessageReqConstruct_PreDesc_MaxSize,
  RtemsMessageReqConstruct_PreDesc_Free,
  RtemsMessageReqConstruct_PreDesc_Area,
  RtemsMessageReqConstruct_PreDesc_AreaSize,
  RtemsMessageReqConstruct_PreDesc_StorageFree,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

#define MAX_MESSAGE_QUEUES 4

#define MAX_PENDING_MESSAGES 1

#define MAX_MESSAGE_SIZE 1

typedef RtemsMessageReqConstruct_Context Context;

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

static void StorageFree( void *ptr )
{
  Context *ctx;

  ctx = T_fixture_context();
  T_eq_ptr( ptr, buffers );
  ++ctx->storage_free_counter;
}

static void RtemsMessageReqConstruct_Pre_Config_Prepare(
  RtemsMessageReqConstruct_Context   *ctx,
  RtemsMessageReqConstruct_Pre_Config state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Pre_Config_Valid: {
      /*
       * While the ``config`` parameter references an object of type
       * rtems_message_queue_config.
       */
      ctx->config = &ctx->config_obj;
      break;
    }

    case RtemsMessageReqConstruct_Pre_Config_Null: {
      /*
       * While the ``config`` parameter is NULL.
       */
      ctx->config = NULL;
      break;
    }

    case RtemsMessageReqConstruct_Pre_Config_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Pre_Name_Prepare(
  RtemsMessageReqConstruct_Context *ctx,
  RtemsMessageReqConstruct_Pre_Name state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Pre_Name_Valid: {
      /*
       * While the name of the message queue configuration is valid.
       */
      ctx->config_obj.name = NAME;
      break;
    }

    case RtemsMessageReqConstruct_Pre_Name_Invalid: {
      /*
       * While the name of the message queue configuration is invalid.
       */
      ctx->config_obj.name = 0;
      break;
    }

    case RtemsMessageReqConstruct_Pre_Name_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Pre_Id_Prepare(
  RtemsMessageReqConstruct_Context *ctx,
  RtemsMessageReqConstruct_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Pre_Id_Id: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_obj;
      break;
    }

    case RtemsMessageReqConstruct_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsMessageReqConstruct_Pre_Id_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Pre_MaxPending_Prepare(
  RtemsMessageReqConstruct_Context       *ctx,
  RtemsMessageReqConstruct_Pre_MaxPending state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Pre_MaxPending_Valid: {
      /*
       * While the maximum number of pending messages of the message queue
       * configuration is valid.
       */
      ctx->config_obj.maximum_pending_messages = MAX_PENDING_MESSAGES;
      break;
    }

    case RtemsMessageReqConstruct_Pre_MaxPending_Zero: {
      /*
       * While the maximum number of pending messages of the message queue
       * configuration is zero.
       */
      ctx->config_obj.maximum_pending_messages = 0;
      break;
    }

    case RtemsMessageReqConstruct_Pre_MaxPending_Big: {
      /*
       * While the maximum number of pending messages of the message queue
       * configuration is big enough so that a calculation to get the message
       * buffer storage area size overflows.
       */
      ctx->config_obj.maximum_pending_messages = UINT32_MAX;
      break;
    }

    case RtemsMessageReqConstruct_Pre_MaxPending_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Pre_MaxSize_Prepare(
  RtemsMessageReqConstruct_Context    *ctx,
  RtemsMessageReqConstruct_Pre_MaxSize state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Pre_MaxSize_Valid: {
      /*
       * While the maximum message size of the message queue configuration is
       * valid.
       */
      if ( ctx->config_obj.maximum_pending_messages == UINT32_MAX ) {
        /*
         * At least on 64-bit systems we need a bit of help to ensure that we
         * meet the Big state of the MaxPending pre-condition.  The following
         * message size is valid with respect to calculations involving only
         * the message size.
         */
        ctx->config_obj.maximum_message_size = SIZE_MAX - sizeof( uintptr_t ) +
          1 - sizeof( CORE_message_queue_Buffer );
      } else {
        ctx->config_obj.maximum_message_size = MAX_MESSAGE_SIZE;
      }
      break;
    }

    case RtemsMessageReqConstruct_Pre_MaxSize_Zero: {
      /*
       * While the maximum message size of the message queue configuration is
       * zero.
       */
      ctx->config_obj.maximum_message_size = 0;
      break;
    }

    case RtemsMessageReqConstruct_Pre_MaxSize_Big: {
      /*
       * While the maximum message size of the message queue configuration is
       * big enough so that a calculation to get the message buffer storage
       * area size overflows.
       */
      ctx->config_obj.maximum_message_size = SIZE_MAX;
      break;
    }

    case RtemsMessageReqConstruct_Pre_MaxSize_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Pre_Free_Prepare(
  RtemsMessageReqConstruct_Context *ctx,
  RtemsMessageReqConstruct_Pre_Free state
)
{
  size_t i;

  switch ( state ) {
    case RtemsMessageReqConstruct_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive message queue object
       * available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsMessageReqConstruct_Pre_Free_No: {
      /*
       * While the system has no inactive message queue object available.
       */
      i = 0;
      ctx->seized_objects = T_seize_objects( Create, &i );
      break;
    }

    case RtemsMessageReqConstruct_Pre_Free_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Pre_Area_Prepare(
  RtemsMessageReqConstruct_Context *ctx,
  RtemsMessageReqConstruct_Pre_Area state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Pre_Area_Valid: {
      /*
       * While the message buffer storage area begin pointer of the message
       * queue configuration is valid.
       */
      ctx->config_obj.storage_area = buffers;
      break;
    }

    case RtemsMessageReqConstruct_Pre_Area_Null: {
      /*
       * While the message buffer storage area begin pointer of the message
       * queue configuration is NULL.
       */
      ctx->config_obj.storage_area = NULL;
      break;
    }

    case RtemsMessageReqConstruct_Pre_Area_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Pre_AreaSize_Prepare(
  RtemsMessageReqConstruct_Context     *ctx,
  RtemsMessageReqConstruct_Pre_AreaSize state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Pre_AreaSize_Valid: {
      /*
       * While the message buffer storage area size of the message queue
       * configuration is valid.
       */
      ctx->config_obj.storage_size = sizeof( buffers );
      break;
    }

    case RtemsMessageReqConstruct_Pre_AreaSize_Invalid: {
      /*
       * While the message buffer storage area size of the message queue
       * configuration is invalid.
       */
      ctx->config_obj.storage_size = SIZE_MAX;
      break;
    }

    case RtemsMessageReqConstruct_Pre_AreaSize_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Pre_StorageFree_Prepare(
  RtemsMessageReqConstruct_Context        *ctx,
  RtemsMessageReqConstruct_Pre_StorageFree state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Pre_StorageFree_Null: {
      /*
       * While the storage free member of the message queue configuration is
       * equal to NULL.
       */
      ctx->config_obj.storage_free = NULL;
      break;
    }

    case RtemsMessageReqConstruct_Pre_StorageFree_Handler: {
      /*
       * While the storage free member of the message queue configuration
       * references a storage free handler.
       */
      ctx->config_obj.storage_free = StorageFree;
      break;
    }

    case RtemsMessageReqConstruct_Pre_StorageFree_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Post_Status_Check(
  RtemsMessageReqConstruct_Context    *ctx,
  RtemsMessageReqConstruct_Post_Status state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Post_Status_Ok: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsMessageReqConstruct_Post_Status_InvAddr: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsMessageReqConstruct_Post_Status_InvName: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsMessageReqConstruct_Post_Status_InvNum: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsMessageReqConstruct_Post_Status_InvSize: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsMessageReqConstruct_Post_Status_TooMany: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsMessageReqConstruct_Post_Status_Unsat: {
      /*
       * The return status of rtems_message_queue_construct() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED  );
      break;
    }

    case RtemsMessageReqConstruct_Post_Status_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Post_Name_Check(
  RtemsMessageReqConstruct_Context  *ctx,
  RtemsMessageReqConstruct_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsMessageReqConstruct_Post_Name_Valid: {
      /*
       * The unique object name shall identify the message queue constructed by
       * the rtems_message_queue_construct() call.
       */
      id = 0;
      sc = rtems_message_queue_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_obj );
      break;
    }

    case RtemsMessageReqConstruct_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a message queue.
       */
      sc = rtems_message_queue_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsMessageReqConstruct_Post_Name_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Post_IdObj_Check(
  RtemsMessageReqConstruct_Context   *ctx,
  RtemsMessageReqConstruct_Post_IdObj state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Post_IdObj_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the object identifier of the constructed message queue after
       * the return of the rtems_message_queue_construct() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_obj );
      T_ne_u32( ctx->id_obj, INVALID_ID );
      break;
    }

    case RtemsMessageReqConstruct_Post_IdObj_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_message_queue_construct() shall not be accessed by the
       * rtems_message_queue_construct() call.
       */
      T_eq_u32( ctx->id_obj, INVALID_ID );
      break;
    }

    case RtemsMessageReqConstruct_Post_IdObj_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Post_StorageFree_Check(
  RtemsMessageReqConstruct_Context         *ctx,
  RtemsMessageReqConstruct_Post_StorageFree state
)
{
  switch ( state ) {
    case RtemsMessageReqConstruct_Post_StorageFree_Free: {
      /*
       * The storage free handler of the message queue configuration specified
       * by the ``config`` parameter shall be used to free the message queue
       * storage area.
       */
      ctx->expected_storage_free_counter = 1;
      break;
    }

    case RtemsMessageReqConstruct_Post_StorageFree_Nop: {
      /*
       * No operation shall be performed to free the message queue storage
       * area.
       */
      ctx->expected_storage_free_counter = 0;
      break;
    }

    case RtemsMessageReqConstruct_Post_StorageFree_NA:
      break;
  }
}

static void RtemsMessageReqConstruct_Prepare(
  RtemsMessageReqConstruct_Context *ctx
)
{
  ctx->id_obj = INVALID_ID;
  ctx->storage_free_counter = 0;
  ctx->expected_storage_free_counter = UINT32_MAX;
  memset( &ctx->config_obj, 0, sizeof( ctx->config_obj ) );
}

static void RtemsMessageReqConstruct_Action(
  RtemsMessageReqConstruct_Context *ctx
)
{
  ctx->status = rtems_message_queue_construct( ctx->config, ctx->id );
}

static void RtemsMessageReqConstruct_Cleanup(
  RtemsMessageReqConstruct_Context *ctx
)
{
  if ( ctx->id_obj != INVALID_ID ) {
    rtems_status_code sc;

    sc = rtems_message_queue_delete( ctx->id_obj );
    T_rsc_success( sc );

    T_eq_u32( ctx->storage_free_counter, ctx->expected_storage_free_counter );
  } else {
    T_eq_u32( ctx->storage_free_counter, 0 );
  }

  T_surrender_objects( &ctx->seized_objects, rtems_message_queue_delete );
}

static const RtemsMessageReqConstruct_Entry
RtemsMessageReqConstruct_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqConstruct_Post_Status_InvAddr,
    RtemsMessageReqConstruct_Post_Name_Invalid,
    RtemsMessageReqConstruct_Post_IdObj_Nop,
    RtemsMessageReqConstruct_Post_StorageFree_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqConstruct_Post_Status_InvName,
    RtemsMessageReqConstruct_Post_Name_Invalid,
    RtemsMessageReqConstruct_Post_IdObj_Nop,
    RtemsMessageReqConstruct_Post_StorageFree_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqConstruct_Post_Status_InvNum,
    RtemsMessageReqConstruct_Post_Name_Invalid,
    RtemsMessageReqConstruct_Post_IdObj_Nop,
    RtemsMessageReqConstruct_Post_StorageFree_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqConstruct_Post_Status_InvSize,
    RtemsMessageReqConstruct_Post_Name_Invalid,
    RtemsMessageReqConstruct_Post_IdObj_Nop,
    RtemsMessageReqConstruct_Post_StorageFree_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqConstruct_Post_Status_TooMany,
    RtemsMessageReqConstruct_Post_Name_Invalid,
    RtemsMessageReqConstruct_Post_IdObj_Nop,
    RtemsMessageReqConstruct_Post_StorageFree_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqConstruct_Post_Status_Unsat,
    RtemsMessageReqConstruct_Post_Name_Invalid,
    RtemsMessageReqConstruct_Post_IdObj_Nop,
    RtemsMessageReqConstruct_Post_StorageFree_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqConstruct_Post_Status_Ok,
    RtemsMessageReqConstruct_Post_Name_Valid,
    RtemsMessageReqConstruct_Post_IdObj_Set,
    RtemsMessageReqConstruct_Post_StorageFree_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsMessageReqConstruct_Post_Status_Ok,
    RtemsMessageReqConstruct_Post_Name_Valid,
    RtemsMessageReqConstruct_Post_IdObj_Set,
    RtemsMessageReqConstruct_Post_StorageFree_Free }
};

static const uint8_t
RtemsMessageReqConstruct_Map[] = {
  6, 7, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

static size_t RtemsMessageReqConstruct_Scope( void *arg, char *buf, size_t n )
{
  RtemsMessageReqConstruct_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsMessageReqConstruct_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsMessageReqConstruct_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsMessageReqConstruct_Scope,
  .initial_context = &RtemsMessageReqConstruct_Instance
};

static inline RtemsMessageReqConstruct_Entry RtemsMessageReqConstruct_PopEntry(
  RtemsMessageReqConstruct_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMessageReqConstruct_Entries[
    RtemsMessageReqConstruct_Map[ index ]
  ];
}

static void RtemsMessageReqConstruct_TestVariant(
  RtemsMessageReqConstruct_Context *ctx
)
{
  RtemsMessageReqConstruct_Pre_Config_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMessageReqConstruct_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsMessageReqConstruct_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsMessageReqConstruct_Pre_MaxPending_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsMessageReqConstruct_Pre_MaxSize_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsMessageReqConstruct_Pre_Free_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsMessageReqConstruct_Pre_Area_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsMessageReqConstruct_Pre_AreaSize_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  RtemsMessageReqConstruct_Pre_StorageFree_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  RtemsMessageReqConstruct_Action( ctx );
  RtemsMessageReqConstruct_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsMessageReqConstruct_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsMessageReqConstruct_Post_IdObj_Check( ctx, ctx->Map.entry.Post_IdObj );
  RtemsMessageReqConstruct_Post_StorageFree_Check(
    ctx,
    ctx->Map.entry.Post_StorageFree
  );
}

/**
 * @fn void T_case_body_RtemsMessageReqConstruct( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMessageReqConstruct,
  &RtemsMessageReqConstruct_Fixture
)
{
  RtemsMessageReqConstruct_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsMessageReqConstruct_Pre_Config_Valid;
    ctx->Map.pcs[ 0 ] < RtemsMessageReqConstruct_Pre_Config_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsMessageReqConstruct_Pre_Name_Valid;
      ctx->Map.pcs[ 1 ] < RtemsMessageReqConstruct_Pre_Name_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsMessageReqConstruct_Pre_Id_Id;
        ctx->Map.pcs[ 2 ] < RtemsMessageReqConstruct_Pre_Id_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsMessageReqConstruct_Pre_MaxPending_Valid;
          ctx->Map.pcs[ 3 ] < RtemsMessageReqConstruct_Pre_MaxPending_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsMessageReqConstruct_Pre_MaxSize_Valid;
            ctx->Map.pcs[ 4 ] < RtemsMessageReqConstruct_Pre_MaxSize_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsMessageReqConstruct_Pre_Free_Yes;
              ctx->Map.pcs[ 5 ] < RtemsMessageReqConstruct_Pre_Free_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = RtemsMessageReqConstruct_Pre_Area_Valid;
                ctx->Map.pcs[ 6 ] < RtemsMessageReqConstruct_Pre_Area_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                for (
                  ctx->Map.pcs[ 7 ] = RtemsMessageReqConstruct_Pre_AreaSize_Valid;
                  ctx->Map.pcs[ 7 ] < RtemsMessageReqConstruct_Pre_AreaSize_NA;
                  ++ctx->Map.pcs[ 7 ]
                ) {
                  for (
                    ctx->Map.pcs[ 8 ] = RtemsMessageReqConstruct_Pre_StorageFree_Null;
                    ctx->Map.pcs[ 8 ] < RtemsMessageReqConstruct_Pre_StorageFree_NA;
                    ++ctx->Map.pcs[ 8 ]
                  ) {
                    ctx->Map.entry = RtemsMessageReqConstruct_PopEntry( ctx );
                    RtemsMessageReqConstruct_Prepare( ctx );
                    RtemsMessageReqConstruct_TestVariant( ctx );
                    RtemsMessageReqConstruct_Cleanup( ctx );
                  }
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
