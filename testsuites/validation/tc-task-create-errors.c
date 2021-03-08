/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsTaskReqCreateErrors
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
 * @defgroup RTEMSTestCaseRtemsTaskReqCreateErrors \
 *   spec:/rtems/task/req/create-errors
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqCreateErrors_Pre_Id_Valid,
  RtemsTaskReqCreateErrors_Pre_Id_Null,
  RtemsTaskReqCreateErrors_Pre_Id_NA
} RtemsTaskReqCreateErrors_Pre_Id;

typedef enum {
  RtemsTaskReqCreateErrors_Pre_Name_Valid,
  RtemsTaskReqCreateErrors_Pre_Name_Inv,
  RtemsTaskReqCreateErrors_Pre_Name_NA
} RtemsTaskReqCreateErrors_Pre_Name;

typedef enum {
  RtemsTaskReqCreateErrors_Pre_SysTsk_Yes,
  RtemsTaskReqCreateErrors_Pre_SysTsk_No,
  RtemsTaskReqCreateErrors_Pre_SysTsk_NA
} RtemsTaskReqCreateErrors_Pre_SysTsk;

typedef enum {
  RtemsTaskReqCreateErrors_Pre_Prio_Valid,
  RtemsTaskReqCreateErrors_Pre_Prio_Zero,
  RtemsTaskReqCreateErrors_Pre_Prio_Inv,
  RtemsTaskReqCreateErrors_Pre_Prio_NA
} RtemsTaskReqCreateErrors_Pre_Prio;

typedef enum {
  RtemsTaskReqCreateErrors_Pre_Free_Yes,
  RtemsTaskReqCreateErrors_Pre_Free_No,
  RtemsTaskReqCreateErrors_Pre_Free_NA
} RtemsTaskReqCreateErrors_Pre_Free;

typedef enum {
  RtemsTaskReqCreateErrors_Pre_Stack_Normal,
  RtemsTaskReqCreateErrors_Pre_Stack_Small,
  RtemsTaskReqCreateErrors_Pre_Stack_Huge,
  RtemsTaskReqCreateErrors_Pre_Stack_NA
} RtemsTaskReqCreateErrors_Pre_Stack;

typedef enum {
  RtemsTaskReqCreateErrors_Pre_Ext_Ok,
  RtemsTaskReqCreateErrors_Pre_Ext_Err,
  RtemsTaskReqCreateErrors_Pre_Ext_NA
} RtemsTaskReqCreateErrors_Pre_Ext;

typedef enum {
  RtemsTaskReqCreateErrors_Post_Status_Ok,
  RtemsTaskReqCreateErrors_Post_Status_InvAddr,
  RtemsTaskReqCreateErrors_Post_Status_InvName,
  RtemsTaskReqCreateErrors_Post_Status_InvPrio,
  RtemsTaskReqCreateErrors_Post_Status_TooMany,
  RtemsTaskReqCreateErrors_Post_Status_Unsat,
  RtemsTaskReqCreateErrors_Post_Status_NA
} RtemsTaskReqCreateErrors_Post_Status;

typedef enum {
  RtemsTaskReqCreateErrors_Post_Name_Valid,
  RtemsTaskReqCreateErrors_Post_Name_Invalid,
  RtemsTaskReqCreateErrors_Post_Name_NA
} RtemsTaskReqCreateErrors_Post_Name;

typedef enum {
  RtemsTaskReqCreateErrors_Post_IdValue_Assigned,
  RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
  RtemsTaskReqCreateErrors_Post_IdValue_NA
} RtemsTaskReqCreateErrors_Post_IdValue;

typedef enum {
  RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
  RtemsTaskReqCreateErrors_Post_CreateExt_No,
  RtemsTaskReqCreateErrors_Post_CreateExt_NA
} RtemsTaskReqCreateErrors_Post_CreateExt;

typedef enum {
  RtemsTaskReqCreateErrors_Post_DelExt_Yes,
  RtemsTaskReqCreateErrors_Post_DelExt_No,
  RtemsTaskReqCreateErrors_Post_DelExt_NA
} RtemsTaskReqCreateErrors_Post_DelExt;

