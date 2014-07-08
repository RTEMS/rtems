/**
 * @file
 *
 * @brief CBS Scheduler Implementation
 *
 * @ingroup ScoreSchedulerCBS
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
 * @addtogroup ScoreSchedulerCBS
 *
 * @{
 */

RTEMS_INLINE_ROUTINE Scheduler_CBS_Node *_Scheduler_CBS_Thread_get_node(
  Thread_Control *the_thread
)
{
  return (Scheduler_CBS_Node *) _Scheduler_Thread_get_node( the_thread );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERCBSIMPL_H */
