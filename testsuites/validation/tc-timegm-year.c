/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CTimeReqTimegmYear
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

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CTimeReqTimegmYear spec:/c/time/req/timegm-year
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CTimeReqTimegmYear_Pre_Year_Min,
  CTimeReqTimegmYear_Pre_Year_BeforeEpoch,
  CTimeReqTimegmYear_Pre_Year_AfterEpoch,
  CTimeReqTimegmYear_Pre_Year_Max,
  CTimeReqTimegmYear_Pre_Year_NA
} CTimeReqTimegmYear_Pre_Year;

typedef enum {
  CTimeReqTimegmYear_Pre_LeapYear_Common,
  CTimeReqTimegmYear_Pre_LeapYear_Leap4,
  CTimeReqTimegmYear_Pre_LeapYear_Century,
  CTimeReqTimegmYear_Pre_LeapYear_Leap400,
  CTimeReqTimegmYear_Pre_LeapYear_NA
} CTimeReqTimegmYear_Pre_LeapYear;

typedef enum {
  CTimeReqTimegmYear_Pre_Carry_Back,
  CTimeReqTimegmYear_Pre_Carry_None,
  CTimeReqTimegmYear_Pre_Carry_Forward,
  CTimeReqTimegmYear_Pre_Carry_NA
} CTimeReqTimegmYear_Pre_Carry;

typedef enum {
  CTimeReqTimegmYear_Pre_Dst_Negative,
  CTimeReqTimegmYear_Pre_Dst_Zero,
  CTimeReqTimegmYear_Pre_Dst_Positive,
  CTimeReqTimegmYear_Pre_Dst_NA
} CTimeReqTimegmYear_Pre_Dst;

typedef enum {
  CTimeReqTimegmYear_Post_Return_Time,
  CTimeReqTimegmYear_Post_Return_Error,
  CTimeReqTimegmYear_Post_Return_NA
} CTimeReqTimegmYear_Post_Return;

typedef enum {
  CTimeReqTimegmYear_Post_Errno_Eoverflow,
  CTimeReqTimegmYear_Post_Errno_NA
} CTimeReqTimegmYear_Post_Errno;

typedef enum {
  CTimeReqTimegmYear_Post_Second_Set,
  CTimeReqTimegmYear_Post_Second_NA
} CTimeReqTimegmYear_Post_Second;

typedef enum {
  CTimeReqTimegmYear_Post_Minute_Set,
  CTimeReqTimegmYear_Post_Minute_NA
} CTimeReqTimegmYear_Post_Minute;

typedef enum {
  CTimeReqTimegmYear_Post_Hour_Set,
  CTimeReqTimegmYear_Post_Hour_NA
} CTimeReqTimegmYear_Post_Hour;

typedef enum {
  CTimeReqTimegmYear_Post_MonthDay_Set,
  CTimeReqTimegmYear_Post_MonthDay_NA
} CTimeReqTimegmYear_Post_MonthDay;

typedef enum {
  CTimeReqTimegmYear_Post_Month_Set,
  CTimeReqTimegmYear_Post_Month_NA
} CTimeReqTimegmYear_Post_Month;

typedef enum {
  CTimeReqTimegmYear_Post_Year_Set,
  CTimeReqTimegmYear_Post_Year_NA
} CTimeReqTimegmYear_Post_Year;

typedef enum {
  CTimeReqTimegmYear_Post_WeekDay_Set,
  CTimeReqTimegmYear_Post_WeekDay_Nop,
  CTimeReqTimegmYear_Post_WeekDay_NA
} CTimeReqTimegmYear_Post_WeekDay;

typedef enum {
  CTimeReqTimegmYear_Post_YearDay_Set,
  CTimeReqTimegmYear_Post_YearDay_NA
} CTimeReqTimegmYear_Post_YearDay;

typedef enum {
  CTimeReqTimegmYear_Post_Dst_Set,
  CTimeReqTimegmYear_Post_Dst_NA
} CTimeReqTimegmYear_Post_Dst;

