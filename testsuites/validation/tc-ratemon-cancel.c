/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRatemonReqCancel
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
 * @defgroup RtemsRatemonReqCancel spec:/rtems/ratemon/req/cancel
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsRatemonReqCancel_Pre_Id_Valid,
  RtemsRatemonReqCancel_Pre_Id_Invalid,
  RtemsRatemonReqCancel_Pre_Id_NA
} RtemsRatemonReqCancel_Pre_Id;

typedef enum {
  RtemsRatemonReqCancel_Pre_Caller_OwnerTask,
  RtemsRatemonReqCancel_Pre_Caller_OtherTask,
  RtemsRatemonReqCancel_Pre_Caller_NA
} RtemsRatemonReqCancel_Pre_Caller;

typedef enum {
  RtemsRatemonReqCancel_Pre_State_Inactive,
  RtemsRatemonReqCancel_Pre_State_Active,
  RtemsRatemonReqCancel_Pre_State_Expired,
  RtemsRatemonReqCancel_Pre_State_NA
} RtemsRatemonReqCancel_Pre_State;

typedef enum {
  RtemsRatemonReqCancel_Pre_Postponed_Zero,
  RtemsRatemonReqCancel_Pre_Postponed_One,
  RtemsRatemonReqCancel_Pre_Postponed_Several,
  RtemsRatemonReqCancel_Pre_Postponed_NA
} RtemsRatemonReqCancel_Pre_Postponed;

typedef enum {
  RtemsRatemonReqCancel_Post_Status_Ok,
  RtemsRatemonReqCancel_Post_Status_InvId,
  RtemsRatemonReqCancel_Post_Status_NotOwn,
  RtemsRatemonReqCancel_Post_Status_NA
} RtemsRatemonReqCancel_Post_Status;

typedef enum {
  RtemsRatemonReqCancel_Post_State_Inactive,
  RtemsRatemonReqCancel_Post_State_Nop,
  RtemsRatemonReqCancel_Post_State_NA
} RtemsRatemonReqCancel_Post_State;

typedef enum {
  RtemsRatemonReqCancel_Post_Postponed_Zero,
  RtemsRatemonReqCancel_Post_Postponed_Nop,
  RtemsRatemonReqCancel_Post_Postponed_NA
} RtemsRatemonReqCancel_Post_Postponed;

typedef enum {
  RtemsRatemonReqCancel_Post_Scheduler_Called,
  RtemsRatemonReqCancel_Post_Scheduler_Nop,
  RtemsRatemonReqCancel_Post_Scheduler_NA
} RtemsRatemonReqCancel_Post_Scheduler;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Caller_NA : 1;
  uint16_t Pre_State_NA : 1;
  uint16_t Pre_Postponed_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_State : 2;
  uint16_t Post_Postponed : 2;
  uint16_t Post_Scheduler : 2;
} RtemsRatemonReqCancel_Entry;

/**
 * @brief Test context for spec:/rtems/ratemon/req/cancel test case.
 */
