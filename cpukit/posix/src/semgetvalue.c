/**
 * @file
 *
 * @brief Get the Value of a Semaphore
 * @ingroup POSIX_SEMAPHORE
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/semaphoreimpl.h>

int sem_getvalue(
  sem_t  *__restrict _sem,
  int    *__restrict sval
)
{
  Sem_Control          *sem;
  ISR_Level             level;
  Thread_queue_Context  queue_context;

  POSIX_SEMAPHORE_VALIDATE_OBJECT( _sem );

  sem = _Sem_Get( &_sem->_Semaphore );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  _Sem_Queue_acquire_critical( sem, &queue_context );
  *sval = (int) sem->count;
  _Sem_Queue_release( sem, level, &queue_context );
  return 0;
}
