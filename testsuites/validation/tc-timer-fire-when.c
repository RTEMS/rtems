/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerReqFireWhen
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
 * @defgroup RtemsTimerReqFireWhen spec:/rtems/timer/req/fire-when
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTimerReqFireWhen_Pre_RtClock_Set,
  RtemsTimerReqFireWhen_Pre_RtClock_Unset,
  RtemsTimerReqFireWhen_Pre_RtClock_NA
} RtemsTimerReqFireWhen_Pre_RtClock;

typedef enum {
  RtemsTimerReqFireWhen_Pre_Routine_Valid,
  RtemsTimerReqFireWhen_Pre_Routine_Null,
  RtemsTimerReqFireWhen_Pre_Routine_NA
} RtemsTimerReqFireWhen_Pre_Routine;

typedef enum {
  RtemsTimerReqFireWhen_Pre_WallTime_Valid,
  RtemsTimerReqFireWhen_Pre_WallTime_Invalid,
  RtemsTimerReqFireWhen_Pre_WallTime_Past,
  RtemsTimerReqFireWhen_Pre_WallTime_Null,
  RtemsTimerReqFireWhen_Pre_WallTime_NA
} RtemsTimerReqFireWhen_Pre_WallTime;

typedef enum {
  RtemsTimerReqFireWhen_Pre_Id_Valid,
  RtemsTimerReqFireWhen_Pre_Id_Invalid,
  RtemsTimerReqFireWhen_Pre_Id_NA
} RtemsTimerReqFireWhen_Pre_Id;

typedef enum {
  RtemsTimerReqFireWhen_Pre_Context_None,
  RtemsTimerReqFireWhen_Pre_Context_Interrupt,
  RtemsTimerReqFireWhen_Pre_Context_Server,
  RtemsTimerReqFireWhen_Pre_Context_NA
} RtemsTimerReqFireWhen_Pre_Context;

typedef enum {
  RtemsTimerReqFireWhen_Pre_Clock_None,
  RtemsTimerReqFireWhen_Pre_Clock_Ticks,
  RtemsTimerReqFireWhen_Pre_Clock_Realtime,
  RtemsTimerReqFireWhen_Pre_Clock_NA
} RtemsTimerReqFireWhen_Pre_Clock;

typedef enum {
  RtemsTimerReqFireWhen_Pre_State_Inactive,
  RtemsTimerReqFireWhen_Pre_State_Scheduled,
  RtemsTimerReqFireWhen_Pre_State_Pending,
  RtemsTimerReqFireWhen_Pre_State_NA
} RtemsTimerReqFireWhen_Pre_State;

typedef enum {
  RtemsTimerReqFireWhen_Post_Status_Ok,
  RtemsTimerReqFireWhen_Post_Status_NotDef,
  RtemsTimerReqFireWhen_Post_Status_InvId,
  RtemsTimerReqFireWhen_Post_Status_InvAddr,
  RtemsTimerReqFireWhen_Post_Status_InvClock,
  RtemsTimerReqFireWhen_Post_Status_NA
} RtemsTimerReqFireWhen_Post_Status;

typedef enum {
  RtemsTimerReqFireWhen_Post_Context_None,
  RtemsTimerReqFireWhen_Post_Context_Interrupt,
  RtemsTimerReqFireWhen_Post_Context_Server,
  RtemsTimerReqFireWhen_Post_Context_Nop,
  RtemsTimerReqFireWhen_Post_Context_NA
} RtemsTimerReqFireWhen_Post_Context;

typedef enum {
  RtemsTimerReqFireWhen_Post_Clock_None,
  RtemsTimerReqFireWhen_Post_Clock_Ticks,
  RtemsTimerReqFireWhen_Post_Clock_Realtime,
  RtemsTimerReqFireWhen_Post_Clock_Nop,
  RtemsTimerReqFireWhen_Post_Clock_NA
} RtemsTimerReqFireWhen_Post_Clock;

typedef enum {
  RtemsTimerReqFireWhen_Post_State_Scheduled,
  RtemsTimerReqFireWhen_Post_State_Nop,
  RtemsTimerReqFireWhen_Post_State_NA
} RtemsTimerReqFireWhen_Post_State;