typedef struct {
  /**
   * @brief This member contains a valid identifier of a period.
   */
  rtems_id period_id;

  /**
   * @brief This member is used to receive the
   *   rtems_rate_monotonic_period_status after the action.
   */
  rtems_rate_monotonic_period_status period_status;

  /**
   * @brief This member specifies the ``id`` parameter for the action.
   */
  rtems_id id_param;

  /**
   * @brief This member contains the returned status code of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains the pointer to the function which executes the
   *   action.
   *
   * The action is either executed by the owner task or by the worker task
   * depending on the function pointer used here.  ``argument`` is a pointer to
   * this context structure.
   */
  void ( *do_action )( void *ctx );

  /**
   * @brief This member contains the task identifier of the owner task.
   */
  rtems_id task_id;

  /**
   * @brief This member contains the task identifier of the worker task (which
   *   is not the owner task).
   */
  rtems_id worker_id;

  /**
   * @brief This member contains a backup of the task priority before the
   *   execution of this test.
   */
  rtems_id original_priority;

  /**
   * @brief This member contains the number of postponed jobs before the
   *   action.
   */
  uint32_t postponed_jobs_count;

  /**
   * @brief This member contains the state before the action.
   */
  rtems_rate_monotonic_period_states previous_state;

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
    RtemsRatemonReqCancel_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsRatemonReqCancel_Context;

static RtemsRatemonReqCancel_Context
  RtemsRatemonReqCancel_Instance;

static const char * const RtemsRatemonReqCancel_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsRatemonReqCancel_PreDesc_Caller[] = {
  "OwnerTask",
  "OtherTask",
  "NA"
};

static const char * const RtemsRatemonReqCancel_PreDesc_State[] = {
  "Inactive",
  "Active",
  "Expired",
  "NA"
};

static const char * const RtemsRatemonReqCancel_PreDesc_Postponed[] = {
  "Zero",
  "One",
  "Several",
  "NA"
};

static const char * const * const RtemsRatemonReqCancel_PreDesc[] = {
  RtemsRatemonReqCancel_PreDesc_Id,
  RtemsRatemonReqCancel_PreDesc_Caller,
  RtemsRatemonReqCancel_PreDesc_State,
  RtemsRatemonReqCancel_PreDesc_Postponed,
  NULL
};

static const rtems_interval period_length = 5;
static const rtems_task_priority background_task_priority = 100;
static const rtems_task_priority foreground_task_priority = 10;
static const rtems_event_set wake_main_task_event = RTEMS_EVENT_17;

static void TickTheClock(
  RtemsRatemonReqCancel_Context *ctx,
  uint32_t ticks
)
{
  uint32_t i;
  for ( i = 0; i < ticks; ++i ) {
    TimecounterTick();
  }
}

static void Action( void *ctx_in )
{
  RtemsRatemonReqCancel_Context *ctx = ctx_in;
  ctx->status = rtems_rate_monotonic_cancel( ctx->id_param );
}

static void WorkerTask( rtems_task_argument argument )
{
  RtemsRatemonReqCancel_Context *ctx =
    (RtemsRatemonReqCancel_Context *) argument;

  if ( ctx != NULL ) {
    Action( ctx );
    T_rsc_success( rtems_event_send( ctx->task_id, wake_main_task_event ) );
  }

  T_rsc_success( rtems_task_suspend( RTEMS_SELF ) );
}

static void WorkerTaskAction( void *ctx_in )
{
  rtems_status_code status;
  rtems_event_set event_set;
  RtemsRatemonReqCancel_Context *ctx = ctx_in;

  status = rtems_task_restart( ctx->worker_id, (rtems_task_argument) ctx );
  T_rsc_success( status );

  /* Wait till the worker task finishes */
  status = rtems_event_receive(
    wake_main_task_event,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &event_set
  );
  T_rsc_success( status );
}

static void CreatePostponedJobs(
  RtemsRatemonReqCancel_Context *ctx,
  uint32_t jobs_count
)
{
  rtems_status_code status;
  ctx->postponed_jobs_count = jobs_count;
  if ( ctx->previous_state == RATE_MONOTONIC_ACTIVE ) {
    TickTheClock( ctx, ( jobs_count + 1 ) * period_length );
    status = rtems_rate_monotonic_period( ctx->period_id, period_length );
    T_rsc( status, RTEMS_TIMEOUT );
  } else {
    /* ctx->previous_state == RATE_MONOTONIC_INACTIVE || _EXPIRED */
    TickTheClock( ctx, jobs_count * period_length );
  }
}

static void RtemsRatemonReqCancel_Pre_Id_Prepare(
  RtemsRatemonReqCancel_Context *ctx,
  RtemsRatemonReqCancel_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsRatemonReqCancel_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->period_id;
      break;
    }

    case RtemsRatemonReqCancel_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsRatemonReqCancel_Pre_Id_NA:
      break;
  }
}

