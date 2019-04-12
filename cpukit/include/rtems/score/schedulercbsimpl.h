/**
 * @file
 *
 * @brief CBS Scheduler Implementation
 *
 * @ingroup RTEMSScoreSchedulerCBS
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
RTEMS_INLINE_ROUTINE Scheduler_CBS_Node *_Scheduler_CBS_Thread_get_node(
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
RTEMS_INLINE_ROUTINE Scheduler_CBS_Node *_Scheduler_CBS_Node_downcast(
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
