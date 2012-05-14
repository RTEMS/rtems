/*
 *  SuperCore Barrier Handler
 *
 *  DESCRIPTION:
 *
 *  This package is part of the implementation of the SuperCore Barrier Handler.
 *
 *  COPYRIGHT (c) 1989-2006.
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
#include <rtems/score/corebarrier.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*
 *  _CORE_barrier_Initialize
 *
 *  This function initialize a barrier and sets the initial value based
 *  on the given count.
 *
 *  Input parameters:
 *    the_barrier            - the barrier control block to initialize
 *    the_barrier_attributes - the attributes specified at create time
 *
 *  Output parameters:  NONE
 */

void _CORE_barrier_Initialize(
  CORE_barrier_Control       *the_barrier,
  CORE_barrier_Attributes    *the_barrier_attributes
)
{

  the_barrier->Attributes                = *the_barrier_attributes;
  the_barrier->number_of_waiting_threads = 0;

  _Thread_queue_Initialize(
    &the_barrier->Wait_queue,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_BARRIER,
    CORE_BARRIER_TIMEOUT
  );
}
