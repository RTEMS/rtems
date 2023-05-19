/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsClockReqSet
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
#include <rtems/score/todimpl.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsClockReqSet spec:/rtems/clock/req/set
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsClockReqSet_Pre_ToD_Valid,
  RtemsClockReqSet_Pre_ToD_ValidLeap4,
  RtemsClockReqSet_Pre_ToD_ValidLeap400,
  RtemsClockReqSet_Pre_ToD_Youngest,
  RtemsClockReqSet_Pre_ToD_Oldest,
  RtemsClockReqSet_Pre_ToD_TooJung,
  RtemsClockReqSet_Pre_ToD_TooOld,
  RtemsClockReqSet_Pre_ToD_InvMonth0,
  RtemsClockReqSet_Pre_ToD_InvMonth,
  RtemsClockReqSet_Pre_ToD_InvDay0,
  RtemsClockReqSet_Pre_ToD_InvDay,
  RtemsClockReqSet_Pre_ToD_InvHour,
  RtemsClockReqSet_Pre_ToD_InvMinute,
  RtemsClockReqSet_Pre_ToD_InvSecond,
  RtemsClockReqSet_Pre_ToD_InvTicks,
  RtemsClockReqSet_Pre_ToD_InvLeap4,
  RtemsClockReqSet_Pre_ToD_InvLeap100,
  RtemsClockReqSet_Pre_ToD_InvLeap400,
  RtemsClockReqSet_Pre_ToD_AtTimer,
  RtemsClockReqSet_Pre_ToD_BeforeTimer,
  RtemsClockReqSet_Pre_ToD_AfterTimer,
  RtemsClockReqSet_Pre_ToD_Null,
  RtemsClockReqSet_Pre_ToD_NA
} RtemsClockReqSet_Pre_ToD;

typedef enum {
  RtemsClockReqSet_Pre_Hook_None,
  RtemsClockReqSet_Pre_Hook_Ok,
  RtemsClockReqSet_Pre_Hook_NotOk,
  RtemsClockReqSet_Pre_Hook_NA
} RtemsClockReqSet_Pre_Hook;

typedef enum {
  RtemsClockReqSet_Post_Status_Ok,
  RtemsClockReqSet_Post_Status_InvAddr,
  RtemsClockReqSet_Post_Status_InvClk,
  RtemsClockReqSet_Post_Status_Hook,
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

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_ToD_NA : 1;
  uint16_t Pre_Hook_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Clock : 2;
  uint16_t Post_Timer : 2;
} RtemsClockReqSet_Entry;

/**
 * @brief Test context for spec:/rtems/clock/req/set test case.
 */
