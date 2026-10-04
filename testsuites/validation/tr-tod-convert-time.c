/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTodReqConvertTime
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include "tr-tod-convert-time.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTodReqConvertTime spec:/score/tod/req/convert-time
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_TicksValidation_NA : 1;
  uint16_t Pre_Hour_NA : 1;
  uint16_t Pre_Minute_NA : 1;
  uint16_t Pre_Second_NA : 1;
  uint16_t Pre_Ticks_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_Effect : 2;
} ScoreTodReqConvertTime_Entry;

typedef enum {
  STATE_FIRST,
  STATE_BETWEEN,
  STATE_LAST,
  STATE_ABOVE,
  STATE_MAX
} TODTimeState;

/**
 * @brief Test context for spec:/score/tod/req/convert-time test case.
 */
typedef struct {
  /**
   * @brief This member specifies the hour state.
   */
  TODTimeState hour;

  /**
   * @brief This member specifies the minute state.
   */
  TODTimeState minute;

  /**
   * @brief This member specifies the second state.
   */
  TODTimeState second;

  /**
   * @brief This member specifies the ticks state.
   */
  TODTimeState ticks;

  /**
   * @brief This member contains the object referenced by the `time_of_day`
   *   parameter.
   */
  rtems_time_of_day tod;

  /**
   * @brief This member contains the expected seconds since the Epoch.
   */
  int64_t expected;

  /**
   * @brief This member contains the seconds since the Epoch which the
   *   directive call used.
   */
  int64_t seconds;

  /**
   * @brief This member is true, if the directive call accepted the time of
   *   day.
   */
  bool accepted;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTodReqConvertTime_Run() parameter.
   */
  TODCall call;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTodReqConvertTime_Run() parameter.
   */
  void *arg;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTodReqConvertTime_Run() parameter.
   */
  bool ticks_validation;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    ScoreTodReqConvertTime_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTodReqConvertTime_Context;

static ScoreTodReqConvertTime_Context ScoreTodReqConvertTime_Instance;

static const char *const ScoreTodReqConvertTime_PreDesc_TicksValidation[] =
  { "Enabled", "Disabled", "NA" };

static const char *const ScoreTodReqConvertTime_PreDesc_Hour[] =
  { "First", "Between", "Last", "Above", "Max", "NA" };

static const char *const ScoreTodReqConvertTime_PreDesc_Minute[] =
  { "First", "Between", "Last", "Above", "Max", "NA" };

static const char *const ScoreTodReqConvertTime_PreDesc_Second[] =
  { "First", "Between", "Last", "Above", "Max", "NA" };

static const char *const ScoreTodReqConvertTime_PreDesc_Ticks[] =
  { "First", "Between", "Last", "Above", "Max", "NA" };

static const char *const *const ScoreTodReqConvertTime_PreDesc[] = {
  ScoreTodReqConvertTime_PreDesc_TicksValidation,
  ScoreTodReqConvertTime_PreDesc_Hour,
  ScoreTodReqConvertTime_PreDesc_Minute,
  ScoreTodReqConvertTime_PreDesc_Second,
  ScoreTodReqConvertTime_PreDesc_Ticks,
  NULL
};

static uint32_t GetValue( TODTimeState state, uint32_t last )
{
  switch ( state ) {
    case STATE_FIRST:
      return 0;
    case STATE_BETWEEN:
      return last / 2;
    case STATE_LAST:
      return last;
    case STATE_ABOVE:
      return last + 1;
    default:
      return UINT32_MAX;
  }
}

static void ScoreTodReqConvertTime_Pre_TicksValidation_Prepare(
  ScoreTodReqConvertTime_Context            *ctx,
  ScoreTodReqConvertTime_Pre_TicksValidation state
)
{
  switch ( state ) {
    case ScoreTodReqConvertTime_Pre_TicksValidation_Enabled: {
      /*
       * Where the directive validates the ticks of the time of day.
       */
      if ( !ctx->ticks_validation ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTodReqConvertTime_Pre_TicksValidation_Disabled: {
      /*
       * Where the directive ignores the ticks of the time of day.
       */
      if ( ctx->ticks_validation ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTodReqConvertTime_Pre_TicksValidation_NA:
      break;
  }
}

static void ScoreTodReqConvertTime_Pre_Hour_Prepare(
  ScoreTodReqConvertTime_Context *ctx,
  ScoreTodReqConvertTime_Pre_Hour state
)
{
  switch ( state ) {
    case ScoreTodReqConvertTime_Pre_Hour_First: {
      /*
       * While the hour of the object referenced by the `time_of_day` parameter
       * is zero.
       */
      ctx->hour = STATE_FIRST;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Hour_Between: {
      /*
       * While the hour of the object referenced by the `time_of_day` parameter
       * is greater than zero and less than 23.
       */
      ctx->hour = STATE_BETWEEN;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Hour_Last: {
      /*
       * While the hour of the object referenced by the `time_of_day` parameter
       * is 23.
       */
      ctx->hour = STATE_LAST;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Hour_Above: {
      /*
       * While the hour of the object referenced by the `time_of_day` parameter
       * is greater than 23 and less than the greatest value of type uint32_t.
       */
      ctx->hour = STATE_ABOVE;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Hour_Max: {
      /*
       * While the hour of the object referenced by the `time_of_day` parameter
       * is the greatest value of type uint32_t.
       */
      ctx->hour = STATE_MAX;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Hour_NA:
      break;
  }
}

static void ScoreTodReqConvertTime_Pre_Minute_Prepare(
  ScoreTodReqConvertTime_Context   *ctx,
  ScoreTodReqConvertTime_Pre_Minute state
)
{
  switch ( state ) {
    case ScoreTodReqConvertTime_Pre_Minute_First: {
      /*
       * While the minute of the object referenced by the `time_of_day`
       * parameter is zero.
       */
      ctx->minute = STATE_FIRST;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Minute_Between: {
      /*
       * While the minute of the object referenced by the `time_of_day`
       * parameter is greater than zero and less than 59.
       */
      ctx->minute = STATE_BETWEEN;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Minute_Last: {
      /*
       * While the minute of the object referenced by the `time_of_day`
       * parameter is 59.
       */
      ctx->minute = STATE_LAST;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Minute_Above: {
      /*
       * While the minute of the object referenced by the `time_of_day`
       * parameter is greater than 59 and less than the greatest value of type
       * uint32_t.
       */
      ctx->minute = STATE_ABOVE;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Minute_Max: {
      /*
       * While the minute of the object referenced by the `time_of_day`
       * parameter is the greatest value of type uint32_t.
       */
      ctx->minute = STATE_MAX;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Minute_NA:
      break;
  }
}

static void ScoreTodReqConvertTime_Pre_Second_Prepare(
  ScoreTodReqConvertTime_Context   *ctx,
  ScoreTodReqConvertTime_Pre_Second state
)
{
  switch ( state ) {
    case ScoreTodReqConvertTime_Pre_Second_First: {
      /*
       * While the second of the object referenced by the `time_of_day`
       * parameter is zero.
       */
      ctx->second = STATE_FIRST;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Second_Between: {
      /*
       * While the second of the object referenced by the `time_of_day`
       * parameter is greater than zero and less than 59.
       */
      ctx->second = STATE_BETWEEN;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Second_Last: {
      /*
       * While the second of the object referenced by the `time_of_day`
       * parameter is 59.
       */
      ctx->second = STATE_LAST;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Second_Above: {
      /*
       * While the second of the object referenced by the `time_of_day`
       * parameter is greater than 59 and less than the greatest value of type
       * uint32_t.
       */
      ctx->second = STATE_ABOVE;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Second_Max: {
      /*
       * While the second of the object referenced by the `time_of_day`
       * parameter is the greatest value of type uint32_t.
       */
      ctx->second = STATE_MAX;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Second_NA:
      break;
  }
}

static void ScoreTodReqConvertTime_Pre_Ticks_Prepare(
  ScoreTodReqConvertTime_Context  *ctx,
  ScoreTodReqConvertTime_Pre_Ticks state
)
{
  switch ( state ) {
    case ScoreTodReqConvertTime_Pre_Ticks_First: {
      /*
       * While the ticks of the object referenced by the `time_of_day`
       * parameter are zero.
       */
      ctx->ticks = STATE_FIRST;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Ticks_Between: {
      /*
       * While the ticks of the object referenced by the `time_of_day`
       * parameter are greater than zero and less than the
       * rtems_clock_get_ticks_per_second() value minus one.
       */
      ctx->ticks = STATE_BETWEEN;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Ticks_Last: {
      /*
       * While the ticks of the object referenced by the `time_of_day`
       * parameter are the rtems_clock_get_ticks_per_second() value minus one.
       */
      ctx->ticks = STATE_LAST;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Ticks_Above: {
      /*
       * While the ticks of the object referenced by the `time_of_day`
       * parameter are greater than the rtems_clock_get_ticks_per_second()
       * value minus one and less than the greatest value of type uint32_t.
       */
      ctx->ticks = STATE_ABOVE;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Ticks_Max: {
      /*
       * While the ticks of the object referenced by the `time_of_day`
       * parameter are the greatest value of type uint32_t.
       */
      ctx->ticks = STATE_MAX;
      break;
    }

    case ScoreTodReqConvertTime_Pre_Ticks_NA:
      break;
  }
}

static void ScoreTodReqConvertTime_Post_Result_Check(
  ScoreTodReqConvertTime_Context    *ctx,
  ScoreTodReqConvertTime_Post_Result state
)
{
  switch ( state ) {
    case ScoreTodReqConvertTime_Post_Result_Accept: {
      /*
       * The directive call shall accept the time of day referenced by the
       * `time_of_day` parameter.
       */
      T_true( ctx->accepted );
      break;
    }

    case ScoreTodReqConvertTime_Post_Result_Reject: {
      /*
       * The directive call shall reject the time of day referenced by the
       * `time_of_day` parameter.
       */
      T_false( ctx->accepted );
      break;
    }

    case ScoreTodReqConvertTime_Post_Result_NA:
      break;
  }
}

static void ScoreTodReqConvertTime_Post_Effect_Check(
  ScoreTodReqConvertTime_Context    *ctx,
  ScoreTodReqConvertTime_Post_Effect state
)
{
  switch ( state ) {
    case ScoreTodReqConvertTime_Post_Effect_Seconds: {
      /*
       * The directive call shall use the seconds since the Unix epoch of the
       * date and the time of the object referenced by the `time_of_day`
       * parameter.
       */
      T_eq_i64( ctx->seconds, ctx->expected );
      break;
    }

    case ScoreTodReqConvertTime_Post_Effect_Nop: {
      /*
       * The directive call shall have no effect.
       */
      T_eq_i64( ctx->seconds, TOD_NO_EFFECT );
      break;
    }

    case ScoreTodReqConvertTime_Post_Effect_NA:
      break;
  }
}

static void ScoreTodReqConvertTime_Action(
  ScoreTodReqConvertTime_Context *ctx
)
{
  ctx->tod.year = 2023;
  ctx->tod.month = 6;
  ctx->tod.day = 15;
  ctx->tod.hour = GetValue( ctx->hour, 23 );
  ctx->tod.minute = GetValue( ctx->minute, 59 );
  ctx->tod.second = GetValue( ctx->second, 59 );
  ctx->tod.ticks = GetValue(
    ctx->ticks,
    rtems_clock_get_ticks_per_second() - 1
  );
  ctx->expected = DaysFromCivil(
                    ctx->tod.year,
                    ctx->tod.month,
                    ctx->tod.day
                  ) *
                    86400 +
                  (int64_t) ctx->tod.hour * 3600 +
                  (int64_t) ctx->tod.minute * 60 + ctx->tod.second;
  ctx->seconds = TOD_NO_EFFECT;
  ctx->accepted = ( *ctx->call )( ctx->arg, &ctx->tod, &ctx->seconds );
}

/* clang-format off */

static const ScoreTodReqConvertTime_Entry
ScoreTodReqConvertTime_Entries[] = {
  { 0, 0, 0, 0, 0, 0, ScoreTodReqConvertTime_Post_Result_Reject,
    ScoreTodReqConvertTime_Post_Effect_Nop },
  { 0, 0, 0, 0, 0, 0, ScoreTodReqConvertTime_Post_Result_Accept,
    ScoreTodReqConvertTime_Post_Effect_Seconds }
};

static const uint8_t
ScoreTodReqConvertTime_Map[] = {
  1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
  1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1,
  1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1,
  1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0,
  1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0
};

/* clang-format on */

static size_t ScoreTodReqConvertTime_Scope( void *arg, char *buf, size_t n )
{
  ScoreTodReqConvertTime_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreTodReqConvertTime_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreTodReqConvertTime_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreTodReqConvertTime_Scope,
  .initial_context = &ScoreTodReqConvertTime_Instance
};

static const uint16_t ScoreTodReqConvertTime_Weights[] =
  { 625, 125, 25, 5, 1 };

static void ScoreTodReqConvertTime_Skip(
  ScoreTodReqConvertTime_Context *ctx,
  size_t                          index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTodReqConvertTime_Pre_Hour_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreTodReqConvertTime_Pre_Minute_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pcs[ 3 ] = ScoreTodReqConvertTime_Pre_Second_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pcs[ 4 ] = ScoreTodReqConvertTime_Pre_Ticks_NA - 1;
      break;
  }
}

static inline ScoreTodReqConvertTime_Entry ScoreTodReqConvertTime_PopEntry(
  ScoreTodReqConvertTime_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 5; ++i ) {
      index += ScoreTodReqConvertTime_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTodReqConvertTime_Entries[ ScoreTodReqConvertTime_Map[ index ] ];
}

static void ScoreTodReqConvertTime_TestVariant(
  ScoreTodReqConvertTime_Context *ctx
)
{
  ScoreTodReqConvertTime_Pre_TicksValidation_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreTodReqConvertTime_Skip( ctx, 0 );
    return;
  }

  ScoreTodReqConvertTime_Pre_Hour_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreTodReqConvertTime_Pre_Minute_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreTodReqConvertTime_Pre_Second_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  ScoreTodReqConvertTime_Pre_Ticks_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  ScoreTodReqConvertTime_Action( ctx );
  ScoreTodReqConvertTime_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  ScoreTodReqConvertTime_Post_Effect_Check( ctx, ctx->Map.entry.Post_Effect );
}

static T_fixture_node ScoreTodReqConvertTime_Node;

static T_remark ScoreTodReqConvertTime_Remark = {
  .next = NULL,
  .remark = "ScoreTodReqConvertTime"
};

void ScoreTodReqConvertTime_Run(
  TODCall call,
  void   *arg,
  bool    ticks_validation
)
{
  ScoreTodReqConvertTime_Context *ctx;

  ctx = &ScoreTodReqConvertTime_Instance;
  ctx->call = call;
  ctx->arg = arg;
  ctx->ticks_validation = ticks_validation;

  ctx = T_push_fixture(
    &ScoreTodReqConvertTime_Node,
    &ScoreTodReqConvertTime_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTodReqConvertTime_Pre_TicksValidation_Enabled;
    ctx->Map.pcs[ 0 ] < ScoreTodReqConvertTime_Pre_TicksValidation_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTodReqConvertTime_Pre_Hour_First;
      ctx->Map.pcs[ 1 ] < ScoreTodReqConvertTime_Pre_Hour_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTodReqConvertTime_Pre_Minute_First;
        ctx->Map.pcs[ 2 ] < ScoreTodReqConvertTime_Pre_Minute_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = ScoreTodReqConvertTime_Pre_Second_First;
          ctx->Map.pcs[ 3 ] < ScoreTodReqConvertTime_Pre_Second_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = ScoreTodReqConvertTime_Pre_Ticks_First;
            ctx->Map.pcs[ 4 ] < ScoreTodReqConvertTime_Pre_Ticks_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = ScoreTodReqConvertTime_PopEntry( ctx );
            ScoreTodReqConvertTime_TestVariant( ctx );
          }
        }
      }
    }
  }

  T_add_remark( &ScoreTodReqConvertTime_Remark );
  T_pop_fixture();
}

/** @} */
