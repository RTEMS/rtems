/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqSetPriority
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

#ifndef _TR_INTR_SET_PRIORITY_H
#define _TR_INTR_SET_PRIORITY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RtemsIntrReqSetPriority
 *
 * @{
 */

typedef enum {
  RtemsIntrReqSetPriority_Pre_Vector_Valid,
  RtemsIntrReqSetPriority_Pre_Vector_Invalid,
  RtemsIntrReqSetPriority_Pre_Vector_NA
} RtemsIntrReqSetPriority_Pre_Vector;

typedef enum {
  RtemsIntrReqSetPriority_Pre_Priority_Valid,
  RtemsIntrReqSetPriority_Pre_Priority_Invalid,
  RtemsIntrReqSetPriority_Pre_Priority_NA
} RtemsIntrReqSetPriority_Pre_Priority;

typedef enum {
  RtemsIntrReqSetPriority_Pre_CanSetPriority_Yes,
  RtemsIntrReqSetPriority_Pre_CanSetPriority_No,
  RtemsIntrReqSetPriority_Pre_CanSetPriority_NA
} RtemsIntrReqSetPriority_Pre_CanSetPriority;

typedef enum {
  RtemsIntrReqSetPriority_Post_Status_Ok,
  RtemsIntrReqSetPriority_Post_Status_InvId,
  RtemsIntrReqSetPriority_Post_Status_InvPrio,
  RtemsIntrReqSetPriority_Post_Status_Unsat,
  RtemsIntrReqSetPriority_Post_Status_NA
} RtemsIntrReqSetPriority_Post_Status;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in] valid_vector is a valid interrupt vector number.
 *
 * @param[in] maximum_priority is the maximum supported priority value.
 *
 * @param[in] can_set_priority is true, if setting the priority is supported.
 */
void RtemsIntrReqSetPriority_Run(
  rtems_vector_number valid_vector,
  uint32_t            maximum_priority,
  bool                can_set_priority
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_INTR_SET_PRIORITY_H */
