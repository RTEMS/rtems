/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerReqReset
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
 * @defgroup RtemsTimerReqReset spec:/rtems/timer/req/reset
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTimerReqReset_Pre_Id_Valid,
  RtemsTimerReqReset_Pre_Id_Invalid,
  RtemsTimerReqReset_Pre_Id_NA
} RtemsTimerReqReset_Pre_Id;

typedef enum {
  RtemsTimerReqReset_Pre_Context_None,
  RtemsTimerReqReset_Pre_Context_Interrupt,
  RtemsTimerReqReset_Pre_Context_Server,
  RtemsTimerReqReset_Pre_Context_NA
} RtemsTimerReqReset_Pre_Context;

typedef enum {
  RtemsTimerReqReset_Pre_Clock_None,
  RtemsTimerReqReset_Pre_Clock_Ticks,
  RtemsTimerReqReset_Pre_Clock_Realtime,
  RtemsTimerReqReset_Pre_Clock_NA
} RtemsTimerReqReset_Pre_Clock;

typedef enum {
  RtemsTimerReqReset_Pre_State_Inactive,
  RtemsTimerReqReset_Pre_State_Scheduled,
  RtemsTimerReqReset_Pre_State_Pending,
  RtemsTimerReqReset_Pre_State_NA
} RtemsTimerReqReset_Pre_State;

typedef enum {
  RtemsTimerReqReset_Post_Status_Ok,
  RtemsTimerReqReset_Post_Status_InvId,
  RtemsTimerReqReset_Post_Status_NotDef,
  RtemsTimerReqReset_Post_Status_NA
} RtemsTimerReqReset_Post_Status;

typedef enum {
  RtemsTimerReqReset_Post_Context_None,
  RtemsTimerReqReset_Post_Context_Interrupt,
  RtemsTimerReqReset_Post_Context_Server,
  RtemsTimerReqReset_Post_Context_Nop,
  RtemsTimerReqReset_Post_Context_NA
} RtemsTimerReqReset_Post_Context;

typedef enum {
  RtemsTimerReqReset_Post_Clock_None,
  RtemsTimerReqReset_Post_Clock_Ticks,
  RtemsTimerReqReset_Post_Clock_Realtime,
  RtemsTimerReqReset_Post_Clock_Nop,
  RtemsTimerReqReset_Post_Clock_NA
} RtemsTimerReqReset_Post_Clock;

typedef enum {
  RtemsTimerReqReset_Post_State_Scheduled,
  RtemsTimerReqReset_Post_State_Nop,
  RtemsTimerReqReset_Post_State_NA
} RtemsTimerReqReset_Post_State;

typedef enum {
  RtemsTimerReqReset_Post_Interval_Last,
  RtemsTimerReqReset_Post_Interval_Nop,
  RtemsTimerReqReset_Post_Interval_NA
} RtemsTimerReqReset_Post_Interval;

typedef enum {
  RtemsTimerReqReset_Post_Routine_Last,
  RtemsTimerReqReset_Post_Routine_Nop,
  RtemsTimerReqReset_Post_Routine_NA
} RtemsTimerReqReset_Post_Routine;

typedef enum {
  RtemsTimerReqReset_Post_UserData_Last,
  RtemsTimerReqReset_Post_UserData_Nop,
  RtemsTimerReqReset_Post_UserData_NA
} RtemsTimerReqReset_Post_UserData;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_Context_NA : 1;
  uint32_t Pre_Clock_NA : 1;
  uint32_t Pre_State_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Context : 3;
  uint32_t Post_Clock : 3;
  uint32_t Post_State : 2;
  uint32_t Post_Interval : 2;
  uint32_t Post_Routine : 2;
  uint32_t Post_UserData : 2;
} RtemsTimerReqReset_Entry;

typedef enum {
  PRE_NONE      = 0,
  PRE_INTERRUPT = 1,
  PRE_SERVER    = 2
} PreConditionContext;

