/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsTaskReqConstructErrors
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
 * @defgroup RTEMSTestCaseRtemsTaskReqConstructErrors \
 *   spec:/rtems/task/req/construct-errors
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Id_Id,
  RtemsTaskReqConstructErrors_Pre_Id_Null,
  RtemsTaskReqConstructErrors_Pre_Id_NA
} RtemsTaskReqConstructErrors_Pre_Id;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Name_Valid,
  RtemsTaskReqConstructErrors_Pre_Name_Inv,
  RtemsTaskReqConstructErrors_Pre_Name_NA
} RtemsTaskReqConstructErrors_Pre_Name;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Prio_Valid,
  RtemsTaskReqConstructErrors_Pre_Prio_Zero,
  RtemsTaskReqConstructErrors_Pre_Prio_Inv,
  RtemsTaskReqConstructErrors_Pre_Prio_NA
} RtemsTaskReqConstructErrors_Pre_Prio;

typedef enum {
  RtemsTaskReqConstructErrors_Pre_Tasks_Avail,
  RtemsTaskReqConstructErrors_Pre_Tasks_None,
  RtemsTaskReqConstructErrors_Pre_Tasks_NA
} RtemsTaskReqConstructErrors_Pre_Tasks;

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
  RtemsTaskReqConstructErrors_Pre_Preempt_Yes,
  RtemsTaskReqConstructErrors_Pre_Preempt_No,
  RtemsTaskReqConstructErrors_Pre_Preempt_NA
} RtemsTaskReqConstructErrors_Pre_Preempt;

typedef enum {
  RtemsTaskReqConstructErrors_Post_Status_Ok,
  RtemsTaskReqConstructErrors_Post_Status_InvAddress,
  RtemsTaskReqConstructErrors_Post_Status_InvName,
  RtemsTaskReqConstructErrors_Post_Status_InvPrio,
  RtemsTaskReqConstructErrors_Post_Status_InvSize,
  RtemsTaskReqConstructErrors_Post_Status_TooMany,
  RtemsTaskReqConstructErrors_Post_Status_Unsatisfied,
  RtemsTaskReqConstructErrors_Post_Status_NA
} RtemsTaskReqConstructErrors_Post_Status;

