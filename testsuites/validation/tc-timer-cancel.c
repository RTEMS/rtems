/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerReqCancel
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
 * @defgroup RtemsTimerReqCancel spec:/rtems/timer/req/cancel
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTimerReqCancel_Pre_Id_Valid,
  RtemsTimerReqCancel_Pre_Id_Invalid,
  RtemsTimerReqCancel_Pre_Id_NA
} RtemsTimerReqCancel_Pre_Id;

typedef enum {
  RtemsTimerReqCancel_Pre_Context_None,
  RtemsTimerReqCancel_Pre_Context_Interrupt,
  RtemsTimerReqCancel_Pre_Context_Server,
  RtemsTimerReqCancel_Pre_Context_NA
} RtemsTimerReqCancel_Pre_Context;

typedef enum {
  RtemsTimerReqCancel_Pre_Clock_None,
  RtemsTimerReqCancel_Pre_Clock_Ticks,
  RtemsTimerReqCancel_Pre_Clock_Realtime,
  RtemsTimerReqCancel_Pre_Clock_NA
} RtemsTimerReqCancel_Pre_Clock;

typedef enum {
  RtemsTimerReqCancel_Pre_State_Inactive,
  RtemsTimerReqCancel_Pre_State_Scheduled,
  RtemsTimerReqCancel_Pre_State_Pending,
  RtemsTimerReqCancel_Pre_State_NA
} RtemsTimerReqCancel_Pre_State;

typedef enum {
  RtemsTimerReqCancel_Post_Status_Ok,
  RtemsTimerReqCancel_Post_Status_InvId,
  RtemsTimerReqCancel_Post_Status_NA
} RtemsTimerReqCancel_Post_Status;

typedef enum {
  RtemsTimerReqCancel_Post_Context_None,
  RtemsTimerReqCancel_Post_Context_Interrupt,
  RtemsTimerReqCancel_Post_Context_Server,
  RtemsTimerReqCancel_Post_Context_Nop,
  RtemsTimerReqCancel_Post_Context_NA
} RtemsTimerReqCancel_Post_Context;

typedef enum {
  RtemsTimerReqCancel_Post_Clock_None,
  RtemsTimerReqCancel_Post_Clock_Ticks,
  RtemsTimerReqCancel_Post_Clock_Realtime,
  RtemsTimerReqCancel_Post_Clock_Nop,
  RtemsTimerReqCancel_Post_Clock_NA
} RtemsTimerReqCancel_Post_Clock;

typedef enum {
  RtemsTimerReqCancel_Post_State_Inactive,
  RtemsTimerReqCancel_Post_State_Nop,
  RtemsTimerReqCancel_Post_State_NA
} RtemsTimerReqCancel_Post_State;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Context_NA : 1;
  uint16_t Pre_Clock_NA : 1;
  uint16_t Pre_State_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Context : 3;
  uint16_t Post_Clock : 3;
  uint16_t Post_State : 2;
} RtemsTimerReqCancel_Entry;

typedef enum {
  PRE_NONE = 0,
  PRE_INTERRUPT = 1,
  PRE_SERVER = 2
} PreConditionContext;

