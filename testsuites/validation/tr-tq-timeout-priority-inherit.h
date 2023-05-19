/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqTimeoutPriorityInherit
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

#ifndef _TR_TQ_TIMEOUT_PRIORITY_INHERIT_H
#define _TR_TQ_TIMEOUT_PRIORITY_INHERIT_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqTimeoutPriorityInherit
 *
 * @{
 */

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_Home,
  ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_Helping,
  ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_NA
} ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_One,
  ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_More,
  ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_NA
} ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Pre_Queue_Only,
  ScoreTqReqTimeoutPriorityInherit_Pre_Queue_Vital,
  ScoreTqReqTimeoutPriorityInherit_Pre_Queue_Dispensable,
  ScoreTqReqTimeoutPriorityInherit_Pre_Queue_NA
} ScoreTqReqTimeoutPriorityInherit_Pre_Queue;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_Only,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_Vital,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_Dispensable,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_NA
} ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_NotEnqueued,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_FIFO,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_Priority,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_PriorityInherit,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_NA
} ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_Only,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_Vital,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_Dispensable,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_NA
} ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_Only,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_Vital,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_Dispensable,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_NA
} ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_Blocked,
  ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_IntendToBlock,
  ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_ReadyAgain,
  ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_NA
} ScoreTqReqTimeoutPriorityInherit_Pre_WaitState;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Post_Status_Ok,
  ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
  ScoreTqReqTimeoutPriorityInherit_Post_Status_NA
} ScoreTqReqTimeoutPriorityInherit_Post_Status;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
  ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
  ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA
} ScoreTqReqTimeoutPriorityInherit_Post_Unblock;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA
} ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority;

typedef enum {
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop,
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Lower,
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Drop,
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA
} ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue test context.
 */
void ScoreTqReqTimeoutPriorityInherit_Run( TQContext *tq_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_TIMEOUT_PRIORITY_INHERIT_H */