typedef enum {
  SCHEDULE_NONE  = 0,
  SCHEDULE_SOON  = 1,
  SCHEDULE_LATER = 2,
  SCHEDULE_MAX   = 5
} Scheduling_Ticks;

/**
 * @brief Test context for spec:/rtems/timer/req/reset test case.
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
   * @brief This member contains the returned status code of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains the user data for Timer Service Routine "A".
   */
  void *data_a;

  /**
   * @brief This member contains the user data for Timer Service Routine "B".
   */
  void *data_b;

  /**
   * @brief This member contains the counter for invocations of Timer Service
   *   Routine "A".
   */
  int invocations_a;

  /**
   * @brief This member contains the counter for invocations of Timer Service
   *   Routine "B".
   */
  int invocations_b;

  /**
   * @brief This member contains the number of ticks needed to fire the Timer
   *   Service Routine.
   */
  Scheduling_Ticks ticks_till_fire;

  /**
   * @brief This member identifier the user data given to the Timer Service
   *   Routine when called. It either the address of member data_a or data_b.
   */
  void **routine_user_data;

  /**
   * @brief This member contains a reference to the user data to be used in the
   *   next call to the Timer Service Routine.
   */
  void **scheduled_user_data;

  /**
   * @brief This member contains 1 if the Timer Service Routine "A" has been
   *   scheduled otherwise 0.
   */
  int scheduled_invocations_a;

  /**
   * @brief This member contains 1 if the Timer Service Routine "B" has been
   *   scheduled otherwise 0.
   */
  int scheduled_invocations_b;

  /**
   * @brief This member specifies the number of ticks till the scheduled Timer
   *   Service Routine should fire.
   */
  Scheduling_Ticks scheduled_ticks_till_fire;

  /**
   * @brief This member specifies which pre-condition context (none, interrupt
   *   context, server context) must be created before the rtems_timer_reset()
   *   action gets executed.
   */
  PreConditionContext pre_cond_contex;

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
    RtemsTimerReqReset_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTimerReqReset_Context;

static RtemsTimerReqReset_Context
  RtemsTimerReqReset_Instance;

static const char * const RtemsTimerReqReset_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsTimerReqReset_PreDesc_Context[] = {
  "None",
  "Interrupt",
  "Server",
  "NA"
};

static const char * const RtemsTimerReqReset_PreDesc_Clock[] = {
  "None",
  "Ticks",
  "Realtime",
  "NA"
};

static const char * const RtemsTimerReqReset_PreDesc_State[] = {
  "Inactive",
  "Scheduled",
  "Pending",
  "NA"
};

static const char * const * const RtemsTimerReqReset_PreDesc[] = {
  RtemsTimerReqReset_PreDesc_Id,
  RtemsTimerReqReset_PreDesc_Context,
  RtemsTimerReqReset_PreDesc_Clock,
  RtemsTimerReqReset_PreDesc_State,
  NULL
};

static const rtems_time_of_day tod_now      = { 2000, 1, 1, 0, 0, 0, 0 };
static const rtems_time_of_day tod_schedule = { 2000, 1, 1, 1, 0, 0, 0 };
static const rtems_time_of_day tod_fire     = { 2000, 1, 2, 0, 0, 0, 0 };

static Scheduling_Ticks TriggerTimer( const RtemsTimerReqReset_Context *ctx )
{
  int ticks_fired = SCHEDULE_NONE;
  int invocations_old = ctx->invocations_a + ctx->invocations_b;

  /* Fire the timer service routine for ticks and realtime clock */
  int i;
  for ( i = 1; i <= SCHEDULE_MAX; ++i ) {
    ClockTick();
    if ( ctx->invocations_a + ctx->invocations_b > invocations_old ) {
      ticks_fired = i;
      break;
    }
  }

  T_rsc_success( rtems_clock_set( &tod_fire ) );

  return ticks_fired;
}

static void TimerServiceRoutineA(
  rtems_id timer_id,
  void *user_data
)
{
  RtemsTimerReqReset_Context *ctx =
    *(RtemsTimerReqReset_Context **) user_data;
  ++( ctx->invocations_a );
  ctx->routine_user_data = user_data;
}

