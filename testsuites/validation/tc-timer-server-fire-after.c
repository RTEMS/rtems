/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerReqServerFireAfter
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
 * @defgroup RtemsTimerReqServerFireAfter \
 *   spec:/rtems/timer/req/server-fire-after
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTimerReqServerFireAfter_Pre_Server_Init,
  RtemsTimerReqServerFireAfter_Pre_Server_NotInit,
  RtemsTimerReqServerFireAfter_Pre_Server_NA
} RtemsTimerReqServerFireAfter_Pre_Server;

typedef enum {
  RtemsTimerReqServerFireAfter_Pre_Ticks_Valid,
  RtemsTimerReqServerFireAfter_Pre_Ticks_Is0,
  RtemsTimerReqServerFireAfter_Pre_Ticks_NA
} RtemsTimerReqServerFireAfter_Pre_Ticks;

typedef enum {
  RtemsTimerReqServerFireAfter_Pre_Routine_Valid,
  RtemsTimerReqServerFireAfter_Pre_Routine_Null,
  RtemsTimerReqServerFireAfter_Pre_Routine_NA
} RtemsTimerReqServerFireAfter_Pre_Routine;

typedef enum {
  RtemsTimerReqServerFireAfter_Pre_Id_Valid,
  RtemsTimerReqServerFireAfter_Pre_Id_Invalid,
  RtemsTimerReqServerFireAfter_Pre_Id_NA
} RtemsTimerReqServerFireAfter_Pre_Id;

typedef enum {
  RtemsTimerReqServerFireAfter_Pre_Context_None,
  RtemsTimerReqServerFireAfter_Pre_Context_Interrupt,
  RtemsTimerReqServerFireAfter_Pre_Context_Server,
  RtemsTimerReqServerFireAfter_Pre_Context_NA
} RtemsTimerReqServerFireAfter_Pre_Context;

typedef enum {
  RtemsTimerReqServerFireAfter_Pre_Clock_None,
  RtemsTimerReqServerFireAfter_Pre_Clock_Ticks,
  RtemsTimerReqServerFireAfter_Pre_Clock_Realtime,
  RtemsTimerReqServerFireAfter_Pre_Clock_NA
} RtemsTimerReqServerFireAfter_Pre_Clock;

typedef enum {
  RtemsTimerReqServerFireAfter_Pre_State_Inactive,
  RtemsTimerReqServerFireAfter_Pre_State_Scheduled,
  RtemsTimerReqServerFireAfter_Pre_State_Pending,
  RtemsTimerReqServerFireAfter_Pre_State_NA
} RtemsTimerReqServerFireAfter_Pre_State;

typedef enum {
  RtemsTimerReqServerFireAfter_Post_Status_Ok,
  RtemsTimerReqServerFireAfter_Post_Status_InvId,
  RtemsTimerReqServerFireAfter_Post_Status_InvAddr,
  RtemsTimerReqServerFireAfter_Post_Status_InvNum,
  RtemsTimerReqServerFireAfter_Post_Status_IncStat,
  RtemsTimerReqServerFireAfter_Post_Status_NA
} RtemsTimerReqServerFireAfter_Post_Status;

typedef enum {
  RtemsTimerReqServerFireAfter_Post_Context_None,
  RtemsTimerReqServerFireAfter_Post_Context_Interrupt,
  RtemsTimerReqServerFireAfter_Post_Context_Server,
  RtemsTimerReqServerFireAfter_Post_Context_Nop,
  RtemsTimerReqServerFireAfter_Post_Context_NA
} RtemsTimerReqServerFireAfter_Post_Context;

typedef enum {
  RtemsTimerReqServerFireAfter_Post_Clock_None,
  RtemsTimerReqServerFireAfter_Post_Clock_Ticks,
  RtemsTimerReqServerFireAfter_Post_Clock_Realtime,
  RtemsTimerReqServerFireAfter_Post_Clock_Nop,
  RtemsTimerReqServerFireAfter_Post_Clock_NA
} RtemsTimerReqServerFireAfter_Post_Clock;

