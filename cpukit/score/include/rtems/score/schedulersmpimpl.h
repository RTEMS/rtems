/**
 * @file
 *
 * @brief SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerSMP
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSMPIMPL_H
#define _RTEMS_SCORE_SCHEDULERSMPIMPL_H

#include <rtems/score/schedulersmp.h>
#include <rtems/score/scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup ScoreSchedulerSMP
 *
 * @{
 */

static inline Scheduler_SMP_Control *_Scheduler_SMP_Instance( void )
{
  return _Scheduler.information;
}

static inline void _Scheduler_SMP_Allocate_processor(
  Thread_Control *scheduled,
  Thread_Control *victim
)
{
  Per_CPU_Control *cpu_of_scheduled = scheduled->cpu;
  Per_CPU_Control *cpu_of_victim = victim->cpu;
  Thread_Control *heir;

  scheduled->is_scheduled = true;
  victim->is_scheduled = false;

  _Per_CPU_Acquire( cpu_of_scheduled );

  if ( scheduled->is_executing ) {
    heir = cpu_of_scheduled->heir;
    cpu_of_scheduled->heir = scheduled;
  } else {
    heir = scheduled;
  }

  _Per_CPU_Release( cpu_of_scheduled );

  if ( heir != victim ) {
    const Per_CPU_Control *cpu_of_executing = _Per_CPU_Get();

    heir->cpu = cpu_of_victim;

    /*
     * FIXME: Here we need atomic store operations with a relaxed memory order.
     * The _CPU_SMP_Send_interrupt() will ensure that the change can be
     * observed consistently.
     */
    cpu_of_victim->heir = heir;
    cpu_of_victim->dispatch_necessary = true;

    if ( cpu_of_victim != cpu_of_executing ) {
      _Per_CPU_Send_interrupt( cpu_of_victim );
    }
  }
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMPIMPL_H */