static void TimerServiceRoutineB(
  rtems_id timer_id,
  void *user_data
)
{
  RtemsTimerReqReset_Context *ctx =
    *(RtemsTimerReqReset_Context **) user_data;
  ++( ctx->invocations_b );
  ctx->routine_user_data = user_data;
}

static void RtemsTimerReqReset_Pre_Id_Prepare(
  RtemsTimerReqReset_Context *ctx,
  RtemsTimerReqReset_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTimerReqReset_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->timer_id;
      break;
    }

    case RtemsTimerReqReset_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsTimerReqReset_Pre_Id_NA:
      break;
  }
}

static void RtemsTimerReqReset_Pre_Context_Prepare(
  RtemsTimerReqReset_Context    *ctx,
  RtemsTimerReqReset_Pre_Context state
)
{
  switch ( state ) {
    case RtemsTimerReqReset_Pre_Context_None: {
      /*
       * While the Timer Service Routine has never been scheduled since
       * creation of the timer. See also none.
       */
      ctx->pre_cond_contex = PRE_NONE;
      break;
    }

    case RtemsTimerReqReset_Pre_Context_Interrupt: {
      /*
       * While the timer is in interrupt context.
       */
      ctx->pre_cond_contex = PRE_INTERRUPT;
      break;
    }

    case RtemsTimerReqReset_Pre_Context_Server: {
      /*
       * While the timer is in server context.
       */
      ctx->pre_cond_contex = PRE_SERVER;
      break;
    }

    case RtemsTimerReqReset_Pre_Context_NA:
      break;
  }
}

static void RtemsTimerReqReset_Pre_Clock_Prepare(
  RtemsTimerReqReset_Context  *ctx,
  RtemsTimerReqReset_Pre_Clock state
)
{
  switch ( state ) {
    case RtemsTimerReqReset_Pre_Clock_None: {
      /*
       * While the timer has never been scheduled since creation of the timer.
       */
      T_eq_int( ctx->pre_cond_contex, PRE_NONE );
      break;
    }

    case RtemsTimerReqReset_Pre_Clock_Ticks: {
      /*
       * While the clock used to determine when the timer will fire is the
       * ticks based clock.
       */
      rtems_status_code status;

      if ( ctx->pre_cond_contex == PRE_INTERRUPT ) {
        ctx->scheduled_ticks_till_fire = SCHEDULE_SOON;
        ctx->scheduled_invocations_a = 1;
        ctx->scheduled_user_data = &ctx->data_a;
        status = rtems_timer_fire_after(
          ctx->timer_id,
          ctx->scheduled_ticks_till_fire,
          TimerServiceRoutineA,
          ctx->scheduled_user_data
        );
      } else {
        ctx->scheduled_ticks_till_fire = SCHEDULE_LATER;
        ctx->scheduled_invocations_b = 1;
        ctx->scheduled_user_data = &ctx->data_b;
         status = rtems_timer_server_fire_after(
          ctx->timer_id,
          ctx->scheduled_ticks_till_fire,
          TimerServiceRoutineB,
          ctx->scheduled_user_data
        );
      }
      T_rsc_success( status );
      break;
    }

    case RtemsTimerReqReset_Pre_Clock_Realtime: {
      /*
       * While the clock used to determine when the timer will fire is the
       * realtime clock.
       */
      rtems_status_code status;

      if ( ctx->pre_cond_contex == PRE_INTERRUPT ) {
        status = rtems_timer_fire_when(
          ctx->timer_id,
          &tod_schedule,
          TimerServiceRoutineA,
          &ctx->data_a
        );
      } else {
        status = rtems_timer_server_fire_when(
          ctx->timer_id,
          &tod_schedule,
          TimerServiceRoutineB,
          &ctx->data_b
        );
      }
      T_rsc_success( status );
      break;
    }

    case RtemsTimerReqReset_Pre_Clock_NA:
      break;
  }
}