typedef enum {
  CTimeReqTimegmYear_Post_GmtOff_Set,
  CTimeReqTimegmYear_Post_GmtOff_NA
} CTimeReqTimegmYear_Post_GmtOff;

typedef enum {
  CTimeReqTimegmYear_Post_Zone_Set,
  CTimeReqTimegmYear_Post_Zone_NA
} CTimeReqTimegmYear_Post_Zone;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Year_NA : 1;
  uint32_t Pre_LeapYear_NA : 1;
  uint32_t Pre_Carry_NA : 1;
  uint32_t Pre_Dst_NA : 1;
  uint32_t Post_Return : 2;
  uint32_t Post_Errno : 1;
  uint32_t Post_Second : 1;
  uint32_t Post_Minute : 1;
  uint32_t Post_Hour : 1;
  uint32_t Post_MonthDay : 1;
  uint32_t Post_Month : 1;
  uint32_t Post_Year : 1;
  uint32_t Post_WeekDay : 2;
  uint32_t Post_YearDay : 1;
  uint32_t Post_Dst : 1;
  uint32_t Post_GmtOff : 1;
  uint32_t Post_Zone : 1;
} CTimeReqTimegmYear_Entry;

typedef enum {
  YEAR_MIN,
  YEAR_BEFORE_EPOCH,
  YEAR_AFTER_EPOCH,
  YEAR_MAX
} YearState;

typedef enum { CARRY_BACK, CARRY_NONE, CARRY_FORWARD } Carry;

typedef enum { DST_NEGATIVE, DST_ZERO, DST_POSITIVE } Dst;

typedef enum {
  LEAP_YEAR_COMMON,
  LEAP_YEAR_4,
  LEAP_YEAR_CENTURY,
  LEAP_YEAR_400
} LeapYear;

/**
 * @brief Test context for spec:/c/time/req/timegm-year test case.
 */
