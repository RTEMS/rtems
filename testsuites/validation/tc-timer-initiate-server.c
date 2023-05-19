/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerReqInitiateServer
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
#include <rtems/rtems/timerimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTimerReqInitiateServer spec:/rtems/timer/req/initiate-server
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTimerReqInitiateServer_Pre_Priority_Valid,
  RtemsTimerReqInitiateServer_Pre_Priority_Default,
  RtemsTimerReqInitiateServer_Pre_Priority_Invalid,
  RtemsTimerReqInitiateServer_Pre_Priority_NA
} RtemsTimerReqInitiateServer_Pre_Priority;

typedef enum {
  RtemsTimerReqInitiateServer_Pre_Stack_Allocatable,
  RtemsTimerReqInitiateServer_Pre_Stack_TooLarge,
  RtemsTimerReqInitiateServer_Pre_Stack_NA
} RtemsTimerReqInitiateServer_Pre_Stack;

typedef enum {
  RtemsTimerReqInitiateServer_Pre_Started_Yes,
  RtemsTimerReqInitiateServer_Pre_Started_No,
  RtemsTimerReqInitiateServer_Pre_Started_NA
} RtemsTimerReqInitiateServer_Pre_Started;

typedef enum {
  RtemsTimerReqInitiateServer_Pre_TaskObj_Available,
  RtemsTimerReqInitiateServer_Pre_TaskObj_Unavailable,
  RtemsTimerReqInitiateServer_Pre_TaskObj_NA
} RtemsTimerReqInitiateServer_Pre_TaskObj;

typedef enum {
  RtemsTimerReqInitiateServer_Post_Status_Ok,
  RtemsTimerReqInitiateServer_Post_Status_IncStat,
  RtemsTimerReqInitiateServer_Post_Status_InvPrio,
  RtemsTimerReqInitiateServer_Post_Status_TooMany,
  RtemsTimerReqInitiateServer_Post_Status_Unsat,
  RtemsTimerReqInitiateServer_Post_Status_NA
} RtemsTimerReqInitiateServer_Post_Status;

typedef enum {
  RtemsTimerReqInitiateServer_Post_Started_Yes,
  RtemsTimerReqInitiateServer_Post_Started_No,
  RtemsTimerReqInitiateServer_Post_Started_NA
} RtemsTimerReqInitiateServer_Post_Started;

typedef enum {
  RtemsTimerReqInitiateServer_Post_TaskPrio_Set,
  RtemsTimerReqInitiateServer_Post_TaskPrio_Nop,
  RtemsTimerReqInitiateServer_Post_TaskPrio_NA
} RtemsTimerReqInitiateServer_Post_TaskPrio;

typedef enum {
  RtemsTimerReqInitiateServer_Post_TaskStack_Set,
  RtemsTimerReqInitiateServer_Post_TaskStack_Nop,
  RtemsTimerReqInitiateServer_Post_TaskStack_NA
} RtemsTimerReqInitiateServer_Post_TaskStack;

typedef enum {
  RtemsTimerReqInitiateServer_Post_TaskAttr_Set,
  RtemsTimerReqInitiateServer_Post_TaskAttr_Nop,
  RtemsTimerReqInitiateServer_Post_TaskAttr_NA
} RtemsTimerReqInitiateServer_Post_TaskAttr;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Priority_NA : 1;
  uint16_t Pre_Stack_NA : 1;
  uint16_t Pre_Started_NA : 1;
  uint16_t Pre_TaskObj_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Started : 2;
  uint16_t Post_TaskPrio : 2;
  uint16_t Post_TaskStack : 2;
  uint16_t Post_TaskAttr : 2;
} RtemsTimerReqInitiateServer_Entry;

/**
 * @brief Test context for spec:/rtems/timer/req/initiate-server test case.
 */
