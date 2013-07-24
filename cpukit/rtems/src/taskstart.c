/**
 * @file
 *
 * @brief  RTEMS Start Task
 * @ingroup ClassicTasks Tasks
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/threadimpl.h>

/*
 *  rtems_task_start
 *
 *  This directive readies the thread identified by the "id"
 *  based on its current priorty, to await execution.  A thread
 *  can be started only from the dormant state.
 *
 *  Input parameters:
 *    id          - thread id
 *    entry_point - start execution address of thread
 *    argument    - thread argument
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_start(
  rtems_id         	id,
  rtems_task_entry 	entry_point,
  rtems_task_argument	argument
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  bool                     successfully_started;

  if ( entry_point == NULL )
    return RTEMS_INVALID_ADDRESS;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      successfully_started = _Thread_Start(
        the_thread,
        THREAD_START_NUMERIC,
        entry_point,
        NULL,
        argument,
        NULL
      );

      _Objects_Put( &the_thread->Object );

      if ( successfully_started ) {
        return RTEMS_SUCCESSFUL;
      } else {
        return RTEMS_INCORRECT_STATE;
      }

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
