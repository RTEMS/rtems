/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreMtxReqSurrender
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

#ifndef _TR_MTX_SURRENDER_H
#define _TR_MTX_SURRENDER_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreMtxReqSurrender
 *
 * @{
 */

typedef enum {
  ScoreMtxReqSurrender_Pre_Protocol_None,
  ScoreMtxReqSurrender_Pre_Protocol_Inherit,
  ScoreMtxReqSurrender_Pre_Protocol_Ceiling,
  ScoreMtxReqSurrender_Pre_Protocol_MrsP,
  ScoreMtxReqSurrender_Pre_Protocol_NA
} ScoreMtxReqSurrender_Pre_Protocol;

typedef enum {
  ScoreMtxReqSurrender_Pre_Discipline_FIFO,
  ScoreMtxReqSurrender_Pre_Discipline_Priority,
  ScoreMtxReqSurrender_Pre_Discipline_NA
} ScoreMtxReqSurrender_Pre_Discipline;

typedef enum {
  ScoreMtxReqSurrender_Pre_Recursive_Allowed,
  ScoreMtxReqSurrender_Pre_Recursive_NotAllowed,
  ScoreMtxReqSurrender_Pre_Recursive_NA
} ScoreMtxReqSurrender_Pre_Recursive;

typedef enum {
  ScoreMtxReqSurrender_Pre_OwnerCheck_Yes,
  ScoreMtxReqSurrender_Pre_OwnerCheck_No,
  ScoreMtxReqSurrender_Pre_OwnerCheck_NA
} ScoreMtxReqSurrender_Pre_OwnerCheck;

typedef enum {
  ScoreMtxReqSurrender_Pre_Owner_None,
  ScoreMtxReqSurrender_Pre_Owner_Caller,
  ScoreMtxReqSurrender_Pre_Owner_Other,
  ScoreMtxReqSurrender_Pre_Owner_NA
} ScoreMtxReqSurrender_Pre_Owner;

typedef enum {
  ScoreMtxReqSurrender_Pre_Nested_Yes,
  ScoreMtxReqSurrender_Pre_Nested_No,
  ScoreMtxReqSurrender_Pre_Nested_NA
} ScoreMtxReqSurrender_Pre_Nested;

typedef enum {
  ScoreMtxReqSurrender_Pre_Blocked_Yes,
  ScoreMtxReqSurrender_Pre_Blocked_No,
  ScoreMtxReqSurrender_Pre_Blocked_NA
} ScoreMtxReqSurrender_Pre_Blocked;

typedef enum {
  ScoreMtxReqSurrender_Pre_Priority_High,
  ScoreMtxReqSurrender_Pre_Priority_Equal,
  ScoreMtxReqSurrender_Pre_Priority_Low,
  ScoreMtxReqSurrender_Pre_Priority_NA
} ScoreMtxReqSurrender_Pre_Priority;

typedef enum {
  ScoreMtxReqSurrender_Post_Status_Ok,
  ScoreMtxReqSurrender_Post_Status_NotOwner,
  ScoreMtxReqSurrender_Post_Status_NA
} ScoreMtxReqSurrender_Post_Status;

typedef enum {
  ScoreMtxReqSurrender_Post_Owner_None,
  ScoreMtxReqSurrender_Post_Owner_Caller,
  ScoreMtxReqSurrender_Post_Owner_Other,
  ScoreMtxReqSurrender_Post_Owner_First,
  ScoreMtxReqSurrender_Post_Owner_NA
} ScoreMtxReqSurrender_Post_Owner;

typedef enum {
  ScoreMtxReqSurrender_Post_Surrender_Nop,
  ScoreMtxReqSurrender_Post_Surrender_FIFO,
  ScoreMtxReqSurrender_Post_Surrender_Priority,
  ScoreMtxReqSurrender_Post_Surrender_PriorityInherit,
  ScoreMtxReqSurrender_Post_Surrender_MrsP,
  ScoreMtxReqSurrender_Post_Surrender_NA
} ScoreMtxReqSurrender_Post_Surrender;

typedef enum {
  ScoreMtxReqSurrender_Post_Priority_Nop,
  ScoreMtxReqSurrender_Post_Priority_Low,
  ScoreMtxReqSurrender_Post_Priority_NA
} ScoreMtxReqSurrender_Post_Priority;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue context.
 */
void ScoreMtxReqSurrender_Run( TQMtxContext *tq_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_MTX_SURRENDER_H */