static void RtemsRatemonReqCancel_Pre_Caller_Prepare(
  RtemsRatemonReqCancel_Context   *ctx,
  RtemsRatemonReqCancel_Pre_Caller state
)
{
  switch ( state ) {
    case RtemsRatemonReqCancel_Pre_Caller_OwnerTask: {
      /*
       * While the task invoking rtems_rate_monotonic_cancel() is the task
       * which created the period - the owner task.
       */
      ctx->do_action = Action;
      break;
    }

    case RtemsRatemonReqCancel_Pre_Caller_OtherTask: {
      /*
       * While the task invoking rtems_rate_monotonic_cancel() is not the owner
       * task.
       */
      ctx->do_action = WorkerTaskAction;
      break;
    }

    case RtemsRatemonReqCancel_Pre_Caller_NA:
      break;
  }
}

static void RtemsRatemonReqCancel_Pre_State_Prepare(
  RtemsRatemonReqCancel_Context  *ctx,
  RtemsRatemonReqCancel_Pre_State state
)
{
  switch ( state ) {
    case RtemsRatemonReqCancel_Pre_State_Inactive: {
      /*
       * While the ``id`` parameter references an period object in inactive
       * state.
       */
      /* Nothing to do here as the period is newly created. */
      ctx->previous_state = RATE_MONOTONIC_INACTIVE;
      break;
    }

    case RtemsRatemonReqCancel_Pre_State_Active: {
      /*
       * While the ``id`` parameter references an period object in active
       * state.
       */
      rtems_status_code status;
      status = rtems_rate_monotonic_period( ctx->period_id, period_length );
      T_rsc_success( status );
      ctx->previous_state = RATE_MONOTONIC_ACTIVE;
      break;
    }

    case RtemsRatemonReqCancel_Pre_State_Expired: {
      /*
       * While the ``id`` parameter references an period object in expired
       * state.
       */
      rtems_status_code status;
      status = rtems_rate_monotonic_period( ctx->period_id, period_length );
      T_rsc_success( status );
      ctx->previous_state = RATE_MONOTONIC_EXPIRED;
      break;
    }

    case RtemsRatemonReqCancel_Pre_State_NA:
      break;
  }
}

static void RtemsRatemonReqCancel_Pre_Postponed_Prepare(
  RtemsRatemonReqCancel_Context      *ctx,
  RtemsRatemonReqCancel_Pre_Postponed state
)
{
  switch ( state ) {
    case RtemsRatemonReqCancel_Pre_Postponed_Zero: {
      /*
       * While the period is not in expired state.
       */
      ctx->postponed_jobs_count = 0;
      break;
    }

    case RtemsRatemonReqCancel_Pre_Postponed_One: {
      /*
       * While there is one postponed job.
       */
      CreatePostponedJobs( ctx, 1 );
      break;
    }

    case RtemsRatemonReqCancel_Pre_Postponed_Several: {
      /*
       * While there are two or more postponed jobs.
       */
      CreatePostponedJobs( ctx, 5 );
      break;
    }

    case RtemsRatemonReqCancel_Pre_Postponed_NA:
      break;
  }
}

static void RtemsRatemonReqCancel_Post_Status_Check(
  RtemsRatemonReqCancel_Context    *ctx,
  RtemsRatemonReqCancel_Post_Status state
)
{
  switch ( state ) {
    case RtemsRatemonReqCancel_Post_Status_Ok: {
      /*
       * The return status of rtems_rate_monotonic_cancel() shall be
       * RTEMS_SUCCESSFUL
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsRatemonReqCancel_Post_Status_InvId: {
      /*
       * The return status of rtems_rate_monotonic_cancel() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsRatemonReqCancel_Post_Status_NotOwn: {
      /*
       * The return status of rtems_rate_monotonic_cancel() shall be
       * RTEMS_NOT_OWNER_OF_RESOURCE.
       */
      T_rsc( ctx->status, RTEMS_NOT_OWNER_OF_RESOURCE );
      break;
    }

    case RtemsRatemonReqCancel_Post_Status_NA:
      break;
  }
}