typedef enum {
  RtemsTimerReqServerFireAfter_Post_State_Scheduled,
  RtemsTimerReqServerFireAfter_Post_State_Nop,
  RtemsTimerReqServerFireAfter_Post_State_NA
} RtemsTimerReqServerFireAfter_Post_State;

typedef enum {
  RtemsTimerReqServerFireAfter_Post_Interval_Param,
  RtemsTimerReqServerFireAfter_Post_Interval_Nop,
  RtemsTimerReqServerFireAfter_Post_Interval_NA
} RtemsTimerReqServerFireAfter_Post_Interval;

typedef enum {
  RtemsTimerReqServerFireAfter_Post_Routine_Param,
  RtemsTimerReqServerFireAfter_Post_Routine_Nop,
  RtemsTimerReqServerFireAfter_Post_Routine_NA
} RtemsTimerReqServerFireAfter_Post_Routine;

typedef enum {
  RtemsTimerReqServerFireAfter_Post_UserData_Param,
  RtemsTimerReqServerFireAfter_Post_UserData_Nop,
  RtemsTimerReqServerFireAfter_Post_UserData_NA
} RtemsTimerReqServerFireAfter_Post_UserData;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Server_NA : 1;
  uint32_t Pre_Ticks_NA : 1;
  uint32_t Pre_Routine_NA : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_Context_NA : 1;
  uint32_t Pre_Clock_NA : 1;
  uint32_t Pre_State_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Context : 3;
  uint32_t Post_Clock : 3;
  uint32_t Post_State : 2;
  uint32_t Post_Interval : 2;
  uint32_t Post_Routine : 2;
  uint32_t Post_UserData : 2;
} RtemsTimerReqServerFireAfter_Entry;

typedef enum {
  PRE_NONE      = 0,
  PRE_INTERRUPT = 1,
  PRE_SERVER    = 2
} PreConditionContext;

typedef enum {
  SCHEDULE_NONE       = 0,
  SCHEDULE_VERY_SOON  = 1,
  SCHEDULE_SOON       = 2,
  SCHEDULE_LATER      = 3,
  SCHEDULE_MAX        = 5
} Scheduling_Ticks;

/**
 * @brief Test context for spec:/rtems/timer/req/server-fire-after test case.
 */
