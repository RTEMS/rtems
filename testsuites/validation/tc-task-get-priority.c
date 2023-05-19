/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqGetPriority
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

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqGetPriority spec:/rtems/task/req/get-priority
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqGetPriority_Pre_TaskId_Invalid,
  RtemsTaskReqGetPriority_Pre_TaskId_Task,
  RtemsTaskReqGetPriority_Pre_TaskId_NA
} RtemsTaskReqGetPriority_Pre_TaskId;

typedef enum {
  RtemsTaskReqGetPriority_Pre_SchedulerId_Invalid,
  RtemsTaskReqGetPriority_Pre_SchedulerId_Scheduler,
  RtemsTaskReqGetPriority_Pre_SchedulerId_NA
} RtemsTaskReqGetPriority_Pre_SchedulerId;

typedef enum {
  RtemsTaskReqGetPriority_Pre_Scheduler_Eligible,
  RtemsTaskReqGetPriority_Pre_Scheduler_Ineligible,
  RtemsTaskReqGetPriority_Pre_Scheduler_NA
} RtemsTaskReqGetPriority_Pre_Scheduler;

typedef enum {
  RtemsTaskReqGetPriority_Pre_Priority_Valid,
  RtemsTaskReqGetPriority_Pre_Priority_Null,
  RtemsTaskReqGetPriority_Pre_Priority_NA
} RtemsTaskReqGetPriority_Pre_Priority;

typedef enum {
  RtemsTaskReqGetPriority_Post_Status_Ok,
  RtemsTaskReqGetPriority_Post_Status_InvAddr,
  RtemsTaskReqGetPriority_Post_Status_InvId,
  RtemsTaskReqGetPriority_Post_Status_NotDef,
  RtemsTaskReqGetPriority_Post_Status_NA
} RtemsTaskReqGetPriority_Post_Status;

typedef enum {
  RtemsTaskReqGetPriority_Post_PriorityObj_Set,
  RtemsTaskReqGetPriority_Post_PriorityObj_Nop,
  RtemsTaskReqGetPriority_Post_PriorityObj_NA
} RtemsTaskReqGetPriority_Post_PriorityObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_TaskId_NA : 1;
  uint16_t Pre_SchedulerId_NA : 1;
  uint16_t Pre_Scheduler_NA : 1;
  uint16_t Pre_Priority_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_PriorityObj : 2;
} RtemsTaskReqGetPriority_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/get-priority test case.
 */