/**
 * @brief Test context for spec:/rtems/timer/req/cancel test case.
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
   * @brief This member contains the return status of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains the counter for invocations of the Timer
   *   Service Routine.
   */
  int invocations;

  /**
   * @brief This member specifies which pre-condition context (none, interrupt
   *   context, server context) must be created before the cancel action gets
   *   executed.
   */
  PreConditionContext pre_cond_contex;

  /**
   * @brief This member stores RTEMS internal clock and context settings of the
   *   timer before the execution of the test action.
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
    RtemsTimerReqCancel_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTimerReqCancel_Context;

static RtemsTimerReqCancel_Context
  RtemsTimerReqCancel_Instance;

static const char * const RtemsTimerReqCancel_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsTimerReqCancel_PreDesc_Context[] = {
  "None",
  "Interrupt",
  "Server",
  "NA"
};

static const char * const RtemsTimerReqCancel_PreDesc_Clock[] = {
  "None",
  "Ticks",
  "Realtime",
  "NA"
};

static const char * const RtemsTimerReqCancel_PreDesc_State[] = {
  "Inactive",
  "Scheduled",
  "Pending",
  "NA"
};

static const char * const * const RtemsTimerReqCancel_PreDesc[] = {
  RtemsTimerReqCancel_PreDesc_Id,
  RtemsTimerReqCancel_PreDesc_Context,
  RtemsTimerReqCancel_PreDesc_Clock,
  RtemsTimerReqCancel_PreDesc_State,
  NULL
};

static const rtems_time_of_day tod_now      = { 2000, 1, 1, 0, 0, 0, 0 };
static const rtems_time_of_day tod_schedule = { 2000, 1, 1, 1, 0, 0, 0 };
static const rtems_time_of_day tod_fire     = { 2000, 1, 2, 0, 0, 0, 0 };

static void TriggerTimer( void )
{
  /* Fire the timer service routine for ticks and realtime clock */
  int i;
  for ( i = 0; i < 5; i++ ) {
    ClockTick();
  }
  T_rsc_success( rtems_clock_set( &tod_fire ) );
}

static void TimerServiceRoutine(
  rtems_id timer_id,
  void *user_data
)
{
  RtemsTimerReqCancel_Context *ctx = user_data;
  ++( ctx->invocations );
}

static void RtemsTimerReqCancel_Pre_Id_Prepare(
  RtemsTimerReqCancel_Context *ctx,
  RtemsTimerReqCancel_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTimerReqCancel_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->timer_id;
      break;
    }

    case RtemsTimerReqCancel_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsTimerReqCancel_Pre_Id_NA:
      break;
  }
}

static void RtemsTimerReqCancel_Pre_Context_Prepare(
  RtemsTimerReqCancel_Context    *ctx,
  RtemsTimerReqCancel_Pre_Context state
)
{
  switch ( state ) {
    case RtemsTimerReqCancel_Pre_Context_None: {
      /*
       * While the Timer Service Routine has never been scheduled since
       * creation of the timer. See also none.
       */
      ctx->pre_cond_contex = PRE_NONE;
      break;
    }

    case RtemsTimerReqCancel_Pre_Context_Interrupt: {
      /*
       * While the timer is in interrupt context.
       */
      ctx->pre_cond_contex = PRE_INTERRUPT;
      break;
    }

    case RtemsTimerReqCancel_Pre_Context_Server: {
      /*
       * While the timer is in server context.
       */
      ctx->pre_cond_contex = PRE_SERVER;
      break;
    }

    case RtemsTimerReqCancel_Pre_Context_NA:
      break;
  }
}

