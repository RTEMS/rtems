/*
 *  CORE Semaphore Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the CORE Semaphore Handler.
 *  This core object utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/isr.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*
 *  _CORE_semaphore_Flush
 *
 *  This function a flushes the semaphore's task wait queue.
 *
 *  Input parameters:
 *    the_semaphore          - the semaphore to be flushed
 *    remote_extract_callout - function to invoke remotely
 *    status                 - status to pass to thread
 *
 *  Output parameters:  NONE
 */

void _CORE_semaphore_Flush(
  CORE_semaphore_Control     *the_semaphore,
  Thread_queue_Flush_callout  remote_extract_callout,
  uint32_t                    status
)
{

  _Thread_queue_Flush(
    &the_semaphore->Wait_queue,
    remote_extract_callout,
    status
  );

}
