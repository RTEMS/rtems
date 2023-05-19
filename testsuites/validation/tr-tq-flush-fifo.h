/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqFlushFifo
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

#ifndef _TR_TQ_FLUSH_FIFO_H
#define _TR_TQ_FLUSH_FIFO_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqFlushFifo
 *
 * @{
 */

typedef enum {
  ScoreTqReqFlushFifo_Pre_MayStop_Yes,
  ScoreTqReqFlushFifo_Pre_MayStop_No,
  ScoreTqReqFlushFifo_Pre_MayStop_NA
} ScoreTqReqFlushFifo_Pre_MayStop;

typedef enum {
  ScoreTqReqFlushFifo_Pre_QueueEmpty_Yes,
  ScoreTqReqFlushFifo_Pre_QueueEmpty_No,
  ScoreTqReqFlushFifo_Pre_QueueEmpty_NA
} ScoreTqReqFlushFifo_Pre_QueueEmpty;

typedef enum {
  ScoreTqReqFlushFifo_Pre_Stop_Yes,
  ScoreTqReqFlushFifo_Pre_Stop_No,
  ScoreTqReqFlushFifo_Pre_Stop_NA
} ScoreTqReqFlushFifo_Pre_Stop;

typedef enum {
  ScoreTqReqFlushFifo_Pre_WaitState_Blocked,
  ScoreTqReqFlushFifo_Pre_WaitState_IntendToBlock,
  ScoreTqReqFlushFifo_Pre_WaitState_NA
} ScoreTqReqFlushFifo_Pre_WaitState;

typedef enum {
  ScoreTqReqFlushFifo_Post_Operation_Nop,
  ScoreTqReqFlushFifo_Post_Operation_ExtractAll,
  ScoreTqReqFlushFifo_Post_Operation_ExtractPartial,
  ScoreTqReqFlushFifo_Post_Operation_NA
} ScoreTqReqFlushFifo_Post_Operation;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue test context.
 *
 * @param may_stop is true, if a partial flush is supported.
 */
void ScoreTqReqFlushFifo_Run( TQContext *tq_ctx, bool may_stop );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_FLUSH_FIFO_H */
