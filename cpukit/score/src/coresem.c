/*
 *  CORE Semaphore Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the CORE Semaphore Handler.
 *  This core object utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/mpci.h>

/*PAGE
 *
 *  CORE_semaphore_Initialize
 *
 *  This function initialize a semaphore and sets the initial value based
 *  on the given count.
 *
 *  Input parameters:
 *    the_semaphore            - the semaphore control block to initialize
 *    the_class                - the API class of the object
 *    the_semaphore_attributes - the attributes specified at create time
 *    initial_value            - semaphore's initial value
 *    proxy_extract_callout    - MP specific extract callout
 *
 *  Output parameters:  NONE
 */

void _CORE_semaphore_Initialize(
  CORE_semaphore_Control       *the_semaphore,
  Objects_Classes               the_class,
  CORE_semaphore_Attributes    *the_semaphore_attributes,
  unsigned32                    initial_value,
  Thread_queue_Extract_callout  proxy_extract_callout
)
{

  the_semaphore->Attributes = *the_semaphore_attributes;
  the_semaphore->count      = initial_value;

  _Thread_queue_Initialize(
    &the_semaphore->Wait_queue,
    the_class,
    _CORE_semaphore_Is_priority( the_semaphore_attributes ) ?
              THREAD_QUEUE_DISCIPLINE_PRIORITY : THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_SEMAPHORE,
    proxy_extract_callout,
    CORE_SEMAPHORE_TIMEOUT
  );
}

/*PAGE
 *
 *  _CORE_semaphore_Surrender
 *
 *  Input parameters:
 *    the_semaphore            - the semaphore to be flushed
 *    id                       - id of parent semaphore
 *    api_semaphore_mp_support - api dependent MP support actions
 *
 *  Output parameters:
 *    CORE_SEMAPHORE_STATUS_SUCCESSFUL - if successful
 *    core error code                  - if unsuccessful
 *
 *  Output parameters:
 */

CORE_semaphore_Status _CORE_semaphore_Surrender(
  CORE_semaphore_Control                *the_semaphore,
  Objects_Id                             id,
  CORE_semaphore_API_mp_support_callout  api_semaphore_mp_support
)
{
  Thread_Control *the_thread;

  if ( (the_thread = _Thread_queue_Dequeue(&the_semaphore->Wait_queue)) ) {

    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      (*api_semaphore_mp_support) ( the_thread, id );

  } else
    the_semaphore->count += 1;

  return( CORE_SEMAPHORE_STATUS_SUCCESSFUL );
}

/*PAGE
 *
 *  _CORE_semaphore_Seize
 *
 *  This routine attempts to allocate a core semaphore to the calling thread.
 *
 *  Input parameters:
 *    the_semaphore - pointer to semaphore control block
 *    id            - id of object to wait on
 *    wait          - TRUE if wait is allowed, FALSE otherwise
 *    timeout       - number of ticks to wait (0 means forever)
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 */

void _CORE_semaphore_Seize(
  CORE_semaphore_Control  *the_semaphore,
  Objects_Id               id,
  boolean                  wait,
  Watchdog_Interval        timeout
)
{
  Thread_Control *executing;
  ISR_Level       level;

  executing = _Thread_Executing;
  executing->Wait.return_code = CORE_SEMAPHORE_STATUS_SUCCESSFUL;
  _ISR_Disable( level );
  if ( the_semaphore->count != 0 ) {
    the_semaphore->count -= 1;
    _ISR_Enable( level );
    return;
  }

  if ( !wait ) {
    _ISR_Enable( level );
    executing->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  _Thread_queue_Enter_critical_section( &the_semaphore->Wait_queue );
  executing->Wait.queue          = &the_semaphore->Wait_queue;
  executing->Wait.id             = id;
  _ISR_Enable( level );

  _Thread_queue_Enqueue( &the_semaphore->Wait_queue, timeout );
}


/*PAGE
 *
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
  unsigned32                  status
)
{
 
  _Thread_queue_Flush(
    &the_semaphore->Wait_queue,
    remote_extract_callout,
    status
  );
 
}