typedef enum {
  RtemsTimerReqFireWhen_Post_WallTime_Param,
  RtemsTimerReqFireWhen_Post_WallTime_Nop,
  RtemsTimerReqFireWhen_Post_WallTime_NA
} RtemsTimerReqFireWhen_Post_WallTime;

typedef enum {
  RtemsTimerReqFireWhen_Post_Routine_Param,
  RtemsTimerReqFireWhen_Post_Routine_Nop,
  RtemsTimerReqFireWhen_Post_Routine_NA
} RtemsTimerReqFireWhen_Post_Routine;

typedef enum {
  RtemsTimerReqFireWhen_Post_UserData_Param,
  RtemsTimerReqFireWhen_Post_UserData_Nop,
  RtemsTimerReqFireWhen_Post_UserData_NA
} RtemsTimerReqFireWhen_Post_UserData;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_RtClock_NA : 1;
  uint32_t Pre_Routine_NA : 1;
  uint32_t Pre_WallTime_NA : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_Context_NA : 1;
  uint32_t Pre_Clock_NA : 1;
  uint32_t Pre_State_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Context : 3;
  uint32_t Post_Clock : 3;
  uint32_t Post_State : 2;
  uint32_t Post_WallTime : 2;
  uint32_t Post_Routine : 2;
  uint32_t Post_UserData : 2;
} RtemsTimerReqFireWhen_Entry;

typedef enum {
  PRE_NONE      = 0,
  PRE_INTERRUPT = 1,
  PRE_SERVER    = 2
} PreConditionContext;

typedef enum {
  SCHEDULE_NONE  = 0,
  SCHEDULE_SOON  = 1,
  SCHEDULE_MAX   = 5
} Scheduling_Ticks;

/**
 * @brief Test context for spec:/rtems/timer/req/fire-when test case.
 */
typedef struct {
  /**
   * @brief This member contains a valid id of a timer.
   */
  rtems_id timer_id;

  /**
   * @brief This member specifies the ``id`` parameter for the action.
   */
  rtems_id id_param;

  /**
   * @brief This member specifies the ``wall_time`` parameter for the action.
   */
  const rtems_time_of_day *wall_time_param;

  /**
   * @brief This member specifies the ``routine`` parameter for the action.
   */
  rtems_timer_service_routine_entry routine_param;

  /**
   * @brief This member contains the returned status code of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains a counter of invocations of the Timer Service
   *   Routine.
   */
  int invocations;

  /**
   * @brief Function TriggerTimer() is used to figure out when the Timer
   *   Service Routine gets executed. This member contains the time-of-day when
   *   the Timer Service Routine fires (see fire).
   */
  rtems_time_of_day tod_till_fire;

  /**
   * @brief This member contains the user data given to the Timer Service
   *   Routine when called.
   */
  void *routine_user_data;

  /**
   * @brief This member specifies which pre-condition context (none, interrupt
   *   context, server context) must be created before the
   *   rtems_timer_fire_when() action gets executed.
   */
  PreConditionContext pre_cond_contex;

  /**
   * @brief This member specifies the pre-condition state of the realtime
   *   clock. It should either be set to the value referenced by pre_cond_tod
   *   or if NULL, then the realtime clock should be not set.
   */
  const rtems_time_of_day *pre_cond_tod;

  /**
   * @brief This member stores internal clock and context settings of the timer
   *   before the execution of the test action.
   */
  Timer_Classes pre_class;

  /**
   * @brief This member stores the state of the timer before the execution of
   *   the test action.
   */
  Timer_States pre_state;

  /**
   * @brief This member stores the state of the timer after the execution of
   *   the test action.
   */
  Timer_States post_state;

  /**
   * @brief This member stores the scheduling data of the timer before the
   *   execution of the test action.
   */
  Timer_Scheduling_Data pre_scheduling_data;

  /**
   * @brief This member stores the scheduling data of the timer after the
   *   execution of the test action.
   */
  Timer_Scheduling_Data post_scheduling_data;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 7 ];

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
    RtemsTimerReqFireWhen_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTimerReqFireWhen_Context;

static RtemsTimerReqFireWhen_Context
  RtemsTimerReqFireWhen_Instance;