static void RtemsTimerReqCancel_Pre_Clock_Prepare(
  RtemsTimerReqCancel_Context  *ctx,
  RtemsTimerReqCancel_Pre_Clock state
)
{
  switch ( state ) {
    case RtemsTimerReqCancel_Pre_Clock_None: {
      /*
       * While the timer has never been scheduled since creation of the timer.
       */
      T_eq_int( ctx->pre_cond_contex, PRE_NONE );
      break;
    }

    case RtemsTimerReqCancel_Pre_Clock_Ticks: {
      /*
       * While the clock used to determine when the timer will fire is the
       * ticks based clock.
       */
      rtems_status_code status;

      if ( ctx->pre_cond_contex == PRE_INTERRUPT ) {
        status = rtems_timer_fire_after(
          ctx->timer_id,
          1,
          TimerServiceRoutine,
          ctx
        );
      } else {
         status = rtems_timer_server_fire_after(
          ctx->timer_id,
          1,
          TimerServiceRoutine,
          ctx
        );
      }
      T_rsc_success( status );
      break;
    }

    case RtemsTimerReqCancel_Pre_Clock_Realtime: {
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

    case RtemsTimerReqCancel_Pre_Clock_NA:
      break;
  }
}

static void RtemsTimerReqCancel_Pre_State_Prepare(
  RtemsTimerReqCancel_Context  *ctx,
  RtemsTimerReqCancel_Pre_State state
)
{
  switch ( state ) {
    case RtemsTimerReqCancel_Pre_State_Inactive: {
      /*
       * While the timer is in inactive state.
       */
      TriggerTimer();
      T_eq_int(
        ctx->invocations,
        ( ctx->pre_cond_contex == PRE_NONE ) ? 0 : 1
      );
      ctx->invocations = 0;
      ctx->pre_state = TIMER_INACTIVE;
      break;
    }

    case RtemsTimerReqCancel_Pre_State_Scheduled: {
      /*
       * While the timer is in scheduled state.
       */
      /* The timer was already scheduled in the "Clock" pre-conditions. */
      ctx->pre_state = TIMER_SCHEDULED;
      break;
    }

    case RtemsTimerReqCancel_Pre_State_Pending: {
      /*
       * While the timer is in pending state.
       */
      T_rsc_success( rtems_task_suspend( GetTimerServerTaskId() ) );
      TriggerTimer();
      T_eq_int( ctx->invocations, 0 );
      ctx->pre_state = TIMER_PENDING;
      break;
    }

    case RtemsTimerReqCancel_Pre_State_NA:
      break;
  }
}

static void RtemsTimerReqCancel_Post_Status_Check(
  RtemsTimerReqCancel_Context    *ctx,
  RtemsTimerReqCancel_Post_Status state
)
{
  switch ( state ) {
    case RtemsTimerReqCancel_Post_Status_Ok: {
      /*
       * The return status of rtems_timer_cancel() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTimerReqCancel_Post_Status_InvId: {
      /*
       * The return status of rtems_timer_cancel() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTimerReqCancel_Post_Status_NA:
      break;
  }
}

static void RtemsTimerReqCancel_Post_Context_Check(
  RtemsTimerReqCancel_Context     *ctx,
  RtemsTimerReqCancel_Post_Context state
)
{
  Timer_Classes class;
  class = GetTimerClass( ctx->timer_id );

  switch ( state ) {
    case RtemsTimerReqCancel_Post_Context_None: {
      /*
       * The timer shall have never been scheduled. See also none.
       */
      T_eq_int( class, TIMER_DORMANT );
      break;
    }

    case RtemsTimerReqCancel_Post_Context_Interrupt: {
      /*
       * The timer shall be in interrupt context.
       */
      T_eq_int( class & TIMER_CLASS_BIT_ON_TASK, 0 );
      break;
    }

    case RtemsTimerReqCancel_Post_Context_Server: {
      /*
       * The timer shall be in server context.
       */
      T_eq_int( class & TIMER_CLASS_BIT_ON_TASK, TIMER_CLASS_BIT_ON_TASK );
      break;
    }

    case RtemsTimerReqCancel_Post_Context_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past call to
       * rtems_timer_cancel() shall not be accessed by the rtems_timer_cancel()
       * call. See also Nop.
       */
      T_eq_int( class, ctx->pre_class );
      break;
    }

    case RtemsTimerReqCancel_Post_Context_NA:
      break;
  }
}

static void RtemsTimerReqCancel_Post_Clock_Check(
  RtemsTimerReqCancel_Context   *ctx,
  RtemsTimerReqCancel_Post_Clock state
)
{
  Timer_Classes class;
  class = GetTimerClass( ctx->timer_id );

  switch ( state ) {
    case RtemsTimerReqCancel_Post_Clock_None: {
      /*
       * The timer shall have never been scheduled.
       */
      T_eq_int( class, TIMER_DORMANT );
      break;
    }

    case RtemsTimerReqCancel_Post_Clock_Ticks: {
      /*
       * The timer shall use the ticks based clock.
       */
      T_eq_int( class & TIMER_CLASS_BIT_TIME_OF_DAY, 0 );
      break;
    }

    case RtemsTimerReqCancel_Post_Clock_Realtime: {
      /*
       * The timer shall use the realtime clock.
       */
      T_eq_int(
        class & TIMER_CLASS_BIT_TIME_OF_DAY,
        TIMER_CLASS_BIT_TIME_OF_DAY
      );
      break;
    }

    case RtemsTimerReqCancel_Post_Clock_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past call to
       * rtems_timer_cancel() shall not be accessed by the rtems_timer_cancel()
       * call.
       */
      T_eq_int( class, ctx->pre_class );
      break;
    }

    case RtemsTimerReqCancel_Post_Clock_NA:
      break;
  }
}