typedef struct {
  /**
   * @brief This member contains the value of errno after the function call.
   */
  int errno_value;

  /**
   * @brief This member specifies the state of the member tm_year.
   */
  YearState year_state;

  /**
   * @brief This member specifies the move of the year.
   */
  Carry carry;

  /**
   * @brief This member specifies the state of the member tm_isdst.
   */
  Dst dst;

  /**
   * @brief This member specifies the leap year class.
   */
  LeapYear leap_year;

  /**
   * @brief This member contains the broken-down time of the function call.
   */
  struct tm tm;

  /**
   * @brief This member contains the broken-down time which the call shall
   *   produce.
   */
  struct tm expected;

  /**
   * @brief This member contains the time which the call shall return.
   */
  time_t time;

  /**
   * @brief This member contains the return value of the function call.
   */
  time_t result;

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
    CTimeReqTimegmYear_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CTimeReqTimegmYear_Context;

static CTimeReqTimegmYear_Context CTimeReqTimegmYear_Instance;

static const char *const CTimeReqTimegmYear_PreDesc_Year[] =
  { "Min", "BeforeEpoch", "AfterEpoch", "Max", "NA" };

static const char *const CTimeReqTimegmYear_PreDesc_LeapYear[] =
  { "Common", "Leap4", "Century", "Leap400", "NA" };

static const char *const CTimeReqTimegmYear_PreDesc_Carry[] =
  { "Back", "None", "Forward", "NA" };

static const char *const CTimeReqTimegmYear_PreDesc_Dst[] =
  { "Negative", "Zero", "Positive", "NA" };

static const char *const *const CTimeReqTimegmYear_PreDesc[] = {
  CTimeReqTimegmYear_PreDesc_Year,
  CTimeReqTimegmYear_PreDesc_LeapYear,
  CTimeReqTimegmYear_PreDesc_Carry,
  CTimeReqTimegmYear_PreDesc_Dst,
  NULL
};

typedef CTimeReqTimegmYear_Context Context;

#define WDAY_SENTINEL 7

/* A zero marks a class which the year does not have. */
static const int years[ 4 ][ 4 ] = {
  { 0, INT_MIN, 0, 0 },
  { 69, 68, 0, -300 },
  { 123, 124, 200, 100 },
  { INT_MAX, 0, 0, 0 }
};

static const int months[ 3 ] = { -1, 5, 12 };

static const int dst[ 3 ] = { -1, 0, 1 };

static int64_t FloorDivide( int64_t a, int64_t b )
{
  int64_t q;

  q = a / b;

  if ( ( a % b ) != 0 && ( ( a < 0 ) != ( b < 0 ) ) ) {
    --q;
  }

  return q;
}

/*
 * The expected time counts each member in its unit from the first day of the
 * month which the members tm_year and tm_mon specify.
 */
static void GetExpected( Context *ctx )
{
  int64_t months;
  int64_t year;
  int64_t month;
  int64_t days;

  months = (int64_t) ctx->tm.tm_year * 12 + ctx->tm.tm_mon;
  year = FloorDivide( months, 12 ) + 1900;
  month = months - FloorDivide( months, 12 ) * 12 + 1;
  days = DaysFromCivil( year, (unsigned int) month, 1 ) + ctx->tm.tm_mday - 1;
  ctx->time = (time_t) ( days * 86400 + (int64_t) ctx->tm.tm_hour * 3600 +
                         (int64_t) ctx->tm.tm_min * 60 + ctx->tm.tm_sec );
  memset( &ctx->expected, 0, sizeof( ctx->expected ) );
  (void) gmtime_r( &ctx->time, &ctx->expected );
}

static void CTimeReqTimegmYear_Pre_Year_Prepare(
  CTimeReqTimegmYear_Context *ctx,
  CTimeReqTimegmYear_Pre_Year state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Pre_Year_Min: {
      /*
       * While the member tm_year of the object referenced by the `timeptr`
       * parameter is the least value of type int.
       */
      ctx->year_state = YEAR_MIN;
      break;
    }

    case CTimeReqTimegmYear_Pre_Year_BeforeEpoch: {
      /*
       * While the member tm_year of the object referenced by the `timeptr`
       * parameter is greater than the least value of type int and less than
       * the year of the Unix epoch minus 1900.
       */
      ctx->year_state = YEAR_BEFORE_EPOCH;
      break;
    }

    case CTimeReqTimegmYear_Pre_Year_AfterEpoch: {
      /*
       * While the member tm_year of the object referenced by the `timeptr`
       * parameter is greater than or equal to the year of the Unix epoch minus
       * 1900 and less than the greatest value of type int.
       */
      ctx->year_state = YEAR_AFTER_EPOCH;
      break;
    }

    case CTimeReqTimegmYear_Pre_Year_Max: {
      /*
       * While the member tm_year of the object referenced by the `timeptr`
       * parameter is the greatest value of type int.
       */
      ctx->year_state = YEAR_MAX;
      break;
    }

    case CTimeReqTimegmYear_Pre_Year_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Pre_LeapYear_Prepare(
  CTimeReqTimegmYear_Context     *ctx,
  CTimeReqTimegmYear_Pre_LeapYear state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Pre_LeapYear_Common: {
      /*
       * While the year which the member tm_year of the object referenced by
       * the `timeptr` parameter specifies is not divisible by four.
       */
      ctx->leap_year = LEAP_YEAR_COMMON;
      break;
    }

    case CTimeReqTimegmYear_Pre_LeapYear_Leap4: {
      /*
       * While the year which the member tm_year of the object referenced by
       * the `timeptr` parameter specifies is divisible by four and not
       * divisible by 100.
       */
      ctx->leap_year = LEAP_YEAR_4;
      break;
    }

    case CTimeReqTimegmYear_Pre_LeapYear_Century: {
      /*
       * While the year which the member tm_year of the object referenced by
       * the `timeptr` parameter specifies is divisible by 100 and not
       * divisible by 400.
       */
      ctx->leap_year = LEAP_YEAR_CENTURY;
      break;
    }

    case CTimeReqTimegmYear_Pre_LeapYear_Leap400: {
      /*
       * While the year which the member tm_year of the object referenced by
       * the `timeptr` parameter specifies is divisible by 400.
       */
      ctx->leap_year = LEAP_YEAR_400;
      break;
    }

    case CTimeReqTimegmYear_Pre_LeapYear_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Pre_Carry_Prepare(
  CTimeReqTimegmYear_Context  *ctx,
  CTimeReqTimegmYear_Pre_Carry state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Pre_Carry_Back: {
      /*
       * While the normalization of the members of the object referenced by the
       * `timeptr` parameter moves the year back by one year.
       */
      ctx->carry = CARRY_BACK;
      break;
    }

    case CTimeReqTimegmYear_Pre_Carry_None: {
      /*
       * While the normalization of the members of the object referenced by the
       * `timeptr` parameter keeps the year.
       */
      ctx->carry = CARRY_NONE;
      break;
    }

    case CTimeReqTimegmYear_Pre_Carry_Forward: {
      /*
       * While the normalization of the members of the object referenced by the
       * `timeptr` parameter moves the year forward by one year.
       */
      ctx->carry = CARRY_FORWARD;
      break;
    }

    case CTimeReqTimegmYear_Pre_Carry_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Pre_Dst_Prepare(
  CTimeReqTimegmYear_Context *ctx,
  CTimeReqTimegmYear_Pre_Dst  state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Pre_Dst_Negative: {
      /*
       * While the member tm_isdst of the object referenced by the `timeptr`
       * parameter is less than zero.
       */
      ctx->dst = DST_NEGATIVE;
      break;
    }

    case CTimeReqTimegmYear_Pre_Dst_Zero: {
      /*
       * While the member tm_isdst of the object referenced by the `timeptr`
       * parameter is zero.
       */
      ctx->dst = DST_ZERO;
      break;
    }

    case CTimeReqTimegmYear_Pre_Dst_Positive: {
      /*
       * While the member tm_isdst of the object referenced by the `timeptr`
       * parameter is greater than zero.
       */
      ctx->dst = DST_POSITIVE;
      break;
    }

    case CTimeReqTimegmYear_Pre_Dst_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_Return_Check(
  CTimeReqTimegmYear_Context    *ctx,
  CTimeReqTimegmYear_Post_Return state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_Return_Time: {
      /*
       * The return value of timegm() shall be the seconds since the Unix epoch
       * of the time which the members of the object referenced by the
       * `timeptr` parameter specify in the proleptic Gregorian calendar.
       */
      T_eq_i64( (int64_t) ctx->result, (int64_t) ctx->time );
      break;
    }

    case CTimeReqTimegmYear_Post_Return_Error: {
      /*
       * The return value of timegm() shall be minus one.
       */
      T_eq_i64( (int64_t) ctx->result, -1 );
      break;
    }

    case CTimeReqTimegmYear_Post_Return_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_Errno_Check(
  CTimeReqTimegmYear_Context   *ctx,
  CTimeReqTimegmYear_Post_Errno state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_Errno_Eoverflow: {
      /*
       * The value of errno shall be EOVERFLOW.
       */
      T_eq_int( ctx->errno_value, EOVERFLOW );
      break;
    }

    case CTimeReqTimegmYear_Post_Errno_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_Second_Check(
  CTimeReqTimegmYear_Context    *ctx,
  CTimeReqTimegmYear_Post_Second state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_Second_Set: {
      /*
       * The value of the member tm_sec of the object referenced by the
       * `timeptr` parameter shall be the second of the minute of the returned
       * time.
       */
      T_eq_int( ctx->tm.tm_sec, ctx->expected.tm_sec );
      break;
    }

    case CTimeReqTimegmYear_Post_Second_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_Minute_Check(
  CTimeReqTimegmYear_Context    *ctx,
  CTimeReqTimegmYear_Post_Minute state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_Minute_Set: {
      /*
       * The value of the member tm_min of the object referenced by the
       * `timeptr` parameter shall be the minute of the hour of the returned
       * time.
       */
      T_eq_int( ctx->tm.tm_min, ctx->expected.tm_min );
      break;
    }

    case CTimeReqTimegmYear_Post_Minute_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_Hour_Check(
  CTimeReqTimegmYear_Context  *ctx,
  CTimeReqTimegmYear_Post_Hour state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_Hour_Set: {
      /*
       * The value of the member tm_hour of the object referenced by the
       * `timeptr` parameter shall be the hour of the day of the returned time.
       */
      T_eq_int( ctx->tm.tm_hour, ctx->expected.tm_hour );
      break;
    }

    case CTimeReqTimegmYear_Post_Hour_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_MonthDay_Check(
  CTimeReqTimegmYear_Context      *ctx,
  CTimeReqTimegmYear_Post_MonthDay state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_MonthDay_Set: {
      /*
       * The value of the member tm_mday of the object referenced by the
       * `timeptr` parameter shall be the day of the month of the returned time
       * in the proleptic Gregorian calendar.
       */
      T_eq_int( ctx->tm.tm_mday, ctx->expected.tm_mday );
      break;
    }

    case CTimeReqTimegmYear_Post_MonthDay_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_Month_Check(
  CTimeReqTimegmYear_Context   *ctx,
  CTimeReqTimegmYear_Post_Month state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_Month_Set: {
      /*
       * The value of the member tm_mon of the object referenced by the
       * `timeptr` parameter shall be the number of months since January of the
       * returned time in the proleptic Gregorian calendar.
       */
      T_eq_int( ctx->tm.tm_mon, ctx->expected.tm_mon );
      break;
    }

    case CTimeReqTimegmYear_Post_Month_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_Year_Check(
  CTimeReqTimegmYear_Context  *ctx,
  CTimeReqTimegmYear_Post_Year state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_Year_Set: {
      /*
       * The value of the member tm_year of the object referenced by the
       * `timeptr` parameter shall be the year of the returned time in the
       * proleptic Gregorian calendar minus 1900\.
       */
      T_eq_int( ctx->tm.tm_year, ctx->expected.tm_year );
      break;
    }

    case CTimeReqTimegmYear_Post_Year_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_WeekDay_Check(
  CTimeReqTimegmYear_Context     *ctx,
  CTimeReqTimegmYear_Post_WeekDay state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_WeekDay_Set: {
      /*
       * The value of the member tm_wday of the object referenced by the
       * `timeptr` parameter shall be the number of days since Sunday of the
       * returned time.
       */
      T_eq_int( ctx->tm.tm_wday, ctx->expected.tm_wday );
      break;
    }

    case CTimeReqTimegmYear_Post_WeekDay_Nop: {
      /*
       * The value of the member tm_wday of the object referenced by the
       * `timeptr` parameter shall not be modified by the function call.
       */
      T_eq_int( ctx->tm.tm_wday, WDAY_SENTINEL );
      break;
    }

    case CTimeReqTimegmYear_Post_WeekDay_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_YearDay_Check(
  CTimeReqTimegmYear_Context     *ctx,
  CTimeReqTimegmYear_Post_YearDay state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_YearDay_Set: {
      /*
       * The value of the member tm_yday of the object referenced by the
       * `timeptr` parameter shall be the number of days since January 1 of the
       * returned time in the proleptic Gregorian calendar.
       */
      T_eq_int( ctx->tm.tm_yday, ctx->expected.tm_yday );
      break;
    }

    case CTimeReqTimegmYear_Post_YearDay_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_Dst_Check(
  CTimeReqTimegmYear_Context *ctx,
  CTimeReqTimegmYear_Post_Dst state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_Dst_Set: {
      /*
       * The value of the member tm_isdst of the object referenced by the
       * `timeptr` parameter shall be zero.
       */
      T_eq_int( ctx->tm.tm_isdst, 0 );
      break;
    }

    case CTimeReqTimegmYear_Post_Dst_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_GmtOff_Check(
  CTimeReqTimegmYear_Context    *ctx,
  CTimeReqTimegmYear_Post_GmtOff state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_GmtOff_Set: {
      /*
       * The value of the member tm_gmtoff of the object referenced by the
       * `timeptr` parameter shall be zero.
       */
      T_eq_long( ctx->tm.tm_gmtoff, 0 );
      break;
    }

    case CTimeReqTimegmYear_Post_GmtOff_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Post_Zone_Check(
  CTimeReqTimegmYear_Context  *ctx,
  CTimeReqTimegmYear_Post_Zone state
)
{
  switch ( state ) {
    case CTimeReqTimegmYear_Post_Zone_Set: {
      /*
       * The member tm_zone of the object referenced by the `timeptr` parameter
       * shall reference the string UTC.
       */
      T_assert_not_null( ctx->tm.tm_zone );
      T_eq_str( ctx->tm.tm_zone, "UTC" );
      break;
    }

    case CTimeReqTimegmYear_Post_Zone_NA:
      break;
  }
}

static void CTimeReqTimegmYear_Action( CTimeReqTimegmYear_Context *ctx )
{
  ctx->tm.tm_year = years[ ctx->year_state ][ ctx->leap_year ];
  ctx->tm.tm_mon = months[ ctx->carry ];
  ctx->tm.tm_mday = 15;
  ctx->tm.tm_hour = 12;
  ctx->tm.tm_min = 30;
  ctx->tm.tm_sec = 30;
  ctx->tm.tm_wday = WDAY_SENTINEL;
  ctx->tm.tm_yday = -1;
  ctx->tm.tm_isdst = dst[ ctx->dst ];
  ctx->tm.tm_gmtoff = -1;
  ctx->tm.tm_zone = NULL;
  GetExpected( ctx );
  errno = 0;
  ctx->result = timegm( &ctx->tm );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CTimeReqTimegmYear_Entry
CTimeReqTimegmYear_Entries[] = {
  { 0, 0, 0, 0, 0, CTimeReqTimegmYear_Post_Return_Time,
    CTimeReqTimegmYear_Post_Errno_NA, CTimeReqTimegmYear_Post_Second_Set,
    CTimeReqTimegmYear_Post_Minute_Set, CTimeReqTimegmYear_Post_Hour_Set,
    CTimeReqTimegmYear_Post_MonthDay_Set, CTimeReqTimegmYear_Post_Month_Set,
    CTimeReqTimegmYear_Post_Year_Set, CTimeReqTimegmYear_Post_WeekDay_Set,
    CTimeReqTimegmYear_Post_YearDay_Set, CTimeReqTimegmYear_Post_Dst_Set,
    CTimeReqTimegmYear_Post_GmtOff_Set, CTimeReqTimegmYear_Post_Zone_Set },
  { 1, 0, 0, 0, 0, CTimeReqTimegmYear_Post_Return_NA,
    CTimeReqTimegmYear_Post_Errno_NA, CTimeReqTimegmYear_Post_Second_NA,
    CTimeReqTimegmYear_Post_Minute_NA, CTimeReqTimegmYear_Post_Hour_NA,
    CTimeReqTimegmYear_Post_MonthDay_NA, CTimeReqTimegmYear_Post_Month_NA,
    CTimeReqTimegmYear_Post_Year_NA, CTimeReqTimegmYear_Post_WeekDay_NA,
    CTimeReqTimegmYear_Post_YearDay_NA, CTimeReqTimegmYear_Post_Dst_NA,
    CTimeReqTimegmYear_Post_GmtOff_NA, CTimeReqTimegmYear_Post_Zone_NA },
  { 0, 0, 0, 0, 0, CTimeReqTimegmYear_Post_Return_Error,
    CTimeReqTimegmYear_Post_Errno_Eoverflow, CTimeReqTimegmYear_Post_Second_NA,
    CTimeReqTimegmYear_Post_Minute_NA, CTimeReqTimegmYear_Post_Hour_NA,
    CTimeReqTimegmYear_Post_MonthDay_NA, CTimeReqTimegmYear_Post_Month_NA,
    CTimeReqTimegmYear_Post_Year_NA, CTimeReqTimegmYear_Post_WeekDay_Nop,
    CTimeReqTimegmYear_Post_YearDay_NA, CTimeReqTimegmYear_Post_Dst_NA,
    CTimeReqTimegmYear_Post_GmtOff_NA, CTimeReqTimegmYear_Post_Zone_NA }
};

static const uint8_t
CTimeReqTimegmYear_Map[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t CTimeReqTimegmYear_Scope( void *arg, char *buf, size_t n )
{
  CTimeReqTimegmYear_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CTimeReqTimegmYear_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CTimeReqTimegmYear_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CTimeReqTimegmYear_Scope,
  .initial_context = &CTimeReqTimegmYear_Instance
};

static inline CTimeReqTimegmYear_Entry CTimeReqTimegmYear_PopEntry(
  CTimeReqTimegmYear_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CTimeReqTimegmYear_Entries[ CTimeReqTimegmYear_Map[ index ] ];
}

static void CTimeReqTimegmYear_TestVariant( CTimeReqTimegmYear_Context *ctx )
{
  CTimeReqTimegmYear_Pre_Year_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CTimeReqTimegmYear_Pre_LeapYear_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CTimeReqTimegmYear_Pre_Carry_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CTimeReqTimegmYear_Pre_Dst_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CTimeReqTimegmYear_Action( ctx );
  CTimeReqTimegmYear_Post_Return_Check( ctx, ctx->Map.entry.Post_Return );
  CTimeReqTimegmYear_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CTimeReqTimegmYear_Post_Second_Check( ctx, ctx->Map.entry.Post_Second );
  CTimeReqTimegmYear_Post_Minute_Check( ctx, ctx->Map.entry.Post_Minute );
  CTimeReqTimegmYear_Post_Hour_Check( ctx, ctx->Map.entry.Post_Hour );
  CTimeReqTimegmYear_Post_MonthDay_Check( ctx, ctx->Map.entry.Post_MonthDay );
  CTimeReqTimegmYear_Post_Month_Check( ctx, ctx->Map.entry.Post_Month );
  CTimeReqTimegmYear_Post_Year_Check( ctx, ctx->Map.entry.Post_Year );
  CTimeReqTimegmYear_Post_WeekDay_Check( ctx, ctx->Map.entry.Post_WeekDay );
  CTimeReqTimegmYear_Post_YearDay_Check( ctx, ctx->Map.entry.Post_YearDay );
  CTimeReqTimegmYear_Post_Dst_Check( ctx, ctx->Map.entry.Post_Dst );
  CTimeReqTimegmYear_Post_GmtOff_Check( ctx, ctx->Map.entry.Post_GmtOff );
  CTimeReqTimegmYear_Post_Zone_Check( ctx, ctx->Map.entry.Post_Zone );
}

/**
 * @fn void T_case_body_CTimeReqTimegmYear( void )
 */
T_TEST_CASE_FIXTURE( CTimeReqTimegmYear, &CTimeReqTimegmYear_Fixture )
{
  CTimeReqTimegmYear_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CTimeReqTimegmYear_Pre_Year_Min;
    ctx->Map.pcs[ 0 ] < CTimeReqTimegmYear_Pre_Year_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CTimeReqTimegmYear_Pre_LeapYear_Common;
      ctx->Map.pcs[ 1 ] < CTimeReqTimegmYear_Pre_LeapYear_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CTimeReqTimegmYear_Pre_Carry_Back;
        ctx->Map.pcs[ 2 ] < CTimeReqTimegmYear_Pre_Carry_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CTimeReqTimegmYear_Pre_Dst_Negative;
          ctx->Map.pcs[ 3 ] < CTimeReqTimegmYear_Pre_Dst_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = CTimeReqTimegmYear_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CTimeReqTimegmYear_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
