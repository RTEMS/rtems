/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRatemonReqPeriod
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
 * @defgroup RtemsRatemonReqPeriod spec:/rtems/ratemon/req/period
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsRatemonReqPeriod_Pre_Id_Valid,
  RtemsRatemonReqPeriod_Pre_Id_Invalid,
  RtemsRatemonReqPeriod_Pre_Id_NA
} RtemsRatemonReqPeriod_Pre_Id;

typedef enum {
  RtemsRatemonReqPeriod_Pre_Caller_OwnerTask,
  RtemsRatemonReqPeriod_Pre_Caller_OtherTask,
  RtemsRatemonReqPeriod_Pre_Caller_NA
} RtemsRatemonReqPeriod_Pre_Caller;

typedef enum {
  RtemsRatemonReqPeriod_Pre_Length_Ticks,
  RtemsRatemonReqPeriod_Pre_Length_Status,
  RtemsRatemonReqPeriod_Pre_Length_NA
} RtemsRatemonReqPeriod_Pre_Length;

typedef enum {
  RtemsRatemonReqPeriod_Pre_State_Inactive,
  RtemsRatemonReqPeriod_Pre_State_Active,
  RtemsRatemonReqPeriod_Pre_State_Expired,
  RtemsRatemonReqPeriod_Pre_State_NA
} RtemsRatemonReqPeriod_Pre_State;

typedef enum {
  RtemsRatemonReqPeriod_Pre_Postponed_Zero,
  RtemsRatemonReqPeriod_Pre_Postponed_One,
  RtemsRatemonReqPeriod_Pre_Postponed_Several,
  RtemsRatemonReqPeriod_Pre_Postponed_NA
} RtemsRatemonReqPeriod_Pre_Postponed;

typedef enum {
  RtemsRatemonReqPeriod_Pre_InactiveCause_New,
  RtemsRatemonReqPeriod_Pre_InactiveCause_Canceled,
  RtemsRatemonReqPeriod_Pre_InactiveCause_NA
} RtemsRatemonReqPeriod_Pre_InactiveCause;

typedef enum {
  RtemsRatemonReqPeriod_Post_Status_Ok,
  RtemsRatemonReqPeriod_Post_Status_InvId,
  RtemsRatemonReqPeriod_Post_Status_NotOwn,
  RtemsRatemonReqPeriod_Post_Status_NotDef,
  RtemsRatemonReqPeriod_Post_Status_TimeOut,
  RtemsRatemonReqPeriod_Post_Status_NA
} RtemsRatemonReqPeriod_Post_Status;

typedef enum {
  RtemsRatemonReqPeriod_Post_State_Inactive,
  RtemsRatemonReqPeriod_Post_State_Active,
  RtemsRatemonReqPeriod_Post_State_Expired,
  RtemsRatemonReqPeriod_Post_State_Nop,
  RtemsRatemonReqPeriod_Post_State_NA
} RtemsRatemonReqPeriod_Post_State;

typedef enum {
  RtemsRatemonReqPeriod_Post_Postponed_Zero,
  RtemsRatemonReqPeriod_Post_Postponed_OneOrMore,
  RtemsRatemonReqPeriod_Post_Postponed_Nop,
  RtemsRatemonReqPeriod_Post_Postponed_NA
} RtemsRatemonReqPeriod_Post_Postponed;

typedef enum {
  RtemsRatemonReqPeriod_Post_Delay_None,
  RtemsRatemonReqPeriod_Post_Delay_TillDeadline,
  RtemsRatemonReqPeriod_Post_Delay_NA
} RtemsRatemonReqPeriod_Post_Delay;