static void RtemsTimerReqReset_Pre_State_Prepare(
  RtemsTimerReqReset_Context  *ctx,
  RtemsTimerReqReset_Pre_State state
)
{
  switch ( state ) {
    case RtemsTimerReqReset_Pre_State_Inactive: {
      /*
       * While the timer is in inactive state.
       */
      TriggerTimer( ctx );
      T_eq_int(
        ctx->invocations_a + ctx->invocations_b,
        ( ctx->pre_cond_contex == PRE_NONE ) ? 0 : 1
      );
      ctx->invocations_a = 0;
      ctx->invocations_b = 0;
      ctx->pre_state = TIMER_INACTIVE;
      break;
    }

    case RtemsTimerReqReset_Pre_State_Scheduled: {
      /*
       * While the timer is in scheduled state.
       */
      /* The timer was already scheduled in the "Clock" pre-conditions. */
      ctx->pre_state = TIMER_SCHEDULED;
      break;
    }

    case RtemsTimerReqReset_Pre_State_Pending: {
      /*
       * While the timer is in pending state.
       */
      T_rsc_success( rtems_task_suspend( GetTimerServerTaskId() ) );
      TriggerTimer( ctx );
      T_eq_int( ctx->invocations_a + ctx->invocations_b, 0 );
      ctx->pre_state = TIMER_PENDING;
      break;
    }

    case RtemsTimerReqReset_Pre_State_NA:
      break;
  }
}

static void RtemsTimerReqReset_Post_Status_Check(
  RtemsTimerReqReset_Context    *ctx,
  RtemsTimerReqReset_Post_Status state
)
{
  switch ( state ) {
    case RtemsTimerReqReset_Post_Status_Ok: {
      /*
       * The return status of rtems_timer_reset() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTimerReqReset_Post_Status_InvId: {
      /*
       * The return status of rtems_timer_reset() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTimerReqReset_Post_Status_NotDef: {
      /*
       * The return status of rtems_timer_reset() shall be RTEMS_NOT_DEFINED
       */
      T_rsc( ctx->status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsTimerReqReset_Post_Status_NA:
      break;
  }
}

static void RtemsTimerReqReset_Post_Context_Check(
  RtemsTimerReqReset_Context     *ctx,
  RtemsTimerReqReset_Post_Context state
)
{
  Timer_Classes class;
  class = GetTimerClass( ctx->timer_id );

  switch ( state ) {
    case RtemsTimerReqReset_Post_Context_None: {
      /*
       * The timer shall have never been scheduled. See also none.
       */
      T_eq_int( class, TIMER_DORMANT );
      break;
    }

    case RtemsTimerReqReset_Post_Context_Interrupt: {
      /*
       * The timer shall be in interrupt context.
       */
      T_eq_int( class & TIMER_CLASS_BIT_ON_TASK, 0 );
      break;
    }

    case RtemsTimerReqReset_Post_Context_Server: {
      /*
       * The timer shall be in server context.
       */
      T_eq_int( class & TIMER_CLASS_BIT_ON_TASK, TIMER_CLASS_BIT_ON_TASK );
      break;
    }

    case RtemsTimerReqReset_Post_Context_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past call to
       * rtems_timer_reset() shall not be accessed by the rtems_timer_reset()
       * call. See also Nop.
       */
      T_eq_int( class, ctx->pre_class );
      break;
    }

    case RtemsTimerReqReset_Post_Context_NA:
      break;
  }
}

static void RtemsTimerReqReset_Post_Clock_Check(
  RtemsTimerReqReset_Context   *ctx,
  RtemsTimerReqReset_Post_Clock state
)
{
  Timer_Classes class;
  class = GetTimerClass( ctx->timer_id );

  switch ( state ) {
    case RtemsTimerReqReset_Post_Clock_None: {
      /*
       * The timer shall have never been scheduled.
       */
      T_eq_int( class, TIMER_DORMANT );
      break;
    }

    case RtemsTimerReqReset_Post_Clock_Ticks: {
      /*
       * The timer shall use the ticks based clock.
       */
      T_eq_int( class & TIMER_CLASS_BIT_TIME_OF_DAY, 0 );
      break;
    }

    case RtemsTimerReqReset_Post_Clock_Realtime: {
      /*
       * The timer shall use the realtime clock.
       */
      T_eq_int(
        class & TIMER_CLASS_BIT_TIME_OF_DAY,
        TIMER_CLASS_BIT_TIME_OF_DAY
      );
      break;
    }

    case RtemsTimerReqReset_Post_Clock_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past call to
       * rtems_timer_reset() shall not be accessed by the rtems_timer_reset()
       * call.
       */
      T_eq_int( class, ctx->pre_class );
      break;
    }

    case RtemsTimerReqReset_Post_Clock_NA:
      break;
  }
}

