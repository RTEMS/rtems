/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqResume
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
 * @defgroup RtemsTaskReqResume spec:/rtems/task/req/resume
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqResume_Pre_Id_Invalid,
  RtemsTaskReqResume_Pre_Id_Task,
  RtemsTaskReqResume_Pre_Id_NA
} RtemsTaskReqResume_Pre_Id;

typedef enum {
  RtemsTaskReqResume_Pre_Suspended_Yes,
  RtemsTaskReqResume_Pre_Suspended_No,
  RtemsTaskReqResume_Pre_Suspended_NA
} RtemsTaskReqResume_Pre_Suspended;

typedef enum {
  RtemsTaskReqResume_Post_Status_Ok,
  RtemsTaskReqResume_Post_Status_InvId,
  RtemsTaskReqResume_Post_Status_IncStat,
  RtemsTaskReqResume_Post_Status_NA
} RtemsTaskReqResume_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_Suspended_NA : 1;
  uint8_t Post_Status : 2;
} RtemsTaskReqResume_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/resume test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of a task.
   */
  rtems_id worker_id;

  /**
   * @brief If this member is true, then the worker is suspended before the
   *   rtems_task_resume() call.
   */
  bool suspend;

  /**
   * @brief This member contains the return value of the rtems_task_resume()
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
    RtemsTaskReqResume_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqResume_Context;

static RtemsTaskReqResume_Context
  RtemsTaskReqResume_Instance;

static const char * const RtemsTaskReqResume_PreDesc_Id[] = {
  "Invalid",
  "Task",
  "NA"
};

static const char * const RtemsTaskReqResume_PreDesc_Suspended[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsTaskReqResume_PreDesc[] = {
  RtemsTaskReqResume_PreDesc_Id,
  RtemsTaskReqResume_PreDesc_Suspended,
  NULL
};

static void Worker( rtems_task_argument arg )
{
  while ( true ) {
    /* Do nothing */
  }
}

static void RtemsTaskReqResume_Pre_Id_Prepare(
  RtemsTaskReqResume_Context *ctx,
  RtemsTaskReqResume_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqResume_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a task.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsTaskReqResume_Pre_Id_Task: {
      /*
       * While the ``id`` parameter is associated with a task.
       */
      ctx->id = ctx->worker_id;
      break;
    }

    case RtemsTaskReqResume_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqResume_Pre_Suspended_Prepare(
  RtemsTaskReqResume_Context      *ctx,
  RtemsTaskReqResume_Pre_Suspended state
)
{
  switch ( state ) {
    case RtemsTaskReqResume_Pre_Suspended_Yes: {
      /*
       * While the task specified by the ``id`` parameter is suspended.
       */
      ctx->suspend = true;
      break;
    }

    case RtemsTaskReqResume_Pre_Suspended_No: {
      /*
       * While the task specified by the ``id`` parameter is not suspended.
       */
      ctx->suspend = false;
      break;
    }

    case RtemsTaskReqResume_Pre_Suspended_NA:
      break;
  }
}

static void RtemsTaskReqResume_Post_Status_Check(
  RtemsTaskReqResume_Context    *ctx,
  RtemsTaskReqResume_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqResume_Post_Status_Ok: {
      /*
       * The return status of rtems_task_resume() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqResume_Post_Status_InvId: {
      /*
       * The return status of rtems_task_resume() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqResume_Post_Status_IncStat: {
      /*
       * The return status of rtems_task_resume() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsTaskReqResume_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqResume_Setup( RtemsTaskReqResume_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", PRIO_LOW );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsTaskReqResume_Setup_Wrap( void *arg )
{
  RtemsTaskReqResume_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqResume_Setup( ctx );
}

static void RtemsTaskReqResume_Teardown( RtemsTaskReqResume_Context *ctx )
{
  DeleteTask( ctx->worker_id );
}

static void RtemsTaskReqResume_Teardown_Wrap( void *arg )
{
  RtemsTaskReqResume_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqResume_Teardown( ctx );
}

static void RtemsTaskReqResume_Action( RtemsTaskReqResume_Context *ctx )
{
  if ( ctx->suspend ) {
    SuspendTask( ctx->worker_id );
  }

  ctx->status = rtems_task_resume( ctx->id );

  if ( ctx->suspend && ctx->status != RTEMS_SUCCESSFUL ) {
    ResumeTask( ctx->worker_id );
  }
}

static const RtemsTaskReqResume_Entry
RtemsTaskReqResume_Entries[] = {
  { 0, 0, 1, RtemsTaskReqResume_Post_Status_InvId },
  { 0, 0, 0, RtemsTaskReqResume_Post_Status_Ok },
  { 0, 0, 0, RtemsTaskReqResume_Post_Status_IncStat }
};

static const uint8_t
RtemsTaskReqResume_Map[] = {
  0, 0, 1, 2
};

static size_t RtemsTaskReqResume_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqResume_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqResume_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqResume_Fixture = {
  .setup = RtemsTaskReqResume_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqResume_Teardown_Wrap,
  .scope = RtemsTaskReqResume_Scope,
  .initial_context = &RtemsTaskReqResume_Instance
};

static inline RtemsTaskReqResume_Entry RtemsTaskReqResume_PopEntry(
  RtemsTaskReqResume_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqResume_Entries[
    RtemsTaskReqResume_Map[ index ]
  ];
}

static void RtemsTaskReqResume_SetPreConditionStates(
  RtemsTaskReqResume_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Suspended_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsTaskReqResume_Pre_Suspended_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void RtemsTaskReqResume_TestVariant( RtemsTaskReqResume_Context *ctx )
{
  RtemsTaskReqResume_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqResume_Pre_Suspended_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqResume_Action( ctx );
  RtemsTaskReqResume_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_RtemsTaskReqResume( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqResume, &RtemsTaskReqResume_Fixture )
{
  RtemsTaskReqResume_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqResume_Pre_Id_Invalid;
    ctx->Map.pci[ 0 ] < RtemsTaskReqResume_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqResume_Pre_Suspended_Yes;
      ctx->Map.pci[ 1 ] < RtemsTaskReqResume_Pre_Suspended_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = RtemsTaskReqResume_PopEntry( ctx );
      RtemsTaskReqResume_SetPreConditionStates( ctx );
      RtemsTaskReqResume_TestVariant( ctx );
    }
  }
}

/** @} */