typedef enum {
  RtemsRatemonReqPeriod_Post_Scheduler_Called,
  RtemsRatemonReqPeriod_Post_Scheduler_Nop,
  RtemsRatemonReqPeriod_Post_Scheduler_NA
} RtemsRatemonReqPeriod_Post_Scheduler;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_Caller_NA : 1;
  uint32_t Pre_Length_NA : 1;
  uint32_t Pre_State_NA : 1;
  uint32_t Pre_Postponed_NA : 1;
  uint32_t Pre_InactiveCause_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_State : 3;
  uint32_t Post_Postponed : 2;
  uint32_t Post_Delay : 2;
  uint32_t Post_Scheduler : 2;
} RtemsRatemonReqPeriod_Entry;

/**
 * @brief Test context for spec:/rtems/ratemon/req/period test case.
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
   * @brief This member specifies the ``length`` parameter for the action.
   */
  rtems_interval length_param;

  /**
   * @brief This member contains the returned status code of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains the pointer to the function which executes the
   *   action.
   *
   * The action is either executed by the owner task or by the worker task
   * depending on the function pointer used here.  ``ctx_arg`` must be a
   * pointer to this context structure.
   */
  uint32_t ( *do_action )( void *ctx, void (*todo)( void *ctx_arg ) );

  /**
   * @brief This member serves to pass the pointer to the function which the
   *   work owner task shall execute from function ``OwnerDoWork`` to function
   *   ``WorkerTask``.
   */
  void (*worker_todo)( void *ctx );

  /**
   * @brief This member contains the owner task identifier of the owner task.
   */
  rtems_id task_id;

  /**
   * @brief This member contains the owner task identifier of the worker task
   *   (which is not the owner task).
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

  /**
   * @brief This member contains the number of clock ticks passed since the
   *   test started.
   */
  uint32_t test_duration;

  /**
   * @brief This member contains the number of clock ticks passed since the
   *   test started till (before) the rtems_rate_monotonic_period() action is
   *   invoked.
   */
  uint32_t test_duration_till_action;

  /**
   * @brief This member contains the number of times the
   *   rtems_rate_monotonic_period() function returned since the test started.
   */
  uint32_t period_calls;

  /**
   * @brief This member contains the number of clock ticks which passed in the
   *   action till the rtems_rate_monotonic_period() function returned.
   */
  uint32_t action_duration;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 6 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    RtemsRatemonReqPeriod_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsRatemonReqPeriod_Context;

static RtemsRatemonReqPeriod_Context
  RtemsRatemonReqPeriod_Instance;

static const char * const RtemsRatemonReqPeriod_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsRatemonReqPeriod_PreDesc_Caller[] = {
  "OwnerTask",
  "OtherTask",
  "NA"
};

static const char * const RtemsRatemonReqPeriod_PreDesc_Length[] = {
  "Ticks",
  "Status",
  "NA"
};

static const char * const RtemsRatemonReqPeriod_PreDesc_State[] = {
  "Inactive",
  "Active",
  "Expired",
  "NA"
};

static const char * const RtemsRatemonReqPeriod_PreDesc_Postponed[] = {
  "Zero",
  "One",
  "Several",
  "NA"
};

static const char * const RtemsRatemonReqPeriod_PreDesc_InactiveCause[] = {
  "New",
  "Canceled",
  "NA"
};

static const char * const * const RtemsRatemonReqPeriod_PreDesc[] = {
  RtemsRatemonReqPeriod_PreDesc_Id,
  RtemsRatemonReqPeriod_PreDesc_Caller,
  RtemsRatemonReqPeriod_PreDesc_Length,
  RtemsRatemonReqPeriod_PreDesc_State,
  RtemsRatemonReqPeriod_PreDesc_Postponed,
  RtemsRatemonReqPeriod_PreDesc_InactiveCause,
  NULL
};

static const rtems_interval period_length = 5;
static const rtems_task_priority background_task_priority = 100;
static const rtems_task_priority foreground_task_priority = 10;
static const rtems_event_set wake_main_task_event = RTEMS_EVENT_17;

