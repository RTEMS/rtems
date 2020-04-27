/**
 * @file
 *
 * @ingroup ClassicTasks Tasks
 *
 * @brief rtems_task_is_suspended
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_is_suspended( rtems_id id )
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  States_Control    current_state;

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  current_state = the_thread->current_state;
  _ISR_lock_ISR_enable( &lock_context);
  return _States_Is_suspended( current_state ) ?
    RTEMS_ALREADY_SUSPENDED : RTEMS_SUCCESSFUL;
}
