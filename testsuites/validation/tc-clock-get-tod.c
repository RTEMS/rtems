/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsClockReqGetTod
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
#include <sys/timetc.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/todimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsClockReqGetTod spec:/rtems/clock/req/get-tod
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsClockReqGetTod_Pre_Set_Yes,
  RtemsClockReqGetTod_Pre_Set_No,
  RtemsClockReqGetTod_Pre_Set_NA
} RtemsClockReqGetTod_Pre_Set;

typedef enum {
  RtemsClockReqGetTod_Pre_Year_Earliest,
  RtemsClockReqGetTod_Pre_Year_Valid,
  RtemsClockReqGetTod_Pre_Year_Latest,
  RtemsClockReqGetTod_Pre_Year_End,
  RtemsClockReqGetTod_Pre_Year_NA
} RtemsClockReqGetTod_Pre_Year;

typedef enum {
  RtemsClockReqGetTod_Pre_LeapYear_Common,
  RtemsClockReqGetTod_Pre_LeapYear_Leap4,
  RtemsClockReqGetTod_Pre_LeapYear_Century,
  RtemsClockReqGetTod_Pre_LeapYear_Leap400,
  RtemsClockReqGetTod_Pre_LeapYear_NA
} RtemsClockReqGetTod_Pre_LeapYear;

typedef enum {
  RtemsClockReqGetTod_Pre_Month_January,
  RtemsClockReqGetTod_Pre_Month_February,
  RtemsClockReqGetTod_Pre_Month_Between,
  RtemsClockReqGetTod_Pre_Month_December,
  RtemsClockReqGetTod_Pre_Month_NA
} RtemsClockReqGetTod_Pre_Month;

typedef enum {
  RtemsClockReqGetTod_Pre_Day_First,
  RtemsClockReqGetTod_Pre_Day_Between,
  RtemsClockReqGetTod_Pre_Day_Last,
  RtemsClockReqGetTod_Pre_Day_NA
} RtemsClockReqGetTod_Pre_Day;

typedef enum {
  RtemsClockReqGetTod_Pre_DayPart_Begin,
  RtemsClockReqGetTod_Pre_DayPart_Between,
  RtemsClockReqGetTod_Pre_DayPart_End,
  RtemsClockReqGetTod_Pre_DayPart_NA
} RtemsClockReqGetTod_Pre_DayPart;

typedef enum {
  RtemsClockReqGetTod_Pre_Param_Valid,
  RtemsClockReqGetTod_Pre_Param_Null,
  RtemsClockReqGetTod_Pre_Param_NA
} RtemsClockReqGetTod_Pre_Param;

typedef enum {
  RtemsClockReqGetTod_Post_Status_Ok,
  RtemsClockReqGetTod_Post_Status_InvAddr,
  RtemsClockReqGetTod_Post_Status_NotDef,
  RtemsClockReqGetTod_Post_Status_NA
} RtemsClockReqGetTod_Post_Status;

typedef enum {
  RtemsClockReqGetTod_Post_Value_TimeOfDay,
  RtemsClockReqGetTod_Post_Value_Unchanged,
  RtemsClockReqGetTod_Post_Value_NA
} RtemsClockReqGetTod_Post_Value;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Set_NA : 1;
  uint16_t Pre_Year_NA : 1;
  uint16_t Pre_LeapYear_NA : 1;
  uint16_t Pre_Month_NA : 1;
  uint16_t Pre_Day_NA : 1;
  uint16_t Pre_DayPart_NA : 1;
  uint16_t Pre_Param_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Value : 2;
} RtemsClockReqGetTod_Entry;

typedef enum { YEAR_EARLIEST, YEAR_VALID, YEAR_LATEST, YEAR_END } Year;

typedef enum {
  LEAP_YEAR_COMMON,
  LEAP_YEAR_4,
  LEAP_YEAR_CENTURY,
  LEAP_YEAR_400
} LeapYear;

typedef enum { DAY_FIRST, DAY_BETWEEN, DAY_LAST } Day;

typedef enum { DAY_PART_BEGIN, DAY_PART_BETWEEN, DAY_PART_END } DayPart;

/**
 * @brief Test context for spec:/rtems/clock/req/get-tod test case.
 */
