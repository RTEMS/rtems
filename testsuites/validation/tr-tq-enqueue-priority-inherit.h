/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueuePriorityInherit
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

#ifndef _TR_TQ_ENQUEUE_PRIORITY_INHERIT_H
#define _TR_TQ_ENQUEUE_PRIORITY_INHERIT_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqEnqueuePriorityInherit
 *
 * @{
 */

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_One,
  ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_Two,
  ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_Three,
  ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_More,
  ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_None,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_High,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_Equal,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_Low,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_None,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_Only,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_Before,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_After,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Vital,
  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Dispensable,
  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Vital,
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Dispensable,
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NotEnqueued,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_FIFO,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_Priority,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_PriorityInherit,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialFirst,
  ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialLast,
  ScoreTqReqEnqueuePriorityInherit_Post_Position_First,
  ScoreTqReqEnqueuePriorityInherit_Post_Position_Second,
  ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstFirst,
  ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondFirst,
  ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstLast,
  ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondLast,
  ScoreTqReqEnqueuePriorityInherit_Post_Position_NA
} ScoreTqReqEnqueuePriorityInherit_Post_Position;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA
} ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA
} ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA
} ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA
} ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue context.
 */
void ScoreTqReqEnqueuePriorityInherit_Run( TQContext *tq_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_ENQUEUE_PRIORITY_INHERIT_H */