static void TickTheClock(
  RtemsRatemonReqPeriod_Context *ctx,
  uint32_t ticks
)
{
  uint32_t i;
  for ( i = 0; i < ticks; ++i ) {
    TimecounterTick();
    ctx->test_duration++;
  }
}

static rtems_status_code CallPeriodFunction(
  RtemsRatemonReqPeriod_Context *ctx,
  rtems_id id,
  rtems_interval length
)
{
  rtems_status_code status;
  status = rtems_rate_monotonic_period( id, length );
  ctx->period_calls++;
  return status;
}

static void CreatePeriod( void *ctx_in )
{
  RtemsRatemonReqPeriod_Context *ctx = ctx_in;
  rtems_status_code status;
  status =  rtems_rate_monotonic_create(
    rtems_build_name( 'R', 'M', 'O', 'N' ),
    &ctx->period_id
  );
  T_rsc_success( status );
}

static void DeletePeriod( void *ctx_in )
{
  RtemsRatemonReqPeriod_Context *ctx = ctx_in;
  T_rsc_success( rtems_rate_monotonic_delete( ctx->period_id ) );
}

static void CancelPeriod( void *ctx_in )
{
  RtemsRatemonReqPeriod_Context *ctx = ctx_in;
  T_rsc_success( rtems_rate_monotonic_cancel( ctx->period_id ) );
}

static void CallPeriod( void *ctx_in )
{
  RtemsRatemonReqPeriod_Context *ctx = ctx_in;
  T_rsc_success( CallPeriodFunction( ctx, ctx->period_id, period_length ) );
}

static void CallPeriodTimeout( void *ctx_in )
{
  RtemsRatemonReqPeriod_Context *ctx = ctx_in;
  rtems_status_code status;
  status = CallPeriodFunction( ctx, ctx->period_id, period_length );
  T_rsc( status, RTEMS_TIMEOUT );
}

static void DoAction( void *ctx_in )
{
  RtemsRatemonReqPeriod_Context *ctx = ctx_in;
  ctx->status = CallPeriodFunction( ctx, ctx->id_param, ctx->length_param );
}

static void WorkerTask( rtems_task_argument argument )
{
  RtemsRatemonReqPeriod_Context *ctx =
    (RtemsRatemonReqPeriod_Context *) argument;
  if ( ctx != NULL ) {
    ctx->worker_todo( ctx );
    T_rsc_success( rtems_event_send( ctx->task_id, wake_main_task_event ) );
  }
  T_rsc_success( rtems_task_suspend( RTEMS_SELF ) );
}

static uint32_t OwnerDoWork( void *ctx_in, void (*todo)( void *ctx_arg ) )
{
  RtemsRatemonReqPeriod_Context *ctx = ctx_in;
  uint32_t ticks_to_wait = period_length + 1;
  rtems_status_code status;
  rtems_event_set event_set;

  ctx->worker_todo = todo;
  status = rtems_task_restart( ctx->worker_id, (rtems_task_argument) ctx );
  T_rsc_success( status );

  for ( ; ticks_to_wait > 0; --ticks_to_wait ) {
    /* Check whether the worker finished executing the action */
    status = rtems_event_receive(
      RTEMS_PENDING_EVENTS,
      RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
      RTEMS_NO_TIMEOUT,
      &event_set
    );
    T_rsc_success( status );

    if ( ( event_set & wake_main_task_event ) == wake_main_task_event ) {
      break;
    }
    TickTheClock( ctx, 1 );
  }

  /* Wait till the worker task finishes */
  status = rtems_event_receive(
    wake_main_task_event,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &event_set
  );
  T_rsc_success( status );

  return period_length + 1 - ticks_to_wait;
}

static uint32_t OtherDoWork( void *ctx_in, void (*todo)( void *ctx_arg ) )
{
  RtemsRatemonReqPeriod_Context *ctx = ctx_in;
  todo( ctx );
  /* Duration = 0 ticks as DoAction() does not call TickTheClock() */
  return 0;
}

