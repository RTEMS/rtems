/*
 * Copyright (c) 2015, 2017 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SCORE_SEMAPHOREIMPL_H
#define _RTEMS_SCORE_SEMAPHOREIMPL_H

#include <sys/lock.h>

#include <rtems/score/percpu.h>
#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  Thread_queue_Syslock_queue Queue;
  unsigned int count;
} Sem_Control;

#define SEMAPHORE_TQ_OPERATIONS &_Thread_queue_Operations_priority

static inline Sem_Control *_Sem_Get( struct _Semaphore_Control *_sem )
{
  return (Sem_Control *) _sem;
}

static inline Thread_Control *_Sem_Queue_acquire_critical(
  Sem_Control          *sem,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *executing;

  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &sem->Queue.Queue,
    &executing->Potpourri_stats,
    &queue_context->Lock_context.Lock_context
  );

  return executing;
}

static inline void _Sem_Queue_release(
  Sem_Control          *sem,
  ISR_Level             level,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_release_critical(
    &sem->Queue.Queue,
    &queue_context->Lock_context.Lock_context
  );
  _ISR_Local_enable( level );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SEMAPHOREIMPL_H */