typedef struct {
  /**
   * @brief This member contains the scheduler A identifier.
   */
  rtems_id scheduler_a_id;

  /**
   * @brief This member contains the scheduler B identifier.
   */
  rtems_id scheduler_b_id;

  /**
   * @brief This member provides the object referenced by the ``priority``
   *   parameter.
   */
  rtems_task_priority priority_obj;

  /**
   * @brief This member contains the return value of the
   *   rtems_task_get_priority() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``task_id`` parameter value.
   */
  rtems_id task_id;

  /**
   * @brief This member specifies if the ``scheduler_id`` parameter value.
   */
  rtems_id scheduler_id;

  /**
   * @brief This member specifies if the ``priority`` parameter value.
   */
  rtems_id *priority;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

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
    RtemsTaskReqGetPriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqGetPriority_Context;

static RtemsTaskReqGetPriority_Context
  RtemsTaskReqGetPriority_Instance;

static const char * const RtemsTaskReqGetPriority_PreDesc_TaskId[] = {
  "Invalid",
  "Task",
  "NA"
};

static const char * const RtemsTaskReqGetPriority_PreDesc_SchedulerId[] = {
  "Invalid",
  "Scheduler",
  "NA"
};

static const char * const RtemsTaskReqGetPriority_PreDesc_Scheduler[] = {
  "Eligible",
  "Ineligible",
  "NA"
};

static const char * const RtemsTaskReqGetPriority_PreDesc_Priority[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsTaskReqGetPriority_PreDesc[] = {
  RtemsTaskReqGetPriority_PreDesc_TaskId,
  RtemsTaskReqGetPriority_PreDesc_SchedulerId,
  RtemsTaskReqGetPriority_PreDesc_Scheduler,
  RtemsTaskReqGetPriority_PreDesc_Priority,
  NULL
};

static void RtemsTaskReqGetPriority_Pre_TaskId_Prepare(
  RtemsTaskReqGetPriority_Context   *ctx,
  RtemsTaskReqGetPriority_Pre_TaskId state
)
{
  switch ( state ) {
    case RtemsTaskReqGetPriority_Pre_TaskId_Invalid: {
      /*
       * While the ``task_id`` parameter is not associated with a task.
       */
      ctx->task_id = INVALID_ID;
      break;
    }

    case RtemsTaskReqGetPriority_Pre_TaskId_Task: {
      /*
       * While the ``task_id`` parameter is associated with a task.
       */
      ctx->task_id = RTEMS_SELF;
      break;
    }

    case RtemsTaskReqGetPriority_Pre_TaskId_NA:
      break;
  }
}

static void RtemsTaskReqGetPriority_Pre_SchedulerId_Prepare(
  RtemsTaskReqGetPriority_Context        *ctx,
  RtemsTaskReqGetPriority_Pre_SchedulerId state
)
{
  switch ( state ) {
    case RtemsTaskReqGetPriority_Pre_SchedulerId_Invalid: {
      /*
       * While the ``scheduler_id`` parameter is not associated with a
       * scheduler.
       */
      ctx->scheduler_id = INVALID_ID;
      break;
    }

    case RtemsTaskReqGetPriority_Pre_SchedulerId_Scheduler: {
      /*
       * While the ``scheduler_id`` parameter is associated with a scheduler.
       */
      ctx->scheduler_id = ctx->scheduler_a_id;
      break;
    }

    case RtemsTaskReqGetPriority_Pre_SchedulerId_NA:
      break;
  }
}

static void RtemsTaskReqGetPriority_Pre_Scheduler_Prepare(
  RtemsTaskReqGetPriority_Context      *ctx,
  RtemsTaskReqGetPriority_Pre_Scheduler state
)
{
  switch ( state ) {
    case RtemsTaskReqGetPriority_Pre_Scheduler_Eligible: {
      /*
       * While the ``scheduler_id`` parameter is associated with an eligible
       * scheduler of the task specified by ``task_id``.
       */
      ctx->scheduler_id = ctx->scheduler_a_id;
      break;
    }

    case RtemsTaskReqGetPriority_Pre_Scheduler_Ineligible: {
      /*
       * While the ``scheduler_id`` parameter is associated with an ineligible
       * scheduler of the task specified by ``task_id``.
       */
      ctx->scheduler_id = ctx->scheduler_b_id;
      break;
    }

    case RtemsTaskReqGetPriority_Pre_Scheduler_NA:
      break;
  }
}

static void RtemsTaskReqGetPriority_Pre_Priority_Prepare(
  RtemsTaskReqGetPriority_Context     *ctx,
  RtemsTaskReqGetPriority_Pre_Priority state
)
{
  switch ( state ) {
    case RtemsTaskReqGetPriority_Pre_Priority_Valid: {
      /*
       * While the ``priority`` parameter references an object of type
       * rtems_task_priority.
       */
      ctx->priority = &ctx->priority_obj;
      break;
    }

    case RtemsTaskReqGetPriority_Pre_Priority_Null: {
      /*
       * While the ``priority`` parameter is equal to NULL.
       */
      ctx->priority = NULL;
      break;
    }

    case RtemsTaskReqGetPriority_Pre_Priority_NA:
      break;
  }
}

static void RtemsTaskReqGetPriority_Post_Status_Check(
  RtemsTaskReqGetPriority_Context    *ctx,
  RtemsTaskReqGetPriority_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqGetPriority_Post_Status_Ok: {
      /*
       * The return status of rtems_task_get_priority() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqGetPriority_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_get_priority() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqGetPriority_Post_Status_InvId: {
      /*
       * The return status of rtems_task_get_priority() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqGetPriority_Post_Status_NotDef: {
      /*
       * The return status of rtems_task_get_priority() shall be
       * RTEMS_NOT_DEFINED.
       */
      T_rsc( ctx->status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsTaskReqGetPriority_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqGetPriority_Post_PriorityObj_Check(
  RtemsTaskReqGetPriority_Context         *ctx,
  RtemsTaskReqGetPriority_Post_PriorityObj state
)
{
  switch ( state ) {
    case RtemsTaskReqGetPriority_Post_PriorityObj_Set: {
      /*
       * The value of the object referenced by the ``scheduler_id`` parameter
       * shall be set to the object identifier of the home scheduler of the
       * task specified by the ``task_id`` parameter at some point during the
       * call after the return of the rtems_task_get_priority() call.
       */
      T_eq_u32( ctx->priority_obj, PRIO_DEFAULT );
      break;
    }

    case RtemsTaskReqGetPriority_Post_PriorityObj_Nop: {
      /*
       * Objects referenced by the ``scheduler_id`` parameter in past calls to
       * rtems_task_get_priority() shall not be accessed by the
       * rtems_task_get_priority() call.
       */
      T_eq_u32( ctx->priority_obj, PRIO_INVALID );
      break;
    }

    case RtemsTaskReqGetPriority_Post_PriorityObj_NA:
      break;
  }
}

static void RtemsTaskReqGetPriority_Setup(
  RtemsTaskReqGetPriority_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_scheduler_ident(
    TEST_SCHEDULER_A_NAME,
    &ctx->scheduler_a_id
  );
  T_rsc_success( sc );

  #if defined(RTEMS_SMP)
  sc = rtems_scheduler_ident(
    TEST_SCHEDULER_B_NAME,
    &ctx->scheduler_b_id
  );
  T_rsc_success( sc );
  #endif
}

static void RtemsTaskReqGetPriority_Setup_Wrap( void *arg )
{
  RtemsTaskReqGetPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqGetPriority_Setup( ctx );
}

static void RtemsTaskReqGetPriority_Prepare(
  RtemsTaskReqGetPriority_Context *ctx
)
{
  ctx->priority_obj = PRIO_INVALID;
}

static void RtemsTaskReqGetPriority_Action(
  RtemsTaskReqGetPriority_Context *ctx
)
{
  ctx->status = rtems_task_get_priority(
    ctx->task_id,
    ctx->scheduler_id,
    ctx->priority
  );
}

static const RtemsTaskReqGetPriority_Entry
RtemsTaskReqGetPriority_Entries[] = {
  { 0, 0, 0, 1, 0, RtemsTaskReqGetPriority_Post_Status_InvId,
    RtemsTaskReqGetPriority_Post_PriorityObj_Nop },
  { 0, 0, 0, 1, 0, RtemsTaskReqGetPriority_Post_Status_InvAddr,
    RtemsTaskReqGetPriority_Post_PriorityObj_Nop },
  { 0, 0, 0, 0, 0, RtemsTaskReqGetPriority_Post_Status_Ok,
    RtemsTaskReqGetPriority_Post_PriorityObj_Set },
  { 0, 0, 0, 0, 0, RtemsTaskReqGetPriority_Post_Status_InvAddr,
    RtemsTaskReqGetPriority_Post_PriorityObj_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsTaskReqGetPriority_Post_Status_NotDef,
    RtemsTaskReqGetPriority_Post_PriorityObj_Nop },
#else
  { 1, 0, 0, 0, 0, RtemsTaskReqGetPriority_Post_Status_NA,
    RtemsTaskReqGetPriority_Post_PriorityObj_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsTaskReqGetPriority_Post_Status_InvAddr,
    RtemsTaskReqGetPriority_Post_PriorityObj_Nop }
#else
  { 1, 0, 0, 0, 0, RtemsTaskReqGetPriority_Post_Status_NA,
    RtemsTaskReqGetPriority_Post_PriorityObj_NA }
#endif
};

static const uint8_t
RtemsTaskReqGetPriority_Map[] = {
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 4, 5
};

static size_t RtemsTaskReqGetPriority_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqGetPriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsTaskReqGetPriority_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsTaskReqGetPriority_Fixture = {
  .setup = RtemsTaskReqGetPriority_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsTaskReqGetPriority_Scope,
  .initial_context = &RtemsTaskReqGetPriority_Instance
};

static inline RtemsTaskReqGetPriority_Entry RtemsTaskReqGetPriority_PopEntry(
  RtemsTaskReqGetPriority_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqGetPriority_Entries[
    RtemsTaskReqGetPriority_Map[ index ]
  ];
}

static void RtemsTaskReqGetPriority_SetPreConditionStates(
  RtemsTaskReqGetPriority_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Scheduler_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsTaskReqGetPriority_Pre_Scheduler_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void RtemsTaskReqGetPriority_TestVariant(
  RtemsTaskReqGetPriority_Context *ctx
)
{
  RtemsTaskReqGetPriority_Pre_TaskId_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqGetPriority_Pre_SchedulerId_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqGetPriority_Pre_Scheduler_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqGetPriority_Pre_Priority_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqGetPriority_Action( ctx );
  RtemsTaskReqGetPriority_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqGetPriority_Post_PriorityObj_Check(
    ctx,
    ctx->Map.entry.Post_PriorityObj
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqGetPriority( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskReqGetPriority,
  &RtemsTaskReqGetPriority_Fixture
)
{
  RtemsTaskReqGetPriority_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqGetPriority_Pre_TaskId_Invalid;
    ctx->Map.pci[ 0 ] < RtemsTaskReqGetPriority_Pre_TaskId_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqGetPriority_Pre_SchedulerId_Invalid;
      ctx->Map.pci[ 1 ] < RtemsTaskReqGetPriority_Pre_SchedulerId_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsTaskReqGetPriority_Pre_Scheduler_Eligible;
        ctx->Map.pci[ 2 ] < RtemsTaskReqGetPriority_Pre_Scheduler_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsTaskReqGetPriority_Pre_Priority_Valid;
          ctx->Map.pci[ 3 ] < RtemsTaskReqGetPriority_Pre_Priority_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = RtemsTaskReqGetPriority_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsTaskReqGetPriority_SetPreConditionStates( ctx );
          RtemsTaskReqGetPriority_Prepare( ctx );
          RtemsTaskReqGetPriority_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
