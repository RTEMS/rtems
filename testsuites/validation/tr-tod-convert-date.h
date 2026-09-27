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

#ifndef _TR_TOD_CONVERT_DATE_H
#define _TR_TOD_CONVERT_DATE_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTodReqConvertDate
 *
 * @{
 */

typedef enum {
  ScoreTodReqConvertDate_Pre_Year_Zero,
  ScoreTodReqConvertDate_Pre_Year_TooEarly,
  ScoreTodReqConvertDate_Pre_Year_Earliest,
  ScoreTodReqConvertDate_Pre_Year_Valid,
  ScoreTodReqConvertDate_Pre_Year_Latest,
  ScoreTodReqConvertDate_Pre_Year_TooLate,
  ScoreTodReqConvertDate_Pre_Year_Max,
  ScoreTodReqConvertDate_Pre_Year_NA
} ScoreTodReqConvertDate_Pre_Year;

typedef enum {
  ScoreTodReqConvertDate_Pre_LeapYear_Common,
  ScoreTodReqConvertDate_Pre_LeapYear_Leap4,
  ScoreTodReqConvertDate_Pre_LeapYear_Century,
  ScoreTodReqConvertDate_Pre_LeapYear_Leap400,
  ScoreTodReqConvertDate_Pre_LeapYear_NA
} ScoreTodReqConvertDate_Pre_LeapYear;

typedef enum {
  ScoreTodReqConvertDate_Pre_Month_Zero,
  ScoreTodReqConvertDate_Pre_Month_January,
  ScoreTodReqConvertDate_Pre_Month_February,
  ScoreTodReqConvertDate_Pre_Month_Between,
  ScoreTodReqConvertDate_Pre_Month_December,
  ScoreTodReqConvertDate_Pre_Month_Above,
  ScoreTodReqConvertDate_Pre_Month_Max,
  ScoreTodReqConvertDate_Pre_Month_NA
} ScoreTodReqConvertDate_Pre_Month;

typedef enum {
  ScoreTodReqConvertDate_Pre_Day_Zero,
  ScoreTodReqConvertDate_Pre_Day_First,
  ScoreTodReqConvertDate_Pre_Day_Between,
  ScoreTodReqConvertDate_Pre_Day_Last,
  ScoreTodReqConvertDate_Pre_Day_Above,
  ScoreTodReqConvertDate_Pre_Day_Max,
  ScoreTodReqConvertDate_Pre_Day_NA
} ScoreTodReqConvertDate_Pre_Day;

typedef enum {
  ScoreTodReqConvertDate_Post_Result_Accept,
  ScoreTodReqConvertDate_Post_Result_Reject,
  ScoreTodReqConvertDate_Post_Result_NA
} ScoreTodReqConvertDate_Post_Result;

typedef enum {
  ScoreTodReqConvertDate_Post_Effect_Seconds,
  ScoreTodReqConvertDate_Post_Effect_Nop,
  ScoreTodReqConvertDate_Post_Effect_NA
} ScoreTodReqConvertDate_Post_Effect;

/**
 * @brief This constant indicates that the directive call had no effect.
 */
#define TOD_NO_EFFECT INT64_MIN

/**
 * @brief Calls the directive with the time of day.
 *
 * @param arg is the argument of the handler.
 *
 * @param time_of_day is the time of day.
 *
 * @param[out] seconds is the seconds since the Epoch which the directive
 *   call used.  The handler shall not change the object, if the directive
 *   call had no effect.
 *
 * @return Returns true, if the directive call accepted the time of day,
 *   otherwise false.
 */
typedef bool ( *TODCall )(
  void                    *arg,
  const rtems_time_of_day *time_of_day,
  int64_t                 *seconds
);

/**
 * @brief Runs the parameterized test case.
 *
 * @param call is the handler which calls the directive.
 *
 * @param arg is the argument of the handler.
 */
void ScoreTodReqConvertDate_Run( TODCall call, void *arg );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TOD_CONVERT_DATE_H */