static void RtemsTimerReqCancel_Post_State_Check(
  RtemsTimerReqCancel_Context   *ctx,
  RtemsTimerReqCancel_Post_State state
)
{
  switch ( state ) {
    case RtemsTimerReqCancel_Post_State_Inactive: {
      /*
       * The timer shall be in inactive state.
       */
      /* Try to fire the timer service routine - which should not fire. */
      TriggerTimer();
      T_eq_int( ctx->invocations, 0 );
      break;
    }

    case RtemsTimerReqCancel_Post_State_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past call to
       * rtems_timer_cancel() shall not be accessed by the rtems_timer_cancel()
       * call.
       */
      T_eq_int( ctx->post_state, ctx->pre_state );
      break;
    }

    case RtemsTimerReqCancel_Post_State_NA:
      break;
  }
}

static void RtemsTimerReqCancel_Setup( RtemsTimerReqCancel_Context *ctx )
{
  rtems_status_code status;
  status = rtems_timer_initiate_server(
    RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  T_rsc_success( status );
}

static void RtemsTimerReqCancel_Setup_Wrap( void *arg )
{
  RtemsTimerReqCancel_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqCancel_Setup( ctx );
}

/**
 * @brief Make sure the timer server is not running and the realtime clock is
 *   not set after this test.
 */
static void RtemsTimerReqCancel_Teardown( RtemsTimerReqCancel_Context *ctx )
{
  DeleteTimerServer();
  UnsetClock();
}

static void RtemsTimerReqCancel_Teardown_Wrap( void *arg )
{
  RtemsTimerReqCancel_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqCancel_Teardown( ctx );
}

static void RtemsTimerReqCancel_Prepare( RtemsTimerReqCancel_Context *ctx )
{
  rtems_status_code status;
  status = rtems_timer_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    &ctx->timer_id
  );
  T_rsc_success( status );

  ctx->invocations = 0;
  T_rsc_success( rtems_clock_set( &tod_now ) );
}

static void RtemsTimerReqCancel_Action( RtemsTimerReqCancel_Context *ctx )
{
  ctx->pre_class = GetTimerClass( ctx->timer_id );
  ctx->status = rtems_timer_cancel( ctx->id_param );
  ctx->post_state = GetTimerState( ctx->timer_id );
  /* Ignoring return status: the timer server task may be suspended or not. */
  rtems_task_resume( GetTimerServerTaskId() );
}

static void RtemsTimerReqCancel_Cleanup( RtemsTimerReqCancel_Context *ctx )
{
  T_rsc_success( rtems_timer_delete( ctx->timer_id ) );
}