typedef struct {
  /**
   * @brief This member is true, if the realtime clock shall be set.
   */
  bool set;

  /**
   * @brief This member specifies the year state.
   */
  Year year;

  /**
   * @brief This member specifies the class of the year.
   */
  LeapYear leap_year;

  /**
   * @brief This member specifies the month.
   */
  uint32_t month;

  /**
   * @brief This member specifies the day state.
   */
  Day day;

  /**
   * @brief This member specifies the day part state.
   */
  DayPart day_part;

  /**
   * @brief This member contains the expected time of day.
   */
  rtems_time_of_day expected;

  /**
   * @brief This member provides the object referenced by the `time_of_day`
   *   parameter.
   */
  rtems_time_of_day tod;

  /**
   * @brief This member specifies the `time_of_day` parameter value.
   */
  rtems_time_of_day *tod_ref;

  /**
   * @brief This member contains the return status of the rtems_clock_get_tod()
   *   call.
   */
  rtems_status_code status;

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
    RtemsClockReqGetTod_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsClockReqGetTod_Context;

static RtemsClockReqGetTod_Context RtemsClockReqGetTod_Instance;

static const char *const RtemsClockReqGetTod_PreDesc_Set[] =
  { "Yes", "No", "NA" };

static const char *const RtemsClockReqGetTod_PreDesc_Year[] =
  { "Earliest", "Valid", "Latest", "End", "NA" };

static const char *const RtemsClockReqGetTod_PreDesc_LeapYear[] =
  { "Common", "Leap4", "Century", "Leap400", "NA" };

static const char *const RtemsClockReqGetTod_PreDesc_Month[] =
  { "January", "February", "Between", "December", "NA" };

static const char *const RtemsClockReqGetTod_PreDesc_Day[] =
  { "First", "Between", "Last", "NA" };

static const char *const RtemsClockReqGetTod_PreDesc_DayPart[] =
  { "Begin", "Between", "End", "NA" };

static const char *const RtemsClockReqGetTod_PreDesc_Param[] =
  { "Valid", "Null", "NA" };

static const char *const *const RtemsClockReqGetTod_PreDesc[] = {
  RtemsClockReqGetTod_PreDesc_Set,
  RtemsClockReqGetTod_PreDesc_Year,
  RtemsClockReqGetTod_PreDesc_LeapYear,
  RtemsClockReqGetTod_PreDesc_Month,
  RtemsClockReqGetTod_PreDesc_Day,
  RtemsClockReqGetTod_PreDesc_DayPart,
  RtemsClockReqGetTod_PreDesc_Param,
  NULL
};

typedef RtemsClockReqGetTod_Context Context;

static const struct timespec ts_earliest = { .tv_sec = 567993600 };

/* A zero marks a class which the year does not have. */
static const uint32_t years[ 4 ][ 4 ] = {
  { 0, 1988, 0, 0 },
  { 2023, 2096, 2100, 2000 },
  { 2514, 0, 0, 0 },
  { 2514, 0, 0, 0 }
};

static uint32_t GetLastDayOfMonth( uint32_t year, uint32_t month )
{
  if ( month == 12 ) {
    return 31;
  }

  return (uint32_t) ( DaysFromCivil( year, month + 1, 1 ) -
                      DaysFromCivil( year, month, 1 ) );
}

static void SetClock( Context *ctx )
{
  rtems_time_of_day *tod;
  struct timespec    ts;
  struct bintime     bt;
  ISR_lock_Context   lock_context;
  Status_Control     status;
  long               nanoseconds;

  tod = &ctx->expected;
  tod->year = years[ ctx->year ][ ctx->leap_year ];

  if ( ctx->year == YEAR_END ) {
    tod->month = 5;
    tod->day = 30;
    tod->hour = 1;
    tod->minute = 53;
    tod->second = 3;
    nanoseconds = 1000000000 - rtems_configuration_get_nanoseconds_per_tick();
  } else {
    tod->month = ctx->month;

    switch ( ctx->day ) {
      case DAY_FIRST:
        tod->day = 1;
        break;
      case DAY_BETWEEN:
        tod->day = 15;
        break;
      default:
        tod->day = GetLastDayOfMonth( tod->year, tod->month );
        break;
    }

    switch ( ctx->day_part ) {
      case DAY_PART_BEGIN:
        tod->hour = 0;
        tod->minute = 0;
        tod->second = 0;
        nanoseconds = 0;
        break;
      case DAY_PART_BETWEEN:
        tod->hour = 12;
        tod->minute = 30;
        tod->second = 30;
        nanoseconds = 500000000;
        break;
      default:
        tod->hour = 23;
        tod->minute = 59;
        tod->second = 59;
        nanoseconds = 1000000000 -
                      rtems_configuration_get_nanoseconds_per_tick();
        break;
    }
  }

  tod->ticks = (uint32_t) ( nanoseconds /
                            rtems_configuration_get_nanoseconds_per_tick() );
  ts.tv_sec = (time_t) ( DaysFromCivil( tod->year, tod->month, tod->day ) *
                           86400 +
                         tod->hour * 3600 + tod->minute * 60 + tod->second );
  ts.tv_nsec = nanoseconds;

  _Objects_Allocator_lock();
  _TOD_Acquire( &lock_context );
  status = _TOD_Set( &ts_earliest, &lock_context );
  _Objects_Allocator_unlock();
  T_eq_int( status, STATUS_SUCCESSFUL );

  timespec2bintime( &ts, &bt );
  _TOD_Acquire( &lock_context );
  _Timecounter_Set_clock( &bt, &lock_context );
}

static void RtemsClockReqGetTod_Pre_Set_Prepare(
  RtemsClockReqGetTod_Context *ctx,
  RtemsClockReqGetTod_Pre_Set  state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Pre_Set_Yes: {
      /*
       * While the CLOCK_REALTIME was set at least once.
       */
      ctx->set = true;
      break;
    }

    case RtemsClockReqGetTod_Pre_Set_No: {
      /*
       * While the CLOCK_REALTIME was never set.
       */
      ctx->set = false;
      break;
    }

    case RtemsClockReqGetTod_Pre_Set_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Pre_Year_Prepare(
  RtemsClockReqGetTod_Context *ctx,
  RtemsClockReqGetTod_Pre_Year state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Pre_Year_Earliest: {
      /*
       * While the year of the CLOCK_REALTIME is 1988.
       */
      ctx->year = YEAR_EARLIEST;
      break;
    }

    case RtemsClockReqGetTod_Pre_Year_Valid: {
      /*
       * While the year of the CLOCK_REALTIME is greater than 1988 and less
       * than 2514.
       */
      ctx->year = YEAR_VALID;
      break;
    }

    case RtemsClockReqGetTod_Pre_Year_Latest: {
      /*
       * While the year of the CLOCK_REALTIME is 2514, while the CLOCK_REALTIME
       * is before the clock tick which ends at its latest time point.
       */
      ctx->year = YEAR_LATEST;
      break;
    }

    case RtemsClockReqGetTod_Pre_Year_End: {
      /*
       * While the CLOCK_REALTIME is in the clock tick which ends at its latest
       * time point.
       */
      ctx->year = YEAR_END;
      break;
    }

    case RtemsClockReqGetTod_Pre_Year_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Pre_LeapYear_Prepare(
  RtemsClockReqGetTod_Context     *ctx,
  RtemsClockReqGetTod_Pre_LeapYear state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Pre_LeapYear_Common: {
      /*
       * While the year of the CLOCK_REALTIME is not divisible by four.
       */
      ctx->leap_year = LEAP_YEAR_COMMON;
      break;
    }

    case RtemsClockReqGetTod_Pre_LeapYear_Leap4: {
      /*
       * While the year of the CLOCK_REALTIME is divisible by four and not
       * divisible by 100.
       */
      ctx->leap_year = LEAP_YEAR_4;
      break;
    }

    case RtemsClockReqGetTod_Pre_LeapYear_Century: {
      /*
       * While the year of the CLOCK_REALTIME is divisible by 100 and not
       * divisible by 400.
       */
      ctx->leap_year = LEAP_YEAR_CENTURY;
      break;
    }

    case RtemsClockReqGetTod_Pre_LeapYear_Leap400: {
      /*
       * While the year of the CLOCK_REALTIME is divisible by 400.
       */
      ctx->leap_year = LEAP_YEAR_400;
      break;
    }

    case RtemsClockReqGetTod_Pre_LeapYear_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Pre_Month_Prepare(
  RtemsClockReqGetTod_Context  *ctx,
  RtemsClockReqGetTod_Pre_Month state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Pre_Month_January: {
      /*
       * While the month of the CLOCK_REALTIME is January.
       */
      ctx->month = 1;
      break;
    }

    case RtemsClockReqGetTod_Pre_Month_February: {
      /*
       * While the month of the CLOCK_REALTIME is February.
       */
      ctx->month = 2;
      break;
    }

    case RtemsClockReqGetTod_Pre_Month_Between: {
      /*
       * While the month of the CLOCK_REALTIME is after February and before
       * December.
       */
      ctx->month = 4;
      break;
    }

    case RtemsClockReqGetTod_Pre_Month_December: {
      /*
       * While the month of the CLOCK_REALTIME is December.
       */
      ctx->month = 12;
      break;
    }

    case RtemsClockReqGetTod_Pre_Month_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Pre_Day_Prepare(
  RtemsClockReqGetTod_Context *ctx,
  RtemsClockReqGetTod_Pre_Day  state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Pre_Day_First: {
      /*
       * While the day of the CLOCK_REALTIME is the first day of the month.
       */
      ctx->day = DAY_FIRST;
      break;
    }

    case RtemsClockReqGetTod_Pre_Day_Between: {
      /*
       * While the day of the CLOCK_REALTIME is after the first day and before
       * the last day of the month.
       */
      ctx->day = DAY_BETWEEN;
      break;
    }

    case RtemsClockReqGetTod_Pre_Day_Last: {
      /*
       * While the day of the CLOCK_REALTIME is the last day of the month.
       */
      ctx->day = DAY_LAST;
      break;
    }

    case RtemsClockReqGetTod_Pre_Day_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Pre_DayPart_Prepare(
  RtemsClockReqGetTod_Context    *ctx,
  RtemsClockReqGetTod_Pre_DayPart state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Pre_DayPart_Begin: {
      /*
       * While the CLOCK_REALTIME is in the first clock tick of a day.
       */
      ctx->day_part = DAY_PART_BEGIN;
      break;
    }

    case RtemsClockReqGetTod_Pre_DayPart_Between: {
      /*
       * While the CLOCK_REALTIME is after the first clock tick and before the
       * last clock tick of a day.
       */
      ctx->day_part = DAY_PART_BETWEEN;
      break;
    }

    case RtemsClockReqGetTod_Pre_DayPart_End: {
      /*
       * While the CLOCK_REALTIME is in the last clock tick of a day.
       */
      ctx->day_part = DAY_PART_END;
      break;
    }

    case RtemsClockReqGetTod_Pre_DayPart_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Pre_Param_Prepare(
  RtemsClockReqGetTod_Context  *ctx,
  RtemsClockReqGetTod_Pre_Param state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Pre_Param_Valid: {
      /*
       * While the `time_of_day` parameter references an object of type
       * rtems_time_of_day.
       */
      ctx->tod_ref = &ctx->tod;
      break;
    }

    case RtemsClockReqGetTod_Pre_Param_Null: {
      /*
       * While the `time_of_day` parameter is equal to NULL.
       */
      ctx->tod_ref = NULL;
      break;
    }

    case RtemsClockReqGetTod_Pre_Param_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Post_Status_Check(
  RtemsClockReqGetTod_Context    *ctx,
  RtemsClockReqGetTod_Post_Status state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Post_Status_Ok: {
      /*
       * The return status of rtems_clock_get_tod() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsClockReqGetTod_Post_Status_InvAddr: {
      /*
       * The return status of rtems_clock_get_tod() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsClockReqGetTod_Post_Status_NotDef: {
      /*
       * The return status of rtems_clock_get_tod() shall be RTEMS_NOT_DEFINED.
       */
      T_rsc( ctx->status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsClockReqGetTod_Post_Status_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Post_Value_Check(
  RtemsClockReqGetTod_Context   *ctx,
  RtemsClockReqGetTod_Post_Value state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Post_Value_TimeOfDay: {
      /*
       * The object referenced by the `time_of_day` parameter shall be set to
       * the time of day of the CLOCK_REALTIME at a point in time during the
       * rtems_clock_get_tod() call.
       */
      T_eq_u32( ctx->tod.year, ctx->expected.year );
      T_eq_u32( ctx->tod.month, ctx->expected.month );
      T_eq_u32( ctx->tod.day, ctx->expected.day );
      T_eq_u32( ctx->tod.hour, ctx->expected.hour );
      T_eq_u32( ctx->tod.minute, ctx->expected.minute );
      T_eq_u32( ctx->tod.second, ctx->expected.second );
      T_eq_u32( ctx->tod.ticks, ctx->expected.ticks );
      break;
    }

    case RtemsClockReqGetTod_Post_Value_Unchanged: {
      /*
       * The object referenced by the `time_of_day` parameter shall not be
       * modified by the rtems_clock_get_tod() call.
       */
      T_eq_u32( ctx->tod.year, 1 );
      T_eq_u32( ctx->tod.month, 1 );
      T_eq_u32( ctx->tod.day, 1 );
      T_eq_u32( ctx->tod.hour, 1 );
      T_eq_u32( ctx->tod.minute, 1 );
      T_eq_u32( ctx->tod.second, 1 );
      T_eq_u32( ctx->tod.ticks, 1 );
      break;
    }

    case RtemsClockReqGetTod_Post_Value_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Prepare( RtemsClockReqGetTod_Context *ctx )
{
  ctx->tod = (rtems_time_of_day) { 1, 1, 1, 1, 1, 1, 1 };
}

static void RtemsClockReqGetTod_Action( RtemsClockReqGetTod_Context *ctx )
{
  if ( ctx->set ) {
    SetClock( ctx );
  } else {
    UnsetClock();
  }

  ctx->status = rtems_clock_get_tod( ctx->tod_ref );
}

static void RtemsClockReqGetTod_Cleanup( void )
{
  ISR_lock_Context lock_context;
  Status_Control   status;

  _Objects_Allocator_lock();
  _TOD_Acquire( &lock_context );
  status = _TOD_Set( &ts_earliest, &lock_context );
  _Objects_Allocator_unlock();
  T_eq_int( status, STATUS_SUCCESSFUL );
  UnsetClock();
}

/* clang-format off */

static const RtemsClockReqGetTod_Entry
RtemsClockReqGetTod_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsClockReqGetTod_Post_Status_NA,
    RtemsClockReqGetTod_Post_Value_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsClockReqGetTod_Post_Status_InvAddr,
    RtemsClockReqGetTod_Post_Value_Unchanged },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsClockReqGetTod_Post_Status_Ok,
    RtemsClockReqGetTod_Post_Value_TimeOfDay },
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsClockReqGetTod_Post_Status_NotDef,
    RtemsClockReqGetTod_Post_Value_Unchanged },
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsClockReqGetTod_Post_Status_NA,
    RtemsClockReqGetTod_Post_Value_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsClockReqGetTod_Post_Status_NA,
    RtemsClockReqGetTod_Post_Value_NA }
};

static const uint8_t
RtemsClockReqGetTod_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1,
  2, 1, 2, 1, 2, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 1,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
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
  0, 0, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 3, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t RtemsClockReqGetTod_Scope( void *arg, char *buf, size_t n )
{
  RtemsClockReqGetTod_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsClockReqGetTod_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsClockReqGetTod_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsClockReqGetTod_Scope,
  .initial_context = &RtemsClockReqGetTod_Instance
};

static inline RtemsClockReqGetTod_Entry RtemsClockReqGetTod_PopEntry(
  RtemsClockReqGetTod_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsClockReqGetTod_Entries[ RtemsClockReqGetTod_Map[ index ] ];
}

static void RtemsClockReqGetTod_TestVariant( RtemsClockReqGetTod_Context *ctx )
{
  RtemsClockReqGetTod_Pre_Set_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsClockReqGetTod_Pre_Year_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsClockReqGetTod_Pre_LeapYear_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsClockReqGetTod_Pre_Month_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsClockReqGetTod_Pre_Day_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsClockReqGetTod_Pre_DayPart_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsClockReqGetTod_Pre_Param_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsClockReqGetTod_Action( ctx );
  RtemsClockReqGetTod_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsClockReqGetTod_Post_Value_Check( ctx, ctx->Map.entry.Post_Value );
}

/**
 * @fn void T_case_body_RtemsClockReqGetTod( void )
 */
T_TEST_CASE_FIXTURE( RtemsClockReqGetTod, &RtemsClockReqGetTod_Fixture )
{
  RtemsClockReqGetTod_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsClockReqGetTod_Pre_Set_Yes;
    ctx->Map.pcs[ 0 ] < RtemsClockReqGetTod_Pre_Set_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsClockReqGetTod_Pre_Year_Earliest;
      ctx->Map.pcs[ 1 ] < RtemsClockReqGetTod_Pre_Year_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsClockReqGetTod_Pre_LeapYear_Common;
        ctx->Map.pcs[ 2 ] < RtemsClockReqGetTod_Pre_LeapYear_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsClockReqGetTod_Pre_Month_January;
          ctx->Map.pcs[ 3 ] < RtemsClockReqGetTod_Pre_Month_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsClockReqGetTod_Pre_Day_First;
            ctx->Map.pcs[ 4 ] < RtemsClockReqGetTod_Pre_Day_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsClockReqGetTod_Pre_DayPart_Begin;
              ctx->Map.pcs[ 5 ] < RtemsClockReqGetTod_Pre_DayPart_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = RtemsClockReqGetTod_Pre_Param_Valid;
                ctx->Map.pcs[ 6 ] < RtemsClockReqGetTod_Pre_Param_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                ctx->Map.entry = RtemsClockReqGetTod_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                RtemsClockReqGetTod_Prepare( ctx );
                RtemsClockReqGetTod_TestVariant( ctx );
                RtemsClockReqGetTod_Cleanup();
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
