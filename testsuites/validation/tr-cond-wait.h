/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCondReqWait
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

#ifndef _TR_COND_WAIT_H
#define _TR_COND_WAIT_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreCondReqWait
 *
 * @{
 */

typedef enum {
  ScoreCondReqWait_Pre_Recursive_No,
  ScoreCondReqWait_Pre_Recursive_Yes,
  ScoreCondReqWait_Pre_Recursive_NA
} ScoreCondReqWait_Pre_Recursive;

typedef enum {
  ScoreCondReqWait_Pre_Wait_Forever,
  ScoreCondReqWait_Pre_Wait_Timed,
  ScoreCondReqWait_Pre_Wait_NA
} ScoreCondReqWait_Pre_Wait;

typedef enum {
  ScoreCondReqWait_Pre_Nest_One,
  ScoreCondReqWait_Pre_Nest_Many,
  ScoreCondReqWait_Pre_Nest_NA
} ScoreCondReqWait_Pre_Nest;

typedef enum {
  ScoreCondReqWait_Post_OwnerDuringCall_None,
  ScoreCondReqWait_Post_OwnerDuringCall_NA
} ScoreCondReqWait_Post_OwnerDuringCall;

typedef enum {
  ScoreCondReqWait_Post_WaitStateDuringCall_Condition,
  ScoreCondReqWait_Post_WaitStateDuringCall_NA
} ScoreCondReqWait_Post_WaitStateDuringCall;

typedef enum {
  ScoreCondReqWait_Post_OwnerAfterCall_Caller,
  ScoreCondReqWait_Post_OwnerAfterCall_NA
} ScoreCondReqWait_Post_OwnerAfterCall;

typedef enum {
  ScoreCondReqWait_Post_NestAfterCall_Same,
  ScoreCondReqWait_Post_NestAfterCall_NA
} ScoreCondReqWait_Post_NestAfterCall;

typedef enum {
  ScoreCondReqWait_Post_Status_Ok,
  ScoreCondReqWait_Post_Status_NA
} ScoreCondReqWait_Post_Status;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue test context.
 */
void ScoreCondReqWait_Run( TQCondContext *tq_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_COND_WAIT_H */
