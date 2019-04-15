/**
 * @file
 *
 * @ingroup RTEMSScoreSemaphore
 *
 * @brief Semaphore Implementation
 */

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

/**
 * @addtogroup RTEMSScoreSemaphore
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  Thread_queue_Syslock_queue Queue;
  unsigned int count;
} Sem_Control;

#define SEMAPHORE_TQ_OPERATIONS &_Thread_queue_Operations_priority

/**
 * @brief Gets the Sem_Control * of the semaphore.
 *
 * @param sem The Semaphore_Control * to cast to Sem_Control *.
 *
 * @return @a sem cast to Sem_Control *.
 */
static inline Sem_Control *_Sem_Get( struct _Semaphore_Control *_sem )
{
  return (Sem_Control *) _sem;
}

/**
 * @brief Acquires the semaphore queue critical.
 *
 * This routine acquires the semaphore.
 *
 * @param[in, out] sem The semaphore to acquire the queue of.
 * @param queue_context The thread queue context.
 *
 * @return The executing thread.
 */
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

/**
 * @brief Releases the semaphore queue.
 *
 * @param[in, out] sem The semaphore to release the queue of.
 * @param level The interrupt level value to restore the interrupt status on the processor.
 * @param queue_context The thread queue context.
 */
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

/** @} */

#endif /* _RTEMS_SCORE_SEMAPHOREIMPL_H */
