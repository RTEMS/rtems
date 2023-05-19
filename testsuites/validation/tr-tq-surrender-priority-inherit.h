/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqSurrenderPriorityInherit
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

#ifndef _TR_TQ_SURRENDER_PRIORITY_INHERIT_H
#define _TR_TQ_SURRENDER_PRIORITY_INHERIT_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqSurrenderPriorityInherit
 *
 * @{
 */

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_One,
  ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_Two,
  ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_More,
  ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_NA
} ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_Vital,
  ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_Dispensable,
  ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_NA
} ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_Vital,
  ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_Dispensable,
  ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_NA
} ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_Home,
  ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_Helping,
  ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_NA
} ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_Vital,
  ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_Dispensable,
  ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_NA
} ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_Vital,
  ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_Dispensable,
  ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_NA
} ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_Yes,
  ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_No,
  ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_NA
} ScoreTqReqSurrenderPriorityInherit_Pre_Suspended;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_Blocked,
  ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_IntendToBlock,
  ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_NA
} ScoreTqReqSurrenderPriorityInherit_Pre_WaitState;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
  ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA
} ScoreTqReqSurrenderPriorityInherit_Post_Dequeue;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
  ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
  ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA
} ScoreTqReqSurrenderPriorityInherit_Post_Unblock;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
  ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
  ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA
} ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
  ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
  ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA
} ScoreTqReqSurrenderPriorityInherit_Post_NewPriority;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
  ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
  ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA
} ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
  ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
  ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA
} ScoreTqReqSurrenderPriorityInherit_Post_AddHelper;

typedef enum {
  ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes,
  ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No,
  ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA
} ScoreTqReqSurrenderPriorityInherit_Post_Suspended;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue test context.
 */
void ScoreTqReqSurrenderPriorityInherit_Run( TQContext *tq_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_SURRENDER_PRIORITY_INHERIT_H */
