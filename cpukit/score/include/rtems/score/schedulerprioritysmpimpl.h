/**
 * @file
 *
 * @ingroup ScoreSchedulerPrioritySMP
 *
 * @brief Deterministic Priority SMP Scheduler API
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITYSMPIMPL_H
#define _RTEMS_SCORE_SCHEDULERPRIORITYSMPIMPL_H

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/schedulersmp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup ScoreSchedulerPrioritySMP
 * @{
 */

Scheduler_priority_SMP_Context *_Scheduler_priority_SMP_Get_self(
  Scheduler_Context *context
);

Scheduler_priority_SMP_Node *_Scheduler_priority_SMP_Node_get(
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Insert_ready_fifo(
  Scheduler_Context *context,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Insert_ready_lifo(
  Scheduler_Context *context,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Thread_Control *scheduled_to_ready
);

void _Scheduler_priority_SMP_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Thread_Control *ready_to_scheduled
);

void _Scheduler_priority_SMP_Extract_from_ready(
  Scheduler_Context *context,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Do_update(
  Scheduler_Context *context,
  Scheduler_Node *base_node,
  Priority_Control new_priority
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERPRIORITYSMPIMPL_H */
