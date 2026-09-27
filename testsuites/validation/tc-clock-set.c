/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsClockReqSet
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/score/todimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsClockReqSet spec:/rtems/clock/req/set
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsClockReqSet_Pre_Timer_None,
  RtemsClockReqSet_Pre_Timer_Future,
  RtemsClockReqSet_Pre_Timer_Now,
  RtemsClockReqSet_Pre_Timer_Past,
  RtemsClockReqSet_Pre_Timer_NA
} RtemsClockReqSet_Pre_Timer;

typedef enum {
  RtemsClockReqSet_Pre_Hook_None,
  RtemsClockReqSet_Pre_Hook_Success,
  RtemsClockReqSet_Pre_Hook_Failure,
  RtemsClockReqSet_Pre_Hook_NA
} RtemsClockReqSet_Pre_Hook;

typedef enum {
  RtemsClockReqSet_Pre_ToD_Valid,
  RtemsClockReqSet_Pre_ToD_Invalid,
  RtemsClockReqSet_Pre_ToD_Null,
  RtemsClockReqSet_Pre_ToD_NA
} RtemsClockReqSet_Pre_ToD;

typedef enum {
  RtemsClockReqSet_Post_Status_Ok,
  RtemsClockReqSet_Post_Status_InvAddr,
  RtemsClockReqSet_Post_Status_InvClock,
  RtemsClockReqSet_Post_Status_Unsatisfied,
  RtemsClockReqSet_Post_Status_NA
} RtemsClockReqSet_Post_Status;

typedef enum {
  RtemsClockReqSet_Post_Clock_Set,
  RtemsClockReqSet_Post_Clock_Nop,
  RtemsClockReqSet_Post_Clock_NA
} RtemsClockReqSet_Post_Clock;

typedef enum {
  RtemsClockReqSet_Post_Timer_Triggered,
  RtemsClockReqSet_Post_Timer_Nop,
  RtemsClockReqSet_Post_Timer_NA
} RtemsClockReqSet_Post_Timer;

typedef enum {
  RtemsClockReqSet_Post_HookCall_Once,
  RtemsClockReqSet_Post_HookCall_Nop,
  RtemsClockReqSet_Post_HookCall_NA
} RtemsClockReqSet_Post_HookCall;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Timer_NA : 1;
  uint16_t Pre_Hook_NA : 1;
  uint16_t Pre_ToD_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Clock : 2;
  uint16_t Post_Timer : 2;
  uint16_t Post_HookCall : 2;
} RtemsClockReqSet_Entry;

/**
 * @brief Test context for spec:/rtems/clock/req/set test case.
 */
typedef struct {
  /**
   * @brief This member contains the return status of the rtems_clock_set()
   *   call.
   */
  rtems_status_code status;

  /**
   * @brief This member is true, if a TOD hook shall be registered.
   */
  bool register_hook;

  /**
   * @brief This member specifies the status which the TOD hook returns.
   */
  Status_Control hook_status;

  /**
   * @brief This member specifies the `time_of_day` parameter value.
   */
  rtems_time_of_day *target_tod;

  /**
   * @brief This member provides the object referenced by the `time_of_day`
   *   parameter.
   */
  rtems_time_of_day target_tod_value;

  /**
   * @brief This member contains the expected seconds since the Epoch of the
   *   time of day.
   */
  int64_t expected_seconds;

  /**
   * @brief This member contains the CLOCK_REALTIME before the
   *   rtems_clock_set() call.
   */
  struct timespec realtime_before;

  /**
   * @brief This member contains the CLOCK_REALTIME after the rtems_clock_set()
   *   call.
   */
  struct timespec realtime_after;

  /**
   * @brief This member contains the identifier of the timer.
   */
  rtems_id timer_id;

  /**
   * @brief This member counts the executions of the timer routine.
   */
  int timer_routine_counter;

  /**
   * @brief This member contains the seconds of the CLOCK_REALTIME which the
   *   timer routine observed.
   */
  int64_t timer_routine_seconds;

  /**
   * @brief This member counts the calls of the TOD hook.
   */
  int hook_calls;

  /**
   * @brief This member contains the action of the last TOD hook call.
   */
  TOD_Action hook_action;

  /**
   * @brief This member contains the time of day of the last TOD hook call.
   */
  struct timespec hook_tod;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    RtemsClockReqSet_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsClockReqSet_Context;

static RtemsClockReqSet_Context RtemsClockReqSet_Instance;

static const char *const RtemsClockReqSet_PreDesc_Timer[] =
  { "None", "Future", "Now", "Past", "NA" };

static const char *const RtemsClockReqSet_PreDesc_Hook[] =
  { "None", "Success", "Failure", "NA" };

static const char *const RtemsClockReqSet_PreDesc_ToD[] =
  { "Valid", "Invalid", "Null", "NA" };

static const char *const *const RtemsClockReqSet_PreDesc[] = {
  RtemsClockReqSet_PreDesc_Timer,
  RtemsClockReqSet_PreDesc_Hook,
  RtemsClockReqSet_PreDesc_ToD,
  NULL
};

typedef RtemsClockReqSet_Context Context;

static int64_t GetSeconds( const rtems_time_of_day *tod )
{
  return DaysFromCivil( tod->year, tod->month, tod->day ) * 86400 +
         tod->hour * 3600 + tod->minute * 60 + tod->second;
}

static int64_t GetNanoseconds( const struct timespec *ts )
{
  return (int64_t) ts->tv_sec * 1000000000 + ts->tv_nsec;
}

static void CheckClockSet( const Context *ctx )
{
  int64_t expected;
  int64_t progress;

  expected = ctx->expected_seconds * 1000000000 +
             (int64_t) ctx->target_tod_value.ticks *
               rtems_configuration_get_nanoseconds_per_tick();
  progress = GetNanoseconds( &ctx->realtime_after ) - expected;
  T_ge_i64( progress, 0 );
  T_lt_i64( progress, rtems_configuration_get_nanoseconds_per_tick() );
}

static void CheckClockNop( const Context *ctx )
{
  int64_t progress;

  progress = GetNanoseconds( &ctx->realtime_after ) -
             GetNanoseconds( &ctx->realtime_before );
  T_ge_i64( progress, 0 );
  T_lt_i64( progress, rtems_configuration_get_nanoseconds_per_tick() );
}

static void TimerRoutine( rtems_id timer_id, void *user_data )
{
  Context        *ctx;
  struct timespec now;

  (void) timer_id;
  ctx = user_data;
  ++ctx->timer_routine_counter;
  rtems_clock_get_realtime( &now );
  ctx->timer_routine_seconds = now.tv_sec;
}

static void PrepareTimer( Context *ctx )
{
  rtems_status_code status;
  rtems_time_of_day tod = { 1988, 1, 1, 0, 0, 0, 0 };

  status = rtems_clock_set( &tod );
  T_rsc_success( status );

  tod.year = 1989;
  status = rtems_timer_fire_when( ctx->timer_id, &tod, TimerRoutine, ctx );
  T_rsc_success( status );
}

static Status_Control TODHook( TOD_Action action, const struct timespec *tod )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->hook_calls;
  ctx->hook_action = action;
  ctx->hook_tod = *tod;

  return ctx->hook_status;
}

static void RtemsClockReqSet_Pre_Timer_Prepare(
  RtemsClockReqSet_Context  *ctx,
  RtemsClockReqSet_Pre_Timer state
)
{
  switch ( state ) {
    case RtemsClockReqSet_Pre_Timer_None: {
      /*
       * While no timer of the CLOCK_REALTIME is scheduled.
       */
      ctx->target_tod_value = (rtems_time_of_day) {
        2021,
        3,
        11,
        11,
        10,
        59,
        rtems_clock_get_ticks_per_second() / 2
      };
      break;
    }

    case RtemsClockReqSet_Pre_Timer_Future: {
      /*
       * While exactly one timer of the CLOCK_REALTIME is scheduled to fire at
       * a time point after the time of day which the `time_of_day` parameter
       * specifies.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 1988, 12, 31, 23, 59, 59, 0 };
      PrepareTimer( ctx );
      break;
    }

    case RtemsClockReqSet_Pre_Timer_Now: {
      /*
       * While exactly one timer of the CLOCK_REALTIME is scheduled to fire at
       * the time of day which the `time_of_day` parameter specifies.
       */
      ctx->target_tod_value = (rtems_time_of_day) { 1989, 1, 1, 0, 0, 0, 0 };
      PrepareTimer( ctx );
      break;
    }

    case RtemsClockReqSet_Pre_Timer_Past: {
      /*
       * While exactly one timer of the CLOCK_REALTIME is scheduled to fire at
       * a time point before the time of day which the `time_of_day` parameter
       * specifies.
       */
      ctx->target_tod_value = (rtems_time_of_day) { 1989, 1, 1, 1, 0, 0, 0 };
      PrepareTimer( ctx );
      break;
    }

    case RtemsClockReqSet_Pre_Timer_NA:
      break;
  }
}

static void RtemsClockReqSet_Pre_Hook_Prepare(
  RtemsClockReqSet_Context *ctx,
  RtemsClockReqSet_Pre_Hook state
)
{
  switch ( state ) {
    case RtemsClockReqSet_Pre_Hook_None: {
      /*
       * While no TOD hook is registered.
       */
      ctx->register_hook = false;
      break;
    }

    case RtemsClockReqSet_Pre_Hook_Success: {
      /*
       * While exactly one TOD hook is registered, while the hook returns a
       * status code equal to STATUS_SUCCESSFUL.
       */
      ctx->register_hook = true;
      ctx->hook_status = STATUS_SUCCESSFUL;
      break;
    }

    case RtemsClockReqSet_Pre_Hook_Failure: {
      /*
       * While exactly one TOD hook is registered, while the hook returns a
       * status code equal to STATUS_UNAVAILABLE.
       */
      ctx->register_hook = true;
      ctx->hook_status = STATUS_UNAVAILABLE;
      break;
    }

    case RtemsClockReqSet_Pre_Hook_NA:
      break;
  }
}

static void RtemsClockReqSet_Pre_ToD_Prepare(
  RtemsClockReqSet_Context *ctx,
  RtemsClockReqSet_Pre_ToD  state
)
{
  switch ( state ) {
    case RtemsClockReqSet_Pre_ToD_Valid: {
      /*
       * While the `time_of_day` parameter references an object of type
       * rtems_time_of_day, while the object holds a valid time of day.
       */
      ctx->target_tod = &ctx->target_tod_value;
      break;
    }

    case RtemsClockReqSet_Pre_ToD_Invalid: {
      /*
       * While the `time_of_day` parameter references an object of type
       * rtems_time_of_day, while the object holds an invalid time of day.
       */
      ctx->target_tod = &ctx->target_tod_value;
      ctx->target_tod_value.month = 13;
      break;
    }

    case RtemsClockReqSet_Pre_ToD_Null: {
      /*
       * While the `time_of_day` parameter is equal to NULL.
       */
      ctx->target_tod = NULL;
      break;
    }

    case RtemsClockReqSet_Pre_ToD_NA:
      break;
  }
}

static void RtemsClockReqSet_Post_Status_Check(
  RtemsClockReqSet_Context    *ctx,
  RtemsClockReqSet_Post_Status state
)
{
  switch ( state ) {
    case RtemsClockReqSet_Post_Status_Ok: {
      /*
       * The return status of rtems_clock_set() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsClockReqSet_Post_Status_InvAddr: {
      /*
       * The return status of rtems_clock_set() shall be RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsClockReqSet_Post_Status_InvClock: {
      /*
       * The return status of rtems_clock_set() shall be RTEMS_INVALID_CLOCK.
       */
      T_rsc( ctx->status, RTEMS_INVALID_CLOCK );
      break;
    }

    case RtemsClockReqSet_Post_Status_Unsatisfied: {
      /*
       * The return status of rtems_clock_set() shall be RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED );
      break;
    }

    case RtemsClockReqSet_Post_Status_NA:
      break;
  }
}

static void RtemsClockReqSet_Post_Clock_Check(
  RtemsClockReqSet_Context   *ctx,
  RtemsClockReqSet_Post_Clock state
)
{
  switch ( state ) {
    case RtemsClockReqSet_Post_Clock_Set: {
      /*
       * The CLOCK_REALTIME shall be set to the time of day referenced by the
       * `time_of_day` parameter.
       */
      CheckClockSet( ctx );
      break;
    }

    case RtemsClockReqSet_Post_Clock_Nop: {
      /*
       * The CLOCK_REALTIME shall not be changed by the rtems_clock_set() call.
       */
      CheckClockNop( ctx );
      break;
    }

    case RtemsClockReqSet_Post_Clock_NA:
      break;
  }
}

