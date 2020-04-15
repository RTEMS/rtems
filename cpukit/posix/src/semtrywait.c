/**
 *  @file
 *
 *  @brief Lock a Semaphore 
 *  @ingroup POSIX_SEMAPHORE
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/semaphoreimpl.h>

int sem_trywait( sem_t *_sem )
{
  Sem_Control          *sem;
  Thread_queue_Context  queue_context;
  ISR_Level             level;
  unsigned int          count;

  POSIX_SEMAPHORE_VALIDATE_OBJECT( _sem );

  sem = _Sem_Get( &_sem->_Semaphore );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  _Sem_Queue_acquire_critical( sem, &queue_context );

  count = sem->count;
  if ( RTEMS_PREDICT_TRUE( count > 0 ) ) {
    sem->count = count - 1;
    _Sem_Queue_release( sem, level, &queue_context );
    return 0;
  } else {
    _Sem_Queue_release( sem, level, &queue_context );
    rtems_set_errno_and_return_minus_one( EAGAIN );
  }
}