/**
 * @brief Test context for spec:/rtems/task/req/construct-errors test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_task_config config;

  rtems_id *id;

  rtems_id id_value;

  bool create_extension_status;

  size_t stack_size;

  rtems_id extension_id;

  Chain_Control tasks;

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

static const char * const RtemsTaskReqConstructErrors_PreDesc_Id[] = {
  "Id",
  "Null",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_Name[] = {
  "Valid",
  "Inv",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_Prio[] = {
  "Valid",
  "Zero",
  "Inv",
  "NA"
};

static const char * const RtemsTaskReqConstructErrors_PreDesc_Tasks[] = {
  "Avail",
  "None",
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

static const char * const RtemsTaskReqConstructErrors_PreDesc_Preempt[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsTaskReqConstructErrors_PreDesc[] = {
  RtemsTaskReqConstructErrors_PreDesc_Id,
  RtemsTaskReqConstructErrors_PreDesc_Name,
  RtemsTaskReqConstructErrors_PreDesc_Prio,
  RtemsTaskReqConstructErrors_PreDesc_Tasks,
  RtemsTaskReqConstructErrors_PreDesc_TLS,
  RtemsTaskReqConstructErrors_PreDesc_Stack,
  RtemsTaskReqConstructErrors_PreDesc_Ext,
  RtemsTaskReqConstructErrors_PreDesc_Preempt,
  NULL
};

static _Thread_local int tls_variable;

#define MAX_TLS_SIZE RTEMS_ALIGN_UP( 128, RTEMS_TASK_STORAGE_ALIGNMENT )

RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT ) static char task_storage[
  RTEMS_TASK_STORAGE_SIZE(
    MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_FLOATING_POINT
  )
];

static const rtems_task_config valid_task_config = {
  .name = rtems_build_name( 'T', 'A', 'S', 'K' ),
  .initial_priority = 1,
  .storage_area = task_storage,
  .storage_size = sizeof( task_storage ),
  .maximum_thread_local_storage_size = MAX_TLS_SIZE,
  .initial_modes = RTEMS_DEFAULT_MODES,
  .attributes = RTEMS_DEFAULT_MODES
};

static bool ThreadCreate( rtems_tcb *executing, rtems_tcb *created )
{
  (void) executing;
  (void) created;

  return RtemsTaskReqConstructErrors_Instance.create_extension_status;
}

static const rtems_extensions_table extensions = {
  .thread_create = ThreadCreate
};

static void RtemsTaskReqConstructErrors_Pre_Id_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx,
  RtemsTaskReqConstructErrors_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Id_Id: {
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Id_Null: {
      ctx->id = NULL;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_Name_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx,
  RtemsTaskReqConstructErrors_Pre_Name state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Name_Valid: {
      ctx->config.name = rtems_build_name( 'N', 'A', 'M', 'E' );
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Name_Inv: {
      ctx->config.name = 0;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Name_NA:
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
      ctx->config.initial_priority = 254;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Prio_Zero: {
      ctx->config.initial_priority = 0;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Prio_Inv: {
      ctx->config.initial_priority = 0xffffffff;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Prio_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_Tasks_Prepare(
  RtemsTaskReqConstructErrors_Context  *ctx,
  RtemsTaskReqConstructErrors_Pre_Tasks state
)
{
  bool create_extension_status;

  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Tasks_Avail: {
      /* Nothing to do */
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Tasks_None: {
      create_extension_status = ctx->create_extension_status;
      ctx->create_extension_status = true;

      while ( true ) {
        rtems_status_code sc;
        rtems_id id;

        sc = rtems_task_construct( &valid_task_config, &id );

        if ( sc == RTEMS_SUCCESSFUL ) {
          Objects_Control           *obj;
          const Objects_Information *info;

          info = _Objects_Get_information_id( id );
          T_quiet_assert_not_null( info );
          obj = _Objects_Get_no_protection( id, info );
          T_quiet_assert_not_null( obj );
          _Chain_Append_unprotected( &ctx->tasks, &obj->Node );
        } else {
          T_quiet_rsc( sc, RTEMS_TOO_MANY );
          break;
        }
      }

      ctx->create_extension_status = create_extension_status;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Tasks_NA:
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
      ctx->config.maximum_thread_local_storage_size = MAX_TLS_SIZE;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_TLS_Small: {
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
      ctx->stack_size = RTEMS_MINIMUM_STACK_SIZE;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Stack_Small: {
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
      ctx->create_extension_status = true;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Ext_Err: {
      ctx->create_extension_status = false;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Ext_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Pre_Preempt_Prepare(
  RtemsTaskReqConstructErrors_Context    *ctx,
  RtemsTaskReqConstructErrors_Pre_Preempt state
)
{
  switch ( state ) {
    case RtemsTaskReqConstructErrors_Pre_Preempt_Yes: {
      ctx->config.initial_modes &= ~RTEMS_PREEMPT_MASK;
      ctx->config.initial_modes |= RTEMS_PREEMPT;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Preempt_No: {
      ctx->config.initial_modes &= ~RTEMS_PREEMPT_MASK;
      ctx->config.initial_modes |= RTEMS_NO_PREEMPT;
      break;
    }

    case RtemsTaskReqConstructErrors_Pre_Preempt_NA:
      break;
  }
}

static void RtemsTaskReqConstructErrors_Post_Status_Check(
  RtemsTaskReqConstructErrors_Context    *ctx,
  RtemsTaskReqConstructErrors_Post_Status state
)
{
  rtems_status_code sc;

  switch ( state ) {
    case RtemsTaskReqConstructErrors_Post_Status_Ok: {
      T_rsc_success( ctx->status );
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, 0xffffffff );

      sc = rtems_task_delete( ctx->id_value );
      T_rsc_success( sc );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_InvAddress: {
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      T_null( ctx->id );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_InvName: {
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_InvPrio: {
      T_rsc( ctx->status, RTEMS_INVALID_PRIORITY );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_InvSize: {
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_TooMany: {
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_Unsatisfied: {
      T_rsc( ctx->status, RTEMS_UNSATISFIED  );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsTaskReqConstructErrors_Post_Status_NA:
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
    rtems_build_name( 'T', 'C', 'F', 'C' ),
    &extensions,
    &ctx->extension_id
  );
  T_rsc_success( sc );

  _Chain_Initialize_empty( &ctx->tasks );
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

static const uint8_t RtemsTaskReqConstructErrors_TransitionMap[][ 1 ] = {
  {
    RtemsTaskReqConstructErrors_Post_Status_Ok
  }, {
#if defined(RTEMS_SMP)
    RtemsTaskReqConstructErrors_Post_Status_Unsatisfied
#else
    RtemsTaskReqConstructErrors_Post_Status_Ok
#endif
  }, {
    RtemsTaskReqConstructErrors_Post_Status_Unsatisfied
  }, {
    RtemsTaskReqConstructErrors_Post_Status_Unsatisfied
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvSize
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_TooMany
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvPrio
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvAddress
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }, {
    RtemsTaskReqConstructErrors_Post_Status_InvName
  }
};

static const struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Prio_NA : 1;
  uint16_t Pre_Tasks_NA : 1;
  uint16_t Pre_TLS_NA : 1;
  uint16_t Pre_Stack_NA : 1;
  uint16_t Pre_Ext_NA : 1;
  uint16_t Pre_Preempt_NA : 1;
} RtemsTaskReqConstructErrors_TransitionInfo[] = {
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
#if defined(RTEMS_SMP)
    0, 0, 0, 0, 0, 0, 0, 0, 0
#else
    0, 0, 0, 0, 0, 0, 0, 0, 0
#endif
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0
  }
};

static void RtemsTaskReqConstructErrors_Prepare(
  RtemsTaskReqConstructErrors_Context *ctx
)
{
  ctx->id_value = 0xffffffff;
  memset( &ctx->config, 0, sizeof( ctx->config ) );
}

static void RtemsTaskReqConstructErrors_Action(
  RtemsTaskReqConstructErrors_Context *ctx
)
{
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
  Chain_Node *node;

  while ( ( node = _Chain_Get_unprotected( &ctx->tasks ) ) ) {
    Objects_Control   *obj;
    rtems_status_code  sc;

    obj = (Objects_Control *) node;
    sc = rtems_task_delete( obj->id );
    T_quiet_rsc_success( sc );
  }
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
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsTaskReqConstructErrors_Pre_Id_Id;
    ctx->pcs[ 0 ] < RtemsTaskReqConstructErrors_Pre_Id_NA;
    ++ctx->pcs[ 0 ]
  ) {
    if ( RtemsTaskReqConstructErrors_TransitionInfo[ index ].Pre_Id_NA ) {
      ctx->pcs[ 0 ] = RtemsTaskReqConstructErrors_Pre_Id_NA;
      index += ( RtemsTaskReqConstructErrors_Pre_Id_NA - 1 )
        * RtemsTaskReqConstructErrors_Pre_Name_NA
        * RtemsTaskReqConstructErrors_Pre_Prio_NA
        * RtemsTaskReqConstructErrors_Pre_Tasks_NA
        * RtemsTaskReqConstructErrors_Pre_TLS_NA
        * RtemsTaskReqConstructErrors_Pre_Stack_NA
        * RtemsTaskReqConstructErrors_Pre_Ext_NA
        * RtemsTaskReqConstructErrors_Pre_Preempt_NA;
    }

    for (
      ctx->pcs[ 1 ] = RtemsTaskReqConstructErrors_Pre_Name_Valid;
      ctx->pcs[ 1 ] < RtemsTaskReqConstructErrors_Pre_Name_NA;
      ++ctx->pcs[ 1 ]
    ) {
      if ( RtemsTaskReqConstructErrors_TransitionInfo[ index ].Pre_Name_NA ) {
        ctx->pcs[ 1 ] = RtemsTaskReqConstructErrors_Pre_Name_NA;
        index += ( RtemsTaskReqConstructErrors_Pre_Name_NA - 1 )
          * RtemsTaskReqConstructErrors_Pre_Prio_NA
          * RtemsTaskReqConstructErrors_Pre_Tasks_NA
          * RtemsTaskReqConstructErrors_Pre_TLS_NA
          * RtemsTaskReqConstructErrors_Pre_Stack_NA
          * RtemsTaskReqConstructErrors_Pre_Ext_NA
          * RtemsTaskReqConstructErrors_Pre_Preempt_NA;
      }

      for (
        ctx->pcs[ 2 ] = RtemsTaskReqConstructErrors_Pre_Prio_Valid;
        ctx->pcs[ 2 ] < RtemsTaskReqConstructErrors_Pre_Prio_NA;
        ++ctx->pcs[ 2 ]
      ) {
        if ( RtemsTaskReqConstructErrors_TransitionInfo[ index ].Pre_Prio_NA ) {
          ctx->pcs[ 2 ] = RtemsTaskReqConstructErrors_Pre_Prio_NA;
          index += ( RtemsTaskReqConstructErrors_Pre_Prio_NA - 1 )
            * RtemsTaskReqConstructErrors_Pre_Tasks_NA
            * RtemsTaskReqConstructErrors_Pre_TLS_NA
            * RtemsTaskReqConstructErrors_Pre_Stack_NA
            * RtemsTaskReqConstructErrors_Pre_Ext_NA
            * RtemsTaskReqConstructErrors_Pre_Preempt_NA;
        }

        for (
          ctx->pcs[ 3 ] = RtemsTaskReqConstructErrors_Pre_Tasks_Avail;
          ctx->pcs[ 3 ] < RtemsTaskReqConstructErrors_Pre_Tasks_NA;
          ++ctx->pcs[ 3 ]
        ) {
          if ( RtemsTaskReqConstructErrors_TransitionInfo[ index ].Pre_Tasks_NA ) {
            ctx->pcs[ 3 ] = RtemsTaskReqConstructErrors_Pre_Tasks_NA;
            index += ( RtemsTaskReqConstructErrors_Pre_Tasks_NA - 1 )
              * RtemsTaskReqConstructErrors_Pre_TLS_NA
              * RtemsTaskReqConstructErrors_Pre_Stack_NA
              * RtemsTaskReqConstructErrors_Pre_Ext_NA
              * RtemsTaskReqConstructErrors_Pre_Preempt_NA;
          }

          for (
            ctx->pcs[ 4 ] = RtemsTaskReqConstructErrors_Pre_TLS_Enough;
            ctx->pcs[ 4 ] < RtemsTaskReqConstructErrors_Pre_TLS_NA;
            ++ctx->pcs[ 4 ]
          ) {
            if ( RtemsTaskReqConstructErrors_TransitionInfo[ index ].Pre_TLS_NA ) {
              ctx->pcs[ 4 ] = RtemsTaskReqConstructErrors_Pre_TLS_NA;
              index += ( RtemsTaskReqConstructErrors_Pre_TLS_NA - 1 )
                * RtemsTaskReqConstructErrors_Pre_Stack_NA
                * RtemsTaskReqConstructErrors_Pre_Ext_NA
                * RtemsTaskReqConstructErrors_Pre_Preempt_NA;
            }

            for (
              ctx->pcs[ 5 ] = RtemsTaskReqConstructErrors_Pre_Stack_Enough;
              ctx->pcs[ 5 ] < RtemsTaskReqConstructErrors_Pre_Stack_NA;
              ++ctx->pcs[ 5 ]
            ) {
              if ( RtemsTaskReqConstructErrors_TransitionInfo[ index ].Pre_Stack_NA ) {
                ctx->pcs[ 5 ] = RtemsTaskReqConstructErrors_Pre_Stack_NA;
                index += ( RtemsTaskReqConstructErrors_Pre_Stack_NA - 1 )
                  * RtemsTaskReqConstructErrors_Pre_Ext_NA
                  * RtemsTaskReqConstructErrors_Pre_Preempt_NA;
              }

              for (
                ctx->pcs[ 6 ] = RtemsTaskReqConstructErrors_Pre_Ext_Ok;
                ctx->pcs[ 6 ] < RtemsTaskReqConstructErrors_Pre_Ext_NA;
                ++ctx->pcs[ 6 ]
              ) {
                if ( RtemsTaskReqConstructErrors_TransitionInfo[ index ].Pre_Ext_NA ) {
                  ctx->pcs[ 6 ] = RtemsTaskReqConstructErrors_Pre_Ext_NA;
                  index += ( RtemsTaskReqConstructErrors_Pre_Ext_NA - 1 )
                    * RtemsTaskReqConstructErrors_Pre_Preempt_NA;
                }

                for (
                  ctx->pcs[ 7 ] = RtemsTaskReqConstructErrors_Pre_Preempt_Yes;
                  ctx->pcs[ 7 ] < RtemsTaskReqConstructErrors_Pre_Preempt_NA;
                  ++ctx->pcs[ 7 ]
                ) {
                  if ( RtemsTaskReqConstructErrors_TransitionInfo[ index ].Pre_Preempt_NA ) {
                    ctx->pcs[ 7 ] = RtemsTaskReqConstructErrors_Pre_Preempt_NA;
                    index += ( RtemsTaskReqConstructErrors_Pre_Preempt_NA - 1 );
                  }

                  if ( RtemsTaskReqConstructErrors_TransitionInfo[ index ].Skip ) {
                    ++index;
                    continue;
                  }

                  RtemsTaskReqConstructErrors_Prepare( ctx );
                  RtemsTaskReqConstructErrors_Pre_Id_Prepare(
                    ctx,
                    ctx->pcs[ 0 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Name_Prepare(
                    ctx,
                    ctx->pcs[ 1 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Prio_Prepare(
                    ctx,
                    ctx->pcs[ 2 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Tasks_Prepare(
                    ctx,
                    ctx->pcs[ 3 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_TLS_Prepare(
                    ctx,
                    ctx->pcs[ 4 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Stack_Prepare(
                    ctx,
                    ctx->pcs[ 5 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Ext_Prepare(
                    ctx,
                    ctx->pcs[ 6 ]
                  );
                  RtemsTaskReqConstructErrors_Pre_Preempt_Prepare(
                    ctx,
                    ctx->pcs[ 7 ]
                  );
                  RtemsTaskReqConstructErrors_Action( ctx );
                  RtemsTaskReqConstructErrors_Post_Status_Check(
                    ctx,
                    RtemsTaskReqConstructErrors_TransitionMap[ index ][ 0 ]
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