static void RtemsClockReqSet_Post_Timer_Check(
  RtemsClockReqSet_Context   *ctx,
  RtemsClockReqSet_Post_Timer state
)
{
  switch ( state ) {
    case RtemsClockReqSet_Post_Timer_Triggered: {
      /*
       * The timer routine shall be executed once after the CLOCK_REALTIME is
       * set and before the rtems_clock_set() call returns.
       */
      T_eq_int( ctx->timer_routine_counter, 1 );
      T_eq_i64( ctx->timer_routine_seconds, ctx->expected_seconds );
      break;
    }

    case RtemsClockReqSet_Post_Timer_Nop: {
      /*
       * The timer routine shall not be executed during the rtems_clock_set()
       * call.
       */
      T_eq_int( ctx->timer_routine_counter, 0 );
      break;
    }

    case RtemsClockReqSet_Post_Timer_NA:
      break;
  }
}

static void RtemsClockReqSet_Post_HookCall_Check(
  RtemsClockReqSet_Context      *ctx,
  RtemsClockReqSet_Post_HookCall state
)
{
  switch ( state ) {
    case RtemsClockReqSet_Post_HookCall_Once: {
      /*
       * The TOD hook shall be called once with the action to set the clock and
       * the time of day referenced by the `time_of_day` parameter.
       */
      T_eq_int( ctx->hook_calls, 1 );
      T_eq_int( ctx->hook_action, TOD_ACTION_SET_CLOCK );
      T_eq_i64( ctx->hook_tod.tv_sec, ctx->expected_seconds );
      T_eq_long(
        ctx->hook_tod.tv_nsec,
        (long) ( ctx->target_tod_value.ticks *
                 rtems_configuration_get_nanoseconds_per_tick() )
      );
      break;
    }

    case RtemsClockReqSet_Post_HookCall_Nop: {
      /*
       * The TOD hook shall not be called by the rtems_clock_set() call.
       */
      T_eq_int( ctx->hook_calls, 0 );
      break;
    }

    case RtemsClockReqSet_Post_HookCall_NA:
      break;
  }
}

static void RtemsClockReqSet_Setup( RtemsClockReqSet_Context *ctx )
{
  rtems_status_code status;

  ctx->timer_id = RTEMS_ID_NONE;
  status = rtems_timer_create( OBJECT_NAME, &ctx->timer_id );
  T_rsc_success( status );
}

static void RtemsClockReqSet_Setup_Wrap( void *arg )
{
  RtemsClockReqSet_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsClockReqSet_Setup( ctx );
}

static void RtemsClockReqSet_Teardown( RtemsClockReqSet_Context *ctx )
{
  rtems_status_code status;

  if ( ctx->timer_id != RTEMS_ID_NONE ) {
    status = rtems_timer_delete( ctx->timer_id );
    T_rsc_success( status );
  }
}

static void RtemsClockReqSet_Teardown_Wrap( void *arg )
{
  RtemsClockReqSet_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsClockReqSet_Teardown( ctx );
}

static void RtemsClockReqSet_Prepare( RtemsClockReqSet_Context *ctx )
{
  rtems_status_code status;

  status = rtems_timer_cancel( ctx->timer_id );
  T_rsc_success( status );
  ctx->timer_routine_counter = 0;
  ctx->timer_routine_seconds = 0;
  ctx->hook_calls = 0;
}

static void RtemsClockReqSet_Action( RtemsClockReqSet_Context *ctx )
{
  TOD_Hook hook = { .handler = TODHook };

  if ( ctx->register_hook ) {
    _TOD_Hook_Register( &hook );
  }

  ctx->expected_seconds = GetSeconds( &ctx->target_tod_value );
  rtems_clock_get_realtime( &ctx->realtime_before );
  ctx->status = rtems_clock_set( ctx->target_tod );
  rtems_clock_get_realtime( &ctx->realtime_after );

  if ( ctx->register_hook ) {
    _TOD_Hook_Unregister( &hook );
  }
}

/* clang-format off */

