/**
 *  @file
 *
 *  @brief Destroy a Barrier Object 
 *  @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2017 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/barrierimpl.h>

int pthread_barrier_destroy( pthread_barrier_t *_barrier )
{
  POSIX_Barrier_Control *barrier;
  Thread_queue_Context   queue_context;

  POSIX_BARRIER_VALIDATE_OBJECT( _barrier );

  barrier = _POSIX_Barrier_Get( _barrier );

  _POSIX_Barrier_Queue_acquire( barrier, &queue_context );

  if ( barrier->waiting_threads != 0 ) {
    _POSIX_Barrier_Queue_release( barrier, &queue_context );
    return EBUSY;
  }

  barrier->flags = 0;
  _POSIX_Barrier_Queue_release( barrier, &queue_context );
  return 0;
}
