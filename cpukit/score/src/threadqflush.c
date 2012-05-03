/*
 *  Thread Queue Handler
 *
 *
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

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*
 *  _Thread_queue_Flush
 *
 *  This kernel routine flushes the given thread queue.
 *
 *  Input parameters:
 *    the_thread_queue       - pointer to threadq to be flushed
 *    remote_extract_callout - pointer to routine which extracts a remote thread
 *    status                 - status to return to the thread
 *
 *  Output parameters:  NONE
 */

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