/**
 * @brief Test context for spec:/rtems/task/req/create-errors test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_id *id;

  rtems_id id_value;

  bool create_extension_status;

  uint32_t create_extension_calls;

  uint32_t delete_extension_calls;

  rtems_name name;

  rtems_task_priority initial_priority;

  size_t stack_size;

  rtems_attribute attributes;

  rtems_id extension_id;

  void *seized_objects;

  /**
   * @brief This member defines the pre-condition states for the next action.
   */
  size_t pcs[ 7 ];

  /**
   * @brief This member indicates if the test action loop is currently
   *   executed.
   */
  bool in_action_loop;
} RtemsTaskReqCreateErrors_Context;

static RtemsTaskReqCreateErrors_Context
  RtemsTaskReqCreateErrors_Instance;

static const char * const RtemsTaskReqCreateErrors_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsTaskReqCreateErrors_PreDesc_Name[] = {
  "Valid",
  "Inv",
  "NA"
};

static const char * const RtemsTaskReqCreateErrors_PreDesc_SysTsk[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqCreateErrors_PreDesc_Prio[] = {
  "Valid",
  "Zero",
  "Inv",
  "NA"
};

static const char * const RtemsTaskReqCreateErrors_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqCreateErrors_PreDesc_Stack[] = {
  "Normal",
  "Small",
  "Huge",
  "NA"
};

static const char * const RtemsTaskReqCreateErrors_PreDesc_Ext[] = {
  "Ok",
  "Err",
  "NA"
};

static const char * const * const RtemsTaskReqCreateErrors_PreDesc[] = {
  RtemsTaskReqCreateErrors_PreDesc_Id,
  RtemsTaskReqCreateErrors_PreDesc_Name,
  RtemsTaskReqCreateErrors_PreDesc_SysTsk,
  RtemsTaskReqCreateErrors_PreDesc_Prio,
  RtemsTaskReqCreateErrors_PreDesc_Free,
  RtemsTaskReqCreateErrors_PreDesc_Stack,
  RtemsTaskReqCreateErrors_PreDesc_Ext,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

#define INVALID_ID 0xffffffff

typedef RtemsTaskReqCreateErrors_Context Context;

static rtems_status_code Create( void *arg, uint32_t *id )
{
  Context          *ctx;
  bool              create_extension_status;
  rtems_status_code sc;

  ctx = arg;
  create_extension_status = ctx->create_extension_status;
  ctx->create_extension_status = true;
  sc = rtems_task_create(
    rtems_build_name( 'S', 'I', 'Z', 'E' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    id
  );
  ctx->create_extension_status = create_extension_status;

  return sc;
}

static bool ThreadCreate( rtems_tcb *executing, rtems_tcb *created )
{
  (void) executing;
  (void) created;

  ++RtemsTaskReqCreateErrors_Instance.create_extension_calls;
  return RtemsTaskReqCreateErrors_Instance.create_extension_status;
}

static void ThreadDelete( rtems_tcb *executing, rtems_tcb *deleted )
{
  (void) executing;
  (void) deleted;

  ++RtemsTaskReqCreateErrors_Instance.delete_extension_calls;
}

static const rtems_extensions_table extensions = {
  .thread_create = ThreadCreate,
  .thread_delete = ThreadDelete
};

static void RtemsTaskReqCreateErrors_Pre_Id_Prepare(
  RtemsTaskReqCreateErrors_Context *ctx,
  RtemsTaskReqCreateErrors_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Pre_Id_Valid: {
      /*
       * The ``id`` parameter shall reference an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Id_Null: {
      /*
       * The ``id`` parameter shall be NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Pre_Name_Prepare(
  RtemsTaskReqCreateErrors_Context *ctx,
  RtemsTaskReqCreateErrors_Pre_Name state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Pre_Name_Valid: {
      /*
       * The ``name`` parameter shall be valid.
       */
      ctx->name = NAME;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Name_Inv: {
      /*
       * The ``name`` parameter shall be invalid.
       */
      ctx->name = 0;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Name_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Pre_SysTsk_Prepare(
  RtemsTaskReqCreateErrors_Context   *ctx,
  RtemsTaskReqCreateErrors_Pre_SysTsk state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Pre_SysTsk_Yes: {
      /*
       * The ``attribute_set`` parameter shall specify a system task.
       */
      ctx->attributes = RTEMS_SYSTEM_TASK;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_SysTsk_No: {
      /*
       * The ``attribute_set`` parameter shall specify an application task.
       */
      ctx->attributes = RTEMS_DEFAULT_ATTRIBUTES;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_SysTsk_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Pre_Prio_Prepare(
  RtemsTaskReqCreateErrors_Context *ctx,
  RtemsTaskReqCreateErrors_Pre_Prio state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Pre_Prio_Valid: {
      /*
       * The ``initial_priority`` parameter shall be valid and non-zero.
       */
      ctx->initial_priority = 254;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Prio_Zero: {
      /*
       * The ``initial_priority`` parameter shall be zero.
       */
      ctx->initial_priority = 0;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Prio_Inv: {
      /*
       * The ``initial_priority`` parameter shall be invalid.
       */
      ctx->initial_priority = 0xffffffff;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Prio_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Pre_Free_Prepare(
  RtemsTaskReqCreateErrors_Context *ctx,
  RtemsTaskReqCreateErrors_Pre_Free state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Pre_Free_Yes: {
      /*
       * The system shall have at least one inactive task object available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Free_No: {
      /*
       * The system shall have no inactive task object available.
       */
      ctx->seized_objects = T_seize_objects( Create, ctx );
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Free_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Pre_Stack_Prepare(
  RtemsTaskReqCreateErrors_Context  *ctx,
  RtemsTaskReqCreateErrors_Pre_Stack state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Pre_Stack_Normal: {
      /*
       * The ``initial_priority`` parameter shall be greater than or equal to
       * the configured minimum size and less than or equal to the maximum
       * stack size which can be allocated by the system.
       */
      ctx->stack_size = RTEMS_MINIMUM_STACK_SIZE;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Stack_Small: {
      /*
       * The ``initial_priority`` parameter shall be less than the configured
       * minimum size.
       */
      ctx->stack_size = 0;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Stack_Huge: {
      /*
       * The ``initial_priority`` parameter shall be greater than the maximum
       * stack size which can be allocated by the system.
       */
      ctx->stack_size = SIZE_MAX;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Stack_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Pre_Ext_Prepare(
  RtemsTaskReqCreateErrors_Context *ctx,
  RtemsTaskReqCreateErrors_Pre_Ext  state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Pre_Ext_Ok: {
      /*
       * None of the task create extensions shall fail.
       */
      ctx->create_extension_status = true;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Ext_Err: {
      /*
       * At least one of the task create extensions shall fail.
       */
      ctx->create_extension_status = false;
      break;
    }

    case RtemsTaskReqCreateErrors_Pre_Ext_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Post_Status_Check(
  RtemsTaskReqCreateErrors_Context    *ctx,
  RtemsTaskReqCreateErrors_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Post_Status_Ok: {
      /*
       * The return status of rtems_task_create() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_create() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_Status_InvName: {
      /*
       * The return status of rtems_task_create() shall be RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_Status_InvPrio: {
      /*
       * The return status of rtems_task_create() shall be
       * RTEMS_INVALID_PRIORITY.
       */
      T_rsc( ctx->status, RTEMS_INVALID_PRIORITY );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_Status_TooMany: {
      /*
       * The return status of rtems_task_create() shall be RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_Status_Unsat: {
      /*
       * The return status of rtems_task_create() shall be RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED  );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Post_Name_Check(
  RtemsTaskReqCreateErrors_Context  *ctx,
  RtemsTaskReqCreateErrors_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsTaskReqCreateErrors_Post_Name_Valid: {
      /*
       * The unique object name shall identify the task created by the
       * rtems_task_create() call.
       */
      id = 0;
      sc = rtems_task_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a task.
       */
      sc = rtems_task_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_Name_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Post_IdValue_Check(
  RtemsTaskReqCreateErrors_Context     *ctx,
  RtemsTaskReqCreateErrors_Post_IdValue state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Post_IdValue_Assigned: {
      /*
       * The value of the object identifier variable shall be equal to the
       * object identifier of the task created by the rtems_task_create() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_IdValue_Unchanged: {
      /*
       * The value of the object identifier variable shall be unchanged by the
       * rtems_task_create() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_IdValue_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Post_CreateExt_Check(
  RtemsTaskReqCreateErrors_Context       *ctx,
  RtemsTaskReqCreateErrors_Post_CreateExt state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Post_CreateExt_Yes: {
      /*
       * The create user extensions shall be invoked during the
       * rtems_task_create() call.
       */
      T_eq_u32( ctx->create_extension_calls, 1 );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_CreateExt_No: {
      /*
       * The create user extensions shall not be invoked during the
       * rtems_task_create() call.
       */
      T_eq_u32( ctx->create_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_CreateExt_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Post_DelExt_Check(
  RtemsTaskReqCreateErrors_Context    *ctx,
  RtemsTaskReqCreateErrors_Post_DelExt state
)
{
  switch ( state ) {
    case RtemsTaskReqCreateErrors_Post_DelExt_Yes: {
      /*
       * The delete user extensions shall be invoked during the
       * rtems_task_create() call.
       */
      T_eq_u32( ctx->delete_extension_calls, 1 );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_DelExt_No: {
      /*
       * The delete user extensions shall not be invoked during the
       * rtems_task_create() call.
       */
      T_eq_u32( ctx->delete_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqCreateErrors_Post_DelExt_NA:
      break;
  }
}

static void RtemsTaskReqCreateErrors_Setup(
  RtemsTaskReqCreateErrors_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_extension_create(
    rtems_build_name( 'T', 'E', 'X', 'T' ),
    &extensions,
    &ctx->extension_id
  );
  T_rsc_success( sc );
}

static void RtemsTaskReqCreateErrors_Setup_Wrap( void *arg )
{
  RtemsTaskReqCreateErrors_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsTaskReqCreateErrors_Setup( ctx );
}

static void RtemsTaskReqCreateErrors_Teardown(
  RtemsTaskReqCreateErrors_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_extension_delete( ctx->extension_id );
  T_rsc_success( sc );
}

static void RtemsTaskReqCreateErrors_Teardown_Wrap( void *arg )
{
  RtemsTaskReqCreateErrors_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsTaskReqCreateErrors_Teardown( ctx );
}

static size_t RtemsTaskReqCreateErrors_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqCreateErrors_Context *ctx;

  ctx = arg;

  if ( ctx->in_action_loop ) {
    return T_get_scope( RtemsTaskReqCreateErrors_PreDesc, buf, n, ctx->pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqCreateErrors_Fixture = {
  .setup = RtemsTaskReqCreateErrors_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqCreateErrors_Teardown_Wrap,
  .scope = RtemsTaskReqCreateErrors_Scope,
  .initial_context = &RtemsTaskReqCreateErrors_Instance
};

static const uint8_t RtemsTaskReqCreateErrors_TransitionMap[][ 5 ] = {
  {
    RtemsTaskReqCreateErrors_Post_Status_Ok,
    RtemsTaskReqCreateErrors_Post_Name_Valid,
    RtemsTaskReqCreateErrors_Post_IdValue_Assigned,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_Yes
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Ok,
    RtemsTaskReqCreateErrors_Post_Name_Valid,
    RtemsTaskReqCreateErrors_Post_IdValue_Assigned,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_Yes
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Ok,
    RtemsTaskReqCreateErrors_Post_Name_Valid,
    RtemsTaskReqCreateErrors_Post_IdValue_Assigned,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_Yes
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Ok,
    RtemsTaskReqCreateErrors_Post_Name_Valid,
    RtemsTaskReqCreateErrors_Post_IdValue_Assigned,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_Yes
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Ok,
    RtemsTaskReqCreateErrors_Post_Name_Valid,
    RtemsTaskReqCreateErrors_Post_IdValue_Assigned,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_Yes
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Ok,
    RtemsTaskReqCreateErrors_Post_Name_Valid,
    RtemsTaskReqCreateErrors_Post_IdValue_Assigned,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_Yes,
    RtemsTaskReqCreateErrors_Post_DelExt_Yes
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_Unsat,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_TooMany,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvPrio,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvAddr,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }, {
    RtemsTaskReqCreateErrors_Post_Status_InvName,
    RtemsTaskReqCreateErrors_Post_Name_Invalid,
    RtemsTaskReqCreateErrors_Post_IdValue_Unchanged,
    RtemsTaskReqCreateErrors_Post_CreateExt_No,
    RtemsTaskReqCreateErrors_Post_DelExt_No
  }
};

static const struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_Name_NA : 1;
  uint8_t Pre_SysTsk_NA : 1;
  uint8_t Pre_Prio_NA : 1;
  uint8_t Pre_Free_NA : 1;
  uint8_t Pre_Stack_NA : 1;
  uint8_t Pre_Ext_NA : 1;
} RtemsTaskReqCreateErrors_TransitionInfo[] = {
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

static void RtemsTaskReqCreateErrors_Prepare(
  RtemsTaskReqCreateErrors_Context *ctx
)
{
  _RTEMS_Lock_allocator();
  _Thread_Kill_zombies();
  _RTEMS_Unlock_allocator();

  ctx->id_value = INVALID_ID;
}

static void RtemsTaskReqCreateErrors_Action(
  RtemsTaskReqCreateErrors_Context *ctx
)
{
  ctx->create_extension_calls = 0;
  ctx->delete_extension_calls = 0;
  ctx->status = rtems_task_create(
    ctx->name,
    ctx->initial_priority,
    ctx->stack_size,
    RTEMS_DEFAULT_MODES,
    ctx->attributes,
    ctx->id
  );
}

static void RtemsTaskReqCreateErrors_Cleanup(
  RtemsTaskReqCreateErrors_Context *ctx
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

/**
 * @fn void T_case_body_RtemsTaskReqCreateErrors( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskReqCreateErrors,
  &RtemsTaskReqCreateErrors_Fixture
)
{
  RtemsTaskReqCreateErrors_Context *ctx;
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsTaskReqCreateErrors_Pre_Id_Valid;
    ctx->pcs[ 0 ] < RtemsTaskReqCreateErrors_Pre_Id_NA;
    ++ctx->pcs[ 0 ]
  ) {
    if ( RtemsTaskReqCreateErrors_TransitionInfo[ index ].Pre_Id_NA ) {
      ctx->pcs[ 0 ] = RtemsTaskReqCreateErrors_Pre_Id_NA;
      index += ( RtemsTaskReqCreateErrors_Pre_Id_NA - 1 )
        * RtemsTaskReqCreateErrors_Pre_Name_NA
        * RtemsTaskReqCreateErrors_Pre_SysTsk_NA
        * RtemsTaskReqCreateErrors_Pre_Prio_NA
        * RtemsTaskReqCreateErrors_Pre_Free_NA
        * RtemsTaskReqCreateErrors_Pre_Stack_NA
        * RtemsTaskReqCreateErrors_Pre_Ext_NA;
    }

    for (
      ctx->pcs[ 1 ] = RtemsTaskReqCreateErrors_Pre_Name_Valid;
      ctx->pcs[ 1 ] < RtemsTaskReqCreateErrors_Pre_Name_NA;
      ++ctx->pcs[ 1 ]
    ) {
      if ( RtemsTaskReqCreateErrors_TransitionInfo[ index ].Pre_Name_NA ) {
        ctx->pcs[ 1 ] = RtemsTaskReqCreateErrors_Pre_Name_NA;
        index += ( RtemsTaskReqCreateErrors_Pre_Name_NA - 1 )
          * RtemsTaskReqCreateErrors_Pre_SysTsk_NA
          * RtemsTaskReqCreateErrors_Pre_Prio_NA
          * RtemsTaskReqCreateErrors_Pre_Free_NA
          * RtemsTaskReqCreateErrors_Pre_Stack_NA
          * RtemsTaskReqCreateErrors_Pre_Ext_NA;
      }

      for (
        ctx->pcs[ 2 ] = RtemsTaskReqCreateErrors_Pre_SysTsk_Yes;
        ctx->pcs[ 2 ] < RtemsTaskReqCreateErrors_Pre_SysTsk_NA;
        ++ctx->pcs[ 2 ]
      ) {
        if ( RtemsTaskReqCreateErrors_TransitionInfo[ index ].Pre_SysTsk_NA ) {
          ctx->pcs[ 2 ] = RtemsTaskReqCreateErrors_Pre_SysTsk_NA;
          index += ( RtemsTaskReqCreateErrors_Pre_SysTsk_NA - 1 )
            * RtemsTaskReqCreateErrors_Pre_Prio_NA
            * RtemsTaskReqCreateErrors_Pre_Free_NA
            * RtemsTaskReqCreateErrors_Pre_Stack_NA
            * RtemsTaskReqCreateErrors_Pre_Ext_NA;
        }

        for (
          ctx->pcs[ 3 ] = RtemsTaskReqCreateErrors_Pre_Prio_Valid;
          ctx->pcs[ 3 ] < RtemsTaskReqCreateErrors_Pre_Prio_NA;
          ++ctx->pcs[ 3 ]
        ) {
          if ( RtemsTaskReqCreateErrors_TransitionInfo[ index ].Pre_Prio_NA ) {
            ctx->pcs[ 3 ] = RtemsTaskReqCreateErrors_Pre_Prio_NA;
            index += ( RtemsTaskReqCreateErrors_Pre_Prio_NA - 1 )
              * RtemsTaskReqCreateErrors_Pre_Free_NA
              * RtemsTaskReqCreateErrors_Pre_Stack_NA
              * RtemsTaskReqCreateErrors_Pre_Ext_NA;
          }

          for (
            ctx->pcs[ 4 ] = RtemsTaskReqCreateErrors_Pre_Free_Yes;
            ctx->pcs[ 4 ] < RtemsTaskReqCreateErrors_Pre_Free_NA;
            ++ctx->pcs[ 4 ]
          ) {
            if ( RtemsTaskReqCreateErrors_TransitionInfo[ index ].Pre_Free_NA ) {
              ctx->pcs[ 4 ] = RtemsTaskReqCreateErrors_Pre_Free_NA;
              index += ( RtemsTaskReqCreateErrors_Pre_Free_NA - 1 )
                * RtemsTaskReqCreateErrors_Pre_Stack_NA
                * RtemsTaskReqCreateErrors_Pre_Ext_NA;
            }

            for (
              ctx->pcs[ 5 ] = RtemsTaskReqCreateErrors_Pre_Stack_Normal;
              ctx->pcs[ 5 ] < RtemsTaskReqCreateErrors_Pre_Stack_NA;
              ++ctx->pcs[ 5 ]
            ) {
              if ( RtemsTaskReqCreateErrors_TransitionInfo[ index ].Pre_Stack_NA ) {
                ctx->pcs[ 5 ] = RtemsTaskReqCreateErrors_Pre_Stack_NA;
                index += ( RtemsTaskReqCreateErrors_Pre_Stack_NA - 1 )
                  * RtemsTaskReqCreateErrors_Pre_Ext_NA;
              }

              for (
                ctx->pcs[ 6 ] = RtemsTaskReqCreateErrors_Pre_Ext_Ok;
                ctx->pcs[ 6 ] < RtemsTaskReqCreateErrors_Pre_Ext_NA;
                ++ctx->pcs[ 6 ]
              ) {
                if ( RtemsTaskReqCreateErrors_TransitionInfo[ index ].Pre_Ext_NA ) {
                  ctx->pcs[ 6 ] = RtemsTaskReqCreateErrors_Pre_Ext_NA;
                  index += ( RtemsTaskReqCreateErrors_Pre_Ext_NA - 1 );
                }

                if ( RtemsTaskReqCreateErrors_TransitionInfo[ index ].Skip ) {
                  ++index;
                  continue;
                }

                RtemsTaskReqCreateErrors_Prepare( ctx );
                RtemsTaskReqCreateErrors_Pre_Id_Prepare( ctx, ctx->pcs[ 0 ] );
                RtemsTaskReqCreateErrors_Pre_Name_Prepare(
                  ctx,
                  ctx->pcs[ 1 ]
                );
                RtemsTaskReqCreateErrors_Pre_SysTsk_Prepare(
                  ctx,
                  ctx->pcs[ 2 ]
                );
                RtemsTaskReqCreateErrors_Pre_Prio_Prepare(
                  ctx,
                  ctx->pcs[ 3 ]
                );
                RtemsTaskReqCreateErrors_Pre_Free_Prepare(
                  ctx,
                  ctx->pcs[ 4 ]
                );
                RtemsTaskReqCreateErrors_Pre_Stack_Prepare(
                  ctx,
                  ctx->pcs[ 5 ]
                );
                RtemsTaskReqCreateErrors_Pre_Ext_Prepare( ctx, ctx->pcs[ 6 ] );
                RtemsTaskReqCreateErrors_Action( ctx );
                RtemsTaskReqCreateErrors_Post_Status_Check(
                  ctx,
                  RtemsTaskReqCreateErrors_TransitionMap[ index ][ 0 ]
                );
                RtemsTaskReqCreateErrors_Post_Name_Check(
                  ctx,
                  RtemsTaskReqCreateErrors_TransitionMap[ index ][ 1 ]
                );
                RtemsTaskReqCreateErrors_Post_IdValue_Check(
                  ctx,
                  RtemsTaskReqCreateErrors_TransitionMap[ index ][ 2 ]
                );
                RtemsTaskReqCreateErrors_Post_CreateExt_Check(
                  ctx,
                  RtemsTaskReqCreateErrors_TransitionMap[ index ][ 3 ]
                );
                RtemsTaskReqCreateErrors_Post_DelExt_Check(
                  ctx,
                  RtemsTaskReqCreateErrors_TransitionMap[ index ][ 4 ]
                );
                RtemsTaskReqCreateErrors_Cleanup( ctx );
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