typedef struct {
  /**
   * @brief This member contains a valid id of a timer.
   */
  rtems_id timer_id;

  /**
   * @brief This member contains a valid id of a second timer.
   *
   * This timer is used reach branch coverage in an if-statement which only
   * serves performance optimization.
   */
  rtems_id timer_cover_id;

  /**
   * @brief This member specifies the ``id`` parameter for the action.
   */
  rtems_id id_param;

  /**
   * @brief This member specifies the ``ticks`` parameter for the action.
   */
  rtems_interval ticks_param;

  /**
   * @brief This member specifies the ``routine`` parameter for the action.
   */
  rtems_timer_service_routine_entry routine_param;

  /**
   * @brief This member contains the returned status code of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains a counter for invocations of the Timer Service
   *   Routine.
   */
  int invocations;

  /**
   * @brief Function TriggerTimer() is used to figure out how many ticks (see
   *   tick) are needed till the Timer Service Routine gets executed. This
   *   member contains the number of ticks needed to fire the Timer Service
   *   Routine.
   */
  Scheduling_Ticks ticks_till_fire;

  /**
   * @brief This member contains the user data given to the Timer Service
   *   Routine when called.
   */
  void *routine_user_data;

  /**
   * @brief This member specifies which pre-condition context (none, interrupt
   *   context, server context) must be created before the
   *   rtems_timer_server_fire_after() action gets executed.
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
    RtemsTimerReqServerFireAfter_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTimerReqServerFireAfter_Context;

static RtemsTimerReqServerFireAfter_Context
  RtemsTimerReqServerFireAfter_Instance;

static const char * const RtemsTimerReqServerFireAfter_PreDesc_Server[] = {
  "Init",
  "NotInit",
  "NA"
};

static const char * const RtemsTimerReqServerFireAfter_PreDesc_Ticks[] = {
  "Valid",
  "Is0",
  "NA"
};

static const char * const RtemsTimerReqServerFireAfter_PreDesc_Routine[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsTimerReqServerFireAfter_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsTimerReqServerFireAfter_PreDesc_Context[] = {
  "None",
  "Interrupt",
  "Server",
  "NA"
};

static const char * const RtemsTimerReqServerFireAfter_PreDesc_Clock[] = {
  "None",
  "Ticks",
  "Realtime",
  "NA"
};

static const char * const RtemsTimerReqServerFireAfter_PreDesc_State[] = {
  "Inactive",
  "Scheduled",
  "Pending",
  "NA"
};

static const char * const * const RtemsTimerReqServerFireAfter_PreDesc[] = {
  RtemsTimerReqServerFireAfter_PreDesc_Server,
  RtemsTimerReqServerFireAfter_PreDesc_Ticks,
  RtemsTimerReqServerFireAfter_PreDesc_Routine,
  RtemsTimerReqServerFireAfter_PreDesc_Id,
  RtemsTimerReqServerFireAfter_PreDesc_Context,
  RtemsTimerReqServerFireAfter_PreDesc_Clock,
  RtemsTimerReqServerFireAfter_PreDesc_State,
  NULL
};

static const rtems_time_of_day tod_now      = { 2000, 1, 1, 0, 0, 0, 0 };
static const rtems_time_of_day tod_schedule = { 2000, 1, 1, 1, 0, 0, 0 };
static const rtems_time_of_day tod_fire     = { 2000, 1, 2, 0, 0, 0, 0 };

static Scheduling_Ticks TriggerTimer( const RtemsTimerReqServerFireAfter_Context *ctx )
{
  int ticks_fired = SCHEDULE_NONE;
  int invocations_old = ctx->invocations;

  /* Fire the timer service routine for ticks and realtime clock */
  int i;
  for ( i = 1; i <= SCHEDULE_MAX; ++i ) {
    ClockTick();
    if ( ctx->invocations > invocations_old ) {
      ticks_fired = i;
      break;
    }
  }

  T_rsc_success( rtems_clock_set( &tod_fire ) );

  return ticks_fired;
}

static void TimerServiceRoutine(
  rtems_id timer_id,
  void *user_data
)
{
  RtemsTimerReqServerFireAfter_Context *ctx = user_data;
  ++( ctx->invocations );
  ctx->routine_user_data = user_data;
}

/*
 * This service routine is used reach branch coverage in an if-statement
 * which only serves performance optimization.
 */
static void TimerServiceRoutineCover(
  rtems_id timer_cover_id,
  void *user_data
)
{
  (void) timer_cover_id;
  (void) user_data;
}