static const char * const RtemsTimerReqFireWhen_PreDesc_RtClock[] = {
  "Set",
  "Unset",
  "NA"
};

static const char * const RtemsTimerReqFireWhen_PreDesc_Routine[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsTimerReqFireWhen_PreDesc_WallTime[] = {
  "Valid",
  "Invalid",
  "Past",
  "Null",
  "NA"
};

static const char * const RtemsTimerReqFireWhen_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsTimerReqFireWhen_PreDesc_Context[] = {
  "None",
  "Interrupt",
  "Server",
  "NA"
};

static const char * const RtemsTimerReqFireWhen_PreDesc_Clock[] = {
  "None",
  "Ticks",
  "Realtime",
  "NA"
};

static const char * const RtemsTimerReqFireWhen_PreDesc_State[] = {
  "Inactive",
  "Scheduled",
  "Pending",
  "NA"
};

static const char * const * const RtemsTimerReqFireWhen_PreDesc[] = {
  RtemsTimerReqFireWhen_PreDesc_RtClock,
  RtemsTimerReqFireWhen_PreDesc_Routine,
  RtemsTimerReqFireWhen_PreDesc_WallTime,
  RtemsTimerReqFireWhen_PreDesc_Id,
  RtemsTimerReqFireWhen_PreDesc_Context,
  RtemsTimerReqFireWhen_PreDesc_Clock,
  RtemsTimerReqFireWhen_PreDesc_State,
  NULL
};

static const rtems_time_of_day tod_now      = { 2000, 1, 1, 0, 0, 0, 0 };
static const rtems_time_of_day tod_schedule = { 2000, 1, 1, 5, 0, 0, 0 };
static const rtems_time_of_day tod_invalid  = { 1985, 1, 1, 0, 0, 0, 0 };
/*
 * rtems_fire_when() ignores ticks and treads all wall times in the
 * current second like being in the "past". This border case is tested.
 */
static const rtems_time_of_day tod_past     = { 2000, 1, 1, 0, 0, 0, 50 };

static void TriggerTimer(
  const RtemsTimerReqFireWhen_Context *ctx,
  rtems_time_of_day *tod_fire
)
{
  rtems_time_of_day tod = tod_now;
  int invocations_old = ctx->invocations;
  int i;

  /* Fire the timer service routine for ticks and realtime clock */
  for ( i = 1; i <= SCHEDULE_MAX; ++i ) {
    ClockTick();
  }

  for ( i = 1; i < 24; ++i ) {
    tod.hour = i;
    T_rsc_success( rtems_clock_set( &tod ) );
    if ( tod_fire != NULL && ctx->invocations > invocations_old ) {
      *tod_fire = tod;
      break;
    }
  }
}

static void TimerServiceRoutine(
  rtems_id timer_id,
  void *user_data
)
{
  RtemsTimerReqFireWhen_Context *ctx = user_data;
  ++( ctx->invocations );
  ctx->routine_user_data = user_data;
}

static void RtemsTimerReqFireWhen_Pre_RtClock_Prepare(
  RtemsTimerReqFireWhen_Context    *ctx,
  RtemsTimerReqFireWhen_Pre_RtClock state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Pre_RtClock_Set: {
      /*
       * While the realtime clock is set to a valid time-of-day.
       */
      ctx->pre_cond_tod = &tod_now;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_RtClock_Unset: {
      /*
       * While the realtime clock has never been set.
       */
      ctx->pre_cond_tod = NULL;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_RtClock_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Pre_Routine_Prepare(
  RtemsTimerReqFireWhen_Context    *ctx,
  RtemsTimerReqFireWhen_Pre_Routine state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Pre_Routine_Valid: {
      /*
       * While the ``routine`` parameter references an object of type
       * rtems_timer_service_routine_entry.
       */
      ctx->routine_param = TimerServiceRoutine;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Routine_Null: {
      /*
       * While the ``routine`` parameter is NULL..
       */
      ctx->routine_param = NULL;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Routine_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Pre_WallTime_Prepare(
  RtemsTimerReqFireWhen_Context     *ctx,
  RtemsTimerReqFireWhen_Pre_WallTime state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Pre_WallTime_Valid: {
      /*
       * While the ``wall_time`` parameter references a time at least one
       * second in the future but not later than the last second of the year
       * 2105. (Times after 2105 are invalid.)
       */
      ctx->wall_time_param = &tod_schedule;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_WallTime_Invalid: {
      /*
       * While the ``wall_time`` parameter is invalid.
       */
      ctx->wall_time_param = &tod_invalid;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_WallTime_Past: {
      /*
       * While the ``wall_time`` parameter references a time in the current
       * second or in the past but not earlier than 1988. (Times before 1988
       * are invalid.)
       */
      ctx->wall_time_param = &tod_past;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_WallTime_Null: {
      /*
       * While the ``wall_time`` parameter is 0.
       */
      ctx->wall_time_param = NULL;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_WallTime_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Pre_Id_Prepare(
  RtemsTimerReqFireWhen_Context *ctx,
  RtemsTimerReqFireWhen_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->timer_id;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Id_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Pre_Context_Prepare(
  RtemsTimerReqFireWhen_Context    *ctx,
  RtemsTimerReqFireWhen_Pre_Context state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Pre_Context_None: {
      /*
       * While the Timer Service Routine has never been scheduled since
       * creation of the timer. See also none.
       */
      ctx->pre_cond_contex = PRE_NONE;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Context_Interrupt: {
      /*
       * While the timer is in interrupt context.
       */
      ctx->pre_cond_contex = PRE_INTERRUPT;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Context_Server: {
      /*
       * While the timer is in server context.
       */
      ctx->pre_cond_contex = PRE_SERVER;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Context_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Pre_Clock_Prepare(
  RtemsTimerReqFireWhen_Context  *ctx,
  RtemsTimerReqFireWhen_Pre_Clock state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Pre_Clock_None: {
      /*
       * While the timer has never been scheduled since creation of the timer.
       */
      T_eq_int( ctx->pre_cond_contex, PRE_NONE );
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Clock_Ticks: {
      /*
       * While the clock used to determine when the timer will fire is the
       * ticks based clock.
       */
      rtems_status_code status;

      if ( ctx->pre_cond_contex == PRE_INTERRUPT ) {
        status = rtems_timer_fire_after(
          ctx->timer_id,
          SCHEDULE_SOON,
          TimerServiceRoutine,
          ctx
        );
      } else {
        status = rtems_timer_server_fire_after(
          ctx->timer_id,
          SCHEDULE_SOON,
          TimerServiceRoutine,
          ctx
        );
      }
      T_rsc_success( status );
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Clock_Realtime: {
      /*
       * While the clock used to determine when the timer will fire is the
       * realtime clock.
       */
      rtems_status_code status;
      T_rsc_success( rtems_clock_set( &tod_now ) );

      if ( ctx->pre_cond_contex == PRE_INTERRUPT ) {
        status = rtems_timer_fire_when(
          ctx->timer_id,
          &tod_schedule,
          TimerServiceRoutine,
          ctx
        );
      } else {
        status = rtems_timer_server_fire_when(
          ctx->timer_id,
          &tod_schedule,
          TimerServiceRoutine,
          ctx
        );
      }
      T_rsc_success( status );
      break;
    }

    case RtemsTimerReqFireWhen_Pre_Clock_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Pre_State_Prepare(
  RtemsTimerReqFireWhen_Context  *ctx,
  RtemsTimerReqFireWhen_Pre_State state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Pre_State_Inactive: {
      /*
       * While the timer is in inactive state.
       */
      TriggerTimer( ctx, NULL );
      T_eq_int(
        ctx->invocations,
        ( ctx->pre_cond_contex == PRE_NONE ) ? 0 : 1
      );
      ctx->invocations = 0;
      ctx->pre_state = TIMER_INACTIVE;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_State_Scheduled: {
      /*
       * While the timer is in scheduled state.
       */
      /* The timer was already scheduled in the "Clock" pre-conditions. */
      ctx->pre_state = TIMER_SCHEDULED;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_State_Pending: {
      /*
       * While the timer is in pending state.
       */
      T_rsc_success( rtems_task_suspend( GetTimerServerTaskId() ) );
      TriggerTimer( ctx, NULL );
      T_eq_int( ctx->invocations, 0 );
      ctx->pre_state = TIMER_PENDING;
      break;
    }

    case RtemsTimerReqFireWhen_Pre_State_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Post_Status_Check(
  RtemsTimerReqFireWhen_Context    *ctx,
  RtemsTimerReqFireWhen_Post_Status state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Post_Status_Ok: {
      /*
       * The return status of rtems_timer_fire_when() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Status_NotDef: {
      /*
       * The return status of rtems_timer_fire_when() shall be
       * RTEMS_NOT_DEFINED
       */
      T_rsc( ctx->status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Status_InvId: {
      /*
       * The return status of rtems_timer_fire_when() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Status_InvAddr: {
      /*
       * The return status of rtems_timer_fire_when() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Status_InvClock: {
      /*
       * The return status of rtems_timer_fire_when() shall be
       * RTEMS_INVALID_CLOCK.
       */
      T_rsc( ctx->status, RTEMS_INVALID_CLOCK );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Status_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Post_Context_Check(
  RtemsTimerReqFireWhen_Context     *ctx,
  RtemsTimerReqFireWhen_Post_Context state
)
{
  Timer_Classes class;
  class = GetTimerClass( ctx->timer_id );

  switch ( state ) {
    case RtemsTimerReqFireWhen_Post_Context_None: {
      /*
       * The timer shall have never been scheduled. See also none.
       */
      T_eq_int( class, TIMER_DORMANT );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Context_Interrupt: {
      /*
       * The timer shall be in interrupt context.
       */
      T_eq_int( class & TIMER_CLASS_BIT_ON_TASK, 0 );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Context_Server: {
      /*
       * The timer shall be in server context.
       */
      T_eq_int( class & TIMER_CLASS_BIT_ON_TASK, TIMER_CLASS_BIT_ON_TASK );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Context_Nop: {
      /*
       * Objects referenced by parameters in the past call to
       * rtems_timer_fire_when() shall not be accessed by the
       * rtems_timer_fire_when() call. See also Nop.
       */
      T_eq_int( class, ctx->pre_class );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Context_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Post_Clock_Check(
  RtemsTimerReqFireWhen_Context   *ctx,
  RtemsTimerReqFireWhen_Post_Clock state
)
{
  Timer_Classes class;
  class = GetTimerClass( ctx->timer_id );

  switch ( state ) {
    case RtemsTimerReqFireWhen_Post_Clock_None: {
      /*
       * The timer shall have never been scheduled.
       */
      T_eq_int( class, TIMER_DORMANT );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Clock_Ticks: {
      /*
       * The timer shall use the ticks based clock.
       */
      T_eq_int( class & TIMER_CLASS_BIT_TIME_OF_DAY, 0 );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Clock_Realtime: {
      /*
       * The timer shall use the realtime clock.
       */
      T_eq_int(
        class & TIMER_CLASS_BIT_TIME_OF_DAY,
        TIMER_CLASS_BIT_TIME_OF_DAY
      );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Clock_Nop: {
      /*
       * Objects referenced by parameters in the past call to
       * rtems_timer_fire_when() shall not be accessed by the
       * rtems_timer_fire_when() call.
       */
      T_eq_int( class, ctx->pre_class );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Clock_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Post_State_Check(
  RtemsTimerReqFireWhen_Context   *ctx,
  RtemsTimerReqFireWhen_Post_State state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Post_State_Scheduled: {
      /*
       * The timer shall be in scheduled state.
       */
      TriggerTimer( ctx, &ctx->tod_till_fire );
      T_eq_int( ctx->invocations, 1 );
      break;
    }

    case RtemsTimerReqFireWhen_Post_State_Nop: {
      /*
       * Objects referenced by parameters in the past call to
       * rtems_timer_fire_when() shall not be accessed by the
       * rtems_timer_fire_when() call.
       */
      T_eq_int( ctx->post_state, ctx->pre_state );
      break;
    }

    case RtemsTimerReqFireWhen_Post_State_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Post_WallTime_Check(
  RtemsTimerReqFireWhen_Context      *ctx,
  RtemsTimerReqFireWhen_Post_WallTime state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Post_WallTime_Param: {
      /*
       * The Timer Service Routine shall be invoked at the wall time (see
       * realtime clock) (ignoring ticks), which was provided by the
       * ``wall_time`` parameter in the past call to rtems_timer_fire_when().
       */
      T_eq_mem(
        &ctx->tod_till_fire,
        ctx->wall_time_param,
        sizeof( ctx->tod_till_fire )
      );
      break;
    }

    case RtemsTimerReqFireWhen_Post_WallTime_Nop: {
      /*
       * If and when the Timer Service Routine will be invoked shall not be
       * changed by the past call to rtems_timer_fire_when().
       */
      /*
       * Whether the timer is scheduled has already been tested by the
       * "Nop" "State" post-condition above.
       */
      T_eq_u32(
        ctx->post_scheduling_data.interval,
        ctx->pre_scheduling_data.interval
      );
      break;
    }

    case RtemsTimerReqFireWhen_Post_WallTime_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Post_Routine_Check(
  RtemsTimerReqFireWhen_Context     *ctx,
  RtemsTimerReqFireWhen_Post_Routine state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Post_Routine_Param: {
      /*
       * The function reference used to invoke the Timer Service Routine when
       * the timer will fire shall be the one provided by the ``routine``
       * parameter in the past call to rtems_timer_fire_when().
       */
      T_eq_int( ctx->invocations, 1 );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Routine_Nop: {
      /*
       * The function reference used for any invocation of the Timer Service
       * Routine shall not be changed by the past call to
       * rtems_timer_fire_when().
       */
      T_eq_ptr(
        ctx->post_scheduling_data.routine,
        ctx->pre_scheduling_data.routine
      );
      break;
    }

    case RtemsTimerReqFireWhen_Post_Routine_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Post_UserData_Check(
  RtemsTimerReqFireWhen_Context      *ctx,
  RtemsTimerReqFireWhen_Post_UserData state
)
{
  switch ( state ) {
    case RtemsTimerReqFireWhen_Post_UserData_Param: {
      /*
       * The user data argument for invoking the Timer Service Routine when the
       * timer will fire shall be the one provided by the ``user_data``
       * parameter in the past call to rtems_timer_fire_when().
       */
      T_eq_ptr( ctx->routine_user_data, ctx );
      break;
    }

    case RtemsTimerReqFireWhen_Post_UserData_Nop: {
      /*
       * The user data argument used for any invocation of the Timer Service
       * Routine shall not be changed by the past call to
       * rtems_timer_fire_when().
       */
      T_eq_ptr(
        ctx->post_scheduling_data.user_data,
        ctx->pre_scheduling_data.user_data
      );
      break;
    }

    case RtemsTimerReqFireWhen_Post_UserData_NA:
      break;
  }
}

static void RtemsTimerReqFireWhen_Setup( RtemsTimerReqFireWhen_Context *ctx )
{
  rtems_status_code status;
  status = rtems_timer_initiate_server(
    RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  T_rsc_success( status );
}

static void RtemsTimerReqFireWhen_Setup_Wrap( void *arg )
{
  RtemsTimerReqFireWhen_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqFireWhen_Setup( ctx );
}

/**
 * @brief Make sure the timer server is not running and the realtime clock is
 *   not set after this test.
 */
static void RtemsTimerReqFireWhen_Teardown(
  RtemsTimerReqFireWhen_Context *ctx
)
{
  DeleteTimerServer();
  UnsetClock();
}

static void RtemsTimerReqFireWhen_Teardown_Wrap( void *arg )
{
  RtemsTimerReqFireWhen_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqFireWhen_Teardown( ctx );
}

static void RtemsTimerReqFireWhen_Prepare( RtemsTimerReqFireWhen_Context *ctx )
{
  rtems_status_code status;
  status = rtems_timer_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    &ctx->timer_id
  );
  T_rsc_success( status );

  ctx->invocations       = 0;
  ctx->routine_user_data = NULL;
}

static void RtemsTimerReqFireWhen_Action( RtemsTimerReqFireWhen_Context *ctx )
{
  GetTimerSchedulingData( ctx->timer_id, &ctx->pre_scheduling_data );
  ctx->pre_class = GetTimerClass( ctx->timer_id );
  if ( ctx->pre_cond_tod == NULL ) {
    UnsetClock();
  } else {
    T_rsc_success( rtems_clock_set( ctx->pre_cond_tod ) );
  }
  ctx->status = rtems_timer_fire_when(
    ctx->id_param,
    ctx->wall_time_param,
    ctx->routine_param,
    ctx
  );
  ctx->post_state = GetTimerState( ctx->timer_id );
  GetTimerSchedulingData( ctx->timer_id, &ctx->post_scheduling_data );
  /* Ignoring return status: the timer server task may be suspended or not. */
  rtems_task_resume( GetTimerServerTaskId() );
}

static void RtemsTimerReqFireWhen_Cleanup( RtemsTimerReqFireWhen_Context *ctx )
{
  T_rsc_success( rtems_timer_delete( ctx->timer_id ) );
}

static const RtemsTimerReqFireWhen_Entry
RtemsTimerReqFireWhen_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqFireWhen_Post_Status_NA,
    RtemsTimerReqFireWhen_Post_Context_NA, RtemsTimerReqFireWhen_Post_Clock_NA,
    RtemsTimerReqFireWhen_Post_State_NA,
    RtemsTimerReqFireWhen_Post_WallTime_NA,
    RtemsTimerReqFireWhen_Post_Routine_NA,
    RtemsTimerReqFireWhen_Post_UserData_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqFireWhen_Post_Status_InvAddr,
    RtemsTimerReqFireWhen_Post_Context_Nop,
    RtemsTimerReqFireWhen_Post_Clock_Nop, RtemsTimerReqFireWhen_Post_State_Nop,
    RtemsTimerReqFireWhen_Post_WallTime_Nop,
    RtemsTimerReqFireWhen_Post_Routine_Nop,
    RtemsTimerReqFireWhen_Post_UserData_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqFireWhen_Post_Status_NotDef,
    RtemsTimerReqFireWhen_Post_Context_Nop,
    RtemsTimerReqFireWhen_Post_Clock_Nop, RtemsTimerReqFireWhen_Post_State_Nop,
    RtemsTimerReqFireWhen_Post_WallTime_Nop,
    RtemsTimerReqFireWhen_Post_Routine_Nop,
    RtemsTimerReqFireWhen_Post_UserData_Nop },
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqFireWhen_Post_Status_NA,
    RtemsTimerReqFireWhen_Post_Context_NA, RtemsTimerReqFireWhen_Post_Clock_NA,
    RtemsTimerReqFireWhen_Post_State_NA,
    RtemsTimerReqFireWhen_Post_WallTime_NA,
    RtemsTimerReqFireWhen_Post_Routine_NA,
    RtemsTimerReqFireWhen_Post_UserData_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqFireWhen_Post_Status_InvClock,
    RtemsTimerReqFireWhen_Post_Context_Nop,
    RtemsTimerReqFireWhen_Post_Clock_Nop, RtemsTimerReqFireWhen_Post_State_Nop,
    RtemsTimerReqFireWhen_Post_WallTime_Nop,
    RtemsTimerReqFireWhen_Post_Routine_Nop,
    RtemsTimerReqFireWhen_Post_UserData_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqFireWhen_Post_Status_Ok,
    RtemsTimerReqFireWhen_Post_Context_Interrupt,
    RtemsTimerReqFireWhen_Post_Clock_Realtime,
    RtemsTimerReqFireWhen_Post_State_Scheduled,
    RtemsTimerReqFireWhen_Post_WallTime_Param,
    RtemsTimerReqFireWhen_Post_Routine_Param,
    RtemsTimerReqFireWhen_Post_UserData_Param },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqFireWhen_Post_Status_InvId,
    RtemsTimerReqFireWhen_Post_Context_Nop,
    RtemsTimerReqFireWhen_Post_Clock_Nop, RtemsTimerReqFireWhen_Post_State_Nop,
    RtemsTimerReqFireWhen_Post_WallTime_Nop,
    RtemsTimerReqFireWhen_Post_Routine_Nop,
    RtemsTimerReqFireWhen_Post_UserData_Nop }
};

static const uint8_t
RtemsTimerReqFireWhen_Map[] = {
  5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 5,
  5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 6, 6, 0, 0, 0, 0, 6, 6, 6, 6,
  6, 6, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 4, 4, 0, 0, 0, 0, 4, 4, 4,
  4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 4, 4, 0, 0, 0, 0, 4, 4,
  4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 4, 4, 0, 0, 0, 0, 4,
  4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 4, 4, 0, 0, 0, 0,
  4, 4, 4, 4, 4, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1,
  0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1,
  1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3, 2, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3, 2, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3, 2, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3, 2, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3, 2,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3, 3,
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3, 3,
  3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2, 3,
  3, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2, 2,
  3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2, 2,
  2, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0, 2,
  2, 2, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3, 0, 0, 0,
  2, 2, 2, 3, 3, 3
};

static size_t RtemsTimerReqFireWhen_Scope( void *arg, char *buf, size_t n )
{
  RtemsTimerReqFireWhen_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTimerReqFireWhen_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTimerReqFireWhen_Fixture = {
  .setup = RtemsTimerReqFireWhen_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTimerReqFireWhen_Teardown_Wrap,
  .scope = RtemsTimerReqFireWhen_Scope,
  .initial_context = &RtemsTimerReqFireWhen_Instance
};

static inline RtemsTimerReqFireWhen_Entry RtemsTimerReqFireWhen_PopEntry(
  RtemsTimerReqFireWhen_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTimerReqFireWhen_Entries[
    RtemsTimerReqFireWhen_Map[ index ]
  ];
}

static void RtemsTimerReqFireWhen_TestVariant(
  RtemsTimerReqFireWhen_Context *ctx
)
{
  RtemsTimerReqFireWhen_Pre_RtClock_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTimerReqFireWhen_Pre_Routine_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTimerReqFireWhen_Pre_WallTime_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTimerReqFireWhen_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTimerReqFireWhen_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTimerReqFireWhen_Pre_Clock_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsTimerReqFireWhen_Pre_State_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsTimerReqFireWhen_Action( ctx );
  RtemsTimerReqFireWhen_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTimerReqFireWhen_Post_Context_Check( ctx, ctx->Map.entry.Post_Context );
  RtemsTimerReqFireWhen_Post_Clock_Check( ctx, ctx->Map.entry.Post_Clock );
  RtemsTimerReqFireWhen_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  RtemsTimerReqFireWhen_Post_WallTime_Check(
    ctx,
    ctx->Map.entry.Post_WallTime
  );
  RtemsTimerReqFireWhen_Post_Routine_Check( ctx, ctx->Map.entry.Post_Routine );
  RtemsTimerReqFireWhen_Post_UserData_Check(
    ctx,
    ctx->Map.entry.Post_UserData
  );
}

/**
 * @fn void T_case_body_RtemsTimerReqFireWhen( void )
 */
T_TEST_CASE_FIXTURE( RtemsTimerReqFireWhen, &RtemsTimerReqFireWhen_Fixture )
{
  RtemsTimerReqFireWhen_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTimerReqFireWhen_Pre_RtClock_Set;
    ctx->Map.pcs[ 0 ] < RtemsTimerReqFireWhen_Pre_RtClock_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTimerReqFireWhen_Pre_Routine_Valid;
      ctx->Map.pcs[ 1 ] < RtemsTimerReqFireWhen_Pre_Routine_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTimerReqFireWhen_Pre_WallTime_Valid;
        ctx->Map.pcs[ 2 ] < RtemsTimerReqFireWhen_Pre_WallTime_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsTimerReqFireWhen_Pre_Id_Valid;
          ctx->Map.pcs[ 3 ] < RtemsTimerReqFireWhen_Pre_Id_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsTimerReqFireWhen_Pre_Context_None;
            ctx->Map.pcs[ 4 ] < RtemsTimerReqFireWhen_Pre_Context_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsTimerReqFireWhen_Pre_Clock_None;
              ctx->Map.pcs[ 5 ] < RtemsTimerReqFireWhen_Pre_Clock_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = RtemsTimerReqFireWhen_Pre_State_Inactive;
                ctx->Map.pcs[ 6 ] < RtemsTimerReqFireWhen_Pre_State_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                ctx->Map.entry = RtemsTimerReqFireWhen_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                RtemsTimerReqFireWhen_Prepare( ctx );
                RtemsTimerReqFireWhen_TestVariant( ctx );
                RtemsTimerReqFireWhen_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