static void RtemsRatemonReqCancel_Post_State_Check(
  RtemsRatemonReqCancel_Context   *ctx,
  RtemsRatemonReqCancel_Post_State state
)
{
  switch ( state ) {
    case RtemsRatemonReqCancel_Post_State_Inactive: {
      /*
       * The state of the period shall be inactive after the return of the
       * rtems_rate_monotonic_cancel() call.
       */
      T_eq_int( ctx->period_status.state, RATE_MONOTONIC_INACTIVE );
      break;
    }

    case RtemsRatemonReqCancel_Post_State_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_rate_monotonic_cancel() shall not be accessed by the
       * rtems_rate_monotonic_cancel() call (see also Nop).
       */
      T_eq_u32( ctx->period_status.state, ctx->previous_state );
      break;
    }

    case RtemsRatemonReqCancel_Post_State_NA:
      break;
  }
}

static void RtemsRatemonReqCancel_Post_Postponed_Check(
  RtemsRatemonReqCancel_Context       *ctx,
  RtemsRatemonReqCancel_Post_Postponed state
)
{
  switch ( state ) {
    case RtemsRatemonReqCancel_Post_Postponed_Zero: {
      /*
       * There shall be no postponed jobs after the return of the
       * rtems_rate_monotonic_cancel() call.
       */
      T_eq_u32( ctx->period_status.postponed_jobs_count, 0 );
      break;
    }

    case RtemsRatemonReqCancel_Post_Postponed_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_rate_monotonic_cancel() shall not be accessed by the
       * rtems_rate_monotonic_cancel() call (see also Nop).
       */
      T_eq_u32( ctx->period_status.postponed_jobs_count,
        ctx->postponed_jobs_count );
      break;
    }

    case RtemsRatemonReqCancel_Post_Postponed_NA:
      break;
  }
}

static void RtemsRatemonReqCancel_Post_Scheduler_Check(
  RtemsRatemonReqCancel_Context       *ctx,
  RtemsRatemonReqCancel_Post_Scheduler state
)
{
  switch ( state ) {
    case RtemsRatemonReqCancel_Post_Scheduler_Called: {
      /*
       * The last call of the rtems_rate_monotonic_cancel() function shall
       * execute the ``cancel_job`` scheduler operation of the home scheduler.
       */
      /* Cannot be tested as the effect is unknown. */
      break;
    }

    case RtemsRatemonReqCancel_Post_Scheduler_Nop: {
      /*
       * The last call of the rtems_rate_monotonic_cancel() function shall not
       * execute any scheduler operation.
       */
      /* Cannot be tested as the effect is unknown. */
      break;
    }

    case RtemsRatemonReqCancel_Post_Scheduler_NA:
      break;
  }
}

static void RtemsRatemonReqCancel_Setup( RtemsRatemonReqCancel_Context *ctx )
{
  rtems_status_code status;
  rtems_task_priority priority;
  rtems_event_set event_set;
  ctx->worker_id = RTEMS_INVALID_ID;

  status = rtems_task_ident(
    RTEMS_SELF,
    RTEMS_SEARCH_ALL_NODES,
    &ctx->task_id
  );
  T_rsc_success( status );

  status = rtems_task_set_priority(
    RTEMS_SELF,
    RTEMS_CURRENT_PRIORITY,
    &ctx->original_priority
  );
  T_rsc_success( status );

  status = rtems_task_set_priority(
    RTEMS_SELF,
    background_task_priority,
    &priority
  );
  T_rsc_success( status );

  status = rtems_task_create(
    rtems_build_name( 'W', 'O', 'R', 'K' ),
    foreground_task_priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_id
  );
  T_rsc_success( status );

  /* Defensive programming: clean away any pending events */
  status = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    RTEMS_NO_TIMEOUT,
    &event_set
  );
  T_true( status == RTEMS_SUCCESSFUL || status == RTEMS_UNSATISFIED );

  status = rtems_task_start(
    ctx->worker_id,
    WorkerTask,
    (rtems_task_argument) NULL
  );
  T_rsc_success( status );
}

