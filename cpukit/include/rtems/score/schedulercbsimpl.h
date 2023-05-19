/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerCBS
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSchedulerCBS which are only used by the implementation.
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_SCHEDULERCBSIMPL_H
#define _RTEMS_SCORE_SCHEDULERCBSIMPL_H

#include <rtems/score/schedulercbs.h>
#include <rtems/score/schedulerimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSScoreSchedulerCBS
 *
 * @{
 */

/**
 * @brief Gets the CBS node of the thread.
 *
 * @param the_thread The thread to get the CBS node of.
 *
 * @return Pointer to the scheduler node of @a the_thread.
 */
static inline Scheduler_CBS_Node *_Scheduler_CBS_Thread_get_node(
  Thread_Control *the_thread
)
{
  return (Scheduler_CBS_Node *) _Thread_Scheduler_get_home_node( the_thread );
}

/**
 * @brief Casts the scheduler node to a scheduler CBS node.
 *
 * @param node The node to be casted to a scheduler CBS node.
 *
 * @return CBS Node pointer to @a node.
 */
static inline Scheduler_CBS_Node *_Scheduler_CBS_Node_downcast(
  Scheduler_Node *node
)
{
  return (Scheduler_CBS_Node *) node;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERCBSIMPL_H */
