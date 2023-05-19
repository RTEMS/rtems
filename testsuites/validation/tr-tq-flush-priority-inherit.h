/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqFlushPriorityInherit
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

#ifndef _TR_TQ_FLUSH_PRIORITY_INHERIT_H
#define _TR_TQ_FLUSH_PRIORITY_INHERIT_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqFlushPriorityInherit
 *
 * @{
 */

typedef enum {
  ScoreTqReqFlushPriorityInherit_Pre_Priority_Minimum,
  ScoreTqReqFlushPriorityInherit_Pre_Priority_NotMinimum,
  ScoreTqReqFlushPriorityInherit_Pre_Priority_NA
} ScoreTqReqFlushPriorityInherit_Pre_Priority;

typedef enum {
  ScoreTqReqFlushPriorityInherit_Pre_Queue_Empty,
  ScoreTqReqFlushPriorityInherit_Pre_Queue_NonEmpty,
  ScoreTqReqFlushPriorityInherit_Pre_Queue_NA
} ScoreTqReqFlushPriorityInherit_Pre_Queue;

typedef enum {
  ScoreTqReqFlushPriorityInherit_Post_Extract_Nop,
  ScoreTqReqFlushPriorityInherit_Post_Extract_All,
  ScoreTqReqFlushPriorityInherit_Post_Extract_NA
} ScoreTqReqFlushPriorityInherit_Post_Extract;

typedef enum {
  ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_No,
  ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_Yes,
  ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_NA
} ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue test context.
 */
void ScoreTqReqFlushPriorityInherit_Run( TQContext *tq_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_FLUSH_PRIORITY_INHERIT_H */
