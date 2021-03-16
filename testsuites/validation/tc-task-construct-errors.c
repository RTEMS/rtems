/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsTaskReqConstructErrors
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
#include <rtems/score/apimutex.h>
#include <rtems/score/threadimpl.h>

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestCaseRtemsTaskReqConstructErrors \
 *   spec:/rtems/task/req/construct-errors
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 * @ingroup RTEMSTestSuiteTestsuitesValidation1
 *
 * @{
 */

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Name_Valid,
  RtemsTaskReqConstructErrors_Pre_Name_Inv,
  RtemsTaskReqConstructErrors_Pre_Name_NA
} RtemsTaskReqConstructErrors_Pre_Name;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Id_Valid,
  RtemsTaskReqConstructErrors_Pre_Id_Null,
  RtemsTaskReqConstructErrors_Pre_Id_NA
} RtemsTaskReqConstructErrors_Pre_Id;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_SysTsk_Yes,
  RtemsTaskReqConstructErrors_Pre_SysTsk_No,
  RtemsTaskReqConstructErrors_Pre_SysTsk_NA
} RtemsTaskReqConstructErrors_Pre_SysTsk;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Prio_Valid,
  RtemsTaskReqConstructErrors_Pre_Prio_Zero,
  RtemsTaskReqConstructErrors_Pre_Prio_Inv,
  RtemsTaskReqConstructErrors_Pre_Prio_NA
} RtemsTaskReqConstructErrors_Pre_Prio;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Free_Yes,
  RtemsTaskReqConstructErrors_Pre_Free_No,
  RtemsTaskReqConstructErrors_Pre_Free_NA
} RtemsTaskReqConstructErrors_Pre_Free;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_TLS_Enough,
  RtemsTaskReqConstructErrors_Pre_TLS_Small,
  RtemsTaskReqConstructErrors_Pre_TLS_NA
} RtemsTaskReqConstructErrors_Pre_TLS;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Stack_Enough,
  RtemsTaskReqConstructErrors_Pre_Stack_Small,
  RtemsTaskReqConstructErrors_Pre_Stack_NA
} RtemsTaskReqConstructErrors_Pre_Stack;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Ext_Ok,
  RtemsTaskReqConstructErrors_Pre_Ext_Err,
  RtemsTaskReqConstructErrors_Pre_Ext_NA
} RtemsTaskReqConstructErrors_Pre_Ext;

typedef enum {
  RtemsTaskReqConstructErrors_Post_Status_Ok,
  RtemsTaskReqConstructErrors_Post_Status_InvAddr,
  RtemsTaskReqConstructErrors_Post_Status_InvName,
  RtemsTaskReqConstructErrors_Post_Status_InvPrio,
  RtemsTaskReqConstructErrors_Post_Status_InvSize,
  RtemsTaskReqConstructErrors_Post_Status_TooMany,
  RtemsTaskReqConstructErrors_Post_Status_Unsat,
  RtemsTaskReqConstructErrors_Post_Status_NA
} RtemsTaskReqConstructErrors_Post_Status;

typedef enum {
  RtemsTaskReqConstructErrors_Post_Name_Valid,
  RtemsTaskReqConstructErrors_Post_Name_Invalid,
  RtemsTaskReqConstructErrors_Post_Name_NA
} RtemsTaskReqConstructErrors_Post_Name;

typedef enum {
  RtemsTaskReqConstructErrors_Post_IdVar_Set,
  RtemsTaskReqConstructErrors_Post_IdVar_Nop,
  RtemsTaskReqConstructErrors_Post_IdVar_NA
} RtemsTaskReqConstructErrors_Post_IdVar;

typedef enum {
  RtemsTaskReqConstructErrors_Post_CreateExt_Yes,
  RtemsTaskReqConstructErrors_Post_CreateExt_No,
  RtemsTaskReqConstructErrors_Post_CreateExt_NA
} RtemsTaskReqConstructErrors_Post_CreateExt;

typedef enum {
  RtemsTaskReqConstructErrors_Post_DelExt_Yes,
  RtemsTaskReqConstructErrors_Post_DelExt_No,
  RtemsTaskReqConstructErrors_Post_DelExt_NA
} RtemsTaskReqConstructErrors_Post_DelExt;

typedef enum {
  RtemsTaskReqConstructErrors_Post_StoFree_Yes,
  RtemsTaskReqConstructErrors_Post_StoFree_No,
  RtemsTaskReqConstructErrors_Post_StoFree_NA
} RtemsTaskReqConstructErrors_Post_StoFree;