static void RtemsRatemonReqCancel_Setup_Wrap( void *arg )
{
  RtemsRatemonReqCancel_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRatemonReqCancel_Setup( ctx );
}

static void RtemsRatemonReqCancel_Teardown(
  RtemsRatemonReqCancel_Context *ctx
)
{
  rtems_status_code status;
  rtems_task_priority priority;

  T_rsc_success( rtems_task_delete( ctx->worker_id ) );

  status = rtems_task_set_priority(
    RTEMS_SELF,
    ctx->original_priority,
    &priority
  );
  T_rsc_success( status );
}

static void RtemsRatemonReqCancel_Teardown_Wrap( void *arg )
{
  RtemsRatemonReqCancel_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRatemonReqCancel_Teardown( ctx );
}

static void RtemsRatemonReqCancel_Prepare( RtemsRatemonReqCancel_Context *ctx )
{
  rtems_status_code status;
  status =  rtems_rate_monotonic_create(
    rtems_build_name( 'R', 'M', 'O', 'N' ),
    &ctx->period_id
  );
  T_rsc_success( status );

  ctx->postponed_jobs_count = 0;
}

static void RtemsRatemonReqCancel_Action( RtemsRatemonReqCancel_Context *ctx )
{
  rtems_status_code status;

  ctx->do_action( ctx );

  status = rtems_rate_monotonic_get_status(
    ctx->period_id,
    &ctx->period_status
  );
  T_rsc_success( status );
}

static void RtemsRatemonReqCancel_Cleanup( RtemsRatemonReqCancel_Context *ctx )
{
  T_rsc_success( rtems_rate_monotonic_delete( ctx->period_id ) );
}

static const RtemsRatemonReqCancel_Entry
RtemsRatemonReqCancel_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsRatemonReqCancel_Post_Status_InvId,
    RtemsRatemonReqCancel_Post_State_Nop,
    RtemsRatemonReqCancel_Post_Postponed_Nop,
    RtemsRatemonReqCancel_Post_Scheduler_Nop },
  { 1, 0, 0, 0, 0, RtemsRatemonReqCancel_Post_Status_NA,
    RtemsRatemonReqCancel_Post_State_NA,
    RtemsRatemonReqCancel_Post_Postponed_NA,
    RtemsRatemonReqCancel_Post_Scheduler_NA },
  { 0, 0, 0, 0, 0, RtemsRatemonReqCancel_Post_Status_Ok,
    RtemsRatemonReqCancel_Post_State_Inactive,
    RtemsRatemonReqCancel_Post_Postponed_Zero,
    RtemsRatemonReqCancel_Post_Scheduler_Called },
  { 0, 0, 0, 0, 0, RtemsRatemonReqCancel_Post_Status_NotOwn,
    RtemsRatemonReqCancel_Post_State_Nop,
    RtemsRatemonReqCancel_Post_Postponed_Nop,
    RtemsRatemonReqCancel_Post_Scheduler_Nop },
  { 1, 0, 0, 0, 0, RtemsRatemonReqCancel_Post_Status_NA,
    RtemsRatemonReqCancel_Post_State_NA,
    RtemsRatemonReqCancel_Post_Postponed_NA,
    RtemsRatemonReqCancel_Post_Scheduler_NA },
  { 0, 0, 0, 0, 1, RtemsRatemonReqCancel_Post_Status_InvId,
    RtemsRatemonReqCancel_Post_State_Nop,
    RtemsRatemonReqCancel_Post_Postponed_Nop,
    RtemsRatemonReqCancel_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 1, RtemsRatemonReqCancel_Post_Status_Ok,
    RtemsRatemonReqCancel_Post_State_Inactive,
    RtemsRatemonReqCancel_Post_Postponed_NA,
    RtemsRatemonReqCancel_Post_Scheduler_Called },
  { 0, 0, 0, 0, 1, RtemsRatemonReqCancel_Post_Status_NotOwn,
    RtemsRatemonReqCancel_Post_State_Nop,
    RtemsRatemonReqCancel_Post_Postponed_Nop,
    RtemsRatemonReqCancel_Post_Scheduler_Nop }
};

