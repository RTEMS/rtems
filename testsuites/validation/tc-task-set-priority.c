/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqSetPriority
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
 * @defgroup RtemsTaskReqSetPriority spec:/rtems/task/req/set-priority
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqSetPriority_Pre_Id_Invalid,
  RtemsTaskReqSetPriority_Pre_Id_Task,
  RtemsTaskReqSetPriority_Pre_Id_NA
} RtemsTaskReqSetPriority_Pre_Id;

typedef enum {
  RtemsTaskReqSetPriority_Pre_State_Dormant,
  RtemsTaskReqSetPriority_Pre_State_Ready,
  RtemsTaskReqSetPriority_Pre_State_Scheduled,
  RtemsTaskReqSetPriority_Pre_State_Blocked,
  RtemsTaskReqSetPriority_Pre_State_NA
} RtemsTaskReqSetPriority_Pre_State;

typedef enum {
  RtemsTaskReqSetPriority_Pre_NewPriority_Current,
  RtemsTaskReqSetPriority_Pre_NewPriority_Other,
  RtemsTaskReqSetPriority_Pre_NewPriority_NA
} RtemsTaskReqSetPriority_Pre_NewPriority;

typedef enum {
  RtemsTaskReqSetPriority_Pre_TaskPriority_High,
  RtemsTaskReqSetPriority_Pre_TaskPriority_Equal,
  RtemsTaskReqSetPriority_Pre_TaskPriority_Low,
  RtemsTaskReqSetPriority_Pre_TaskPriority_Invalid,
  RtemsTaskReqSetPriority_Pre_TaskPriority_NA
} RtemsTaskReqSetPriority_Pre_TaskPriority;

typedef enum {
  RtemsTaskReqSetPriority_Pre_OldPriority_Valid,
  RtemsTaskReqSetPriority_Pre_OldPriority_Null,
  RtemsTaskReqSetPriority_Pre_OldPriority_NA
} RtemsTaskReqSetPriority_Pre_OldPriority;

typedef enum {
  RtemsTaskReqSetPriority_Post_Status_Ok,
  RtemsTaskReqSetPriority_Post_Status_InvAddr,
  RtemsTaskReqSetPriority_Post_Status_InvId,
  RtemsTaskReqSetPriority_Post_Status_InvPrio,
  RtemsTaskReqSetPriority_Post_Status_NA
} RtemsTaskReqSetPriority_Post_Status;

typedef enum {
  RtemsTaskReqSetPriority_Post_Priority_Set,
  RtemsTaskReqSetPriority_Post_Priority_Nop,
  RtemsTaskReqSetPriority_Post_Priority_NA
} RtemsTaskReqSetPriority_Post_Priority;

typedef enum {
  RtemsTaskReqSetPriority_Post_OldPriorityObj_Set,
  RtemsTaskReqSetPriority_Post_OldPriorityObj_Nop,
  RtemsTaskReqSetPriority_Post_OldPriorityObj_NA
} RtemsTaskReqSetPriority_Post_OldPriorityObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_State_NA : 1;
  uint16_t Pre_NewPriority_NA : 1;
  uint16_t Pre_TaskPriority_NA : 1;
  uint16_t Pre_OldPriority_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Priority : 2;
  uint16_t Post_OldPriorityObj : 2;
} RtemsTaskReqSetPriority_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/set-priority test case.
 */