static void RtemsTimerReqServerFireAfter_Pre_Server_Prepare(
  RtemsTimerReqServerFireAfter_Context   *ctx,
  RtemsTimerReqServerFireAfter_Pre_Server state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Pre_Server_Init: {
      /*
       * While the Timer Server task has been successfully initialized by a
       * call to rtems_timer_initiate_server().
       */
      rtems_status_code status;
      status = rtems_timer_initiate_server(
        RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_DEFAULT_ATTRIBUTES
      );
      T_rsc_success( status );
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Server_NotInit: {
      /*
       * While the Timer Server task has not been initialized and does not
       * exist.
       */
      DeleteTimerServer();
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Server_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Pre_Ticks_Prepare(
  RtemsTimerReqServerFireAfter_Context  *ctx,
  RtemsTimerReqServerFireAfter_Pre_Ticks state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Pre_Ticks_Valid: {
      /*
       * While the ``ticks`` parameter is a positive (greater 0) number.
       */
      ctx->ticks_param = SCHEDULE_LATER;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Ticks_Is0: {
      /*
       * While the ``ticks`` parameter is 0.
       */
      ctx->ticks_param = 0;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Ticks_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Pre_Routine_Prepare(
  RtemsTimerReqServerFireAfter_Context    *ctx,
  RtemsTimerReqServerFireAfter_Pre_Routine state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Pre_Routine_Valid: {
      /*
       * While the ``routine`` parameter references an object of type
       * rtems_timer_service_routine_entry.
       */
      ctx->routine_param = TimerServiceRoutine;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Routine_Null: {
      /*
       * While the ``routine`` parameter is NULL..
       */
      ctx->routine_param = NULL;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Routine_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Pre_Id_Prepare(
  RtemsTimerReqServerFireAfter_Context *ctx,
  RtemsTimerReqServerFireAfter_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->timer_id;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Id_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Pre_Context_Prepare(
  RtemsTimerReqServerFireAfter_Context    *ctx,
  RtemsTimerReqServerFireAfter_Pre_Context state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Pre_Context_None: {
      /*
       * While the Timer Service Routine has never been scheduled since
       * creation of the timer. See also none.
       */
      ctx->pre_cond_contex = PRE_NONE;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Context_Interrupt: {
      /*
       * While the timer is in interrupt context.
       */
      ctx->pre_cond_contex = PRE_INTERRUPT;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Context_Server: {
      /*
       * While the timer is in server context.
       */
      ctx->pre_cond_contex = PRE_SERVER;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Context_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Pre_Clock_Prepare(
  RtemsTimerReqServerFireAfter_Context  *ctx,
  RtemsTimerReqServerFireAfter_Pre_Clock state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Pre_Clock_None: {
      /*
       * While the timer has never been scheduled since creation of the timer.
       */
      T_eq_int( ctx->pre_cond_contex, PRE_NONE );
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_Clock_Ticks: {
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

    case RtemsTimerReqServerFireAfter_Pre_Clock_Realtime: {
      /*
       * While the clock used to determine when the timer will fire is the
       * realtime clock.
       */
      rtems_status_code status;

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

    case RtemsTimerReqServerFireAfter_Pre_Clock_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Pre_State_Prepare(
  RtemsTimerReqServerFireAfter_Context  *ctx,
  RtemsTimerReqServerFireAfter_Pre_State state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Pre_State_Inactive: {
      /*
       * While the timer is in inactive state.
       */
      TriggerTimer( ctx );
      T_eq_int(
        ctx->invocations,
        ( ctx->pre_cond_contex == PRE_NONE ) ? 0 : 1
      );
      ctx->invocations = 0;
      ctx->pre_state = TIMER_INACTIVE;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_State_Scheduled: {
      /*
       * While the timer is in scheduled state.
       */
      /* The timer was already scheduled in the "Clock" pre-conditions. */
      ctx->pre_state = TIMER_SCHEDULED;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_State_Pending: {
      /*
       * While the timer is in pending state.
       */
      rtems_status_code status;
      if ( ctx->pre_cond_contex == PRE_SERVER ) {
        /*
         * This call to rtems_timer_fire_after() serves to reach branch coverage
         * in an if-statement which only serves performance optimization.
         *
         * cpukit/rtems/src/timerserver.c:70 else-branch was not reached:
         * if ( wakeup ) {
         *   (void) rtems_event_system_send( ts->server_id, RTEMS_EVENT_SYSTEM_SERVER );
         * }
         */
        status = rtems_timer_server_fire_after(
          ctx->timer_cover_id,
          SCHEDULE_VERY_SOON,
          TimerServiceRoutineCover,
          ctx
        );
        T_rsc_success( status );
      }

      T_rsc_success( rtems_task_suspend( GetTimerServerTaskId() ) );
      TriggerTimer( ctx );
      T_eq_int( ctx->invocations, 0 );
      ctx->pre_state = TIMER_PENDING;
      break;
    }

    case RtemsTimerReqServerFireAfter_Pre_State_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Post_Status_Check(
  RtemsTimerReqServerFireAfter_Context    *ctx,
  RtemsTimerReqServerFireAfter_Post_Status state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Post_Status_Ok: {
      /*
       * The return status of rtems_timer_server_fire_after() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Status_InvId: {
      /*
       * The return status of rtems_timer_server_fire_after() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Status_InvAddr: {
      /*
       * The return status of rtems_timer_server_fire_after() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Status_InvNum: {
      /*
       * The return status of rtems_timer_server_fire_after() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Status_IncStat: {
      /*
       * The return status of rtems_timer_server_fire_after() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Status_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Post_Context_Check(
  RtemsTimerReqServerFireAfter_Context     *ctx,
  RtemsTimerReqServerFireAfter_Post_Context state
)
{
  Timer_Classes class;
  class = GetTimerClass( ctx->timer_id );

  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Post_Context_None: {
      /*
       * The timer shall have never been scheduled. See also none.
       */
      T_eq_int( class, TIMER_DORMANT );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Context_Interrupt: {
      /*
       * The timer shall be in interrupt context.
       */
      T_eq_int( class & TIMER_CLASS_BIT_ON_TASK, 0 );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Context_Server: {
      /*
       * The timer shall be in server context.
       */
      T_eq_int( class & TIMER_CLASS_BIT_ON_TASK, TIMER_CLASS_BIT_ON_TASK );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Context_Nop: {
      /*
       * Objects referenced by the parameters in the past call to
       * rtems_timer_server_fire_after() shall not be accessed by the
       * rtems_timer_server_fire_after() call. See also Nop.
       */
      T_eq_int( class, ctx->pre_class );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Context_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Post_Clock_Check(
  RtemsTimerReqServerFireAfter_Context   *ctx,
  RtemsTimerReqServerFireAfter_Post_Clock state
)
{
  Timer_Classes class;
  class = GetTimerClass( ctx->timer_id );

  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Post_Clock_None: {
      /*
       * The timer shall have never been scheduled.
       */
      T_eq_int( class, TIMER_DORMANT );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Clock_Ticks: {
      /*
       * The timer shall use the ticks based clock.
       */
      T_eq_int( class & TIMER_CLASS_BIT_TIME_OF_DAY, 0 );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Clock_Realtime: {
      /*
       * The timer shall use the realtime clock.
       */
      T_eq_int(
        class & TIMER_CLASS_BIT_TIME_OF_DAY,
        TIMER_CLASS_BIT_TIME_OF_DAY
      );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Clock_Nop: {
      /*
       * Objects referenced by the parameters in the past call to
       * rtems_timer_server_fire_after() shall not be accessed by the
       * rtems_timer_server_fire_after() call.
       */
      T_eq_int( class, ctx->pre_class );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Clock_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Post_State_Check(
  RtemsTimerReqServerFireAfter_Context   *ctx,
  RtemsTimerReqServerFireAfter_Post_State state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Post_State_Scheduled: {
      /*
       * The timer shall be in scheduled state.
       */
      ctx->ticks_till_fire = TriggerTimer( ctx );
      T_eq_int( ctx->invocations, 1 );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_State_Nop: {
      /*
       * Objects referenced by the parameters in the past call to
       * rtems_timer_server_fire_after() shall not be accessed by the
       * rtems_timer_server_fire_after() call.
       */
      T_eq_int( ctx->post_state, ctx->pre_state );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_State_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Post_Interval_Check(
  RtemsTimerReqServerFireAfter_Context      *ctx,
  RtemsTimerReqServerFireAfter_Post_Interval state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Post_Interval_Param: {
      /*
       * The Timer Service Routine shall be invoked the number of ticks (see
       * tick), which are provided by the ``ticks`` parameter in the past call
       * to rtems_timer_server_fire_after(), after a point in time during the
       * execution of the rtems_timer_server_fire_after() call.
       */
      T_eq_int( ctx->ticks_till_fire, ctx->ticks_param );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Interval_Nop: {
      /*
       * If and when the Timer Service Routine will be invoked shall not be
       * changed by the past call to rtems_timer_server_fire_after().
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

    case RtemsTimerReqServerFireAfter_Post_Interval_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Post_Routine_Check(
  RtemsTimerReqServerFireAfter_Context     *ctx,
  RtemsTimerReqServerFireAfter_Post_Routine state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Post_Routine_Param: {
      /*
       * The function reference used to invoke the Timer Service Routine when
       * the timer will fire shall be the one provided by the ``routine``
       * parameter in the past call to rtems_timer_server_fire_after().
       */
      T_eq_int( ctx->invocations, 1 );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Routine_Nop: {
      /*
       * The function reference used for any invocation of the Timer Service
       * Routine shall not be changed by the past call to
       * rtems_timer_server_fire_after().
       */
      T_eq_ptr(
        ctx->post_scheduling_data.routine,
        ctx->pre_scheduling_data.routine
      );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_Routine_NA:
      break;
  }
}

static void RtemsTimerReqServerFireAfter_Post_UserData_Check(
  RtemsTimerReqServerFireAfter_Context      *ctx,
  RtemsTimerReqServerFireAfter_Post_UserData state
)
{
  switch ( state ) {
    case RtemsTimerReqServerFireAfter_Post_UserData_Param: {
      /*
       * The user data argument for invoking the Timer Service Routine when the
       * timer will fire shall be the one provided by the ``user_data``
       * parameter in the past call to rtems_timer_server_fire_after().
       */
      T_eq_ptr( ctx->routine_user_data, ctx );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_UserData_Nop: {
      /*
       * The user data argument used for any invocation of the Timer Service
       * Routine shall not be changed by the past call to
       * rtems_timer_server_fire_after().
       */
      T_eq_ptr(
        ctx->post_scheduling_data.user_data,
        ctx->pre_scheduling_data.user_data
      );
      break;
    }

    case RtemsTimerReqServerFireAfter_Post_UserData_NA:
      break;
  }
}

/**
 * @brief Make sure the realtime clock is not set after this test.
 */
static void RtemsTimerReqServerFireAfter_Teardown(
  RtemsTimerReqServerFireAfter_Context *ctx
)
{
  UnsetClock();
}

static void RtemsTimerReqServerFireAfter_Teardown_Wrap( void *arg )
{
  RtemsTimerReqServerFireAfter_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqServerFireAfter_Teardown( ctx );
}

static void RtemsTimerReqServerFireAfter_Prepare(
  RtemsTimerReqServerFireAfter_Context *ctx
)
{
  rtems_status_code status;
  status = rtems_timer_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    &ctx->timer_id
  );
  T_rsc_success( status );

  status = rtems_timer_create(
    rtems_build_name( 'C', 'O', 'V', 'R' ),
    &ctx->timer_cover_id
  );
  T_rsc_success( status );

  ctx->invocations               = 0;
  ctx->ticks_till_fire           = SCHEDULE_NONE;
  ctx->routine_user_data         = NULL;
  T_rsc_success( rtems_clock_set( &tod_now ) );
}

static void RtemsTimerReqServerFireAfter_Action(
  RtemsTimerReqServerFireAfter_Context *ctx
)
{
  GetTimerSchedulingData( ctx->timer_id, &ctx->pre_scheduling_data );
  ctx->pre_class = GetTimerClass( ctx->timer_id );
  ctx->status = rtems_timer_server_fire_after(
    ctx->id_param,
    ctx->ticks_param,
    ctx->routine_param,
    ctx
  );
  ctx->post_state = GetTimerState( ctx->timer_id );
  GetTimerSchedulingData( ctx->timer_id, &ctx->post_scheduling_data );
  /* Ignoring return status: the timer server task may be suspended or not. */
  rtems_task_resume( GetTimerServerTaskId() );
}

static void RtemsTimerReqServerFireAfter_Cleanup(
  RtemsTimerReqServerFireAfter_Context *ctx
)
{
  T_rsc_success( rtems_timer_delete( ctx->timer_cover_id ) );
  T_rsc_success( rtems_timer_delete( ctx->timer_id ) );
  DeleteTimerServer();
}

static const RtemsTimerReqServerFireAfter_Entry
RtemsTimerReqServerFireAfter_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqServerFireAfter_Post_Status_NA,
    RtemsTimerReqServerFireAfter_Post_Context_NA,
    RtemsTimerReqServerFireAfter_Post_Clock_NA,
    RtemsTimerReqServerFireAfter_Post_State_NA,
    RtemsTimerReqServerFireAfter_Post_Interval_NA,
    RtemsTimerReqServerFireAfter_Post_Routine_NA,
    RtemsTimerReqServerFireAfter_Post_UserData_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqServerFireAfter_Post_Status_NA,
    RtemsTimerReqServerFireAfter_Post_Context_NA,
    RtemsTimerReqServerFireAfter_Post_Clock_NA,
    RtemsTimerReqServerFireAfter_Post_State_NA,
    RtemsTimerReqServerFireAfter_Post_Interval_NA,
    RtemsTimerReqServerFireAfter_Post_Routine_NA,
    RtemsTimerReqServerFireAfter_Post_UserData_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqServerFireAfter_Post_Status_InvNum,
    RtemsTimerReqServerFireAfter_Post_Context_Nop,
    RtemsTimerReqServerFireAfter_Post_Clock_Nop,
    RtemsTimerReqServerFireAfter_Post_State_Nop,
    RtemsTimerReqServerFireAfter_Post_Interval_Nop,
    RtemsTimerReqServerFireAfter_Post_Routine_Nop,
    RtemsTimerReqServerFireAfter_Post_UserData_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqServerFireAfter_Post_Status_IncStat,
    RtemsTimerReqServerFireAfter_Post_Context_Nop,
    RtemsTimerReqServerFireAfter_Post_Clock_Nop,
    RtemsTimerReqServerFireAfter_Post_State_Nop,
    RtemsTimerReqServerFireAfter_Post_Interval_Nop,
    RtemsTimerReqServerFireAfter_Post_Routine_Nop,
    RtemsTimerReqServerFireAfter_Post_UserData_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqServerFireAfter_Post_Status_InvAddr,
    RtemsTimerReqServerFireAfter_Post_Context_Nop,
    RtemsTimerReqServerFireAfter_Post_Clock_Nop,
    RtemsTimerReqServerFireAfter_Post_State_Nop,
    RtemsTimerReqServerFireAfter_Post_Interval_Nop,
    RtemsTimerReqServerFireAfter_Post_Routine_Nop,
    RtemsTimerReqServerFireAfter_Post_UserData_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqServerFireAfter_Post_Status_Ok,
    RtemsTimerReqServerFireAfter_Post_Context_Server,
    RtemsTimerReqServerFireAfter_Post_Clock_Ticks,
    RtemsTimerReqServerFireAfter_Post_State_Scheduled,
    RtemsTimerReqServerFireAfter_Post_Interval_Param,
    RtemsTimerReqServerFireAfter_Post_Routine_Param,
    RtemsTimerReqServerFireAfter_Post_UserData_Param },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsTimerReqServerFireAfter_Post_Status_InvId,
    RtemsTimerReqServerFireAfter_Post_Context_Nop,
    RtemsTimerReqServerFireAfter_Post_Clock_Nop,
    RtemsTimerReqServerFireAfter_Post_State_Nop,
    RtemsTimerReqServerFireAfter_Post_Interval_Nop,
    RtemsTimerReqServerFireAfter_Post_Routine_Nop,
    RtemsTimerReqServerFireAfter_Post_UserData_Nop }
};

static const uint8_t
RtemsTimerReqServerFireAfter_Map[] = {
  5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 5,
  5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 6, 6, 0, 0, 0, 0, 6, 6, 6, 6,
  6, 6, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 4, 4, 0, 0, 0, 0, 4, 4, 4,
  4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 4, 4, 0, 0, 0, 0, 4, 4,
  4, 4, 4, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 2,
  2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0,
  2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0,
  0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0,
  0, 0, 2, 2, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 3, 3, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 3, 3,
  0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 3,
  3, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0,
  3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3,
  0, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
  3, 0, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  3, 3, 0, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 3, 3, 0, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
};

static size_t RtemsTimerReqServerFireAfter_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsTimerReqServerFireAfter_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsTimerReqServerFireAfter_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsTimerReqServerFireAfter_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = RtemsTimerReqServerFireAfter_Teardown_Wrap,
  .scope = RtemsTimerReqServerFireAfter_Scope,
  .initial_context = &RtemsTimerReqServerFireAfter_Instance
};

static inline RtemsTimerReqServerFireAfter_Entry
RtemsTimerReqServerFireAfter_PopEntry(
  RtemsTimerReqServerFireAfter_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTimerReqServerFireAfter_Entries[
    RtemsTimerReqServerFireAfter_Map[ index ]
  ];
}

static void RtemsTimerReqServerFireAfter_TestVariant(
  RtemsTimerReqServerFireAfter_Context *ctx
)
{
  RtemsTimerReqServerFireAfter_Pre_Server_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTimerReqServerFireAfter_Pre_Ticks_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTimerReqServerFireAfter_Pre_Routine_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTimerReqServerFireAfter_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTimerReqServerFireAfter_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTimerReqServerFireAfter_Pre_Clock_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsTimerReqServerFireAfter_Pre_State_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsTimerReqServerFireAfter_Action( ctx );
  RtemsTimerReqServerFireAfter_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsTimerReqServerFireAfter_Post_Context_Check(
    ctx,
    ctx->Map.entry.Post_Context
  );
  RtemsTimerReqServerFireAfter_Post_Clock_Check(
    ctx,
    ctx->Map.entry.Post_Clock
  );
  RtemsTimerReqServerFireAfter_Post_State_Check(
    ctx,
    ctx->Map.entry.Post_State
  );
  RtemsTimerReqServerFireAfter_Post_Interval_Check(
    ctx,
    ctx->Map.entry.Post_Interval
  );
  RtemsTimerReqServerFireAfter_Post_Routine_Check(
    ctx,
    ctx->Map.entry.Post_Routine
  );
  RtemsTimerReqServerFireAfter_Post_UserData_Check(
    ctx,
    ctx->Map.entry.Post_UserData
  );
}

/**
 * @fn void T_case_body_RtemsTimerReqServerFireAfter( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTimerReqServerFireAfter,
  &RtemsTimerReqServerFireAfter_Fixture
)
{
  RtemsTimerReqServerFireAfter_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTimerReqServerFireAfter_Pre_Server_Init;
    ctx->Map.pcs[ 0 ] < RtemsTimerReqServerFireAfter_Pre_Server_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTimerReqServerFireAfter_Pre_Ticks_Valid;
      ctx->Map.pcs[ 1 ] < RtemsTimerReqServerFireAfter_Pre_Ticks_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTimerReqServerFireAfter_Pre_Routine_Valid;
        ctx->Map.pcs[ 2 ] < RtemsTimerReqServerFireAfter_Pre_Routine_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsTimerReqServerFireAfter_Pre_Id_Valid;
          ctx->Map.pcs[ 3 ] < RtemsTimerReqServerFireAfter_Pre_Id_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsTimerReqServerFireAfter_Pre_Context_None;
            ctx->Map.pcs[ 4 ] < RtemsTimerReqServerFireAfter_Pre_Context_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsTimerReqServerFireAfter_Pre_Clock_None;
              ctx->Map.pcs[ 5 ] < RtemsTimerReqServerFireAfter_Pre_Clock_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = RtemsTimerReqServerFireAfter_Pre_State_Inactive;
                ctx->Map.pcs[ 6 ] < RtemsTimerReqServerFireAfter_Pre_State_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                ctx->Map.entry = RtemsTimerReqServerFireAfter_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                RtemsTimerReqServerFireAfter_Prepare( ctx );
                RtemsTimerReqServerFireAfter_TestVariant( ctx );
                RtemsTimerReqServerFireAfter_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
