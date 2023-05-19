/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqSurrender
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

#ifndef _TR_TQ_SURRENDER_H
#define _TR_TQ_SURRENDER_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqSurrender
 *
 * @{
 */

typedef enum {
  ScoreTqReqSurrender_Pre_HasOwner_Yes,
  ScoreTqReqSurrender_Pre_HasOwner_No,
  ScoreTqReqSurrender_Pre_HasOwner_NA
} ScoreTqReqSurrender_Pre_HasOwner;

typedef enum {
  ScoreTqReqSurrender_Pre_Discipline_FIFO,
  ScoreTqReqSurrender_Pre_Discipline_Priority,
  ScoreTqReqSurrender_Pre_Discipline_NA
} ScoreTqReqSurrender_Pre_Discipline;

typedef enum {
  ScoreTqReqSurrender_Pre_WaitState_Blocked,
  ScoreTqReqSurrender_Pre_WaitState_IntendToBlock,
  ScoreTqReqSurrender_Pre_WaitState_NA
} ScoreTqReqSurrender_Pre_WaitState;

typedef enum {
  ScoreTqReqSurrender_Post_Dequeue_FIFO,
  ScoreTqReqSurrender_Post_Dequeue_Priority,
  ScoreTqReqSurrender_Post_Dequeue_NA
} ScoreTqReqSurrender_Post_Dequeue;

typedef enum {
  ScoreTqReqSurrender_Post_Unblock_Yes,
  ScoreTqReqSurrender_Post_Unblock_No,
  ScoreTqReqSurrender_Post_Unblock_NA
} ScoreTqReqSurrender_Post_Unblock;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue test context.
 */
void ScoreTqReqSurrender_Run( TQContext *tq_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_SURRENDER_H */
