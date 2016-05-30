/**
 * @file
 *
 * @brief Surrender the Mutex
 * @ingroup ScoreMutex
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coremuteximpl.h>

Status_Control _CORE_mutex_Surrender_slow(
  CORE_mutex_Control   *the_mutex,
  Thread_Control       *executing,
  Thread_queue_Heads   *heads,
  bool                  keep_priority,
  Thread_queue_Context *queue_context
)
{
  if ( heads != NULL ) {
    const Thread_queue_Operations *operations;
    Thread_Control                *new_owner;
    bool                           unblock;

    operations = CORE_MUTEX_TQ_OPERATIONS;
    new_owner = ( *operations->first )( heads );

    _CORE_mutex_Set_owner( the_mutex, new_owner );

    unblock = _Thread_queue_Extract_locked(
      &the_mutex->Wait_queue.Queue,
      operations,
      new_owner,
      queue_context
    );

#if defined(RTEMS_MULTIPROCESSING)
    if ( _Objects_Is_local_id( new_owner->Object.id ) )
#endif
    {
      ++new_owner->resource_count;
      _Thread_queue_Boost_priority( &the_mutex->Wait_queue.Queue, new_owner );
    }

    _Thread_queue_Unblock_critical(
      unblock,
      &the_mutex->Wait_queue.Queue,
      new_owner,
      &queue_context->Lock_context
    );
  } else {
    _CORE_mutex_Release( the_mutex, queue_context );
  }

  if ( !keep_priority ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable();
    _Thread_Restore_priority( executing );
    _Thread_Dispatch_enable( cpu_self );
  }

  return STATUS_SUCCESSFUL;
}
