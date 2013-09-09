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
 *  http://www.rtems.com/license/LICENSE.
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
  Thread_Control *the_thread;

  while ( (the_thread = _Thread_queue_Dequeue( the_thread_queue )) ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      ( *remote_extract_callout )( the_thread );
    else
#endif
      the_thread->Wait.return_code = status;
  }
}
