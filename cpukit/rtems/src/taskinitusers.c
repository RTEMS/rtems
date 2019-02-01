/**
 * @file
 *
 * @brief _RTEMS_tasks_Initialize_user_tasks_body
 * @ingroup ClassicTasks Tasks
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/config.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/assert.h>
#include <rtems/score/stack.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

/*
 *  _RTEMS_tasks_Initialize_user_tasks_body
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */

void _RTEMS_tasks_Initialize_user_tasks_body( void )
{
  uint32_t                          index;
  uint32_t                          maximum;
  rtems_id                          id;
  rtems_status_code                 return_value;
  rtems_initialization_tasks_table *user_tasks;
  rtems_task_entry                  entry_point;

  /*
   *  Move information into local variables
   */
  user_tasks = Configuration_RTEMS_API.User_initialization_tasks_table;
  maximum    = Configuration_RTEMS_API.number_of_initialization_tasks;

  /*
   *  Verify that we have a set of user tasks to iterate
   */
  if ( !user_tasks )
    return;

  /*
   *  Now iterate over the initialization tasks and create/start them.
   */
  for ( index=0 ; index < maximum ; index++ ) {
    return_value = rtems_task_create(
      user_tasks[ index ].name,
      user_tasks[ index ].initial_priority,
      user_tasks[ index ].stack_size,
      user_tasks[ index ].mode_set,
      user_tasks[ index ].attribute_set,
      &id
    );
    if ( !rtems_is_status_successful( return_value ) ) {
      _Internal_error( INTERNAL_ERROR_RTEMS_INIT_TASK_CREATE_FAILED );
    }

    entry_point = user_tasks[ index ].entry_point;
    if ( entry_point == NULL ) {
      _Internal_error( INTERNAL_ERROR_RTEMS_INIT_TASK_ENTRY_IS_NULL );
    }

    return_value = rtems_task_start(
      id,
      entry_point,
      user_tasks[ index ].argument
    );
    _Assert( rtems_is_status_successful( return_value ) );
    (void) return_value;

    if ( _Thread_Global_constructor == 0 ) {
      _Thread_Global_constructor = id;
    }
  }
}
