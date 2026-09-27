/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTodReqConvertDate
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

#include "tr-tod-convert-date.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTodReqConvertDate spec:/score/tod/req/convert-date
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Year_NA : 1;
  uint16_t Pre_LeapYear_NA : 1;
  uint16_t Pre_Month_NA : 1;
  uint16_t Pre_Day_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_Effect : 2;
} ScoreTodReqConvertDate_Entry;

typedef enum {
  YEAR_ZERO,
  YEAR_TOO_EARLY,
  YEAR_EARLIEST,
  YEAR_VALID,
  YEAR_LATEST,
  YEAR_TOO_LATE,
  YEAR_MAX
} TODDateYear;

typedef enum {
  LEAP_YEAR_COMMON,
  LEAP_YEAR_4,
  LEAP_YEAR_CENTURY,
  LEAP_YEAR_400
} TODDateLeapYear;

typedef enum {
  DAY_ZERO,
  DAY_FIRST,
  DAY_BETWEEN,
  DAY_LAST,
  DAY_ABOVE,
  DAY_MAX
} TODDateDay;

/**
 * @brief Test context for spec:/score/tod/req/convert-date test case.
 */
typedef struct {
  /**
   * @brief This member specifies the year state.
   */
  TODDateYear year;

  /**
   * @brief This member specifies the class of the year.
   */
  TODDateLeapYear leap_year;

  /**
   * @brief This member specifies the month.
   */
  uint32_t month;

  /**
   * @brief This member specifies the day state.
   */
  TODDateDay day;

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
   *   ScoreTodReqConvertDate_Run() parameter.
   */
  TODCall call;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTodReqConvertDate_Run() parameter.
   */
  void *arg;

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
    ScoreTodReqConvertDate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTodReqConvertDate_Context;

static ScoreTodReqConvertDate_Context ScoreTodReqConvertDate_Instance;

static const char *const ScoreTodReqConvertDate_PreDesc_Year[] = {
  "Zero",
  "TooEarly",
  "Earliest",
  "Valid",
  "Latest",
  "TooLate",
  "Max",
  "NA"
};

static const char *const ScoreTodReqConvertDate_PreDesc_LeapYear[] =
  { "Common", "Leap4", "Century", "Leap400", "NA" };

static const char *const ScoreTodReqConvertDate_PreDesc_Month[] = {
  "Zero",
  "January",
  "February",
  "Between",
  "December",
  "Above",
  "Max",
  "NA"
};

static const char *const ScoreTodReqConvertDate_PreDesc_Day[] =
  { "Zero", "First", "Between", "Last", "Above", "Max", "NA" };

static const char *const *const ScoreTodReqConvertDate_PreDesc[] = {
  ScoreTodReqConvertDate_PreDesc_Year,
  ScoreTodReqConvertDate_PreDesc_LeapYear,
  ScoreTodReqConvertDate_PreDesc_Month,
  ScoreTodReqConvertDate_PreDesc_Day,
  NULL
};

/* A zero marks a class which the year does not have. */
static const uint32_t years[ 7 ][ 4 ] = {
  { 0, 0, 0, 1 },
  { 1987, 1984, 1900, 1600 },
  { 0, 1988, 0, 0 },
  { 2023, 2096, 2100, 2000 },
  { 0, 0, 0, 2400 },
  { 2401, 2404, 2500, 2800 },
  { UINT32_MAX, 0, 0, 0 }
};

static uint32_t GetYear( TODDateYear year, TODDateLeapYear leap_year )
{
  if ( year == YEAR_ZERO ) {
    return 0;
  }

  return years[ year ][ leap_year ];
}

static uint32_t GetDay( uint32_t year, uint32_t month, TODDateDay day )
{
  uint32_t last;

  if ( day == DAY_ZERO ) {
    return 0;
  }

  if ( day == DAY_FIRST ) {
    return 1;
  }

  if ( day == DAY_MAX ) {
    return UINT32_MAX;
  }

  if ( month == 12 ) {
    last = 31;
  } else {
    last = (uint32_t) ( DaysFromCivil( year, month + 1, 1 ) -
                        DaysFromCivil( year, month, 1 ) );
  }

  if ( day == DAY_BETWEEN ) {
    return 15;
  }

  if ( day == DAY_LAST ) {
    return last;
  }

  return last + 1;
}

static void ScoreTodReqConvertDate_Pre_Year_Prepare(
  ScoreTodReqConvertDate_Context *ctx,
  ScoreTodReqConvertDate_Pre_Year state
)
{
  switch ( state ) {
    case ScoreTodReqConvertDate_Pre_Year_Zero: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is zero.
       */
      ctx->year = YEAR_ZERO;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Year_TooEarly: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is greater than zero and less than 1988.
       */
      ctx->year = YEAR_TOO_EARLY;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Year_Earliest: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is 1988.
       */
      ctx->year = YEAR_EARLIEST;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Year_Valid: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is greater than 1988 and less than 2400.
       */
      ctx->year = YEAR_VALID;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Year_Latest: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is 2400.
       */
      ctx->year = YEAR_LATEST;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Year_TooLate: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is greater than 2400 and less than the greatest value of type
       * uint32_t.
       */
      ctx->year = YEAR_TOO_LATE;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Year_Max: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is the greatest value of type uint32_t.
       */
      ctx->year = YEAR_MAX;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Year_NA:
      break;
  }
}

static void ScoreTodReqConvertDate_Pre_LeapYear_Prepare(
  ScoreTodReqConvertDate_Context     *ctx,
  ScoreTodReqConvertDate_Pre_LeapYear state
)
{
  switch ( state ) {
    case ScoreTodReqConvertDate_Pre_LeapYear_Common: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is not divisible by four.
       */
      ctx->leap_year = LEAP_YEAR_COMMON;
      break;
    }

    case ScoreTodReqConvertDate_Pre_LeapYear_Leap4: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is divisible by four and not divisible by 100.
       */
      ctx->leap_year = LEAP_YEAR_4;
      break;
    }

    case ScoreTodReqConvertDate_Pre_LeapYear_Century: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is divisible by 100 and not divisible by 400.
       */
      ctx->leap_year = LEAP_YEAR_CENTURY;
      break;
    }

    case ScoreTodReqConvertDate_Pre_LeapYear_Leap400: {
      /*
       * While the year of the object referenced by the `time_of_day` parameter
       * is divisible by 400.
       */
      ctx->leap_year = LEAP_YEAR_400;
      break;
    }

    case ScoreTodReqConvertDate_Pre_LeapYear_NA:
      break;
  }
}

