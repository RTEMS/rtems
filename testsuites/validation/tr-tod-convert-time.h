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

#ifndef _TR_TOD_CONVERT_TIME_H
#define _TR_TOD_CONVERT_TIME_H

#include <rtems.h>

#include "tr-tod-convert-date.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTodReqConvertTime
 *
 * @{
 */

typedef enum {
  ScoreTodReqConvertTime_Pre_TicksValidation_Enabled,
  ScoreTodReqConvertTime_Pre_TicksValidation_Disabled,
  ScoreTodReqConvertTime_Pre_TicksValidation_NA
} ScoreTodReqConvertTime_Pre_TicksValidation;

typedef enum {
  ScoreTodReqConvertTime_Pre_Hour_First,
  ScoreTodReqConvertTime_Pre_Hour_Between,
  ScoreTodReqConvertTime_Pre_Hour_Last,
  ScoreTodReqConvertTime_Pre_Hour_Above,
  ScoreTodReqConvertTime_Pre_Hour_Max,
  ScoreTodReqConvertTime_Pre_Hour_NA
} ScoreTodReqConvertTime_Pre_Hour;

typedef enum {
  ScoreTodReqConvertTime_Pre_Minute_First,
  ScoreTodReqConvertTime_Pre_Minute_Between,
  ScoreTodReqConvertTime_Pre_Minute_Last,
  ScoreTodReqConvertTime_Pre_Minute_Above,
  ScoreTodReqConvertTime_Pre_Minute_Max,
  ScoreTodReqConvertTime_Pre_Minute_NA
} ScoreTodReqConvertTime_Pre_Minute;

typedef enum {
  ScoreTodReqConvertTime_Pre_Second_First,
  ScoreTodReqConvertTime_Pre_Second_Between,
  ScoreTodReqConvertTime_Pre_Second_Last,
  ScoreTodReqConvertTime_Pre_Second_Above,
  ScoreTodReqConvertTime_Pre_Second_Max,
  ScoreTodReqConvertTime_Pre_Second_NA
} ScoreTodReqConvertTime_Pre_Second;

typedef enum {
  ScoreTodReqConvertTime_Pre_Ticks_First,
  ScoreTodReqConvertTime_Pre_Ticks_Between,
  ScoreTodReqConvertTime_Pre_Ticks_Last,
  ScoreTodReqConvertTime_Pre_Ticks_Above,
  ScoreTodReqConvertTime_Pre_Ticks_Max,
  ScoreTodReqConvertTime_Pre_Ticks_NA
} ScoreTodReqConvertTime_Pre_Ticks;

typedef enum {
  ScoreTodReqConvertTime_Post_Result_Accept,
  ScoreTodReqConvertTime_Post_Result_Reject,
  ScoreTodReqConvertTime_Post_Result_NA
} ScoreTodReqConvertTime_Post_Result;

typedef enum {
  ScoreTodReqConvertTime_Post_Effect_Seconds,
  ScoreTodReqConvertTime_Post_Effect_Nop,
  ScoreTodReqConvertTime_Post_Effect_NA
} ScoreTodReqConvertTime_Post_Effect;

/**
 * @brief Runs the parameterized test case.
 *
 * @param call is the handler which calls the directive.
 *
 * @param arg is the argument of the handler.
 *
 * @param ticks_validation is true, if the directive validates the ticks of the
 *   time of day.
 */
void ScoreTodReqConvertTime_Run(
  TODCall call,
  void   *arg,
  bool    ticks_validation
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TOD_CONVERT_TIME_H */
