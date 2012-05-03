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
 *  CORE_semaphore_Initialize
 *
 *  This function initialize a semaphore and sets the initial value based
 *  on the given count.
 *
 *  Input parameters:
 *    the_semaphore            - the semaphore control block to initialize
 *    the_semaphore_attributes - the attributes specified at create time
 *    initial_value            - semaphore's initial value
 *
 *  Output parameters:  NONE
 */

void _CORE_semaphore_Initialize(
  CORE_semaphore_Control       *the_semaphore,
  CORE_semaphore_Attributes    *the_semaphore_attributes,
  uint32_t                      initial_value
)
{

  the_semaphore->Attributes = *the_semaphore_attributes;
  the_semaphore->count      = initial_value;

  _Thread_queue_Initialize(
    &the_semaphore->Wait_queue,
    _CORE_semaphore_Is_priority( the_semaphore_attributes ) ?
              THREAD_QUEUE_DISCIPLINE_PRIORITY : THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_SEMAPHORE,
    CORE_SEMAPHORE_TIMEOUT
  );
}