static void RtemsTimerReqReset_Post_State_Check(
  RtemsTimerReqReset_Context   *ctx,
  RtemsTimerReqReset_Post_State state
)
{
  switch ( state ) {
    case RtemsTimerReqReset_Post_State_Scheduled: {
      /*
       * The timer shall be in scheduled state.
       */
      ctx->ticks_till_fire = TriggerTimer( ctx );
      T_eq_int( ctx->invocations_a + ctx->invocations_b, 1 );
      break;
    }

    case RtemsTimerReqReset_Post_State_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past call to
       * rtems_timer_reset() shall not be accessed by the rtems_timer_reset()
       * call.
       */
      T_eq_int( ctx->post_state, ctx->pre_state );
      break;
    }

    case RtemsTimerReqReset_Post_State_NA:
      break;
  }
}

static void RtemsTimerReqReset_Post_Interval_Check(
  RtemsTimerReqReset_Context      *ctx,
  RtemsTimerReqReset_Post_Interval state
)
{
  switch ( state ) {
    case RtemsTimerReqReset_Post_Interval_Last: {
      /*
       * The Timer Service Routine shall be invoked the same number of ticks
       * (see tick), as defined by the last scheduled interval, after a point
       * in time during the execution of the rtems_timer_reset() call.
       */
      T_eq_int( ctx->ticks_till_fire, ctx->scheduled_ticks_till_fire );
      break;
    }

    case RtemsTimerReqReset_Post_Interval_Nop: {
      /*
       * If and when the Timer Service Routine will be invoked shall not be
       * changed by the past call to rtems_timer_reset().
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

    case RtemsTimerReqReset_Post_Interval_NA:
      break;
  }
}

static void RtemsTimerReqReset_Post_Routine_Check(
  RtemsTimerReqReset_Context     *ctx,
  RtemsTimerReqReset_Post_Routine state
)
{
  switch ( state ) {
    case RtemsTimerReqReset_Post_Routine_Last: {
      /*
       * The function reference used to invoke the Timer Service Routine when
       * the timer will fire shall be the same one as the last one scheduled.
       */
      T_eq_int( ctx->invocations_a, ctx->scheduled_invocations_a );
      T_eq_int( ctx->invocations_b, ctx->scheduled_invocations_b );
      break;
    }

    case RtemsTimerReqReset_Post_Routine_Nop: {
      /*
       * The function reference used for any invocation of the Timer Service
       * Routine shall not be changed by the past call to rtems_timer_reset().
       */
      T_eq_ptr(
        ctx->post_scheduling_data.routine,
        ctx->pre_scheduling_data.routine
      );
      break;
    }

    case RtemsTimerReqReset_Post_Routine_NA:
      break;
  }
}