static const uint8_t
RtemsRatemonReqCancel_Map[] = {
  6, 1, 1, 2, 2, 2, 4, 2, 2, 7, 1, 1, 3, 3, 3, 4, 3, 3, 5, 1, 1, 0, 0, 0, 4, 0,
  0, 5, 1, 1, 0, 0, 0, 4, 0, 0
};

static size_t RtemsRatemonReqCancel_Scope( void *arg, char *buf, size_t n )
{
  RtemsRatemonReqCancel_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsRatemonReqCancel_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsRatemonReqCancel_Fixture = {
  .setup = RtemsRatemonReqCancel_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsRatemonReqCancel_Teardown_Wrap,
  .scope = RtemsRatemonReqCancel_Scope,
  .initial_context = &RtemsRatemonReqCancel_Instance
};

static inline RtemsRatemonReqCancel_Entry RtemsRatemonReqCancel_PopEntry(
  RtemsRatemonReqCancel_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsRatemonReqCancel_Entries[
    RtemsRatemonReqCancel_Map[ index ]
  ];
}

static void RtemsRatemonReqCancel_SetPreConditionStates(
  RtemsRatemonReqCancel_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_Postponed_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsRatemonReqCancel_Pre_Postponed_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void RtemsRatemonReqCancel_TestVariant(
  RtemsRatemonReqCancel_Context *ctx
)
{
  RtemsRatemonReqCancel_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsRatemonReqCancel_Pre_Caller_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsRatemonReqCancel_Pre_State_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsRatemonReqCancel_Pre_Postponed_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsRatemonReqCancel_Action( ctx );
  RtemsRatemonReqCancel_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsRatemonReqCancel_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  RtemsRatemonReqCancel_Post_Postponed_Check(
    ctx,
    ctx->Map.entry.Post_Postponed
  );
  RtemsRatemonReqCancel_Post_Scheduler_Check(
    ctx,
    ctx->Map.entry.Post_Scheduler
  );
}

/**
 * @fn void T_case_body_RtemsRatemonReqCancel( void )
 */
T_TEST_CASE_FIXTURE( RtemsRatemonReqCancel, &RtemsRatemonReqCancel_Fixture )
{
  RtemsRatemonReqCancel_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsRatemonReqCancel_Pre_Id_Valid;
    ctx->Map.pci[ 0 ] < RtemsRatemonReqCancel_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsRatemonReqCancel_Pre_Caller_OwnerTask;
      ctx->Map.pci[ 1 ] < RtemsRatemonReqCancel_Pre_Caller_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsRatemonReqCancel_Pre_State_Inactive;
        ctx->Map.pci[ 2 ] < RtemsRatemonReqCancel_Pre_State_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsRatemonReqCancel_Pre_Postponed_Zero;
          ctx->Map.pci[ 3 ] < RtemsRatemonReqCancel_Pre_Postponed_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = RtemsRatemonReqCancel_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsRatemonReqCancel_SetPreConditionStates( ctx );
          RtemsRatemonReqCancel_Prepare( ctx );
          RtemsRatemonReqCancel_TestVariant( ctx );
          RtemsRatemonReqCancel_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