typedef struct {
  /**
   * @brief If the Timer Server task exists before the action, its priority
   *   before the action.
   */
  rtems_task_priority before_priority;

  /**
   * @brief If the Timer Server task exists before the action, its stack size
   *   before the action.
   */
  size_t before_stack_size;

  /**
   * @brief If the Timer Server task exists before the action, whether the
   *   floating point attribute is set before the action.
   */
  bool before_has_floating_point;

  /**
   * @brief The ``priority`` parameter for the action.
   */
  rtems_task_priority priority;

  /**
   * @brief The ``stack_size`` parameter for the action.
   */
  size_t stack_size;

  /**
   * @brief The status value returned by the action
   *   rtems_timer_initiate_server().
   */
  rtems_status_code status;

  /**
   * @brief This member is used by the T_seize_objects() and
   *   T_surrender_objects() support functions.
   */
  void *task_objects;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    RtemsTimerReqInitiateServer_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTimerReqInitiateServer_Context;

static RtemsTimerReqInitiateServer_Context
  RtemsTimerReqInitiateServer_Instance;

static const char * const RtemsTimerReqInitiateServer_PreDesc_Priority[] = {
  "Valid",
  "Default",
  "Invalid",
  "NA"
};

static const char * const RtemsTimerReqInitiateServer_PreDesc_Stack[] = {
  "Allocatable",
  "TooLarge",
  "NA"
};

static const char * const RtemsTimerReqInitiateServer_PreDesc_Started[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTimerReqInitiateServer_PreDesc_TaskObj[] = {
  "Available",
  "Unavailable",
  "NA"
};

static const char * const * const RtemsTimerReqInitiateServer_PreDesc[] = {
  RtemsTimerReqInitiateServer_PreDesc_Priority,
  RtemsTimerReqInitiateServer_PreDesc_Stack,
  RtemsTimerReqInitiateServer_PreDesc_Started,
  RtemsTimerReqInitiateServer_PreDesc_TaskObj,
  NULL
};

static bool ExistTimerServer( void )
{
  return GetTimerServerTaskId() != RTEMS_INVALID_ID;
}

static rtems_task_priority GetTimerServerPriority( void )
{
  return GetPriority( GetTimerServerTaskId() );
}

static size_t GetTimerServerStackSize( void )
{
  rtems_tcb *tcb;
  rtems_id server_task_id;
  server_task_id = GetTimerServerTaskId();
  T_ne_u32( server_task_id, RTEMS_INVALID_ID );
  tcb = GetThread( server_task_id );
  T_not_null( tcb );
  return tcb->Start.Initial_stack.size;
}

static bool HasTimerServerFloatingPoint( void )
{
  rtems_tcb *tcb;
  rtems_id server_task_id;
  server_task_id = GetTimerServerTaskId();
  T_ne_u32( server_task_id, RTEMS_INVALID_ID );
  tcb = GetThread( server_task_id );
  T_not_null( tcb );
  return tcb->is_fp;
}

static rtems_status_code AllocateTaskObject( void *arg, rtems_id *task_id )
{
  (void) arg;

  return rtems_task_create(
    rtems_build_name( 'T', 'A', ' ', 'N' ),
    PRIO_LOW,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    task_id
  );
}

static void RtemsTimerReqInitiateServer_Pre_Priority_Prepare(
  RtemsTimerReqInitiateServer_Context     *ctx,
  RtemsTimerReqInitiateServer_Pre_Priority state
)
{
  switch ( state ) {
    case RtemsTimerReqInitiateServer_Pre_Priority_Valid: {
      /*
       * While the ``priority`` parameter is valid.
       */
      ctx->priority = 13;
      break;
    }

    case RtemsTimerReqInitiateServer_Pre_Priority_Default: {
      /*
       * While the ``priority`` parameter is equal to
       * RTEMS_TIMER_SERVER_DEFAULT_PRIORITY.
       */
      ctx->priority = RTEMS_TIMER_SERVER_DEFAULT_PRIORITY;
      break;
    }

    case RtemsTimerReqInitiateServer_Pre_Priority_Invalid: {
      /*
       * While the ``priority`` parameter is invalid.
       */
      ctx->priority = PRIO_INVALID;
      break;
    }

    case RtemsTimerReqInitiateServer_Pre_Priority_NA:
      break;
  }
}

static void RtemsTimerReqInitiateServer_Pre_Stack_Prepare(
  RtemsTimerReqInitiateServer_Context  *ctx,
  RtemsTimerReqInitiateServer_Pre_Stack state
)
{
  switch ( state ) {
    case RtemsTimerReqInitiateServer_Pre_Stack_Allocatable: {
      /*
       * While the ``stack_size`` parameter specifies a task stack size which
       * is allocatable by the system.
       */
      ctx->stack_size = RTEMS_MINIMUM_STACK_SIZE;
      break;
    }

    case RtemsTimerReqInitiateServer_Pre_Stack_TooLarge: {
      /*
       * While the ``stack_size`` parameter specifies a task stack size which
       * is not allocatable by the system.
       */
      ctx->stack_size = SIZE_MAX;
      break;
    }

    case RtemsTimerReqInitiateServer_Pre_Stack_NA:
      break;
  }
}

static void RtemsTimerReqInitiateServer_Pre_Started_Prepare(
  RtemsTimerReqInitiateServer_Context    *ctx,
  RtemsTimerReqInitiateServer_Pre_Started state
)
{
  switch ( state ) {
    case RtemsTimerReqInitiateServer_Pre_Started_Yes: {
      /*
       * While the Timer Server task is started.
       */
      rtems_status_code status;

      if ( !ExistTimerServer() ) {
        status = rtems_timer_initiate_server(
          RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
          RTEMS_MINIMUM_STACK_SIZE,
          RTEMS_DEFAULT_ATTRIBUTES
        );
        T_rsc_success( status );
      }
      break;
    }

    case RtemsTimerReqInitiateServer_Pre_Started_No: {
      /*
       * While the Timer Server task is not started.
       */
      DeleteTimerServer();
      break;
    }

    case RtemsTimerReqInitiateServer_Pre_Started_NA:
      break;
  }
}

static void RtemsTimerReqInitiateServer_Pre_TaskObj_Prepare(
  RtemsTimerReqInitiateServer_Context    *ctx,
  RtemsTimerReqInitiateServer_Pre_TaskObj state
)
{
  switch ( state ) {
    case RtemsTimerReqInitiateServer_Pre_TaskObj_Available: {
      /*
       * While the system has at least one inactive task object available.
       */
      /* The test clean up ensures that all tasks objects are free. */
      break;
    }

    case RtemsTimerReqInitiateServer_Pre_TaskObj_Unavailable: {
      /*
       * While the system has no inactive task object available.
       */
      ctx->task_objects = T_seize_objects(
        AllocateTaskObject,
        NULL
      );
      break;
    }

    case RtemsTimerReqInitiateServer_Pre_TaskObj_NA:
      break;
  }
}

static void RtemsTimerReqInitiateServer_Post_Status_Check(
  RtemsTimerReqInitiateServer_Context    *ctx,
  RtemsTimerReqInitiateServer_Post_Status state
)
{
  switch ( state ) {
    case RtemsTimerReqInitiateServer_Post_Status_Ok: {
      /*
       * The return status of rtems_timer_initiate_server() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_Status_IncStat: {
      /*
       * The return status of rtems_timer_initiate_server() shall be
       * RTEMS_INCORRECT_STATE
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_Status_InvPrio: {
      /*
       * The return status of rtems_timer_initiate_server() shall be
       * RTEMS_INVALID_PRIORITY.
       */
      T_rsc( ctx->status, RTEMS_INVALID_PRIORITY );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_Status_TooMany: {
      /*
       * The return status of rtems_timer_initiate_server() shall be
       * RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_Status_Unsat: {
      /*
       * The return status of rtems_timer_initiate_server() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED  );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_Status_NA:
      break;
  }
}

static void RtemsTimerReqInitiateServer_Post_Started_Check(
  RtemsTimerReqInitiateServer_Context     *ctx,
  RtemsTimerReqInitiateServer_Post_Started state
)
{
  switch ( state ) {
    case RtemsTimerReqInitiateServer_Post_Started_Yes: {
      /*
       * The Timer Server task shall be started after the
       * rtems_timer_initiate_server() call.
       */
      T_true( ExistTimerServer() );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_Started_No: {
      /*
       * The Timer Server task shall not be started after the
       * rtems_timer_initiate_server() call.
       */
      T_false( ExistTimerServer() );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_Started_NA:
      break;
  }
}

static void RtemsTimerReqInitiateServer_Post_TaskPrio_Check(
  RtemsTimerReqInitiateServer_Context      *ctx,
  RtemsTimerReqInitiateServer_Post_TaskPrio state
)
{
  switch ( state ) {
    case RtemsTimerReqInitiateServer_Post_TaskPrio_Set: {
      /*
       * The priority of the Timer Server task shall be equal to the priority
       * specified by the ``priority`` parameter in the
       * rtems_timer_initiate_server() call.
       */
      if ( ctx->priority == RTEMS_TIMER_SERVER_DEFAULT_PRIORITY ) {
        T_eq_u32( GetTimerServerPriority(), 0 );
      } else {
        T_eq_u32( GetTimerServerPriority(), ctx->priority );
      }
      break;
    }

    case RtemsTimerReqInitiateServer_Post_TaskPrio_Nop: {
      /*
       * The priority of the Timer Server task shall not be modified by the
       * rtems_timer_initiate_server() call.
       */
      T_eq_u32( GetTimerServerPriority(), ctx->before_priority );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_TaskPrio_NA:
      break;
  }
}

static void RtemsTimerReqInitiateServer_Post_TaskStack_Check(
  RtemsTimerReqInitiateServer_Context       *ctx,
  RtemsTimerReqInitiateServer_Post_TaskStack state
)
{
  switch ( state ) {
    case RtemsTimerReqInitiateServer_Post_TaskStack_Set: {
      /*
       * The stack size of the Timer Server task shall be greater than or equal
       * to the stack size specified by the ``stack_size`` parameter in the
       * rtems_timer_initiate_server() call.
       */
      T_ge_sz( GetTimerServerStackSize(), ctx->stack_size );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_TaskStack_Nop: {
      /*
       * The stack size of the Timer Server task shall not be modified by the
       * rtems_timer_initiate_server() call.
       */
      T_ge_sz( GetTimerServerStackSize(), ctx->before_stack_size );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_TaskStack_NA:
      break;
  }
}

static void RtemsTimerReqInitiateServer_Post_TaskAttr_Check(
  RtemsTimerReqInitiateServer_Context      *ctx,
  RtemsTimerReqInitiateServer_Post_TaskAttr state
)
{
  switch ( state ) {
    case RtemsTimerReqInitiateServer_Post_TaskAttr_Set: {
      /*
       * The task attributes of the Timer Server task shall be specified by the
       * ``attribute_set`` parameter in the rtems_timer_initiate_server() call.
       */
      T_true( HasTimerServerFloatingPoint() );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_TaskAttr_Nop: {
      /*
       * The task attributes of the Timer Server task shall not be modified by
       * the rtems_timer_initiate_server() call.
       */
      T_true( HasTimerServerFloatingPoint() == ctx->before_has_floating_point );
      break;
    }

    case RtemsTimerReqInitiateServer_Post_TaskAttr_NA:
      break;
  }
}

static void RtemsTimerReqInitiateServer_Setup(
  RtemsTimerReqInitiateServer_Context *ctx
)
{
  ctx->task_objects = NULL;
}

static void RtemsTimerReqInitiateServer_Setup_Wrap( void *arg )
{
  RtemsTimerReqInitiateServer_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqInitiateServer_Setup( ctx );
}

/**
 * @brief Make sure the time server in not running after this test.
 */
static void RtemsTimerReqInitiateServer_Teardown(
  RtemsTimerReqInitiateServer_Context *ctx
)
{
  DeleteTimerServer();
}

static void RtemsTimerReqInitiateServer_Teardown_Wrap( void *arg )
{
  RtemsTimerReqInitiateServer_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqInitiateServer_Teardown( ctx );
}

static void RtemsTimerReqInitiateServer_Action(
  RtemsTimerReqInitiateServer_Context *ctx
)
{
  if ( ExistTimerServer() ) {
    ctx->before_priority = GetTimerServerPriority();
    ctx->before_stack_size = GetTimerServerStackSize();
    ctx->before_has_floating_point = HasTimerServerFloatingPoint();
  }

  ctx->status = rtems_timer_initiate_server(
    ctx->priority,
    ctx->stack_size,
    RTEMS_FLOATING_POINT
  );
}

static void RtemsTimerReqInitiateServer_Cleanup(
  RtemsTimerReqInitiateServer_Context *ctx
)
{
  T_surrender_objects( &ctx->task_objects, rtems_task_delete );
}

static const RtemsTimerReqInitiateServer_Entry
RtemsTimerReqInitiateServer_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsTimerReqInitiateServer_Post_Status_IncStat,
    RtemsTimerReqInitiateServer_Post_Started_Yes,
    RtemsTimerReqInitiateServer_Post_TaskPrio_Nop,
    RtemsTimerReqInitiateServer_Post_TaskStack_Nop,
    RtemsTimerReqInitiateServer_Post_TaskAttr_Nop },
  { 0, 0, 0, 0, 0, RtemsTimerReqInitiateServer_Post_Status_TooMany,
    RtemsTimerReqInitiateServer_Post_Started_No,
    RtemsTimerReqInitiateServer_Post_TaskPrio_NA,
    RtemsTimerReqInitiateServer_Post_TaskStack_NA,
    RtemsTimerReqInitiateServer_Post_TaskAttr_NA },
  { 0, 0, 0, 0, 0, RtemsTimerReqInitiateServer_Post_Status_InvPrio,
    RtemsTimerReqInitiateServer_Post_Started_No,
    RtemsTimerReqInitiateServer_Post_TaskPrio_NA,
    RtemsTimerReqInitiateServer_Post_TaskStack_NA,
    RtemsTimerReqInitiateServer_Post_TaskAttr_NA },
  { 0, 0, 0, 0, 0, RtemsTimerReqInitiateServer_Post_Status_Ok,
    RtemsTimerReqInitiateServer_Post_Started_Yes,
    RtemsTimerReqInitiateServer_Post_TaskPrio_Set,
    RtemsTimerReqInitiateServer_Post_TaskStack_Set,
    RtemsTimerReqInitiateServer_Post_TaskAttr_Set },
  { 0, 0, 0, 0, 0, RtemsTimerReqInitiateServer_Post_Status_Unsat,
    RtemsTimerReqInitiateServer_Post_Started_No,
    RtemsTimerReqInitiateServer_Post_TaskPrio_NA,
    RtemsTimerReqInitiateServer_Post_TaskStack_NA,
    RtemsTimerReqInitiateServer_Post_TaskAttr_NA }
};

static const uint8_t
RtemsTimerReqInitiateServer_Map[] = {
  0, 0, 3, 1, 0, 0, 4, 1, 0, 0, 3, 1, 0, 0, 4, 1, 0, 0, 2, 2, 0, 0, 2, 2
};

static size_t RtemsTimerReqInitiateServer_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsTimerReqInitiateServer_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsTimerReqInitiateServer_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsTimerReqInitiateServer_Fixture = {
  .setup = RtemsTimerReqInitiateServer_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTimerReqInitiateServer_Teardown_Wrap,
  .scope = RtemsTimerReqInitiateServer_Scope,
  .initial_context = &RtemsTimerReqInitiateServer_Instance
};

static inline RtemsTimerReqInitiateServer_Entry
RtemsTimerReqInitiateServer_PopEntry(
  RtemsTimerReqInitiateServer_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTimerReqInitiateServer_Entries[
    RtemsTimerReqInitiateServer_Map[ index ]
  ];
}

static void RtemsTimerReqInitiateServer_TestVariant(
  RtemsTimerReqInitiateServer_Context *ctx
)
{
  RtemsTimerReqInitiateServer_Pre_Priority_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTimerReqInitiateServer_Pre_Stack_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTimerReqInitiateServer_Pre_Started_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTimerReqInitiateServer_Pre_TaskObj_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTimerReqInitiateServer_Action( ctx );
  RtemsTimerReqInitiateServer_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsTimerReqInitiateServer_Post_Started_Check(
    ctx,
    ctx->Map.entry.Post_Started
  );
  RtemsTimerReqInitiateServer_Post_TaskPrio_Check(
    ctx,
    ctx->Map.entry.Post_TaskPrio
  );
  RtemsTimerReqInitiateServer_Post_TaskStack_Check(
    ctx,
    ctx->Map.entry.Post_TaskStack
  );
  RtemsTimerReqInitiateServer_Post_TaskAttr_Check(
    ctx,
    ctx->Map.entry.Post_TaskAttr
  );
}

/**
 * @fn void T_case_body_RtemsTimerReqInitiateServer( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTimerReqInitiateServer,
  &RtemsTimerReqInitiateServer_Fixture
)
{
  RtemsTimerReqInitiateServer_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTimerReqInitiateServer_Pre_Priority_Valid;
    ctx->Map.pcs[ 0 ] < RtemsTimerReqInitiateServer_Pre_Priority_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTimerReqInitiateServer_Pre_Stack_Allocatable;
      ctx->Map.pcs[ 1 ] < RtemsTimerReqInitiateServer_Pre_Stack_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTimerReqInitiateServer_Pre_Started_Yes;
        ctx->Map.pcs[ 2 ] < RtemsTimerReqInitiateServer_Pre_Started_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsTimerReqInitiateServer_Pre_TaskObj_Available;
          ctx->Map.pcs[ 3 ] < RtemsTimerReqInitiateServer_Pre_TaskObj_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = RtemsTimerReqInitiateServer_PopEntry( ctx );
          RtemsTimerReqInitiateServer_TestVariant( ctx );
          RtemsTimerReqInitiateServer_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
