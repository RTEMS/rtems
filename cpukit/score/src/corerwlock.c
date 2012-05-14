/*
 *  SuperCore RWLock Handler
 *
 *  DESCRIPTION:
 *
 *  This package is part of the implementation of the SuperCore RWLock Handler.
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
#include <rtems/score/corerwlock.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*
 *  _CORE_RWLock_Initialize
 *
 *  This function initialize a rwlock and sets the initial value based
 *  on the given count.
 *
 *  Input parameters:
 *    the_rwlock            - the rwlock control block to initialize
 *    the_rwlock_attributes - the attributes specified at create time
 *
 *  Output parameters:  NONE
 */

void _CORE_RWLock_Initialize(
  CORE_RWLock_Control       *the_rwlock,
  CORE_RWLock_Attributes    *the_rwlock_attributes
)
{

  the_rwlock->Attributes                = *the_rwlock_attributes;
/*
  the_rwlock->number_of_waiting_threads = 0;
*/
  the_rwlock->number_of_readers = 0;
  the_rwlock->current_state = CORE_RWLOCK_UNLOCKED;

  _Thread_queue_Initialize(
    &the_rwlock->Wait_queue,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_RWLOCK,
    CORE_RWLOCK_TIMEOUT
  );
}
