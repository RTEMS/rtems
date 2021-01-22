/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsMessageReqConstructErrors
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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
#include <rtems/score/chainimpl.h>
#include <rtems/score/objectimpl.h>

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
  RtemsMessageReqConstructErrors_Pre_Id_Id,
  RtemsMessageReqConstructErrors_Pre_Id_Null,
  RtemsMessageReqConstructErrors_Pre_Id_NA
} RtemsMessageReqConstructErrors_Pre_Id;

typedef enum {
  RtemsMessageReqConstructErrors_Pre_Name_Valid,
  RtemsMessageReqConstructErrors_Pre_Name_Invalid,
  RtemsMessageReqConstructErrors_Pre_Name_NA
} RtemsMessageReqConstructErrors_Pre_Name;

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
  RtemsMessageReqConstructErrors_Pre_Queues_Avail,
  RtemsMessageReqConstructErrors_Pre_Queues_None,
  RtemsMessageReqConstructErrors_Pre_Queues_NA
} RtemsMessageReqConstructErrors_Pre_Queues;

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
  RtemsMessageReqConstructErrors_Post_Status_InvAddress,
  RtemsMessageReqConstructErrors_Post_Status_InvName,
  RtemsMessageReqConstructErrors_Post_Status_InvNumber,
  RtemsMessageReqConstructErrors_Post_Status_InvSize,
  RtemsMessageReqConstructErrors_Post_Status_TooMany,
  RtemsMessageReqConstructErrors_Post_Status_Unsatisfied,
  RtemsMessageReqConstructErrors_Post_Status_NA
} RtemsMessageReqConstructErrors_Post_Status;

/**
 * @brief Test context for spec:/rtems/message/req/construct-errors test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_message_queue_config config;

  rtems_id *id;

  rtems_id id_value;

  Chain_Control message_queues;

  /**
   * @brief This member defines the pre-condition states for the next action.
   */
  size_t pcs[ 7 ];

  /**
   * @brief This member indicates if the test action loop is currently
   *   executed.
   */
  bool in_action_loop;
} RtemsMessageReqConstructErrors_Context;

static RtemsMessageReqConstructErrors_Context
  RtemsMessageReqConstructErrors_Instance;

static const char * const RtemsMessageReqConstructErrors_PreDesc_Id[] = {
  "Id",
  "Null",
  "NA"
};

