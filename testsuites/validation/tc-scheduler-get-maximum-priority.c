/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerReqGetMaximumPriority
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

#include <limits.h>
#include <rtems.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSchedulerReqGetMaximumPriority \
 *   spec:/rtems/scheduler/req/get-maximum-priority
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSchedulerReqGetMaximumPriority_Pre_Id_Invalid,
  RtemsSchedulerReqGetMaximumPriority_Pre_Id_Scheduler,
  RtemsSchedulerReqGetMaximumPriority_Pre_Id_NA
} RtemsSchedulerReqGetMaximumPriority_Pre_Id;

typedef enum {
  RtemsSchedulerReqGetMaximumPriority_Pre_Prio_Valid,
  RtemsSchedulerReqGetMaximumPriority_Pre_Prio_Null,
  RtemsSchedulerReqGetMaximumPriority_Pre_Prio_NA
} RtemsSchedulerReqGetMaximumPriority_Pre_Prio;

typedef enum {
  RtemsSchedulerReqGetMaximumPriority_Post_Status_Ok,
  RtemsSchedulerReqGetMaximumPriority_Post_Status_InvAddr,
  RtemsSchedulerReqGetMaximumPriority_Post_Status_InvId,
  RtemsSchedulerReqGetMaximumPriority_Post_Status_NA
} RtemsSchedulerReqGetMaximumPriority_Post_Status;

typedef enum {
  RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_Set,
  RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_Nop,
  RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_NA
} RtemsSchedulerReqGetMaximumPriority_Post_PrioObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_Prio_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_PrioObj : 2;
} RtemsSchedulerReqGetMaximumPriority_Entry;

/**
 * @brief Test context for spec:/rtems/scheduler/req/get-maximum-priority test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of a scheduler.
   */
  rtems_id scheduler_id;

  /**
   * @brief This member provides the object referenced by the ``priority``
   *   parameter.
   */
  rtems_task_priority priority_obj;

  /**
   * @brief This member contains the return value of the
   *   rtems_scheduler_get_maximum_priority() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``scheduler_id`` parameter value.
   */
  rtems_id id;

  /**
   * @brief This member specifies if the ``priority`` parameter value.
   */
  rtems_task_priority *priority;

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
    RtemsSchedulerReqGetMaximumPriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSchedulerReqGetMaximumPriority_Context;

static RtemsSchedulerReqGetMaximumPriority_Context
  RtemsSchedulerReqGetMaximumPriority_Instance;

static const char * const RtemsSchedulerReqGetMaximumPriority_PreDesc_Id[] = {
  "Invalid",
  "Scheduler",
  "NA"
};

static const char * const RtemsSchedulerReqGetMaximumPriority_PreDesc_Prio[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsSchedulerReqGetMaximumPriority_PreDesc[] = {
  RtemsSchedulerReqGetMaximumPriority_PreDesc_Id,
  RtemsSchedulerReqGetMaximumPriority_PreDesc_Prio,
  NULL
};

static void RtemsSchedulerReqGetMaximumPriority_Pre_Id_Prepare(
  RtemsSchedulerReqGetMaximumPriority_Context *ctx,
  RtemsSchedulerReqGetMaximumPriority_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSchedulerReqGetMaximumPriority_Pre_Id_Invalid: {
      /*
       * While the ``scheduler_id`` parameter is not associated with a
       * scheduler.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsSchedulerReqGetMaximumPriority_Pre_Id_Scheduler: {
      /*
       * While the ``scheduler_id`` parameter is associated with a scheduler.
       */
      ctx->id = ctx->scheduler_id;
      break;
    }

    case RtemsSchedulerReqGetMaximumPriority_Pre_Id_NA:
      break;
  }
}

static void RtemsSchedulerReqGetMaximumPriority_Pre_Prio_Prepare(
  RtemsSchedulerReqGetMaximumPriority_Context *ctx,
  RtemsSchedulerReqGetMaximumPriority_Pre_Prio state
)
{
  switch ( state ) {
    case RtemsSchedulerReqGetMaximumPriority_Pre_Prio_Valid: {
      /*
       * While the ``priority`` parameter references an object of type
       * rtems_task_priority.
       */
      ctx->priority = &ctx->priority_obj;
      break;
    }

    case RtemsSchedulerReqGetMaximumPriority_Pre_Prio_Null: {
      /*
       * While the ``priority`` parameter is equal to NULL.
       */
      ctx->priority = NULL;
      break;
    }

    case RtemsSchedulerReqGetMaximumPriority_Pre_Prio_NA:
      break;
  }
}