static void RtemsTimerReqReset_Post_UserData_Check(
  RtemsTimerReqReset_Context      *ctx,
  RtemsTimerReqReset_Post_UserData state
)
{
  switch ( state ) {
    case RtemsTimerReqReset_Post_UserData_Last: {
      /*
       * The user data argument for invoking the Timer Service Routine when the
       * timer will fire shall be the same as the last scheduled user data
       * argument.
       */
      T_eq_ptr( ctx->routine_user_data, ctx->scheduled_user_data);
      break;
    }

    case RtemsTimerReqReset_Post_UserData_Nop: {
      /*
       * The user data argument used for any invocation of the Timer Service
       * Routine shall not be changed by the past call to rtems_timer_reset().
       */
      T_eq_ptr(
        ctx->post_scheduling_data.user_data,
        ctx->pre_scheduling_data.user_data
      );
      break;
    }

    case RtemsTimerReqReset_Post_UserData_NA:
      break;
  }
}

static void RtemsTimerReqReset_Setup( RtemsTimerReqReset_Context *ctx )
{
  rtems_status_code status;
  status = rtems_timer_initiate_server(
    RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  T_rsc_success( status );
}

static void RtemsTimerReqReset_Setup_Wrap( void *arg )
{
  RtemsTimerReqReset_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqReset_Setup( ctx );
}

/**
 * @brief Make sure the timer server is not running and the realtime clock is
 *   not set after this test.
 */
static void RtemsTimerReqReset_Teardown( RtemsTimerReqReset_Context *ctx )
{
  DeleteTimerServer();
  UnsetClock();
}

static void RtemsTimerReqReset_Teardown_Wrap( void *arg )
{
  RtemsTimerReqReset_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqReset_Teardown( ctx );
}

static void RtemsTimerReqReset_Prepare( RtemsTimerReqReset_Context *ctx )
{
  rtems_status_code status;
  status = rtems_timer_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    &ctx->timer_id
  );
  T_rsc_success( status );

  ctx->data_a                    = ctx;
  ctx->data_b                    = ctx;
  ctx->invocations_a             = 0;
  ctx->invocations_b             = 0;
  ctx->ticks_till_fire           = SCHEDULE_NONE;
  ctx->routine_user_data         = NULL;
  ctx->scheduled_invocations_a   = 0;
  ctx->scheduled_invocations_b   = 0;
  ctx->scheduled_ticks_till_fire = SCHEDULE_NONE;
  T_rsc_success( rtems_clock_set( &tod_now ) );
}

static void RtemsTimerReqReset_Action( RtemsTimerReqReset_Context *ctx )
{
  GetTimerSchedulingData( ctx->timer_id, &ctx->pre_scheduling_data );
  ctx->pre_class = GetTimerClass( ctx->timer_id );
  ctx->status = rtems_timer_reset( ctx->id_param );
  ctx->post_state = GetTimerState( ctx->timer_id );
  GetTimerSchedulingData( ctx->timer_id, &ctx->post_scheduling_data );
  /* Ignoring return status: the timer server task may be suspended or not. */
  rtems_task_resume( GetTimerServerTaskId() );
}

static void RtemsTimerReqReset_Cleanup( RtemsTimerReqReset_Context *ctx )
{
  T_rsc_success( rtems_timer_delete( ctx->timer_id ) );
}

