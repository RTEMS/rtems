/**
 * @file
 *
 * @ingroup RTEMSScoreRWLock
 *
 * @brief
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/corerwlockimpl.h>
#include <rtems/score/threadqimpl.h>

void _CORE_RWLock_Initialize(
  CORE_RWLock_Control *the_rwlock
)
{
  the_rwlock->number_of_readers = 0;
  the_rwlock->current_state = CORE_RWLOCK_UNLOCKED;
  _Thread_queue_Queue_initialize( &the_rwlock->Queue.Queue, NULL );
}