static void CreatePostponedJobs(
  RtemsRatemonReqPeriod_Context *ctx,
  uint32_t jobs_count
)
{
  ctx->postponed_jobs_count = jobs_count;
  if ( ctx->previous_state == RATE_MONOTONIC_ACTIVE ) {
    TickTheClock( ctx, ( jobs_count + 1 ) * period_length );
    OwnerDoWork( ctx, CallPeriodTimeout );
  } else {
    /* ctx->previous_state == RATE_MONOTONIC_INACTIVE || _EXPIRED */
    TickTheClock( ctx, jobs_count * period_length );
  }
}

static void RtemsRatemonReqPeriod_Pre_Id_Prepare(
  RtemsRatemonReqPeriod_Context *ctx,
  RtemsRatemonReqPeriod_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->period_id;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_Id_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Pre_Caller_Prepare(
  RtemsRatemonReqPeriod_Context   *ctx,
  RtemsRatemonReqPeriod_Pre_Caller state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Pre_Caller_OwnerTask: {
      /*
       * While the task invoking rtems_rate_monotonic_period() is the task
       * which created the period - the owner task.
       */
      ctx->do_action = OwnerDoWork;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_Caller_OtherTask: {
      /*
       * While the task invoking rtems_rate_monotonic_period() is not the owner
       * task.
       */
      ctx->do_action = OtherDoWork;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_Caller_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Pre_Length_Prepare(
  RtemsRatemonReqPeriod_Context   *ctx,
  RtemsRatemonReqPeriod_Pre_Length state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Pre_Length_Ticks: {
      /*
       * While the ``length`` parameter is a number larger than 0.
       *
       * Note:
       *
       * * RTEMS_PERIOD_STATUS == 0
       *
       * * The ``length`` parameter of all calls to
       *   rtems_rate_monotonic_period() must have the same value (see
       *   interval).
       */
      ctx->length_param = period_length;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_Length_Status: {
      /*
       * While the ``length`` parameter is RTEMS_PERIOD_STATUS.
       */
      ctx->length_param = RTEMS_PERIOD_STATUS;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_Length_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Pre_State_Prepare(
  RtemsRatemonReqPeriod_Context  *ctx,
  RtemsRatemonReqPeriod_Pre_State state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Pre_State_Inactive: {
      /*
       * While the ``id`` parameter references an period object in inactive
       * state.
       */
      /* Nothing to do here as the period is newly created. */
      ctx->previous_state = RATE_MONOTONIC_INACTIVE;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_State_Active: {
      /*
       * While the ``id`` parameter references an period object in active
       * state.
       */
      OwnerDoWork( ctx, CallPeriod );
      ctx->previous_state = RATE_MONOTONIC_ACTIVE;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_State_Expired: {
      /*
       * While the ``id`` parameter references an period object in expired
       * state.
       */
      OwnerDoWork( ctx, CallPeriod );
      ctx->previous_state = RATE_MONOTONIC_EXPIRED;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_State_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Pre_Postponed_Prepare(
  RtemsRatemonReqPeriod_Context      *ctx,
  RtemsRatemonReqPeriod_Pre_Postponed state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Pre_Postponed_Zero: {
      /*
       * While there is no postponed job.
       */
      ctx->postponed_jobs_count = 0;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_Postponed_One: {
      /*
       * While there is one postponed job.
       */
      CreatePostponedJobs( ctx, 1 );
      break;
    }

    case RtemsRatemonReqPeriod_Pre_Postponed_Several: {
      /*
       * While there are two or more postponed jobs.
       */
      CreatePostponedJobs( ctx, 5 );
      break;
    }

    case RtemsRatemonReqPeriod_Pre_Postponed_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Pre_InactiveCause_Prepare(
  RtemsRatemonReqPeriod_Context          *ctx,
  RtemsRatemonReqPeriod_Pre_InactiveCause state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Pre_InactiveCause_New: {
      /*
       * While rtems_rate_monotonic_period() has never been invoked with result
       * RTEMS_SUCCESSFUL on the period object referenced by the ``id``
       * parameter since that period object has been created.
       */
      /* Nothing to do here as the period is newly created. */
      ctx->postponed_jobs_count = 0;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_InactiveCause_Canceled: {
      /*
       * While rtems_rate_monotonic_period() has never been invoked with result
       * RTEMS_SUCCESSFUL on the period object referenced by the ``id``
       * parameter since that period object has been canceled using
       * rtems_rate_monotonic_cancel().
       */
      if ( ctx->period_calls == 0 ) {
        OwnerDoWork( ctx, CallPeriod );
        TickTheClock( ctx, ctx->postponed_jobs_count * period_length );
      }
      OwnerDoWork( ctx, CancelPeriod );
      ctx->postponed_jobs_count = 0;
      break;
    }

    case RtemsRatemonReqPeriod_Pre_InactiveCause_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Post_Status_Check(
  RtemsRatemonReqPeriod_Context    *ctx,
  RtemsRatemonReqPeriod_Post_Status state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Post_Status_Ok: {
      /*
       * The return status of rtems_rate_monotonic_period() shall be
       * RTEMS_SUCCESSFUL
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Status_InvId: {
      /*
       * The return status of rtems_rate_monotonic_period() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Status_NotOwn: {
      /*
       * The return status of rtems_rate_monotonic_period() shall be
       * RTEMS_NOT_OWNER_OF_RESOURCE.
       */
      T_rsc( ctx->status, RTEMS_NOT_OWNER_OF_RESOURCE );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Status_NotDef: {
      /*
       * The return status of rtems_rate_monotonic_period() shall be
       * RTEMS_NOT_DEFINED.
       */
      T_rsc( ctx->status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Status_TimeOut: {
      /*
       * The return status of rtems_rate_monotonic_period() shall be
       * RTEMS_TIMEOUT.
       */
      T_rsc( ctx->status, RTEMS_TIMEOUT );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Status_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Post_State_Check(
  RtemsRatemonReqPeriod_Context   *ctx,
  RtemsRatemonReqPeriod_Post_State state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Post_State_Inactive: {
      /*
       * The state of the period shall be inactive after the return of the
       * rtems_rate_monotonic_period() call.
       */
      T_eq_int( ctx->period_status.state, RATE_MONOTONIC_INACTIVE );
      break;
    }

    case RtemsRatemonReqPeriod_Post_State_Active: {
      /*
       * The state of the period shall be RATE_MONOTONIC_ACTIVE after the
       * return of the rtems_rate_monotonic_period() call.
       */
      T_eq_int( ctx->period_status.state, RATE_MONOTONIC_ACTIVE );
      break;
    }

    case RtemsRatemonReqPeriod_Post_State_Expired: {
      /*
       * The state of the period shall be RATE_MONOTONIC_EXPIRED after the
       * return of the rtems_rate_monotonic_period() call.
       */
      T_eq_int( ctx->period_status.state, RATE_MONOTONIC_EXPIRED );
      break;
    }

    case RtemsRatemonReqPeriod_Post_State_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_rate_monotonic_period() shall not be accessed by the
       * rtems_rate_monotonic_period() call (see also Nop).
       */
      T_eq_u32( ctx->period_status.state, ctx->previous_state );
      break;
    }

    case RtemsRatemonReqPeriod_Post_State_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Post_Postponed_Check(
  RtemsRatemonReqPeriod_Context       *ctx,
  RtemsRatemonReqPeriod_Post_Postponed state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Post_Postponed_Zero: {
      /*
       * There shall be no postponed jobs after the return of the
       * rtems_rate_monotonic_period() call.
       */
      T_eq_u32( ctx->period_status.postponed_jobs_count, 0 );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Postponed_OneOrMore: {
      /*
       * The number of postponed jobs shall be the number of deadlines passed
       * minus the number of returned calls to rtems_rate_monotonic_period().
       *
       * The last call to rtems_rate_monotonic_period() where the state changes
       * from inactive to RATE_MONOTONIC_ACTIVE is counted as the first
       * returned call. The first deadline occurred at a point in time during
       * that call to rtems_rate_monotonic_period().
       */
      T_eq_u32(
        ctx->period_status.postponed_jobs_count,
        ( ctx->test_duration / period_length + 1 ) - ctx->period_calls
      );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Postponed_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_rate_monotonic_period() shall not be accessed by the
       * rtems_rate_monotonic_period() call (see also Nop).
       */
      T_eq_u32(
        ctx->period_status.postponed_jobs_count,
        ctx->postponed_jobs_count
      );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Postponed_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Post_Delay_Check(
  RtemsRatemonReqPeriod_Context   *ctx,
  RtemsRatemonReqPeriod_Post_Delay state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Post_Delay_None: {
      /*
       * The last call to rtems_rate_monotonic_period() shall return without
       * delay.
       */
      T_eq_u32( ctx->action_duration, 0 );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Delay_TillDeadline: {
      /*
       * The last call to rtems_rate_monotonic_period() shall block the owner
       * task till the next deadline and return afterwards.
       */
      T_eq_u32(
        ctx->action_duration,
        ( ctx->test_duration_till_action % period_length + 1 ) * period_length -
        ctx->test_duration_till_action
      );
      break;
    }

    case RtemsRatemonReqPeriod_Post_Delay_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Post_Scheduler_Check(
  RtemsRatemonReqPeriod_Context       *ctx,
  RtemsRatemonReqPeriod_Post_Scheduler state
)
{
  switch ( state ) {
    case RtemsRatemonReqPeriod_Post_Scheduler_Called: {
      /*
       * The last call of the rtems_rate_monotonic_period() function shall
       * execute the ``release_job`` scheduler operation of the home scheduler.
       */
      /* Cannot be tested as the effect is unknown. */
      break;
    }

    case RtemsRatemonReqPeriod_Post_Scheduler_Nop: {
      /*
       * The last call of the rtems_rate_monotonic_period() function shall not
       * execute any scheduler operation.
       */
      /* Cannot be tested as the effect is unknown. */
      break;
    }

    case RtemsRatemonReqPeriod_Post_Scheduler_NA:
      break;
  }
}

static void RtemsRatemonReqPeriod_Setup( RtemsRatemonReqPeriod_Context *ctx )
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

static void RtemsRatemonReqPeriod_Setup_Wrap( void *arg )
{
  RtemsRatemonReqPeriod_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRatemonReqPeriod_Setup( ctx );
}

static void RtemsRatemonReqPeriod_Teardown(
  RtemsRatemonReqPeriod_Context *ctx
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

static void RtemsRatemonReqPeriod_Teardown_Wrap( void *arg )
{
  RtemsRatemonReqPeriod_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRatemonReqPeriod_Teardown( ctx );
}

static void RtemsRatemonReqPeriod_Prepare( RtemsRatemonReqPeriod_Context *ctx )
{
  rtems_status_code status;
  rtems_rate_monotonic_period_status period_status;
  ctx->test_duration = 0;
  ctx->period_calls = 0;
  OwnerDoWork( ctx, CreatePeriod );

  /*
   * In case of a new period the postponed jobs count is arbitrary
   * (what ever value happens to be stored in that field of the internal data
   * structure) until period() is called.
   */
  status = rtems_rate_monotonic_get_status(
    ctx->period_id,
    &period_status
  );
  T_rsc_success( status );
}

static void RtemsRatemonReqPeriod_Action( RtemsRatemonReqPeriod_Context *ctx )
{
  rtems_status_code status;

  ctx->test_duration_till_action = ctx->test_duration;
  ctx->action_duration = ctx->do_action( ctx, DoAction );

  status = rtems_rate_monotonic_get_status(
    ctx->period_id,
    &ctx->period_status
  );
  T_rsc_success( status );
}

static void RtemsRatemonReqPeriod_Cleanup( RtemsRatemonReqPeriod_Context *ctx )
{
  OwnerDoWork( ctx, DeletePeriod );
}

static const RtemsRatemonReqPeriod_Entry
RtemsRatemonReqPeriod_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 1, RtemsRatemonReqPeriod_Post_Status_InvId,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 1, RtemsRatemonReqPeriod_Post_Status_NotOwn,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 1, 0, 0, 0, 0, 0, 1, RtemsRatemonReqPeriod_Post_Status_NA,
    RtemsRatemonReqPeriod_Post_State_NA,
    RtemsRatemonReqPeriod_Post_Postponed_NA,
    RtemsRatemonReqPeriod_Post_Delay_NA,
    RtemsRatemonReqPeriod_Post_Scheduler_NA },
  { 0, 0, 0, 0, 0, 1, 0, RtemsRatemonReqPeriod_Post_Status_InvId,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqPeriod_Post_Status_InvId,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 1, RtemsRatemonReqPeriod_Post_Status_Ok,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 0, 1, 0, RtemsRatemonReqPeriod_Post_Status_NotOwn,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqPeriod_Post_Status_NotOwn,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 1, RtemsRatemonReqPeriod_Post_Status_TimeOut,
    RtemsRatemonReqPeriod_Post_State_Active,
    RtemsRatemonReqPeriod_Post_Postponed_Zero,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Called },
  { 0, 0, 0, 0, 0, 0, 1, RtemsRatemonReqPeriod_Post_Status_TimeOut,
    RtemsRatemonReqPeriod_Post_State_Active,
    RtemsRatemonReqPeriod_Post_Postponed_OneOrMore,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Called },
  { 0, 0, 0, 0, 0, 0, 1, RtemsRatemonReqPeriod_Post_Status_TimeOut,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 0, 1, 0, RtemsRatemonReqPeriod_Post_Status_Ok,
    RtemsRatemonReqPeriod_Post_State_Active,
    RtemsRatemonReqPeriod_Post_Postponed_Zero,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Called },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqPeriod_Post_Status_Ok,
    RtemsRatemonReqPeriod_Post_State_Active,
    RtemsRatemonReqPeriod_Post_Postponed_Zero,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Called },
  { 0, 0, 0, 0, 0, 1, 0, RtemsRatemonReqPeriod_Post_Status_NotDef,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqPeriod_Post_Status_NotDef,
    RtemsRatemonReqPeriod_Post_State_Nop,
    RtemsRatemonReqPeriod_Post_Postponed_Nop,
    RtemsRatemonReqPeriod_Post_Delay_None,
    RtemsRatemonReqPeriod_Post_Scheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 1, RtemsRatemonReqPeriod_Post_Status_Ok,
    RtemsRatemonReqPeriod_Post_State_Active,
    RtemsRatemonReqPeriod_Post_Postponed_Zero,
    RtemsRatemonReqPeriod_Post_Delay_TillDeadline,
    RtemsRatemonReqPeriod_Post_Scheduler_Called }
};

static const uint8_t
RtemsRatemonReqPeriod_Map[] = {
  11, 12, 11, 12, 11, 12, 15, 15, 8, 8, 9, 9, 2, 2, 8, 8, 9, 9, 13, 14, 13, 14,
  13, 14, 5, 5, 5, 5, 5, 5, 2, 2, 10, 10, 10, 10, 6, 7, 6, 7, 6, 7, 1, 1, 1, 1,
  1, 1, 2, 2, 1, 1, 1, 1, 6, 7, 6, 7, 6, 7, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1,
  3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 3, 4, 3, 4, 3, 4, 0, 0,
  0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0,
  0, 0, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0
};

static size_t RtemsRatemonReqPeriod_Scope( void *arg, char *buf, size_t n )
{
  RtemsRatemonReqPeriod_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsRatemonReqPeriod_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsRatemonReqPeriod_Fixture = {
  .setup = RtemsRatemonReqPeriod_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsRatemonReqPeriod_Teardown_Wrap,
  .scope = RtemsRatemonReqPeriod_Scope,
  .initial_context = &RtemsRatemonReqPeriod_Instance
};

static inline RtemsRatemonReqPeriod_Entry RtemsRatemonReqPeriod_PopEntry(
  RtemsRatemonReqPeriod_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsRatemonReqPeriod_Entries[
    RtemsRatemonReqPeriod_Map[ index ]
  ];
}

static void RtemsRatemonReqPeriod_SetPreConditionStates(
  RtemsRatemonReqPeriod_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];

  if ( ctx->Map.entry.Pre_Postponed_NA ) {
    ctx->Map.pcs[ 4 ] = RtemsRatemonReqPeriod_Pre_Postponed_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_InactiveCause_NA ) {
    ctx->Map.pcs[ 5 ] = RtemsRatemonReqPeriod_Pre_InactiveCause_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }
}

static void RtemsRatemonReqPeriod_TestVariant(
  RtemsRatemonReqPeriod_Context *ctx
)
{
  RtemsRatemonReqPeriod_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsRatemonReqPeriod_Pre_Caller_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsRatemonReqPeriod_Pre_Length_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsRatemonReqPeriod_Pre_State_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsRatemonReqPeriod_Pre_Postponed_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsRatemonReqPeriod_Pre_InactiveCause_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsRatemonReqPeriod_Action( ctx );
  RtemsRatemonReqPeriod_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsRatemonReqPeriod_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  RtemsRatemonReqPeriod_Post_Postponed_Check(
    ctx,
    ctx->Map.entry.Post_Postponed
  );
  RtemsRatemonReqPeriod_Post_Delay_Check( ctx, ctx->Map.entry.Post_Delay );
  RtemsRatemonReqPeriod_Post_Scheduler_Check(
    ctx,
    ctx->Map.entry.Post_Scheduler
  );
}

/**
 * @fn void T_case_body_RtemsRatemonReqPeriod( void )
 */
T_TEST_CASE_FIXTURE( RtemsRatemonReqPeriod, &RtemsRatemonReqPeriod_Fixture )
{
  RtemsRatemonReqPeriod_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsRatemonReqPeriod_Pre_Id_Valid;
    ctx->Map.pci[ 0 ] < RtemsRatemonReqPeriod_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsRatemonReqPeriod_Pre_Caller_OwnerTask;
      ctx->Map.pci[ 1 ] < RtemsRatemonReqPeriod_Pre_Caller_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsRatemonReqPeriod_Pre_Length_Ticks;
        ctx->Map.pci[ 2 ] < RtemsRatemonReqPeriod_Pre_Length_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsRatemonReqPeriod_Pre_State_Inactive;
          ctx->Map.pci[ 3 ] < RtemsRatemonReqPeriod_Pre_State_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsRatemonReqPeriod_Pre_Postponed_Zero;
            ctx->Map.pci[ 4 ] < RtemsRatemonReqPeriod_Pre_Postponed_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsRatemonReqPeriod_Pre_InactiveCause_New;
              ctx->Map.pci[ 5 ] < RtemsRatemonReqPeriod_Pre_InactiveCause_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = RtemsRatemonReqPeriod_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              RtemsRatemonReqPeriod_SetPreConditionStates( ctx );
              RtemsRatemonReqPeriod_Prepare( ctx );
              RtemsRatemonReqPeriod_TestVariant( ctx );
              RtemsRatemonReqPeriod_Cleanup( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