typedef struct {
  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief If this member is true, then the task shall be started.
   */
  bool started;

  /**
   * @brief If this member is true, then the task shall be blocked.
   */
  bool blocked;

  /**
   * @brief This member provides the object referenced by the ``old_priority``
   *   parameter.
   */
  rtems_task_priority old_priority_obj;

  /**
   * @brief This member contains the return value of the
   *   rtems_task_set_priority() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``id`` parameter value.
   */
  rtems_id id;

  /**
   * @brief This member specifies if the ``new_priority`` parameter value.
   */
  rtems_task_priority new_priority;

  /**
   * @brief This member specifies if the ``old_priority`` parameter value.
   */
  rtems_task_priority *old_priority;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 5 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    RtemsTaskReqSetPriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqSetPriority_Context;

static RtemsTaskReqSetPriority_Context
  RtemsTaskReqSetPriority_Instance;

static const char * const RtemsTaskReqSetPriority_PreDesc_Id[] = {
  "Invalid",
  "Task",
  "NA"
};

static const char * const RtemsTaskReqSetPriority_PreDesc_State[] = {
  "Dormant",
  "Ready",
  "Scheduled",
  "Blocked",
  "NA"
};

static const char * const RtemsTaskReqSetPriority_PreDesc_NewPriority[] = {
  "Current",
  "Other",
  "NA"
};

static const char * const RtemsTaskReqSetPriority_PreDesc_TaskPriority[] = {
  "High",
  "Equal",
  "Low",
  "Invalid",
  "NA"
};

static const char * const RtemsTaskReqSetPriority_PreDesc_OldPriority[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsTaskReqSetPriority_PreDesc[] = {
  RtemsTaskReqSetPriority_PreDesc_Id,
  RtemsTaskReqSetPriority_PreDesc_State,
  RtemsTaskReqSetPriority_PreDesc_NewPriority,
  RtemsTaskReqSetPriority_PreDesc_TaskPriority,
  RtemsTaskReqSetPriority_PreDesc_OldPriority,
  NULL
};

static void Worker( rtems_task_argument arg )
{
  (void) ReceiveAnyEvents();
  (void) ReceiveAnyEvents();
}

static void RtemsTaskReqSetPriority_Pre_Id_Prepare(
  RtemsTaskReqSetPriority_Context *ctx,
  RtemsTaskReqSetPriority_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqSetPriority_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a task.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_Id_Task: {
      /*
       * While the ``id`` parameter is associated with a task.
       */
      ctx->id = ctx->worker_id;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqSetPriority_Pre_State_Prepare(
  RtemsTaskReqSetPriority_Context  *ctx,
  RtemsTaskReqSetPriority_Pre_State state
)
{
  switch ( state ) {
    case RtemsTaskReqSetPriority_Pre_State_Dormant: {
      /*
       * While the task specified by the ``id`` parameter is dormant.
       */
      ctx->started = false;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_State_Ready: {
      /*
       * While the task specified by the ``id`` parameter is ready.
       */
      ctx->started = true;
      ctx->blocked = false;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_State_Scheduled: {
      /*
       * While the task specified by the ``id`` parameter is scheduled.
       */
      ctx->started = false;
      ctx->id = rtems_task_self();
      break;
    }

    case RtemsTaskReqSetPriority_Pre_State_Blocked: {
      /*
       * While the task specified by the ``id`` parameter is blocked.
       */
      ctx->started = true;
      ctx->blocked = true;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_State_NA:
      break;
  }
}

static void RtemsTaskReqSetPriority_Pre_NewPriority_Prepare(
  RtemsTaskReqSetPriority_Context        *ctx,
  RtemsTaskReqSetPriority_Pre_NewPriority state
)
{
  switch ( state ) {
    case RtemsTaskReqSetPriority_Pre_NewPriority_Current: {
      /*
       * While the value of the ``new_priority`` parameter is equal to
       * RTEMS_CURRENT_PRIORITY.
       */
      ctx->new_priority = RTEMS_CURRENT_PRIORITY;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_NewPriority_Other: {
      /*
       * While the value of the ``new_priority`` parameter is not equal to
       * RTEMS_CURRENT_PRIORITY.
       */
      ctx->new_priority = PRIO_NORMAL;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_NewPriority_NA:
      break;
  }
}

static void RtemsTaskReqSetPriority_Pre_TaskPriority_Prepare(
  RtemsTaskReqSetPriority_Context         *ctx,
  RtemsTaskReqSetPriority_Pre_TaskPriority state
)
{
  switch ( state ) {
    case RtemsTaskReqSetPriority_Pre_TaskPriority_High: {
      /*
       * While the value of the ``new_priority`` parameter is a valid task
       * priority with respect to the home scheduler of the task specified by
       * the ``id`` parameter when the new priority is set, while the value of
       * the ``new_priority`` parameter is higher than the task priority with
       * respect to the home scheduler of the task specified by the ``id``
       * parameter at time when the scheduler evaluates the new priority.
       */
      ctx->new_priority = PRIO_HIGH;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_TaskPriority_Equal: {
      /*
       * While the value of the ``new_priority`` parameter is a valid task
       * priority with respect to the home scheduler of the task specified by
       * the ``id`` parameter when the new priority is set, while the value of
       * the ``new_priority`` parameter is equal to the task priority with
       * respect to the home scheduler of the task specified by the ``id``
       * parameter at time when the scheduler evaluates the new priority.
       */
      ctx->new_priority = PRIO_NORMAL;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_TaskPriority_Low: {
      /*
       * While the value of the ``new_priority`` parameter is a valid task
       * priority with respect to the home scheduler of the task specified by
       * the ``id`` parameter when the new priority is set, while the value of
       * the ``new_priority`` parameter is lower than the task priority with
       * respect to the home scheduler of the task specified by the ``id``
       * parameter at time when the scheduler evaluates the new priority.
       */
      ctx->new_priority = PRIO_LOW;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_TaskPriority_Invalid: {
      /*
       * While the value of the ``new_priority`` parameter is an invalid task
       * priority with respect to the home scheduler of the task specified by
       * the ``id`` parameter when the new priority is evaluated.
       */
      ctx->new_priority = PRIO_INVALID;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_TaskPriority_NA:
      break;
  }
}

static void RtemsTaskReqSetPriority_Pre_OldPriority_Prepare(
  RtemsTaskReqSetPriority_Context        *ctx,
  RtemsTaskReqSetPriority_Pre_OldPriority state
)
{
  switch ( state ) {
    case RtemsTaskReqSetPriority_Pre_OldPriority_Valid: {
      /*
       * While the ``old_priority`` parameter references an object of type
       * rtems_task_priority.
       */
      ctx->old_priority = &ctx->old_priority_obj;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_OldPriority_Null: {
      /*
       * While the ``old_priority`` parameter is equal to NULL.
       */
      ctx->old_priority = NULL;
      break;
    }

    case RtemsTaskReqSetPriority_Pre_OldPriority_NA:
      break;
  }
}

static void RtemsTaskReqSetPriority_Post_Status_Check(
  RtemsTaskReqSetPriority_Context    *ctx,
  RtemsTaskReqSetPriority_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqSetPriority_Post_Status_Ok: {
      /*
       * The return status of rtems_task_set_priority() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqSetPriority_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_set_priority() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqSetPriority_Post_Status_InvId: {
      /*
       * The return status of rtems_task_set_priority() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqSetPriority_Post_Status_InvPrio: {
      /*
       * The return status of rtems_task_set_priority() shall be
       * RTEMS_INVALID_PRIORITY.
       */
      T_rsc( ctx->status, RTEMS_INVALID_PRIORITY );
      break;
    }

    case RtemsTaskReqSetPriority_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqSetPriority_Post_Priority_Check(
  RtemsTaskReqSetPriority_Context      *ctx,
  RtemsTaskReqSetPriority_Post_Priority state
)
{
  switch ( state ) {
    case RtemsTaskReqSetPriority_Post_Priority_Set: {
      /*
       * The real priority of the task specified by the ``id`` parameter shall
       * be set to the value specified by the ``new_priority`` parameter at
       * some point during the rtems_task_set_priority() call.
       */
      T_eq_u32( GetPriority( ctx->id ), ctx->new_priority );
      break;
    }

    case RtemsTaskReqSetPriority_Post_Priority_Nop: {
      /*
       * No real priority of a task shall be modified by the
       * rtems_task_set_priority() call.
       */
      T_eq_u32( GetPriority( ctx->worker_id ), PRIO_NORMAL );
      break;
    }

    case RtemsTaskReqSetPriority_Post_Priority_NA:
      break;
  }
}

static void RtemsTaskReqSetPriority_Post_OldPriorityObj_Check(
  RtemsTaskReqSetPriority_Context            *ctx,
  RtemsTaskReqSetPriority_Post_OldPriorityObj state
)
{
  switch ( state ) {
    case RtemsTaskReqSetPriority_Post_OldPriorityObj_Set: {
      /*
       * The value of the object referenced by the ``old_priority`` parameter
       * shall be set after the return of the rtems_task_set_priority() call to
       * the current priority of the task specified by the ``id`` parameter at
       * some point during the call and before the real priority is modified by
       * the call if it is modified by the call.
       */
      T_eq_u32( ctx->old_priority_obj, PRIO_NORMAL );
      break;
    }

    case RtemsTaskReqSetPriority_Post_OldPriorityObj_Nop: {
      /*
       * Objects referenced by the ``old_priority`` parameter in past calls to
       * rtems_task_set_priority() shall not be accessed by the
       * rtems_task_set_priority() call.
       */
      T_eq_u32( ctx->old_priority_obj, PRIO_INVALID );
      break;
    }

    case RtemsTaskReqSetPriority_Post_OldPriorityObj_NA:
      break;
  }
}

static void RtemsTaskReqSetPriority_Setup(
  RtemsTaskReqSetPriority_Context *ctx
)
{
  SetSelfPriority( PRIO_ULTRA_HIGH );
}

static void RtemsTaskReqSetPriority_Setup_Wrap( void *arg )
{
  RtemsTaskReqSetPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqSetPriority_Setup( ctx );
}

static void RtemsTaskReqSetPriority_Teardown(
  RtemsTaskReqSetPriority_Context *ctx
)
{
  RestoreRunnerPriority();
}

static void RtemsTaskReqSetPriority_Teardown_Wrap( void *arg )
{
  RtemsTaskReqSetPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqSetPriority_Teardown( ctx );
}

static void RtemsTaskReqSetPriority_Prepare(
  RtemsTaskReqSetPriority_Context *ctx
)
{
  ctx->old_priority_obj = PRIO_INVALID;
  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  ctx->started = false;
  ctx->blocked = false;
}

static void RtemsTaskReqSetPriority_Action(
  RtemsTaskReqSetPriority_Context *ctx
)
{
  if ( ctx->started ) {
    SetSelfPriority( PRIO_ULTRA_HIGH );
    StartTask( ctx->worker_id, Worker, NULL );

    if ( ctx->blocked ) {
      SetSelfPriority( PRIO_ULTRA_LOW );
      SetSelfPriority( PRIO_ULTRA_HIGH );
    }
  } else {
    SetSelfPriority( PRIO_NORMAL );
  }

  ctx->status = rtems_task_set_priority(
    ctx->id,
    ctx->new_priority,
    ctx->old_priority
  );

  if ( ctx->started ) {
    SendEvents( ctx->worker_id, RTEMS_EVENT_0 );
    SetSelfPriority( PRIO_ULTRA_LOW );
    SetSelfPriority( PRIO_ULTRA_HIGH );
  }
}

static void RtemsTaskReqSetPriority_Cleanup(
  RtemsTaskReqSetPriority_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
}

static const RtemsTaskReqSetPriority_Entry
RtemsTaskReqSetPriority_Entries[] = {
  { 0, 0, 1, 0, 1, 0, RtemsTaskReqSetPriority_Post_Status_InvId,
    RtemsTaskReqSetPriority_Post_Priority_Nop,
    RtemsTaskReqSetPriority_Post_OldPriorityObj_Nop },
  { 0, 0, 1, 0, 1, 0, RtemsTaskReqSetPriority_Post_Status_InvAddr,
    RtemsTaskReqSetPriority_Post_Priority_Nop,
    RtemsTaskReqSetPriority_Post_OldPriorityObj_Nop },
  { 0, 0, 0, 0, 1, 0, RtemsTaskReqSetPriority_Post_Status_Ok,
    RtemsTaskReqSetPriority_Post_Priority_Nop,
    RtemsTaskReqSetPriority_Post_OldPriorityObj_Set },
  { 0, 0, 0, 0, 1, 0, RtemsTaskReqSetPriority_Post_Status_InvAddr,
    RtemsTaskReqSetPriority_Post_Priority_Nop,
    RtemsTaskReqSetPriority_Post_OldPriorityObj_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqSetPriority_Post_Status_InvAddr,
    RtemsTaskReqSetPriority_Post_Priority_Nop,
    RtemsTaskReqSetPriority_Post_OldPriorityObj_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqSetPriority_Post_Status_Ok,
    RtemsTaskReqSetPriority_Post_Priority_Set,
    RtemsTaskReqSetPriority_Post_OldPriorityObj_Set },
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqSetPriority_Post_Status_InvPrio,
    RtemsTaskReqSetPriority_Post_Priority_Nop,
    RtemsTaskReqSetPriority_Post_OldPriorityObj_Set }
};

static const uint8_t
RtemsTaskReqSetPriority_Map[] = {
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 5, 4, 5, 4, 5, 4,
  6, 4, 2, 3, 2, 3, 2, 3, 2, 3, 5, 4, 5, 4, 5, 4, 6, 4, 2, 3, 2, 3, 2, 3, 2, 3,
  5, 4, 5, 4, 5, 4, 6, 4, 2, 3, 2, 3, 2, 3, 2, 3, 5, 4, 5, 4, 5, 4, 6, 4
};

static size_t RtemsTaskReqSetPriority_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqSetPriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsTaskReqSetPriority_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsTaskReqSetPriority_Fixture = {
  .setup = RtemsTaskReqSetPriority_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqSetPriority_Teardown_Wrap,
  .scope = RtemsTaskReqSetPriority_Scope,
  .initial_context = &RtemsTaskReqSetPriority_Instance
};

static inline RtemsTaskReqSetPriority_Entry RtemsTaskReqSetPriority_PopEntry(
  RtemsTaskReqSetPriority_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqSetPriority_Entries[
    RtemsTaskReqSetPriority_Map[ index ]
  ];
}

static void RtemsTaskReqSetPriority_SetPreConditionStates(
  RtemsTaskReqSetPriority_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_State_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsTaskReqSetPriority_Pre_State_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_TaskPriority_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsTaskReqSetPriority_Pre_TaskPriority_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
}

static void RtemsTaskReqSetPriority_TestVariant(
  RtemsTaskReqSetPriority_Context *ctx
)
{
  RtemsTaskReqSetPriority_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqSetPriority_Pre_State_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqSetPriority_Pre_NewPriority_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqSetPriority_Pre_TaskPriority_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqSetPriority_Pre_OldPriority_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTaskReqSetPriority_Action( ctx );
  RtemsTaskReqSetPriority_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqSetPriority_Post_Priority_Check(
    ctx,
    ctx->Map.entry.Post_Priority
  );
  RtemsTaskReqSetPriority_Post_OldPriorityObj_Check(
    ctx,
    ctx->Map.entry.Post_OldPriorityObj
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqSetPriority( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskReqSetPriority,
  &RtemsTaskReqSetPriority_Fixture
)
{
  RtemsTaskReqSetPriority_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqSetPriority_Pre_Id_Invalid;
    ctx->Map.pci[ 0 ] < RtemsTaskReqSetPriority_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqSetPriority_Pre_State_Dormant;
      ctx->Map.pci[ 1 ] < RtemsTaskReqSetPriority_Pre_State_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsTaskReqSetPriority_Pre_NewPriority_Current;
        ctx->Map.pci[ 2 ] < RtemsTaskReqSetPriority_Pre_NewPriority_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsTaskReqSetPriority_Pre_TaskPriority_High;
          ctx->Map.pci[ 3 ] < RtemsTaskReqSetPriority_Pre_TaskPriority_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsTaskReqSetPriority_Pre_OldPriority_Valid;
            ctx->Map.pci[ 4 ] < RtemsTaskReqSetPriority_Pre_OldPriority_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = RtemsTaskReqSetPriority_PopEntry( ctx );
            RtemsTaskReqSetPriority_SetPreConditionStates( ctx );
            RtemsTaskReqSetPriority_Prepare( ctx );
            RtemsTaskReqSetPriority_TestVariant( ctx );
            RtemsTaskReqSetPriority_Cleanup( ctx );
          }
        }
      }
    }
  }
}

/** @} */