static const char * const RtemsMessageReqConstructErrors_PreDesc_Name[] = {
  "Valid",
  "Invalid",
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

static const char * const RtemsMessageReqConstructErrors_PreDesc_Queues[] = {
  "Avail",
  "None",
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
  RtemsMessageReqConstructErrors_PreDesc_Id,
  RtemsMessageReqConstructErrors_PreDesc_Name,
  RtemsMessageReqConstructErrors_PreDesc_MaxPending,
  RtemsMessageReqConstructErrors_PreDesc_MaxSize,
  RtemsMessageReqConstructErrors_PreDesc_Queues,
  RtemsMessageReqConstructErrors_PreDesc_Area,
  RtemsMessageReqConstructErrors_PreDesc_AreaSize,
  NULL
};

#define MAX_MESSAGE_QUEUES 4

#define MAX_PENDING_MESSAGES 1

#define MAX_MESSAGE_SIZE 1

static RTEMS_MESSAGE_QUEUE_BUFFER( MAX_MESSAGE_SIZE )
  exhaust_buffers[ MAX_MESSAGE_QUEUES ][ MAX_PENDING_MESSAGES ];

static RTEMS_MESSAGE_QUEUE_BUFFER( MAX_MESSAGE_SIZE )
  buffers[ MAX_PENDING_MESSAGES ];

static void RtemsMessageReqConstructErrors_Pre_Id_Prepare(
  RtemsMessageReqConstructErrors_Context *ctx,
  RtemsMessageReqConstructErrors_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_Id_Id: {
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Id_Null: {
      ctx->id = NULL;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Id_NA:
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
      ctx->config.name = rtems_build_name( 'N', 'A', 'M', 'E' );
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Name_Invalid: {
      ctx->config.name = 0;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Name_NA:
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
      ctx->config.maximum_pending_messages = MAX_PENDING_MESSAGES;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxPending_Zero: {
      ctx->config.maximum_pending_messages = 0;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxPending_Big: {
      ctx->config.maximum_pending_messages = UINT32_MAX;
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
      ctx->config.maximum_message_size = MAX_MESSAGE_SIZE;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxSize_Zero: {
      ctx->config.maximum_message_size = 0;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxSize_Big: {
      ctx->config.maximum_message_size = SIZE_MAX;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_MaxSize_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Pre_Queues_Prepare(
  RtemsMessageReqConstructErrors_Context   *ctx,
  RtemsMessageReqConstructErrors_Pre_Queues state
)
{
  rtems_message_queue_config config;
  size_t                     i;

  switch ( state ) {
    case RtemsMessageReqConstructErrors_Pre_Queues_Avail: {
      /* Nothing to do */
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Queues_None: {
      memset( &config, 0, sizeof( config ) );
      config.name = rtems_build_name( 'M', 'S', 'G', 'Q' );
      config.maximum_pending_messages = MAX_PENDING_MESSAGES;
      config.maximum_message_size = MAX_MESSAGE_SIZE;
      config.storage_size = sizeof( exhaust_buffers[ 0 ] );
      config.attributes = RTEMS_DEFAULT_ATTRIBUTES;

      i = 0;

      while ( i < MAX_MESSAGE_QUEUES ) {
        rtems_status_code sc;
        rtems_id          id;

        config.storage_area = exhaust_buffers[ i ];

        sc = rtems_message_queue_construct( &config, &id );

        if ( sc == RTEMS_SUCCESSFUL ) {
          Objects_Control           *obj;
          const Objects_Information *info;

          info = _Objects_Get_information_id( id );
          T_quiet_assert_not_null( info );
          obj = _Objects_Get_no_protection( id, info );
          T_quiet_assert_not_null( obj );
          _Chain_Append_unprotected( &ctx->message_queues, &obj->Node );
        } else {
          T_quiet_rsc( sc, RTEMS_TOO_MANY );
          break;
        }

        ++i;
      }
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Queues_NA:
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
      ctx->config.storage_area = buffers;
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_Area_Null: {
      ctx->config.storage_area = NULL;
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
      ctx->config.storage_size = sizeof( buffers );
      break;
    }

    case RtemsMessageReqConstructErrors_Pre_AreaSize_Invalid: {
      ctx->config.storage_size = SIZE_MAX;
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
  rtems_status_code sc;

  switch ( state ) {
    case RtemsMessageReqConstructErrors_Post_Status_Ok: {
      T_rsc_success( ctx->status );
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, 0xffffffff );

      sc = rtems_message_queue_delete( ctx->id_value );
      T_rsc_success( sc );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_InvAddress: {
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      T_null( ctx->id );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_InvName: {
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_InvNumber: {
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_InvSize: {
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_TooMany: {
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_Unsatisfied: {
      T_rsc( ctx->status, RTEMS_UNSATISFIED  );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsMessageReqConstructErrors_Post_Status_NA:
      break;
  }
}

static void RtemsMessageReqConstructErrors_Setup(
  RtemsMessageReqConstructErrors_Context *ctx
)
{
  _Chain_Initialize_empty( &ctx->message_queues );
}

static void RtemsMessageReqConstructErrors_Setup_Wrap( void *arg )
{
  RtemsMessageReqConstructErrors_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsMessageReqConstructErrors_Setup( ctx );
}

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
  .setup = RtemsMessageReqConstructErrors_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsMessageReqConstructErrors_Scope,
  .initial_context = &RtemsMessageReqConstructErrors_Instance
};

static const uint8_t RtemsMessageReqConstructErrors_TransitionMap[][ 1 ] = {
  {
    RtemsMessageReqConstructErrors_Post_Status_Ok
  }, {
    RtemsMessageReqConstructErrors_Post_Status_Unsatisfied
  }, {
    RtemsMessageReqConstructErrors_Post_Status_Unsatisfied
  }, {
    RtemsMessageReqConstructErrors_Post_Status_Unsatisfied
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvNumber
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }, {
    RtemsMessageReqConstructErrors_Post_Status_InvName
  }
};

static const struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_Name_NA : 1;
  uint8_t Pre_MaxPending_NA : 1;
  uint8_t Pre_MaxSize_NA : 1;
  uint8_t Pre_Queues_NA : 1;
  uint8_t Pre_Area_NA : 1;
  uint8_t Pre_AreaSize_NA : 1;
} RtemsMessageReqConstructErrors_TransitionInfo[] = {
  {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0
  }
};

static void RtemsMessageReqConstructErrors_Prepare(
  RtemsMessageReqConstructErrors_Context *ctx
)
{
  ctx->id_value = 0xffffffff;
  memset( &ctx->config, 0, sizeof( ctx->config ) );
}

static void RtemsMessageReqConstructErrors_Action(
  RtemsMessageReqConstructErrors_Context *ctx
)
{
  ctx->status = rtems_message_queue_construct( &ctx->config, ctx->id );
}

static void RtemsMessageReqConstructErrors_Cleanup(
  RtemsMessageReqConstructErrors_Context *ctx
)
{
  Chain_Node *node;

  while ( ( node = _Chain_Get_unprotected( &ctx->message_queues ) ) ) {
    Objects_Control   *obj;
    rtems_status_code  sc;

    obj = (Objects_Control *) node;
    sc = rtems_message_queue_delete( obj->id );
    T_quiet_rsc_success( sc );
  }
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
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsMessageReqConstructErrors_Pre_Id_Id;
    ctx->pcs[ 0 ] < RtemsMessageReqConstructErrors_Pre_Id_NA;
    ++ctx->pcs[ 0 ]
  ) {
    if ( RtemsMessageReqConstructErrors_TransitionInfo[ index ].Pre_Id_NA ) {
      ctx->pcs[ 0 ] = RtemsMessageReqConstructErrors_Pre_Id_NA;
      index += ( RtemsMessageReqConstructErrors_Pre_Id_NA - 1 )
        * RtemsMessageReqConstructErrors_Pre_Name_NA
        * RtemsMessageReqConstructErrors_Pre_MaxPending_NA
        * RtemsMessageReqConstructErrors_Pre_MaxSize_NA
        * RtemsMessageReqConstructErrors_Pre_Queues_NA
        * RtemsMessageReqConstructErrors_Pre_Area_NA
        * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
    }

    for (
      ctx->pcs[ 1 ] = RtemsMessageReqConstructErrors_Pre_Name_Valid;
      ctx->pcs[ 1 ] < RtemsMessageReqConstructErrors_Pre_Name_NA;
      ++ctx->pcs[ 1 ]
    ) {
      if ( RtemsMessageReqConstructErrors_TransitionInfo[ index ].Pre_Name_NA ) {
        ctx->pcs[ 1 ] = RtemsMessageReqConstructErrors_Pre_Name_NA;
        index += ( RtemsMessageReqConstructErrors_Pre_Name_NA - 1 )
          * RtemsMessageReqConstructErrors_Pre_MaxPending_NA
          * RtemsMessageReqConstructErrors_Pre_MaxSize_NA
          * RtemsMessageReqConstructErrors_Pre_Queues_NA
          * RtemsMessageReqConstructErrors_Pre_Area_NA
          * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
      }

      for (
        ctx->pcs[ 2 ] = RtemsMessageReqConstructErrors_Pre_MaxPending_Valid;
        ctx->pcs[ 2 ] < RtemsMessageReqConstructErrors_Pre_MaxPending_NA;
        ++ctx->pcs[ 2 ]
      ) {
        if ( RtemsMessageReqConstructErrors_TransitionInfo[ index ].Pre_MaxPending_NA ) {
          ctx->pcs[ 2 ] = RtemsMessageReqConstructErrors_Pre_MaxPending_NA;
          index += ( RtemsMessageReqConstructErrors_Pre_MaxPending_NA - 1 )
            * RtemsMessageReqConstructErrors_Pre_MaxSize_NA
            * RtemsMessageReqConstructErrors_Pre_Queues_NA
            * RtemsMessageReqConstructErrors_Pre_Area_NA
            * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
        }

        for (
          ctx->pcs[ 3 ] = RtemsMessageReqConstructErrors_Pre_MaxSize_Valid;
          ctx->pcs[ 3 ] < RtemsMessageReqConstructErrors_Pre_MaxSize_NA;
          ++ctx->pcs[ 3 ]
        ) {
          if ( RtemsMessageReqConstructErrors_TransitionInfo[ index ].Pre_MaxSize_NA ) {
            ctx->pcs[ 3 ] = RtemsMessageReqConstructErrors_Pre_MaxSize_NA;
            index += ( RtemsMessageReqConstructErrors_Pre_MaxSize_NA - 1 )
              * RtemsMessageReqConstructErrors_Pre_Queues_NA
              * RtemsMessageReqConstructErrors_Pre_Area_NA
              * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
          }

          for (
            ctx->pcs[ 4 ] = RtemsMessageReqConstructErrors_Pre_Queues_Avail;
            ctx->pcs[ 4 ] < RtemsMessageReqConstructErrors_Pre_Queues_NA;
            ++ctx->pcs[ 4 ]
          ) {
            if ( RtemsMessageReqConstructErrors_TransitionInfo[ index ].Pre_Queues_NA ) {
              ctx->pcs[ 4 ] = RtemsMessageReqConstructErrors_Pre_Queues_NA;
              index += ( RtemsMessageReqConstructErrors_Pre_Queues_NA - 1 )
                * RtemsMessageReqConstructErrors_Pre_Area_NA
                * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
            }

            for (
              ctx->pcs[ 5 ] = RtemsMessageReqConstructErrors_Pre_Area_Valid;
              ctx->pcs[ 5 ] < RtemsMessageReqConstructErrors_Pre_Area_NA;
              ++ctx->pcs[ 5 ]
            ) {
              if ( RtemsMessageReqConstructErrors_TransitionInfo[ index ].Pre_Area_NA ) {
                ctx->pcs[ 5 ] = RtemsMessageReqConstructErrors_Pre_Area_NA;
                index += ( RtemsMessageReqConstructErrors_Pre_Area_NA - 1 )
                  * RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
              }

              for (
                ctx->pcs[ 6 ] = RtemsMessageReqConstructErrors_Pre_AreaSize_Valid;
                ctx->pcs[ 6 ] < RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
                ++ctx->pcs[ 6 ]
              ) {
                if ( RtemsMessageReqConstructErrors_TransitionInfo[ index ].Pre_AreaSize_NA ) {
                  ctx->pcs[ 6 ] = RtemsMessageReqConstructErrors_Pre_AreaSize_NA;
                  index += ( RtemsMessageReqConstructErrors_Pre_AreaSize_NA - 1 );
                }

                if ( RtemsMessageReqConstructErrors_TransitionInfo[ index ].Skip ) {
                  ++index;
                  continue;
                }

                RtemsMessageReqConstructErrors_Prepare( ctx );
                RtemsMessageReqConstructErrors_Pre_Id_Prepare(
                  ctx,
                  ctx->pcs[ 0 ]
                );
                RtemsMessageReqConstructErrors_Pre_Name_Prepare(
                  ctx,
                  ctx->pcs[ 1 ]
                );
                RtemsMessageReqConstructErrors_Pre_MaxPending_Prepare(
                  ctx,
                  ctx->pcs[ 2 ]
                );
                RtemsMessageReqConstructErrors_Pre_MaxSize_Prepare(
                  ctx,
                  ctx->pcs[ 3 ]
                );
                RtemsMessageReqConstructErrors_Pre_Queues_Prepare(
                  ctx,
                  ctx->pcs[ 4 ]
                );
                RtemsMessageReqConstructErrors_Pre_Area_Prepare(
                  ctx,
                  ctx->pcs[ 5 ]
                );
                RtemsMessageReqConstructErrors_Pre_AreaSize_Prepare(
                  ctx,
                  ctx->pcs[ 6 ]
                );
                RtemsMessageReqConstructErrors_Action( ctx );
                RtemsMessageReqConstructErrors_Post_Status_Check(
                  ctx,
                  RtemsMessageReqConstructErrors_TransitionMap[ index ][ 0 ]
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

/** @} */