static const RtemsClockReqSet_Entry
RtemsClockReqSet_Entries[] = {
  { 1, 0, 0, 0, RtemsClockReqSet_Post_Status_NA,
    RtemsClockReqSet_Post_Clock_NA, RtemsClockReqSet_Post_Timer_NA,
    RtemsClockReqSet_Post_HookCall_NA },
  { 0, 0, 0, 0, RtemsClockReqSet_Post_Status_Unsatisfied,
    RtemsClockReqSet_Post_Clock_Nop, RtemsClockReqSet_Post_Timer_Nop,
    RtemsClockReqSet_Post_HookCall_Once },
  { 0, 0, 0, 0, RtemsClockReqSet_Post_Status_Ok,
    RtemsClockReqSet_Post_Clock_Set, RtemsClockReqSet_Post_Timer_Nop,
    RtemsClockReqSet_Post_HookCall_NA },
  { 0, 0, 0, 0, RtemsClockReqSet_Post_Status_Ok,
    RtemsClockReqSet_Post_Clock_Set, RtemsClockReqSet_Post_Timer_Nop,
    RtemsClockReqSet_Post_HookCall_Once },
  { 0, 0, 0, 0, RtemsClockReqSet_Post_Status_InvClock,
    RtemsClockReqSet_Post_Clock_Nop, RtemsClockReqSet_Post_Timer_Nop,
    RtemsClockReqSet_Post_HookCall_Nop },
  { 0, 0, 0, 0, RtemsClockReqSet_Post_Status_InvAddr,
    RtemsClockReqSet_Post_Clock_Nop, RtemsClockReqSet_Post_Timer_Nop,
    RtemsClockReqSet_Post_HookCall_Nop },
  { 0, 0, 0, 0, RtemsClockReqSet_Post_Status_Ok,
    RtemsClockReqSet_Post_Clock_Set, RtemsClockReqSet_Post_Timer_Triggered,
    RtemsClockReqSet_Post_HookCall_NA },
  { 0, 0, 0, 0, RtemsClockReqSet_Post_Status_Ok,
    RtemsClockReqSet_Post_Clock_Set, RtemsClockReqSet_Post_Timer_Triggered,
    RtemsClockReqSet_Post_HookCall_Once },
  { 0, 0, 0, 0, RtemsClockReqSet_Post_Status_InvClock,
    RtemsClockReqSet_Post_Clock_Nop, RtemsClockReqSet_Post_Timer_Nop,
    RtemsClockReqSet_Post_HookCall_NA },
  { 0, 0, 0, 0, RtemsClockReqSet_Post_Status_InvAddr,
    RtemsClockReqSet_Post_Clock_Nop, RtemsClockReqSet_Post_Timer_Nop,
    RtemsClockReqSet_Post_HookCall_NA }
};

static const uint8_t
RtemsClockReqSet_Map[] = {
  2, 8, 9, 3, 4, 5, 1, 4, 5, 2, 0, 0, 3, 0, 0, 1, 0, 0, 6, 0, 0, 7, 0, 0, 1, 0,
  0, 6, 0, 0, 7, 0, 0, 1, 0, 0
};

/* clang-format on */

static size_t RtemsClockReqSet_Scope( void *arg, char *buf, size_t n )
{
  RtemsClockReqSet_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsClockReqSet_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsClockReqSet_Fixture = {
  .setup = RtemsClockReqSet_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsClockReqSet_Teardown_Wrap,
  .scope = RtemsClockReqSet_Scope,
  .initial_context = &RtemsClockReqSet_Instance
};

static inline RtemsClockReqSet_Entry RtemsClockReqSet_PopEntry(
  RtemsClockReqSet_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsClockReqSet_Entries[ RtemsClockReqSet_Map[ index ] ];
}

static void RtemsClockReqSet_TestVariant( RtemsClockReqSet_Context *ctx )
{
  RtemsClockReqSet_Pre_Timer_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsClockReqSet_Pre_Hook_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsClockReqSet_Pre_ToD_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsClockReqSet_Action( ctx );
  RtemsClockReqSet_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsClockReqSet_Post_Clock_Check( ctx, ctx->Map.entry.Post_Clock );
  RtemsClockReqSet_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  RtemsClockReqSet_Post_HookCall_Check( ctx, ctx->Map.entry.Post_HookCall );
}

/**
 * @fn void T_case_body_RtemsClockReqSet( void )
 */
T_TEST_CASE_FIXTURE( RtemsClockReqSet, &RtemsClockReqSet_Fixture )
{
  RtemsClockReqSet_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsClockReqSet_Pre_Timer_None;
    ctx->Map.pcs[ 0 ] < RtemsClockReqSet_Pre_Timer_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsClockReqSet_Pre_Hook_None;
      ctx->Map.pcs[ 1 ] < RtemsClockReqSet_Pre_Hook_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsClockReqSet_Pre_ToD_Valid;
        ctx->Map.pcs[ 2 ] < RtemsClockReqSet_Pre_ToD_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsClockReqSet_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        RtemsClockReqSet_Prepare( ctx );
        RtemsClockReqSet_TestVariant( ctx );
      }
    }
  }
}

/** @} */