static const RtemsTimerReqReset_Entry
RtemsTimerReqReset_Entries[] = {
  { 1, 0, 0, 0, 0, RtemsTimerReqReset_Post_Status_NA,
    RtemsTimerReqReset_Post_Context_NA, RtemsTimerReqReset_Post_Clock_NA,
    RtemsTimerReqReset_Post_State_NA, RtemsTimerReqReset_Post_Interval_NA,
    RtemsTimerReqReset_Post_Routine_NA, RtemsTimerReqReset_Post_UserData_NA },
  { 0, 0, 0, 0, 0, RtemsTimerReqReset_Post_Status_InvId,
    RtemsTimerReqReset_Post_Context_Nop, RtemsTimerReqReset_Post_Clock_Nop,
    RtemsTimerReqReset_Post_State_Nop, RtemsTimerReqReset_Post_Interval_Nop,
    RtemsTimerReqReset_Post_Routine_Nop, RtemsTimerReqReset_Post_UserData_Nop },
  { 0, 0, 0, 0, 0, RtemsTimerReqReset_Post_Status_NotDef,
    RtemsTimerReqReset_Post_Context_Nop, RtemsTimerReqReset_Post_Clock_Nop,
    RtemsTimerReqReset_Post_State_Nop, RtemsTimerReqReset_Post_Interval_Nop,
    RtemsTimerReqReset_Post_Routine_Nop, RtemsTimerReqReset_Post_UserData_Nop },
  { 0, 0, 0, 0, 0, RtemsTimerReqReset_Post_Status_Ok,
    RtemsTimerReqReset_Post_Context_Server,
    RtemsTimerReqReset_Post_Clock_Ticks,
    RtemsTimerReqReset_Post_State_Scheduled,
    RtemsTimerReqReset_Post_Interval_Last,
    RtemsTimerReqReset_Post_Routine_Last, RtemsTimerReqReset_Post_UserData_Last },
  { 0, 0, 0, 0, 0, RtemsTimerReqReset_Post_Status_Ok,
    RtemsTimerReqReset_Post_Context_Interrupt,
    RtemsTimerReqReset_Post_Clock_Ticks,
    RtemsTimerReqReset_Post_State_Scheduled,
    RtemsTimerReqReset_Post_Interval_Last,
    RtemsTimerReqReset_Post_Routine_Last, RtemsTimerReqReset_Post_UserData_Last }
};

static const uint8_t
RtemsTimerReqReset_Map[] = {
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 2, 2, 0, 0, 0, 0, 3, 3, 3, 2, 2,
  2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1,
  1, 1
};

static size_t RtemsTimerReqReset_Scope( void *arg, char *buf, size_t n )
{
  RtemsTimerReqReset_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTimerReqReset_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTimerReqReset_Fixture = {
  .setup = RtemsTimerReqReset_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTimerReqReset_Teardown_Wrap,
  .scope = RtemsTimerReqReset_Scope,
  .initial_context = &RtemsTimerReqReset_Instance
};

static inline RtemsTimerReqReset_Entry RtemsTimerReqReset_PopEntry(
  RtemsTimerReqReset_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTimerReqReset_Entries[
    RtemsTimerReqReset_Map[ index ]
  ];
}

static void RtemsTimerReqReset_TestVariant( RtemsTimerReqReset_Context *ctx )
{
  RtemsTimerReqReset_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTimerReqReset_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTimerReqReset_Pre_Clock_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTimerReqReset_Pre_State_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTimerReqReset_Action( ctx );
  RtemsTimerReqReset_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTimerReqReset_Post_Context_Check( ctx, ctx->Map.entry.Post_Context );
  RtemsTimerReqReset_Post_Clock_Check( ctx, ctx->Map.entry.Post_Clock );
  RtemsTimerReqReset_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  RtemsTimerReqReset_Post_Interval_Check( ctx, ctx->Map.entry.Post_Interval );
  RtemsTimerReqReset_Post_Routine_Check( ctx, ctx->Map.entry.Post_Routine );
  RtemsTimerReqReset_Post_UserData_Check( ctx, ctx->Map.entry.Post_UserData );
}

/**
 * @fn void T_case_body_RtemsTimerReqReset( void )
 */
T_TEST_CASE_FIXTURE( RtemsTimerReqReset, &RtemsTimerReqReset_Fixture )
{
  RtemsTimerReqReset_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTimerReqReset_Pre_Id_Valid;
    ctx->Map.pcs[ 0 ] < RtemsTimerReqReset_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTimerReqReset_Pre_Context_None;
      ctx->Map.pcs[ 1 ] < RtemsTimerReqReset_Pre_Context_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTimerReqReset_Pre_Clock_None;
        ctx->Map.pcs[ 2 ] < RtemsTimerReqReset_Pre_Clock_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsTimerReqReset_Pre_State_Inactive;
          ctx->Map.pcs[ 3 ] < RtemsTimerReqReset_Pre_State_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = RtemsTimerReqReset_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsTimerReqReset_Prepare( ctx );
          RtemsTimerReqReset_TestVariant( ctx );
          RtemsTimerReqReset_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