typedef struct {
  rtems_status_code status;

  bool register_hook;

  Status_Control hook_status;

  rtems_time_of_day *target_tod;

  rtems_time_of_day target_tod_value;

  rtems_time_of_day tod_before;

  rtems_status_code get_tod_before_status;

  rtems_time_of_day tod_after;

  rtems_status_code get_tod_after_status;

  rtems_id timer_id;

  int timer_routine_counter;

  rtems_time_of_day timer_routine_tod;

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
    RtemsClockReqSet_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsClockReqSet_Context;

static RtemsClockReqSet_Context
  RtemsClockReqSet_Instance;

static const char * const RtemsClockReqSet_PreDesc_ToD[] = {
  "Valid",
  "ValidLeap4",
  "ValidLeap400",
  "Youngest",
  "Oldest",
  "TooJung",
  "TooOld",
  "InvMonth0",
  "InvMonth",
  "InvDay0",
  "InvDay",
  "InvHour",
  "InvMinute",
  "InvSecond",
  "InvTicks",
  "InvLeap4",
  "InvLeap100",
  "InvLeap400",
  "AtTimer",
  "BeforeTimer",
  "AfterTimer",
  "Null",
  "NA"
};

static const char * const RtemsClockReqSet_PreDesc_Hook[] = {
  "None",
  "Ok",
  "NotOk",
  "NA"
};

static const char * const * const RtemsClockReqSet_PreDesc[] = {
  RtemsClockReqSet_PreDesc_ToD,
  RtemsClockReqSet_PreDesc_Hook,
  NULL
};

typedef RtemsClockReqSet_Context Context;

static rtems_timer_service_routine _TOD_timer_routine(
  rtems_id   timer_id,
  void      *user_data
)
{
  Context *ctx = user_data;
  rtems_status_code status;
  ++ctx->timer_routine_counter;
  status = rtems_clock_get_tod( &ctx->timer_routine_tod );
  T_rsc_success( status );
}

static void _TOD_prepare_timer( Context *ctx )
{
  rtems_status_code status;
  rtems_time_of_day tod = { 1988, 1, 1, 0, 0, 0, 0 };

  status = rtems_clock_set( &tod );
  T_rsc_success( status );

  tod.year = 1989;
  status = rtems_timer_fire_when(
    ctx->timer_id,
    &tod,
    _TOD_timer_routine,
    ctx
  );
  T_rsc_success( status );
}

static Status_Control TODHook(
  TOD_Action             action,
  const struct timespec *tod
)
{
  Context *ctx;

  ctx = T_fixture_context();
  T_eq_int( action, TOD_ACTION_SET_CLOCK );
  T_not_null( tod );

  return ctx->hook_status;
}

static void RtemsClockReqSet_Pre_ToD_Prepare(
  RtemsClockReqSet_Context *ctx,
  RtemsClockReqSet_Pre_ToD  state
)
{
  switch ( state ) {
    case RtemsClockReqSet_Pre_ToD_Valid: {
      /*
       * While the ``time_of_day`` parameter references an arbitrary valid date
       * and time between 1988-01-01T00:00:00.000000000Z and
       * 2105-12-31T23:59:59.999999999Z.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2021, 3, 11, 11, 10, 59,
        rtems_clock_get_ticks_per_second() / 2 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_ValidLeap4: {
      /*
       * While the ``time_of_day`` parameter references a date for a leap year
       * with the value of 29th of February.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2096, 2, 29, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_ValidLeap400: {
      /*
       * While the ``time_of_day`` parameter references a date for a leap year
       * with the value of 29th of February.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2000, 2, 29, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_Youngest: {
      /*
       * While the ``time_of_day`` parameter references the youngest date and
       * time accepted (1988-01-01T00:00:00.000000000Z).
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 1988, 1, 1, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_Oldest: {
      /*
       * While the ``time_of_day`` parameter references the oldest date and
       * time accepted (2099-12-31T23:59:59.999999999Z).
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2099, 12, 31, 23, 59, 59,
        rtems_clock_get_ticks_per_second() - 1 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_TooJung: {
      /*
       * While the ``time_of_day`` parameter references a valid date and time
       * younger than 1988-01-01T00:00:00.000000000Z.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 1987, 12, 31, 23, 59, 59,
        rtems_clock_get_ticks_per_second() - 1 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_TooOld: {
      /*
       * While the ``time_of_day`` parameter references a valid date and time
       * older than 2105-12-31T23:59:59.999999999Z.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2106, 1, 1, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvMonth0: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value of
       * the month is 0.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2021, 0, 11, 11, 10, 59, 1 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvMonth: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value of
       * the month is larger than 12.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2021, 13, 11, 11, 10, 59, 1 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvDay0: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value of
       * the day is 0.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2021, 3, 0, 11, 10, 59, 1 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvDay: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value of
       * the day is larger than the days of the month.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2021, 2, 29, 11, 10, 59, 1 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvHour: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value of
       * the hour is larger than 23.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2021, 3, 11, 24, 10, 59, 1 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvMinute: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value of
       * the minute is larger than 59.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2021, 3, 11, 11, 60, 59, 1 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvSecond: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value of
       * the second is larger than 59.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2021, 3, 11, 11, 10, 60, 1 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvTicks: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value of
       * the ticks are larger or equal to the ticks per second.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2021, 3, 11, 11, 10, 60,
        rtems_clock_get_ticks_per_second() };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvLeap4: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value 30th
       * of February does not exist in a leap year.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2104, 2, 30, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvLeap100: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value 29th
       * of February does not exist in a non-leap year.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2100, 2, 29, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_InvLeap400: {
      /*
       * While the ``time_of_day`` parameter is invalid because the value 30th
       * of February does not exist in a leap year.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 2000, 2, 30, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqSet_Pre_ToD_AtTimer: {
      /*
       * While the ``time_of_day`` parameter references the same point in time
       * when a timer should fire.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 1989, 1, 1, 0, 0, 0, 0 };
      _TOD_prepare_timer( ctx );
      break;
    }

    case RtemsClockReqSet_Pre_ToD_BeforeTimer: {
      /*
       * While the ``time_of_day`` parameter references a point in time before
       * a timer should fire.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 1988, 12, 31, 23, 59, 59, 0 };
      _TOD_prepare_timer( ctx );
      break;
    }

    case RtemsClockReqSet_Pre_ToD_AfterTimer: {
      /*
       * While the ``time_of_day`` parameter references a point in time after a
       * timer should fire.
       */
      ctx->target_tod_value =
        (rtems_time_of_day) { 1989, 1, 1, 1, 0, 0, 0 };
      _TOD_prepare_timer( ctx );
      break;
    }

    case RtemsClockReqSet_Pre_ToD_Null: {
      /*
       * WHile the ``time_of_day`` parameter is NULL.
       */
      ctx->target_tod = NULL;
      break;
    }

    case RtemsClockReqSet_Pre_ToD_NA:
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

    case RtemsClockReqSet_Pre_Hook_Ok: {
      /*
       * While all TOD hooks invoked by the rtems_clock_set() call return a
       * status code equal to STATUS_SUCCESSFUL.
       */
      ctx->register_hook = true;
      ctx->hook_status = STATUS_SUCCESSFUL;
      break;
    }

    case RtemsClockReqSet_Pre_Hook_NotOk: {
      /*
       * While at least one TOD hook invoked by the rtems_clock_set() call
       * returns a status code not equal to STATUS_SUCCESSFUL.
       */
      ctx->register_hook = true;
      ctx->hook_status = STATUS_UNAVAILABLE;
      break;
    }

    case RtemsClockReqSet_Pre_Hook_NA:
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
       * The return status of rtems_clock_set() shall be RTEMS_SUCCESSFUL
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

    case RtemsClockReqSet_Post_Status_InvClk: {
      /*
       * The return status of rtems_clock_set() shall be RTEMS_INVALID_CLOCK.
       */
      T_rsc( ctx->status, RTEMS_INVALID_CLOCK );
      break;
    }

    case RtemsClockReqSet_Post_Status_Hook: {
      /*
       * The return status of rtems_clock_set() shall be derived from the
       * status returned by the TOD hook.
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
       * The CLOCK_REALTIME shall be set to the values of the object referenced
       * by the ``time_of_day`` parameter during the rtems_clock_set() call.
       */
      T_eq_ptr( ctx->target_tod, &ctx->target_tod_value );
      T_rsc_success( ctx->get_tod_after_status );
      T_eq_u32( ctx->tod_after.year, ctx->target_tod_value.year );
      T_eq_u32( ctx->tod_after.month, ctx->target_tod_value.month );
      T_eq_u32( ctx->tod_after.day, ctx->target_tod_value.day );
      T_eq_u32( ctx->tod_after.hour, ctx->target_tod_value.hour );
      T_eq_u32( ctx->tod_after.minute, ctx->target_tod_value.minute );
      T_eq_u32( ctx->tod_after.second, ctx->target_tod_value.second );
      /* rtems_clock_set() or rtems_clock_get_tod() cause an error of 1 tick */
      T_ge_u32( ctx->tod_after.ticks + 1, ctx->target_tod_value.ticks );
      T_le_u32( ctx->tod_after.ticks, ctx->target_tod_value.ticks );
      break;
    }

    case RtemsClockReqSet_Post_Clock_Nop: {
      /*
       * The state of the CLOCK_REALTIME shall not be changed by the
       * rtems_clock_set() call.
       */
      T_rsc_success( ctx->get_tod_before_status );
      T_eq_u32( ctx->tod_after.year, ctx->tod_before.year );
      T_eq_u32( ctx->tod_after.month, ctx->tod_before.month );
      T_eq_u32( ctx->tod_after.day, ctx->tod_before.day );
      T_eq_u32( ctx->tod_after.hour, ctx->tod_before.hour );
      T_eq_u32( ctx->tod_after.minute, ctx->tod_before.minute );
      T_eq_u32( ctx->tod_after.second, ctx->tod_before.second );
      T_eq_u32( ctx->tod_after.ticks, ctx->tod_before.ticks );
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
       * The timer routine shall be executed once after the CLOCK_REALTIME has
       * been set and before the execution of the rtems_clock_set() call
       * terminates.
       */
      T_eq_int( ctx->timer_routine_counter, 1 );
      T_eq_u32( ctx->timer_routine_tod.year, 1989 );
      T_eq_u32( ctx->timer_routine_tod.month, 1 );
      T_eq_u32( ctx->timer_routine_tod.day, 1 );
      T_eq_u32( ctx->timer_routine_tod.minute, 0 );
      T_eq_u32( ctx->timer_routine_tod.second, 0 );
      T_eq_u32( ctx->timer_routine_tod.ticks, 0 );
      break;
    }

    case RtemsClockReqSet_Post_Timer_Nop: {
      /*
       * The the timer routine shall not be invoked during the
       * rtems_clock_set() call.
       */
      T_eq_int( ctx->timer_routine_counter, 0 );
      break;
    }

    case RtemsClockReqSet_Post_Timer_NA:
      break;
  }
}

static void RtemsClockReqSet_Setup( RtemsClockReqSet_Context *ctx )
{
  rtems_status_code status;
  rtems_name timer_name = rtems_build_name( 'T', 'M', 'R', '0' );
  ctx->timer_id = RTEMS_ID_NONE;

  ctx->target_tod = &ctx->target_tod_value;

  status = rtems_timer_create( timer_name, &ctx->timer_id );
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

  if ( RTEMS_ID_NONE != ctx->timer_id ) {
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
  ctx->timer_routine_tod = (rtems_time_of_day) { 0, 0, 0, 0, 0, 0, 0 };
}

static void RtemsClockReqSet_Action( RtemsClockReqSet_Context *ctx )
{
  TOD_Hook hook = {
    .handler = TODHook
  };

  if ( ctx->register_hook ) {
    _TOD_Hook_Register( &hook );
  }

  ctx->get_tod_before_status = rtems_clock_get_tod( &ctx->tod_before );
  ctx->status = rtems_clock_set( ctx->target_tod );
  ctx->get_tod_after_status = rtems_clock_get_tod( &ctx->tod_after );

  if ( ctx->register_hook ) {
    _TOD_Hook_Unregister( &hook );
  }
}

static const RtemsClockReqSet_Entry
RtemsClockReqSet_Entries[] = {
  { 0, 0, 0, RtemsClockReqSet_Post_Status_InvClk,
    RtemsClockReqSet_Post_Clock_Nop, RtemsClockReqSet_Post_Timer_Nop },
  { 0, 0, 0, RtemsClockReqSet_Post_Status_Ok, RtemsClockReqSet_Post_Clock_Set,
    RtemsClockReqSet_Post_Timer_Nop },
  { 0, 0, 0, RtemsClockReqSet_Post_Status_Hook,
    RtemsClockReqSet_Post_Clock_Nop, RtemsClockReqSet_Post_Timer_Nop },
  { 0, 0, 0, RtemsClockReqSet_Post_Status_Ok, RtemsClockReqSet_Post_Clock_Set,
    RtemsClockReqSet_Post_Timer_Triggered },
  { 0, 0, 0, RtemsClockReqSet_Post_Status_InvAddr,
    RtemsClockReqSet_Post_Clock_Nop, RtemsClockReqSet_Post_Timer_Nop }
};

static const uint8_t
RtemsClockReqSet_Map[] = {
  1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 3, 3, 2, 1, 1, 2, 3, 3, 2, 4, 4, 4
};

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
  return RtemsClockReqSet_Entries[
    RtemsClockReqSet_Map[ index ]
  ];
}

static void RtemsClockReqSet_TestVariant( RtemsClockReqSet_Context *ctx )
{
  RtemsClockReqSet_Pre_ToD_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsClockReqSet_Pre_Hook_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsClockReqSet_Action( ctx );
  RtemsClockReqSet_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsClockReqSet_Post_Clock_Check( ctx, ctx->Map.entry.Post_Clock );
  RtemsClockReqSet_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
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
    ctx->Map.pcs[ 0 ] = RtemsClockReqSet_Pre_ToD_Valid;
    ctx->Map.pcs[ 0 ] < RtemsClockReqSet_Pre_ToD_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsClockReqSet_Pre_Hook_None;
      ctx->Map.pcs[ 1 ] < RtemsClockReqSet_Pre_Hook_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsClockReqSet_PopEntry( ctx );
      RtemsClockReqSet_Prepare( ctx );
      RtemsClockReqSet_TestVariant( ctx );
    }
  }
}

/** @} */