static void ScoreTodReqConvertDate_Pre_Month_Prepare(
  ScoreTodReqConvertDate_Context  *ctx,
  ScoreTodReqConvertDate_Pre_Month state
)
{
  switch ( state ) {
    case ScoreTodReqConvertDate_Pre_Month_Zero: {
      /*
       * While the month of the object referenced by the `time_of_day`
       * parameter is zero.
       */
      ctx->month = 0;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Month_January: {
      /*
       * While the month of the object referenced by the `time_of_day`
       * parameter is January.
       */
      ctx->month = 1;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Month_February: {
      /*
       * While the month of the object referenced by the `time_of_day`
       * parameter is February.
       */
      ctx->month = 2;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Month_Between: {
      /*
       * While the month of the object referenced by the `time_of_day`
       * parameter is after February and before December.
       */
      ctx->month = 4;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Month_December: {
      /*
       * While the month of the object referenced by the `time_of_day`
       * parameter is December.
       */
      ctx->month = 12;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Month_Above: {
      /*
       * While the month of the object referenced by the `time_of_day`
       * parameter is greater than 12 and less than the greatest value of type
       * uint32_t.
       */
      ctx->month = 13;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Month_Max: {
      /*
       * While the month of the object referenced by the `time_of_day`
       * parameter is the greatest value of type uint32_t.
       */
      ctx->month = UINT32_MAX;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Month_NA:
      break;
  }
}

static void ScoreTodReqConvertDate_Pre_Day_Prepare(
  ScoreTodReqConvertDate_Context *ctx,
  ScoreTodReqConvertDate_Pre_Day  state
)
{
  switch ( state ) {
    case ScoreTodReqConvertDate_Pre_Day_Zero: {
      /*
       * While the day of the object referenced by the `time_of_day` parameter
       * is zero.
       */
      ctx->day = DAY_ZERO;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Day_First: {
      /*
       * While the day of the object referenced by the `time_of_day` parameter
       * is one.
       */
      ctx->day = DAY_FIRST;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Day_Between: {
      /*
       * While the day of the object referenced by the `time_of_day` parameter
       * is greater than one and less than the last day of the month.
       */
      ctx->day = DAY_BETWEEN;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Day_Last: {
      /*
       * While the day of the object referenced by the `time_of_day` parameter
       * is the last day of the month.
       */
      ctx->day = DAY_LAST;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Day_Above: {
      /*
       * While the day of the object referenced by the `time_of_day` parameter
       * is greater than the last day of the month and less than the greatest
       * value of type uint32_t.
       */
      ctx->day = DAY_ABOVE;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Day_Max: {
      /*
       * While the day of the object referenced by the `time_of_day` parameter
       * is the greatest value of type uint32_t.
       */
      ctx->day = DAY_MAX;
      break;
    }

    case ScoreTodReqConvertDate_Pre_Day_NA:
      break;
  }
}

static void ScoreTodReqConvertDate_Post_Result_Check(
  ScoreTodReqConvertDate_Context    *ctx,
  ScoreTodReqConvertDate_Post_Result state
)
{
  switch ( state ) {
    case ScoreTodReqConvertDate_Post_Result_Accept: {
      /*
       * The directive call shall accept the time of day referenced by the
       * `time_of_day` parameter.
       */
      T_true( ctx->accepted );
      break;
    }

    case ScoreTodReqConvertDate_Post_Result_Reject: {
      /*
       * The directive call shall reject the time of day referenced by the
       * `time_of_day` parameter.
       */
      T_false( ctx->accepted );
      break;
    }

    case ScoreTodReqConvertDate_Post_Result_NA:
      break;
  }
}

static void ScoreTodReqConvertDate_Post_Effect_Check(
  ScoreTodReqConvertDate_Context    *ctx,
  ScoreTodReqConvertDate_Post_Effect state
)
{
  switch ( state ) {
    case ScoreTodReqConvertDate_Post_Effect_Seconds: {
      /*
       * The directive call shall use the seconds since the Unix epoch of the
       * date and the time of the object referenced by the `time_of_day`
       * parameter.
       */
      T_eq_i64( ctx->seconds, ctx->expected );
      break;
    }

    case ScoreTodReqConvertDate_Post_Effect_Nop: {
      /*
       * The directive call shall have no effect.
       */
      T_eq_i64( ctx->seconds, TOD_NO_EFFECT );
      break;
    }

    case ScoreTodReqConvertDate_Post_Effect_NA:
      break;
  }
}

static void ScoreTodReqConvertDate_Action(
  ScoreTodReqConvertDate_Context *ctx
)
{
  ctx->tod.year = GetYear( ctx->year, ctx->leap_year );
  ctx->tod.month = ctx->month;
  ctx->tod.day = GetDay( ctx->tod.year, ctx->month, ctx->day );
  ctx->tod.hour = 12;
  ctx->tod.minute = 30;
  ctx->tod.second = 30;
  ctx->tod.ticks = 0;
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

static const ScoreTodReqConvertDate_Entry
ScoreTodReqConvertDate_Entries[] = {
  { 1, 0, 0, 0, 0, ScoreTodReqConvertDate_Post_Result_NA,
    ScoreTodReqConvertDate_Post_Effect_NA },
  { 0, 0, 0, 0, 0, ScoreTodReqConvertDate_Post_Result_Reject,
    ScoreTodReqConvertDate_Post_Effect_Nop },
  { 1, 0, 0, 0, 0, ScoreTodReqConvertDate_Post_Result_NA,
    ScoreTodReqConvertDate_Post_Effect_NA },
  { 0, 0, 0, 0, 0, ScoreTodReqConvertDate_Post_Result_Accept,
    ScoreTodReqConvertDate_Post_Effect_Seconds }
};

static const uint8_t
ScoreTodReqConvertDate_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2,
  2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2,
  2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
  2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 3, 3, 3, 1, 1,
  1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
  2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3,
  1, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1,
  1, 1, 2, 2, 2, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 3,
  3, 3, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 3, 3, 3,
  1, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 1,
  1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 3,
  3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 3,
  3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 2, 2,
  2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1,
  2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
  2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t ScoreTodReqConvertDate_Scope( void *arg, char *buf, size_t n )
{
  ScoreTodReqConvertDate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreTodReqConvertDate_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreTodReqConvertDate_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreTodReqConvertDate_Scope,
  .initial_context = &ScoreTodReqConvertDate_Instance
};

static inline ScoreTodReqConvertDate_Entry ScoreTodReqConvertDate_PopEntry(
  ScoreTodReqConvertDate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTodReqConvertDate_Entries[ ScoreTodReqConvertDate_Map[ index ] ];
}

static void ScoreTodReqConvertDate_TestVariant(
  ScoreTodReqConvertDate_Context *ctx
)
{
  ScoreTodReqConvertDate_Pre_Year_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreTodReqConvertDate_Pre_LeapYear_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreTodReqConvertDate_Pre_Month_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreTodReqConvertDate_Pre_Day_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  ScoreTodReqConvertDate_Action( ctx );
  ScoreTodReqConvertDate_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  ScoreTodReqConvertDate_Post_Effect_Check( ctx, ctx->Map.entry.Post_Effect );
}

static T_fixture_node ScoreTodReqConvertDate_Node;

static T_remark ScoreTodReqConvertDate_Remark = {
  .next = NULL,
  .remark = "ScoreTodReqConvertDate"
};

void ScoreTodReqConvertDate_Run( TODCall call, void *arg )
{
  ScoreTodReqConvertDate_Context *ctx;

  ctx = &ScoreTodReqConvertDate_Instance;
  ctx->call = call;
  ctx->arg = arg;

  ctx = T_push_fixture(
    &ScoreTodReqConvertDate_Node,
    &ScoreTodReqConvertDate_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTodReqConvertDate_Pre_Year_Zero;
    ctx->Map.pcs[ 0 ] < ScoreTodReqConvertDate_Pre_Year_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTodReqConvertDate_Pre_LeapYear_Common;
      ctx->Map.pcs[ 1 ] < ScoreTodReqConvertDate_Pre_LeapYear_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTodReqConvertDate_Pre_Month_Zero;
        ctx->Map.pcs[ 2 ] < ScoreTodReqConvertDate_Pre_Month_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = ScoreTodReqConvertDate_Pre_Day_Zero;
          ctx->Map.pcs[ 3 ] < ScoreTodReqConvertDate_Pre_Day_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = ScoreTodReqConvertDate_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          ScoreTodReqConvertDate_TestVariant( ctx );
        }
      }
    }
  }

  T_add_remark( &ScoreTodReqConvertDate_Remark );
  T_pop_fixture();
}

/** @} */
