/*
 *  Mutex Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Mutex Handler.
 *  This handler provides synchronization and mutual exclusion capabilities.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*PAGE
 *
 *  _CORE_mutex_Initialize
 *
 *  This routine initializes a mutex at create time and set the control
 *  structure according to the values passed.
 *
 *  Input parameters: 
 *    the_mutex             - the mutex control block to initialize
 *    the_mutex_attributes  - the mutex attributes specified at create time
 *    initial_lock          - mutex initial lock or unlocked status
 *
 *  Output parameters:  NONE
 */

void _CORE_mutex_Initialize(
  CORE_mutex_Control           *the_mutex,
  CORE_mutex_Attributes        *the_mutex_attributes,
  unsigned32                    initial_lock
)
{

/* Add this to the RTEMS environment later ????????? 
  rtems_assert( initial_lock == CORE_MUTEX_LOCKED ||
                initial_lock == CORE_MUTEX_UNLOCKED );
 */

  the_mutex->Attributes    = *the_mutex_attributes;
  the_mutex->lock          = initial_lock;
  the_mutex->blocked_count = 0;

#if 0
  if ( !the_mutex_attributes->only_owner_release &&
       the_mutex_attributes->nesting_allowed ) {
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_BAD_ATTRIBUTES
    );
  }
#endif

  if ( initial_lock == CORE_MUTEX_LOCKED ) {
    the_mutex->nest_count = 1;
    the_mutex->holder     = _Thread_Executing;
    the_mutex->holder_id  = _Thread_Executing->Object.id;
    if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ||
         _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) )
      _Thread_Executing->resource_count++;
  } else {
    the_mutex->nest_count = 0;
    the_mutex->holder     = NULL;
    the_mutex->holder_id  = 0;
  }

  _Thread_queue_Initialize(
    &the_mutex->Wait_queue,
    _CORE_mutex_Is_fifo( the_mutex_attributes ) ?
      THREAD_QUEUE_DISCIPLINE_FIFO : THREAD_QUEUE_DISCIPLINE_PRIORITY,
    STATES_WAITING_FOR_MUTEX,
    CORE_MUTEX_TIMEOUT
  );
}

