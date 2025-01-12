/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqGetPriority
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#ifndef _TR_INTR_GET_PRIORITY_H
#define _TR_INTR_GET_PRIORITY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RtemsIntrReqGetPriority
 *
 * @{
 */

typedef enum {
  RtemsIntrReqGetPriority_Pre_Vector_Valid,
  RtemsIntrReqGetPriority_Pre_Vector_Invalid,
  RtemsIntrReqGetPriority_Pre_Vector_NA
} RtemsIntrReqGetPriority_Pre_Vector;

typedef enum {
  RtemsIntrReqGetPriority_Pre_Priority_Valid,
  RtemsIntrReqGetPriority_Pre_Priority_Null,
  RtemsIntrReqGetPriority_Pre_Priority_NA
} RtemsIntrReqGetPriority_Pre_Priority;

typedef enum {
  RtemsIntrReqGetPriority_Pre_CanGetPriority_Yes,
  RtemsIntrReqGetPriority_Pre_CanGetPriority_No,
  RtemsIntrReqGetPriority_Pre_CanGetPriority_NA
} RtemsIntrReqGetPriority_Pre_CanGetPriority;

typedef enum {
  RtemsIntrReqGetPriority_Post_Status_Ok,
  RtemsIntrReqGetPriority_Post_Status_InvAddr,
  RtemsIntrReqGetPriority_Post_Status_InvId,
  RtemsIntrReqGetPriority_Post_Status_Unsat,
  RtemsIntrReqGetPriority_Post_Status_NA
} RtemsIntrReqGetPriority_Post_Status;

typedef enum {
  RtemsIntrReqGetPriority_Post_PriorityObj_Set,
  RtemsIntrReqGetPriority_Post_PriorityObj_Nop,
  RtemsIntrReqGetPriority_Post_PriorityObj_NA
} RtemsIntrReqGetPriority_Post_PriorityObj;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in] valid_vector is a valid interrupt vector number.
 *
 * @param[in] can_get_priority is true, if getting the priority is supported.
 */
void RtemsIntrReqGetPriority_Run(
  rtems_vector_number valid_vector,
  bool                can_get_priority
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_INTR_GET_PRIORITY_H */
