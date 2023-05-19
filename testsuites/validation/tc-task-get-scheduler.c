/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqGetScheduler
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
 * @defgroup RtemsTaskReqGetScheduler spec:/rtems/task/req/get-scheduler
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqGetScheduler_Pre_Id_Invalid,
  RtemsTaskReqGetScheduler_Pre_Id_Task,
  RtemsTaskReqGetScheduler_Pre_Id_NA
} RtemsTaskReqGetScheduler_Pre_Id;

typedef enum {
  RtemsTaskReqGetScheduler_Pre_SchedulerID_Valid,
  RtemsTaskReqGetScheduler_Pre_SchedulerID_Null,
  RtemsTaskReqGetScheduler_Pre_SchedulerID_NA
} RtemsTaskReqGetScheduler_Pre_SchedulerID;

typedef enum {
  RtemsTaskReqGetScheduler_Post_Status_Ok,
  RtemsTaskReqGetScheduler_Post_Status_InvAddr,
  RtemsTaskReqGetScheduler_Post_Status_InvId,
  RtemsTaskReqGetScheduler_Post_Status_NA
} RtemsTaskReqGetScheduler_Post_Status;

typedef enum {
  RtemsTaskReqGetScheduler_Post_SchedulerIDObj_Set,
  RtemsTaskReqGetScheduler_Post_SchedulerIDObj_Nop,
  RtemsTaskReqGetScheduler_Post_SchedulerIDObj_NA
} RtemsTaskReqGetScheduler_Post_SchedulerIDObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_SchedulerID_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_SchedulerIDObj : 2;
} RtemsTaskReqGetScheduler_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/get-scheduler test case.
 */