static void RtemsSchedulerReqGetMaximumPriority_Post_Status_Check(
  RtemsSchedulerReqGetMaximumPriority_Context    *ctx,
  RtemsSchedulerReqGetMaximumPriority_Post_Status state
)
{
  switch ( state ) {
    case RtemsSchedulerReqGetMaximumPriority_Post_Status_Ok: {
      /*
       * The return status of rtems_scheduler_get_maximum_priority() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSchedulerReqGetMaximumPriority_Post_Status_InvAddr: {
      /*
       * The return status of rtems_scheduler_get_maximum_priority() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsSchedulerReqGetMaximumPriority_Post_Status_InvId: {
      /*
       * The return status of rtems_scheduler_get_maximum_priority() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSchedulerReqGetMaximumPriority_Post_Status_NA:
      break;
  }
}

static void RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_Check(
  RtemsSchedulerReqGetMaximumPriority_Context     *ctx,
  RtemsSchedulerReqGetMaximumPriority_Post_PrioObj state
)
{
  switch ( state ) {
    case RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_Set: {
      /*
       * The value of the object referenced by the ``priority`` parameter shall
       * be set to the maximum priority value of the scheduler specified by the
       * ``scheduler_id`` parameter after the return of the
       * rtems_scheduler_get_maximum_priority() call.
       */
      #if defined(RTEMS_SMP)
      T_eq_u32( ctx->priority_obj, INT_MAX );
      #else
      T_eq_u32( ctx->priority_obj, 127 );
      #endif
      break;
    }

    case RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_Nop: {
      /*
       * Objects referenced by the ``priority`` parameter in past calls to
       * rtems_scheduler_get_maximum_priority() shall not be accessed by the
       * rtems_scheduler_get_maximum_priority() call.
       */
      T_eq_u32( ctx->priority_obj, PRIO_INVALID );
      break;
    }

    case RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_NA:
      break;
  }
}

static void RtemsSchedulerReqGetMaximumPriority_Setup(
  RtemsSchedulerReqGetMaximumPriority_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_scheduler_ident(
    TEST_SCHEDULER_A_NAME,
    &ctx->scheduler_id
  );
  T_rsc_success( sc );
}

static void RtemsSchedulerReqGetMaximumPriority_Setup_Wrap( void *arg )
{
  RtemsSchedulerReqGetMaximumPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSchedulerReqGetMaximumPriority_Setup( ctx );
}

static void RtemsSchedulerReqGetMaximumPriority_Prepare(
  RtemsSchedulerReqGetMaximumPriority_Context *ctx
)
{
  ctx->priority_obj = PRIO_INVALID;
}

static void RtemsSchedulerReqGetMaximumPriority_Action(
  RtemsSchedulerReqGetMaximumPriority_Context *ctx
)
{
  ctx->status = rtems_scheduler_get_maximum_priority( ctx->id, ctx->priority );
}

static const RtemsSchedulerReqGetMaximumPriority_Entry
RtemsSchedulerReqGetMaximumPriority_Entries[] = {
  { 0, 0, 0, RtemsSchedulerReqGetMaximumPriority_Post_Status_InvAddr,
    RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_Nop },
  { 0, 0, 0, RtemsSchedulerReqGetMaximumPriority_Post_Status_InvId,
    RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_Nop },
  { 0, 0, 0, RtemsSchedulerReqGetMaximumPriority_Post_Status_Ok,
    RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_Set }
};

static const uint8_t
RtemsSchedulerReqGetMaximumPriority_Map[] = {
  1, 0, 2, 0
};

static size_t RtemsSchedulerReqGetMaximumPriority_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsSchedulerReqGetMaximumPriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsSchedulerReqGetMaximumPriority_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsSchedulerReqGetMaximumPriority_Fixture = {
  .setup = RtemsSchedulerReqGetMaximumPriority_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsSchedulerReqGetMaximumPriority_Scope,
  .initial_context = &RtemsSchedulerReqGetMaximumPriority_Instance
};

static inline RtemsSchedulerReqGetMaximumPriority_Entry
RtemsSchedulerReqGetMaximumPriority_PopEntry(
  RtemsSchedulerReqGetMaximumPriority_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSchedulerReqGetMaximumPriority_Entries[
    RtemsSchedulerReqGetMaximumPriority_Map[ index ]
  ];
}

static void RtemsSchedulerReqGetMaximumPriority_TestVariant(
  RtemsSchedulerReqGetMaximumPriority_Context *ctx
)
{
  RtemsSchedulerReqGetMaximumPriority_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSchedulerReqGetMaximumPriority_Pre_Prio_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  RtemsSchedulerReqGetMaximumPriority_Action( ctx );
  RtemsSchedulerReqGetMaximumPriority_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsSchedulerReqGetMaximumPriority_Post_PrioObj_Check(
    ctx,
    ctx->Map.entry.Post_PrioObj
  );
}

/**
 * @fn void T_case_body_RtemsSchedulerReqGetMaximumPriority( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsSchedulerReqGetMaximumPriority,
  &RtemsSchedulerReqGetMaximumPriority_Fixture
)
{
  RtemsSchedulerReqGetMaximumPriority_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSchedulerReqGetMaximumPriority_Pre_Id_Invalid;
    ctx->Map.pcs[ 0 ] < RtemsSchedulerReqGetMaximumPriority_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSchedulerReqGetMaximumPriority_Pre_Prio_Valid;
      ctx->Map.pcs[ 1 ] < RtemsSchedulerReqGetMaximumPriority_Pre_Prio_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsSchedulerReqGetMaximumPriority_PopEntry( ctx );
      RtemsSchedulerReqGetMaximumPriority_Prepare( ctx );
      RtemsSchedulerReqGetMaximumPriority_TestVariant( ctx );
    }
  }
}

/** @} */
