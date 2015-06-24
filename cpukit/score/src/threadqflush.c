/**
 * @file
 *
 * @brief Thread Queue Flush
 * @ingroup ScoreThreadQ
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadqimpl.h>
#include <rtems/score/objectimpl.h>

void _Thread_queue_Flush(
  Thread_queue_Control       *the_thread_queue,
#if defined(RTEMS_MULTIPROCESSING)
  Thread_queue_Flush_callout  remote_extract_callout,
#else
  Thread_queue_Flush_callout  remote_extract_callout __attribute__((unused)),
#endif
  uint32_t                    status
)
{
  ISR_lock_Context  lock_context;
  Thread_Control   *the_thread;

  _Thread_queue_Acquire( the_thread_queue, &lock_context );

  while ( (the_thread = _Thread_queue_First_locked( the_thread_queue ) ) ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Objects_Is_local_id( the_thread->Object.id ) )
#endif
      the_thread->Wait.return_code = status;

    _Thread_queue_Extract_critical(
      &the_thread_queue->Queue,
      the_thread_queue->operations,
      the_thread,
      &lock_context
    );

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      ( *remote_extract_callout )( the_thread );
#endif

    _Thread_queue_Acquire( the_thread_queue, &lock_context );
  }

  _Thread_queue_Release( the_thread_queue, &lock_context );
}
