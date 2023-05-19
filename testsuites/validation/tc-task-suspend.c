/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqSuspend
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
 * @defgroup RtemsTaskReqSuspend spec:/rtems/task/req/suspend
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqSuspend_Pre_Id_Invalid,
  RtemsTaskReqSuspend_Pre_Id_Task,
  RtemsTaskReqSuspend_Pre_Id_NA
} RtemsTaskReqSuspend_Pre_Id;

typedef enum {
  RtemsTaskReqSuspend_Pre_Suspended_Yes,
  RtemsTaskReqSuspend_Pre_Suspended_No,
  RtemsTaskReqSuspend_Pre_Suspended_NA
} RtemsTaskReqSuspend_Pre_Suspended;

typedef enum {
  RtemsTaskReqSuspend_Post_Status_Ok,
  RtemsTaskReqSuspend_Post_Status_InvId,
  RtemsTaskReqSuspend_Post_Status_AlrdySus,
  RtemsTaskReqSuspend_Post_Status_NA
} RtemsTaskReqSuspend_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_Suspended_NA : 1;
  uint8_t Post_Status : 2;
} RtemsTaskReqSuspend_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/suspend test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of a task.
   */
  rtems_id worker_id;

  /**
   * @brief If this member is true, then the worker is suspended before the
   *   rtems_task_suspend() call.
   */
  bool suspend;

  /**
   * @brief This member contains the return value of the rtems_task_suspend()
   *   call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``id`` parameter value.
   */
  rtems_id id;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 2 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    RtemsTaskReqSuspend_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqSuspend_Context;

static RtemsTaskReqSuspend_Context
  RtemsTaskReqSuspend_Instance;

static const char * const RtemsTaskReqSuspend_PreDesc_Id[] = {
  "Invalid",
  "Task",
  "NA"
};

static const char * const RtemsTaskReqSuspend_PreDesc_Suspended[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsTaskReqSuspend_PreDesc[] = {
  RtemsTaskReqSuspend_PreDesc_Id,
  RtemsTaskReqSuspend_PreDesc_Suspended,
  NULL
};

static void Worker( rtems_task_argument arg )
{
  while ( true ) {
    /* Do nothing */
  }
}

static void RtemsTaskReqSuspend_Pre_Id_Prepare(
  RtemsTaskReqSuspend_Context *ctx,
  RtemsTaskReqSuspend_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqSuspend_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a task.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsTaskReqSuspend_Pre_Id_Task: {
      /*
       * While the ``id`` parameter is associated with a task.
       */
      ctx->id = ctx->worker_id;
      break;
    }

    case RtemsTaskReqSuspend_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqSuspend_Pre_Suspended_Prepare(
  RtemsTaskReqSuspend_Context      *ctx,
  RtemsTaskReqSuspend_Pre_Suspended state
)
{
  switch ( state ) {
    case RtemsTaskReqSuspend_Pre_Suspended_Yes: {
      /*
       * While the task specified by the ``id`` parameter is suspended.
       */
      ctx->suspend = true;
      break;
    }

    case RtemsTaskReqSuspend_Pre_Suspended_No: {
      /*
       * While the task specified by the ``id`` parameter is not suspended.
       */
      ctx->suspend = false;
      break;
    }

    case RtemsTaskReqSuspend_Pre_Suspended_NA:
      break;
  }
}

static void RtemsTaskReqSuspend_Post_Status_Check(
  RtemsTaskReqSuspend_Context    *ctx,
  RtemsTaskReqSuspend_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqSuspend_Post_Status_Ok: {
      /*
       * The return status of rtems_task_suspend() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqSuspend_Post_Status_InvId: {
      /*
       * The return status of rtems_task_suspend() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqSuspend_Post_Status_AlrdySus: {
      /*
       * The return status of rtems_task_suspend() shall be
       * RTEMS_ALREADY_SUSPENDED.
       */
      T_rsc( ctx->status, RTEMS_ALREADY_SUSPENDED );
      break;
    }

    case RtemsTaskReqSuspend_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqSuspend_Setup( RtemsTaskReqSuspend_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", PRIO_LOW );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsTaskReqSuspend_Setup_Wrap( void *arg )
{
  RtemsTaskReqSuspend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqSuspend_Setup( ctx );
}

static void RtemsTaskReqSuspend_Teardown( RtemsTaskReqSuspend_Context *ctx )
{
  DeleteTask( ctx->worker_id );
}

static void RtemsTaskReqSuspend_Teardown_Wrap( void *arg )
{
  RtemsTaskReqSuspend_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqSuspend_Teardown( ctx );
}

static void RtemsTaskReqSuspend_Action( RtemsTaskReqSuspend_Context *ctx )
{
  if ( ctx->suspend ) {
    SuspendTask( ctx->worker_id );
  }

  ctx->status = rtems_task_suspend( ctx->id );

  if ( ctx->suspend ) {
    ResumeTask( ctx->worker_id );
  }
}

static const RtemsTaskReqSuspend_Entry
RtemsTaskReqSuspend_Entries[] = {
  { 0, 0, 1, RtemsTaskReqSuspend_Post_Status_InvId },
  { 0, 0, 0, RtemsTaskReqSuspend_Post_Status_AlrdySus },
  { 0, 0, 0, RtemsTaskReqSuspend_Post_Status_Ok }
};

static const uint8_t
RtemsTaskReqSuspend_Map[] = {
  0, 0, 1, 2
};

static size_t RtemsTaskReqSuspend_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqSuspend_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqSuspend_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqSuspend_Fixture = {
  .setup = RtemsTaskReqSuspend_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqSuspend_Teardown_Wrap,
  .scope = RtemsTaskReqSuspend_Scope,
  .initial_context = &RtemsTaskReqSuspend_Instance
};

static inline RtemsTaskReqSuspend_Entry RtemsTaskReqSuspend_PopEntry(
  RtemsTaskReqSuspend_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqSuspend_Entries[
    RtemsTaskReqSuspend_Map[ index ]
  ];
}

static void RtemsTaskReqSuspend_SetPreConditionStates(
  RtemsTaskReqSuspend_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Suspended_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsTaskReqSuspend_Pre_Suspended_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void RtemsTaskReqSuspend_TestVariant( RtemsTaskReqSuspend_Context *ctx )
{
  RtemsTaskReqSuspend_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqSuspend_Pre_Suspended_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqSuspend_Action( ctx );
  RtemsTaskReqSuspend_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_RtemsTaskReqSuspend( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqSuspend, &RtemsTaskReqSuspend_Fixture )
{
  RtemsTaskReqSuspend_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqSuspend_Pre_Id_Invalid;
    ctx->Map.pci[ 0 ] < RtemsTaskReqSuspend_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqSuspend_Pre_Suspended_Yes;
      ctx->Map.pci[ 1 ] < RtemsTaskReqSuspend_Pre_Suspended_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = RtemsTaskReqSuspend_PopEntry( ctx );
      RtemsTaskReqSuspend_SetPreConditionStates( ctx );
      RtemsTaskReqSuspend_TestVariant( ctx );
    }
  }
}

/** @} */