static const RtemsTimerReqCancel_Entry
RtemsTimerReqCancel_Entries[] = {
  { 1, 0, 0, 0, 0, RtemsTimerReqCancel_Post_Status_NA,
    RtemsTimerReqCancel_Post_Context_NA, RtemsTimerReqCancel_Post_Clock_NA,
    RtemsTimerReqCancel_Post_State_NA },
  { 0, 0, 0, 0, 0, RtemsTimerReqCancel_Post_Status_InvId,
    RtemsTimerReqCancel_Post_Context_Nop, RtemsTimerReqCancel_Post_Clock_Nop,
    RtemsTimerReqCancel_Post_State_Nop },
  { 0, 0, 0, 0, 0, RtemsTimerReqCancel_Post_Status_Ok,
    RtemsTimerReqCancel_Post_Context_Server,
    RtemsTimerReqCancel_Post_Clock_Ticks,
    RtemsTimerReqCancel_Post_State_Inactive },
  { 0, 0, 0, 0, 0, RtemsTimerReqCancel_Post_Status_Ok,
    RtemsTimerReqCancel_Post_Context_Server,
    RtemsTimerReqCancel_Post_Clock_Realtime,
    RtemsTimerReqCancel_Post_State_Inactive },
  { 0, 0, 0, 0, 0, RtemsTimerReqCancel_Post_Status_Ok,
    RtemsTimerReqCancel_Post_Context_Interrupt,
    RtemsTimerReqCancel_Post_Clock_Ticks,
    RtemsTimerReqCancel_Post_State_Inactive },
  { 0, 0, 0, 0, 0, RtemsTimerReqCancel_Post_Status_Ok,
    RtemsTimerReqCancel_Post_Context_Interrupt,
    RtemsTimerReqCancel_Post_Clock_Realtime,
    RtemsTimerReqCancel_Post_State_Inactive },
  { 0, 0, 0, 0, 0, RtemsTimerReqCancel_Post_Status_Ok,
    RtemsTimerReqCancel_Post_Context_None, RtemsTimerReqCancel_Post_Clock_None,
    RtemsTimerReqCancel_Post_State_Inactive }
};

static const uint8_t
RtemsTimerReqCancel_Map[] = {
  6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 5, 5, 0, 0, 0, 0, 2, 2, 2, 3, 3,
  3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1,
  1, 1
};

static size_t RtemsTimerReqCancel_Scope( void *arg, char *buf, size_t n )
{
  RtemsTimerReqCancel_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTimerReqCancel_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTimerReqCancel_Fixture = {
  .setup = RtemsTimerReqCancel_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTimerReqCancel_Teardown_Wrap,
  .scope = RtemsTimerReqCancel_Scope,
  .initial_context = &RtemsTimerReqCancel_Instance
};

static inline RtemsTimerReqCancel_Entry RtemsTimerReqCancel_PopEntry(
  RtemsTimerReqCancel_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTimerReqCancel_Entries[
    RtemsTimerReqCancel_Map[ index ]
  ];
}

static void RtemsTimerReqCancel_TestVariant( RtemsTimerReqCancel_Context *ctx )
{
  RtemsTimerReqCancel_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTimerReqCancel_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTimerReqCancel_Pre_Clock_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTimerReqCancel_Pre_State_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTimerReqCancel_Action( ctx );
  RtemsTimerReqCancel_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTimerReqCancel_Post_Context_Check( ctx, ctx->Map.entry.Post_Context );
  RtemsTimerReqCancel_Post_Clock_Check( ctx, ctx->Map.entry.Post_Clock );
  RtemsTimerReqCancel_Post_State_Check( ctx, ctx->Map.entry.Post_State );
}

/**
 * @fn void T_case_body_RtemsTimerReqCancel( void )
 */
T_TEST_CASE_FIXTURE( RtemsTimerReqCancel, &RtemsTimerReqCancel_Fixture )
{
  RtemsTimerReqCancel_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTimerReqCancel_Pre_Id_Valid;
    ctx->Map.pcs[ 0 ] < RtemsTimerReqCancel_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTimerReqCancel_Pre_Context_None;
      ctx->Map.pcs[ 1 ] < RtemsTimerReqCancel_Pre_Context_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTimerReqCancel_Pre_Clock_None;
        ctx->Map.pcs[ 2 ] < RtemsTimerReqCancel_Pre_Clock_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsTimerReqCancel_Pre_State_Inactive;
          ctx->Map.pcs[ 3 ] < RtemsTimerReqCancel_Pre_State_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = RtemsTimerReqCancel_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsTimerReqCancel_Prepare( ctx );
          RtemsTimerReqCancel_TestVariant( ctx );
          RtemsTimerReqCancel_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