typedef struct {
  /**
   * @brief This member provides the object referenced by the ``scheduler_id``
   *   parameter.
   */
  rtems_id scheduler_id_obj;

  /**
   * @brief This member contains the return value of the
   *   rtems_task_get_scheduler() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``task_id`` parameter value.
   */
  rtems_id id;

  /**
   * @brief This member specifies if the ``scheduler_id`` parameter value.
   */
  rtems_id *scheduler_id;

  struct {
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
    RtemsTaskReqGetScheduler_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqGetScheduler_Context;

static RtemsTaskReqGetScheduler_Context
  RtemsTaskReqGetScheduler_Instance;

static const char * const RtemsTaskReqGetScheduler_PreDesc_Id[] = {
  "Invalid",
  "Task",
  "NA"
};

static const char * const RtemsTaskReqGetScheduler_PreDesc_SchedulerID[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsTaskReqGetScheduler_PreDesc[] = {
  RtemsTaskReqGetScheduler_PreDesc_Id,
  RtemsTaskReqGetScheduler_PreDesc_SchedulerID,
  NULL
};

static void RtemsTaskReqGetScheduler_Pre_Id_Prepare(
  RtemsTaskReqGetScheduler_Context *ctx,
  RtemsTaskReqGetScheduler_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqGetScheduler_Pre_Id_Invalid: {
      /*
       * While the ``task_id`` parameter is not associated with a task.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsTaskReqGetScheduler_Pre_Id_Task: {
      /*
       * While the ``task_id`` parameter is associated with a task.
       */
      ctx->id = RTEMS_SELF;
      break;
    }

    case RtemsTaskReqGetScheduler_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqGetScheduler_Pre_SchedulerID_Prepare(
  RtemsTaskReqGetScheduler_Context        *ctx,
  RtemsTaskReqGetScheduler_Pre_SchedulerID state
)
{
  switch ( state ) {
    case RtemsTaskReqGetScheduler_Pre_SchedulerID_Valid: {
      /*
       * While the ``scheduler_id`` parameter references an object of type
       * rtems_id.
       */
      ctx->scheduler_id = &ctx->scheduler_id_obj;
      break;
    }

    case RtemsTaskReqGetScheduler_Pre_SchedulerID_Null: {
      /*
       * While the ``scheduler_id`` parameter is equal to NULL.
       */
      ctx->scheduler_id = NULL;
      break;
    }

    case RtemsTaskReqGetScheduler_Pre_SchedulerID_NA:
      break;
  }
}

static void RtemsTaskReqGetScheduler_Post_Status_Check(
  RtemsTaskReqGetScheduler_Context    *ctx,
  RtemsTaskReqGetScheduler_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqGetScheduler_Post_Status_Ok: {
      /*
       * The return status of rtems_task_get_scheduler() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqGetScheduler_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_get_scheduler() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqGetScheduler_Post_Status_InvId: {
      /*
       * The return status of rtems_task_get_scheduler() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqGetScheduler_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqGetScheduler_Post_SchedulerIDObj_Check(
  RtemsTaskReqGetScheduler_Context            *ctx,
  RtemsTaskReqGetScheduler_Post_SchedulerIDObj state
)
{
  switch ( state ) {
    case RtemsTaskReqGetScheduler_Post_SchedulerIDObj_Set: {
      /*
       * The value of the object referenced by the ``scheduler_id`` parameter
       * shall be set to the object identifier of the home scheduler of the
       * task specified by the ``task_id`` parameter at some point during the
       * call after the return of the rtems_task_get_scheduler() call.
       */
      T_eq_u32( ctx->scheduler_id_obj, 0x0f010001 );
      break;
    }

    case RtemsTaskReqGetScheduler_Post_SchedulerIDObj_Nop: {
      /*
       * Objects referenced by the ``scheduler_id`` parameter in past calls to
       * rtems_task_get_scheduler() shall not be accessed by the
       * rtems_task_get_scheduler() call.
       */
      T_eq_u32( ctx->scheduler_id_obj, INVALID_ID );
      break;
    }

    case RtemsTaskReqGetScheduler_Post_SchedulerIDObj_NA:
      break;
  }
}

static void RtemsTaskReqGetScheduler_Prepare(
  RtemsTaskReqGetScheduler_Context *ctx
)
{
  ctx->scheduler_id_obj = INVALID_ID;
}

static void RtemsTaskReqGetScheduler_Action(
  RtemsTaskReqGetScheduler_Context *ctx
)
{
  ctx->status = rtems_task_get_scheduler( ctx->id, ctx->scheduler_id );
}

static const RtemsTaskReqGetScheduler_Entry
RtemsTaskReqGetScheduler_Entries[] = {
  { 0, 0, 0, RtemsTaskReqGetScheduler_Post_Status_InvAddr,
    RtemsTaskReqGetScheduler_Post_SchedulerIDObj_Nop },
  { 0, 0, 0, RtemsTaskReqGetScheduler_Post_Status_InvId,
    RtemsTaskReqGetScheduler_Post_SchedulerIDObj_Nop },
  { 0, 0, 0, RtemsTaskReqGetScheduler_Post_Status_Ok,
    RtemsTaskReqGetScheduler_Post_SchedulerIDObj_Set }
};

static const uint8_t
RtemsTaskReqGetScheduler_Map[] = {
  1, 0, 2, 0
};

static size_t RtemsTaskReqGetScheduler_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqGetScheduler_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsTaskReqGetScheduler_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsTaskReqGetScheduler_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsTaskReqGetScheduler_Scope,
  .initial_context = &RtemsTaskReqGetScheduler_Instance
};

static inline RtemsTaskReqGetScheduler_Entry RtemsTaskReqGetScheduler_PopEntry(
  RtemsTaskReqGetScheduler_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqGetScheduler_Entries[
    RtemsTaskReqGetScheduler_Map[ index ]
  ];
}

static void RtemsTaskReqGetScheduler_TestVariant(
  RtemsTaskReqGetScheduler_Context *ctx
)
{
  RtemsTaskReqGetScheduler_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqGetScheduler_Pre_SchedulerID_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqGetScheduler_Action( ctx );
  RtemsTaskReqGetScheduler_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsTaskReqGetScheduler_Post_SchedulerIDObj_Check(
    ctx,
    ctx->Map.entry.Post_SchedulerIDObj
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqGetScheduler( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskReqGetScheduler,
  &RtemsTaskReqGetScheduler_Fixture
)
{
  RtemsTaskReqGetScheduler_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTaskReqGetScheduler_Pre_Id_Invalid;
    ctx->Map.pcs[ 0 ] < RtemsTaskReqGetScheduler_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTaskReqGetScheduler_Pre_SchedulerID_Valid;
      ctx->Map.pcs[ 1 ] < RtemsTaskReqGetScheduler_Pre_SchedulerID_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsTaskReqGetScheduler_PopEntry( ctx );
      RtemsTaskReqGetScheduler_Prepare( ctx );
      RtemsTaskReqGetScheduler_TestVariant( ctx );
    }
  }
}

/** @} */