/**
 * @brief Test context for spec:/rtems/task/req/construct-errors test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_task_config config;

  rtems_id *id;

  rtems_id id_value;

  bool create_extension_status;

  uint32_t create_extension_calls;

  uint32_t delete_extension_calls;

  uint32_t storage_free_calls;

  size_t stack_size;

  rtems_id extension_id;

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
} RtemsTaskReqConstructErrors_Context;

static RtemsTaskReqConstructErrors_Context
  RtemsTaskReqConstructErrors_Instance;

static const char * const RtemsTaskReqConstructErrors_PreDesc_Name[] = {
  "Valid",
  "Inv",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_SysTsk[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_Prio[] = {
  "Valid",
  "Zero",
  "Inv",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_TLS[] = {
  "Enough",
  "Small",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_Stack[] = {
  "Enough",
  "Small",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_Ext[] = {
  "Ok",
  "Err",
  "NA"
};

static const char * const * const RtemsTaskReqConstructErrors_PreDesc[] = {
  RtemsTaskReqConstructErrors_PreDesc_Name,
  RtemsTaskReqConstructErrors_PreDesc_Id,
  RtemsTaskReqConstructErrors_PreDesc_SysTsk,
  RtemsTaskReqConstructErrors_PreDesc_Prio,
  RtemsTaskReqConstructErrors_PreDesc_Free,
  RtemsTaskReqConstructErrors_PreDesc_TLS,
  RtemsTaskReqConstructErrors_PreDesc_Stack,
  RtemsTaskReqConstructErrors_PreDesc_Ext,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

#define INVALID_ID 0xffffffff

typedef RtemsTaskReqConstructErrors_Context Context;

static _Thread_local int tls_variable;

#define MAX_TLS_SIZE RTEMS_ALIGN_UP( 128, RTEMS_TASK_STORAGE_ALIGNMENT )

RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT ) static char task_storage[
  RTEMS_TASK_STORAGE_SIZE(
    MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_FLOATING_POINT
  )
];

static const rtems_task_config seize_task_config = {
  .name = rtems_build_name( 'S', 'I', 'Z', 'E' ),
  .initial_priority = 1,
  .storage_area = task_storage,
  .storage_size = sizeof( task_storage ),
  .maximum_thread_local_storage_size = MAX_TLS_SIZE,
  .initial_modes = RTEMS_DEFAULT_MODES,
  .attributes = RTEMS_DEFAULT_MODES
};

static void StorageFree( void *ptr )
{
  T_eq_ptr( ptr, task_storage );
  ++RtemsTaskReqConstructErrors_Instance.storage_free_calls;
}

static rtems_status_code Create( void *arg, uint32_t *id )
{
  Context          *ctx;
  bool              create_extension_status;
  rtems_status_code sc;

  ctx = arg;
  create_extension_status = ctx->create_extension_status;
  ctx->create_extension_status = true;
  sc = rtems_task_construct( &seize_task_config, id );
  ctx->create_extension_status = create_extension_status;

  return sc;
}

static bool ThreadCreate( rtems_tcb *executing, rtems_tcb *created )
{
  (void) executing;
  (void) created;

  ++RtemsTaskReqConstructErrors_Instance.create_extension_calls;
  return RtemsTaskReqConstructErrors_Instance.create_extension_status;
}

static void ThreadDelete( rtems_tcb *executing, rtems_tcb *deleted )
{
  (void) executing;
  (void) deleted;

  ++RtemsTaskReqConstructErrors_Instance.delete_extension_calls;
}

static const rtems_extensions_table extensions = {
  .thread_create = ThreadCreate,
  .thread_delete = ThreadDelete
};

static void RtemsTaskReqConstructErrors_Pre_Name_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx,
  RtemsTaskReqConstructErrors_Pre_Name state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Name_Valid: {
      /*
       * While the name of the task configuration is valid.
       */
      ctx->config.name = NAME;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Name_Inv: {
      /*
       * While the name of the task configuration is invalid.
       */
      ctx->config.name = 0;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Name_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_Id_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx,
  RtemsTaskReqConstructErrors_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_SysTsk_Prepare(
  RtemsTaskReqConstructErrors_Context   *ctx,
  RtemsTaskReqConstructErrors_Pre_SysTsk state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_SysTsk_Yes: {
      /*
       * While the attributes of the task configuration specifies a system
       * task.
       */
      ctx->config.attributes |= RTEMS_SYSTEM_TASK;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_SysTsk_No: {
      /*
       * While the attributes of the task configuration specifies an
       * application task.
       */
      /* Nothing to do */
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_SysTsk_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_Prio_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx,
  RtemsTaskReqConstructErrors_Pre_Prio state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Prio_Valid: {
      /*
       * While the initial priority of the task configuration is valid and
       * non-zero.
       */
      ctx->config.initial_priority = 254;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Prio_Zero: {
      /*
       * While the initial priority of the task configuration is zero.
       */
      ctx->config.initial_priority = 0;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Prio_Inv: {
      /*
       * While the initial priority of the task configuration is invalid.
       */
      ctx->config.initial_priority = 0xffffffff;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Prio_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_Free_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx,
  RtemsTaskReqConstructErrors_Pre_Free state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive task object available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Free_No: {
      /*
       * While the system has no inactive task object available.
       */
      ctx->seized_objects = T_seize_objects( Create, ctx );
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Free_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_TLS_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx,
  RtemsTaskReqConstructErrors_Pre_TLS  state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_TLS_Enough: {
      /*
       * While the maximum thread-local storage size of the task configuration
       * is greater than or equal to the thread-local storage size.
       */
      ctx->config.maximum_thread_local_storage_size = MAX_TLS_SIZE;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_TLS_Small: {
      /*
       * While the maximum thread-local storage size of the task configuration
       * is less than the thread-local storage size.
       */
      ctx->config.maximum_thread_local_storage_size = 0;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_TLS_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_Stack_Prepare(
  RtemsTaskReqConstructErrors_Context  *ctx,
  RtemsTaskReqConstructErrors_Pre_Stack state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Stack_Enough: {
      /*
       * While the task stack size of the task configuration is greater than or
       * equal to the configured minimum size.
       */
      ctx->stack_size = RTEMS_MINIMUM_STACK_SIZE;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Stack_Small: {
      /*
       * While the task stack size of the task configuration is less than the
       * configured minimum size.
       */
      ctx->stack_size = 0;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Stack_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_Ext_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx,
  RtemsTaskReqConstructErrors_Pre_Ext  state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Ext_Ok: {
      /*
       * While none of the task create extensions fails.
       */
      ctx->create_extension_status = true;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Ext_Err: {
      /*
       * While at least one of the task create extensions fails.
       */
      ctx->create_extension_status = false;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Ext_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Post_Status_Check(
  RtemsTaskReqConstructErrors_Context    *ctx,
  RtemsTaskReqConstructErrors_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Post_Status_Ok: {
      /*
       * The return status of rtems_task_construct() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_construct() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_InvName: {
      /*
       * The return status of rtems_task_construct() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_InvPrio: {
      /*
       * The return status of rtems_task_construct() shall be
       * RTEMS_INVALID_PRIORITY.
       */
      T_rsc( ctx->status, RTEMS_INVALID_PRIORITY );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_InvSize: {
      /*
       * The return status of rtems_task_construct() shall be
       * RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_TooMany: {
      /*
       * The return status of rtems_task_construct() shall be RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_Unsat: {
      /*
       * The return status of rtems_task_construct() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED  );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Post_Name_Check(
  RtemsTaskReqConstructErrors_Context  *ctx,
  RtemsTaskReqConstructErrors_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsTaskReqConstructErrors_Post_Name_Valid: {
      /*
       * The unique object name shall identify the task constructed by the
       * rtems_task_construct() call.
       */
      id = 0;
      sc = rtems_task_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a task.
       */
      sc = rtems_task_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Name_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Post_IdVar_Check(
  RtemsTaskReqConstructErrors_Context   *ctx,
  RtemsTaskReqConstructErrors_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the object identifier of the constructed task after the return
       * of the rtems_task_construct() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_task_construct() shall not be accessed by the
       * rtems_task_construct() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_IdVar_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Post_CreateExt_Check(
  RtemsTaskReqConstructErrors_Context       *ctx,
  RtemsTaskReqConstructErrors_Post_CreateExt state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Post_CreateExt_Yes: {
      /*
       * The create user extensions shall be invoked during the
       * rtems_task_construct() call.
       */
      T_eq_u32( ctx->create_extension_calls, 1 );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_CreateExt_No: {
      /*
       * The create user extensions shall not be invoked during the
       * rtems_task_construct() call.
       */
      T_eq_u32( ctx->create_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_CreateExt_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Post_DelExt_Check(
  RtemsTaskReqConstructErrors_Context    *ctx,
  RtemsTaskReqConstructErrors_Post_DelExt state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Post_DelExt_Yes: {
      /*
       * The delete user extensions shall be invoked during the
       * rtems_task_construct() call.
       */
      T_eq_u32( ctx->delete_extension_calls, 1 );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_DelExt_No: {
      /*
       * The delete user extensions shall not be invoked during the
       * rtems_task_construct() call.
       */
      T_eq_u32( ctx->delete_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_DelExt_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Post_StoFree_Check(
  RtemsTaskReqConstructErrors_Context     *ctx,
  RtemsTaskReqConstructErrors_Post_StoFree state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Post_StoFree_Yes: {
      /*
       * The storage free handler of the task configuration shall be invoked
       * during the rtems_task_construct() call.
       */
      T_eq_u32( ctx->storage_free_calls, 1 );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_StoFree_No: {
      /*
       * The storage free handler of the task configuration shall not be
       * invoked during the rtems_task_construct() call.
       */
      T_eq_u32( ctx->storage_free_calls, 0 );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_StoFree_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Setup(
  RtemsTaskReqConstructErrors_Context *ctx
)
{
  rtems_status_code sc;
  int var;

  var = tls_variable;
  RTEMS_OBFUSCATE_VARIABLE( var );
  tls_variable = var;

  sc = rtems_extension_create(
    rtems_build_name( 'T', 'E', 'X', 'T' ),
    &extensions,
    &ctx->extension_id
  );
  T_rsc_success( sc );
}

static void RtemsTaskReqConstructErrors_Setup_Wrap( void *arg )
{
  RtemsTaskReqConstructErrors_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsTaskReqConstructErrors_Setup( ctx );
}

static void RtemsTaskReqConstructErrors_Teardown(
  RtemsTaskReqConstructErrors_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_extension_delete( ctx->extension_id );
  T_rsc_success( sc );
}

static void RtemsTaskReqConstructErrors_Teardown_Wrap( void *arg )
{
  RtemsTaskReqConstructErrors_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsTaskReqConstructErrors_Teardown( ctx );
}

static void RtemsTaskReqConstructErrors_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx
)
{
  _RTEMS_Lock_allocator();
  _Thread_Kill_zombies();
  _RTEMS_Unlock_allocator();

  ctx->id_value = INVALID_ID;
  memset( &ctx->config, 0, sizeof( ctx->config ) );
  ctx->config.storage_area = task_storage,
  ctx->config.storage_free = StorageFree;
}

static void RtemsTaskReqConstructErrors_Action(
  RtemsTaskReqConstructErrors_Context *ctx
)
{
  ctx->create_extension_calls = 0;
  ctx->delete_extension_calls = 0;
  ctx->storage_free_calls = 0;
  ctx->config.storage_size = RTEMS_TASK_STORAGE_SIZE(
    ctx->config.maximum_thread_local_storage_size + ctx->stack_size,
    ctx->config.attributes
  );
  ctx->status = rtems_task_construct( &ctx->config, ctx->id );
}

static void RtemsTaskReqConstructErrors_Cleanup(
  RtemsTaskReqConstructErrors_Context *ctx
)
{
  if ( ctx->id_value != INVALID_ID ) {
    rtems_status_code sc;

    sc = rtems_task_delete( ctx->id_value );
    T_rsc_success( sc );

    ctx->id_value = INVALID_ID;
  }

  T_surrender_objects( &ctx->seized_objects, rtems_task_delete );
}

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Name_NA : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_SysTsk_NA : 1;
  uint32_t Pre_Prio_NA : 1;
  uint32_t Pre_Free_NA : 1;
  uint32_t Pre_TLS_NA : 1;
  uint32_t Pre_Stack_NA : 1;
  uint32_t Pre_Ext_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Name : 2;
  uint32_t Post_IdVar : 2;
  uint32_t Post_CreateExt : 2;
  uint32_t Post_DelExt : 2;
  uint32_t Post_StoFree : 2;
} RtemsTaskReqConstructErrors_Entry;

static const RtemsTaskReqConstructErrors_Entry
RtemsTaskReqConstructErrors_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqConstructErrors_Post_Status_InvName,
    RtemsTaskReqConstructErrors_Post_Name_Invalid,
    RtemsTaskReqConstructErrors_Post_IdVar_Nop,
    RtemsTaskReqConstructErrors_Post_CreateExt_No,
    RtemsTaskReqConstructErrors_Post_DelExt_No,
    RtemsTaskReqConstructErrors_Post_StoFree_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqConstructErrors_Post_Status_InvAddr,
    RtemsTaskReqConstructErrors_Post_Name_Invalid,
    RtemsTaskReqConstructErrors_Post_IdVar_Nop,
    RtemsTaskReqConstructErrors_Post_CreateExt_No,
    RtemsTaskReqConstructErrors_Post_DelExt_No,
    RtemsTaskReqConstructErrors_Post_StoFree_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqConstructErrors_Post_Status_InvPrio,
    RtemsTaskReqConstructErrors_Post_Name_Invalid,
    RtemsTaskReqConstructErrors_Post_IdVar_Nop,
    RtemsTaskReqConstructErrors_Post_CreateExt_No,
    RtemsTaskReqConstructErrors_Post_DelExt_No,
    RtemsTaskReqConstructErrors_Post_StoFree_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqConstructErrors_Post_Status_TooMany,
    RtemsTaskReqConstructErrors_Post_Name_Invalid,
    RtemsTaskReqConstructErrors_Post_IdVar_Nop,
    RtemsTaskReqConstructErrors_Post_CreateExt_No,
    RtemsTaskReqConstructErrors_Post_DelExt_No,
    RtemsTaskReqConstructErrors_Post_StoFree_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqConstructErrors_Post_Status_InvSize,
    RtemsTaskReqConstructErrors_Post_Name_Invalid,
    RtemsTaskReqConstructErrors_Post_IdVar_Nop,
    RtemsTaskReqConstructErrors_Post_CreateExt_No,
    RtemsTaskReqConstructErrors_Post_DelExt_No,
    RtemsTaskReqConstructErrors_Post_StoFree_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqConstructErrors_Post_Status_Ok,
    RtemsTaskReqConstructErrors_Post_Name_Valid,
    RtemsTaskReqConstructErrors_Post_IdVar_Set,
    RtemsTaskReqConstructErrors_Post_CreateExt_Yes,
    RtemsTaskReqConstructErrors_Post_DelExt_No,
    RtemsTaskReqConstructErrors_Post_StoFree_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqConstructErrors_Post_Status_Unsat,
    RtemsTaskReqConstructErrors_Post_Name_Invalid,
    RtemsTaskReqConstructErrors_Post_IdVar_Nop,
    RtemsTaskReqConstructErrors_Post_CreateExt_Yes,
    RtemsTaskReqConstructErrors_Post_DelExt_Yes,
    RtemsTaskReqConstructErrors_Post_StoFree_Yes }
};

static const uint8_t
RtemsTaskReqConstructErrors_Map[] = {
  5, 6, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 5, 6, 4, 4, 4, 4, 4, 4, 3, 3,
  3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5, 6, 4, 4,
  4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static size_t RtemsTaskReqConstructErrors_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsTaskReqConstructErrors_Context *ctx;

  ctx = arg;

  if ( ctx->in_action_loop ) {
    return T_get_scope(
      RtemsTaskReqConstructErrors_PreDesc,
      buf,
      n,
      ctx->pcs
    );
  }

  return 0;
}

static T_fixture RtemsTaskReqConstructErrors_Fixture = {
  .setup = RtemsTaskReqConstructErrors_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqConstructErrors_Teardown_Wrap,
  .scope = RtemsTaskReqConstructErrors_Scope,
  .initial_context = &RtemsTaskReqConstructErrors_Instance
};

static inline RtemsTaskReqConstructErrors_Entry
RtemsTaskReqConstructErrors_GetEntry( size_t index )
{
  return RtemsTaskReqConstructErrors_Entries[
    RtemsTaskReqConstructErrors_Map[ index ]
  ];
}

/**
 * @fn void T_case_body_RtemsTaskReqConstructErrors( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskReqConstructErrors,
  &RtemsTaskReqConstructErrors_Fixture
)
{
  RtemsTaskReqConstructErrors_Context *ctx;
  RtemsTaskReqConstructErrors_Entry entry;
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsTaskReqConstructErrors_Pre_Name_Valid;
    ctx->pcs[ 0 ] < RtemsTaskReqConstructErrors_Pre_Name_NA;
    ++ctx->pcs[ 0 ]
  ) {
    entry = RtemsTaskReqConstructErrors_GetEntry( index );

    if ( entry.Pre_Name_NA ) {
      ctx->pcs[ 0 ] = RtemsTaskReqConstructErrors_Pre_Name_NA;
      index += ( RtemsTaskReqConstructErrors_Pre_Name_NA - 1 )
        * RtemsTaskReqConstructErrors_Pre_Id_NA
        * RtemsTaskReqConstructErrors_Pre_SysTsk_NA
        * RtemsTaskReqConstructErrors_Pre_Prio_NA
        * RtemsTaskReqConstructErrors_Pre_Free_NA
        * RtemsTaskReqConstructErrors_Pre_TLS_NA
        * RtemsTaskReqConstructErrors_Pre_Stack_NA
        * RtemsTaskReqConstructErrors_Pre_Ext_NA;
    }

    for (
      ctx->pcs[ 1 ] = RtemsTaskReqConstructErrors_Pre_Id_Valid;
      ctx->pcs[ 1 ] < RtemsTaskReqConstructErrors_Pre_Id_NA;
      ++ctx->pcs[ 1 ]
    ) {
      entry = RtemsTaskReqConstructErrors_GetEntry( index );

      if ( entry.Pre_Id_NA ) {
        ctx->pcs[ 1 ] = RtemsTaskReqConstructErrors_Pre_Id_NA;
        index += ( RtemsTaskReqConstructErrors_Pre_Id_NA - 1 )
          * RtemsTaskReqConstructErrors_Pre_SysTsk_NA
          * RtemsTaskReqConstructErrors_Pre_Prio_NA
          * RtemsTaskReqConstructErrors_Pre_Free_NA
          * RtemsTaskReqConstructErrors_Pre_TLS_NA
          * RtemsTaskReqConstructErrors_Pre_Stack_NA
          * RtemsTaskReqConstructErrors_Pre_Ext_NA;
      }

      for (
        ctx->pcs[ 2 ] = RtemsTaskReqConstructErrors_Pre_SysTsk_Yes;
        ctx->pcs[ 2 ] < RtemsTaskReqConstructErrors_Pre_SysTsk_NA;
        ++ctx->pcs[ 2 ]
      ) {
        entry = RtemsTaskReqConstructErrors_GetEntry( index );

        if ( entry.Pre_SysTsk_NA ) {
          ctx->pcs[ 2 ] = RtemsTaskReqConstructErrors_Pre_SysTsk_NA;
          index += ( RtemsTaskReqConstructErrors_Pre_SysTsk_NA - 1 )
            * RtemsTaskReqConstructErrors_Pre_Prio_NA
            * RtemsTaskReqConstructErrors_Pre_Free_NA
            * RtemsTaskReqConstructErrors_Pre_TLS_NA
            * RtemsTaskReqConstructErrors_Pre_Stack_NA
            * RtemsTaskReqConstructErrors_Pre_Ext_NA;
        }

        for (
          ctx->pcs[ 3 ] = RtemsTaskReqConstructErrors_Pre_Prio_Valid;
          ctx->pcs[ 3 ] < RtemsTaskReqConstructErrors_Pre_Prio_NA;
          ++ctx->pcs[ 3 ]
        ) {
          entry = RtemsTaskReqConstructErrors_GetEntry( index );

          if ( entry.Pre_Prio_NA ) {
            ctx->pcs[ 3 ] = RtemsTaskReqConstructErrors_Pre_Prio_NA;
            index += ( RtemsTaskReqConstructErrors_Pre_Prio_NA - 1 )
              * RtemsTaskReqConstructErrors_Pre_Free_NA
              * RtemsTaskReqConstructErrors_Pre_TLS_NA
              * RtemsTaskReqConstructErrors_Pre_Stack_NA
              * RtemsTaskReqConstructErrors_Pre_Ext_NA;
          }

          for (
            ctx->pcs[ 4 ] = RtemsTaskReqConstructErrors_Pre_Free_Yes;
            ctx->pcs[ 4 ] < RtemsTaskReqConstructErrors_Pre_Free_NA;
            ++ctx->pcs[ 4 ]
          ) {
            entry = RtemsTaskReqConstructErrors_GetEntry( index );

            if ( entry.Pre_Free_NA ) {
              ctx->pcs[ 4 ] = RtemsTaskReqConstructErrors_Pre_Free_NA;
              index += ( RtemsTaskReqConstructErrors_Pre_Free_NA - 1 )
                * RtemsTaskReqConstructErrors_Pre_TLS_NA
                * RtemsTaskReqConstructErrors_Pre_Stack_NA
                * RtemsTaskReqConstructErrors_Pre_Ext_NA;
            }

            for (
              ctx->pcs[ 5 ] = RtemsTaskReqConstructErrors_Pre_TLS_Enough;
              ctx->pcs[ 5 ] < RtemsTaskReqConstructErrors_Pre_TLS_NA;
              ++ctx->pcs[ 5 ]
            ) {
              entry = RtemsTaskReqConstructErrors_GetEntry( index );

              if ( entry.Pre_TLS_NA ) {
                ctx->pcs[ 5 ] = RtemsTaskReqConstructErrors_Pre_TLS_NA;
                index += ( RtemsTaskReqConstructErrors_Pre_TLS_NA - 1 )
                  * RtemsTaskReqConstructErrors_Pre_Stack_NA
                  * RtemsTaskReqConstructErrors_Pre_Ext_NA;
              }

              for (
                ctx->pcs[ 6 ] = RtemsTaskReqConstructErrors_Pre_Stack_Enough;
                ctx->pcs[ 6 ] < RtemsTaskReqConstructErrors_Pre_Stack_NA;
                ++ctx->pcs[ 6 ]
              ) {
                entry = RtemsTaskReqConstructErrors_GetEntry( index );

                if ( entry.Pre_Stack_NA ) {
                  ctx->pcs[ 6 ] = RtemsTaskReqConstructErrors_Pre_Stack_NA;
                  index += ( RtemsTaskReqConstructErrors_Pre_Stack_NA - 1 )
                    * RtemsTaskReqConstructErrors_Pre_Ext_NA;
                }

                for (
                  ctx->pcs[ 7 ] = RtemsTaskReqConstructErrors_Pre_Ext_Ok;
                  ctx->pcs[ 7 ] < RtemsTaskReqConstructErrors_Pre_Ext_NA;
                  ++ctx->pcs[ 7 ]
                ) {
                  entry = RtemsTaskReqConstructErrors_GetEntry( index );

                  if ( entry.Pre_Ext_NA ) {
                    ctx->pcs[ 7 ] = RtemsTaskReqConstructErrors_Pre_Ext_NA;
                    index += ( RtemsTaskReqConstructErrors_Pre_Ext_NA - 1 );
                  }

                  if ( entry.Skip ) {
                    ++index;
                    continue;
                  }

                  RtemsTaskReqConstructErrors_Prepare( ctx );
                  RtemsTaskReqConstructErrors_Pre_Name_Prepare(
                    ctx,
                    ctx->pcs[ 0 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Id_Prepare(
                    ctx,
                    ctx->pcs[ 1 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_SysTsk_Prepare(
                    ctx,
                    ctx->pcs[ 2 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Prio_Prepare(
                    ctx,
                    ctx->pcs[ 3 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Free_Prepare(
                    ctx,
                    ctx->pcs[ 4 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_TLS_Prepare(
                    ctx,
                    ctx->pcs[ 5 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Stack_Prepare(
                    ctx,
                    ctx->pcs[ 6 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Ext_Prepare(
                    ctx,
                    ctx->pcs[ 7 ]
                  );
                  RtemsTaskReqConstructErrors_Action( ctx );
                  RtemsTaskReqConstructErrors_Post_Status_Check(
                    ctx,
                    entry.Post_Status
                  );
                  RtemsTaskReqConstructErrors_Post_Name_Check(
                    ctx,
                    entry.Post_Name
                  );
                  RtemsTaskReqConstructErrors_Post_IdVar_Check(
                    ctx,
                    entry.Post_IdVar
                  );
                  RtemsTaskReqConstructErrors_Post_CreateExt_Check(
                    ctx,
                    entry.Post_CreateExt
                  );
                  RtemsTaskReqConstructErrors_Post_DelExt_Check(
                    ctx,
                    entry.Post_DelExt
                  );
                  RtemsTaskReqConstructErrors_Post_StoFree_Check(
                    ctx,
                    entry.Post_StoFree
                  );
                  RtemsTaskReqConstructErrors_Cleanup( ctx );
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
