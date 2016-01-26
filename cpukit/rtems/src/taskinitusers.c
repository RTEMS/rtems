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

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/assert.h>
#include <rtems/score/stack.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

static void _RTEMS_Global_construction( rtems_task_argument arg )
{
  Thread_Control           *executing = _Thread_Get_executing();
  Thread_Entry_information  entry = executing->Start.Entry;

  entry.Kinds.Numeric.entry =
    Configuration_RTEMS_API.User_initialization_tasks_table[ 0 ].entry_point;

  (void) arg;
  _Thread_Global_construction( executing, &entry );
}

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
  bool                              register_global_construction;
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

  register_global_construction = true;

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
    if ( !rtems_is_status_successful( return_value ) )
      _Terminate( INTERNAL_ERROR_RTEMS_API, true, return_value );

    entry_point = user_tasks[ index ].entry_point;
    if ( entry_point == NULL ) {
      _Terminate(
        INTERNAL_ERROR_CORE,
        false,
        INTERNAL_ERROR_RTEMS_INIT_TASK_ENTRY_IS_NULL
      );
    }

    if ( register_global_construction ) {
      register_global_construction = false;
      entry_point = _RTEMS_Global_construction;
    }

    return_value = rtems_task_start(
      id,
      entry_point,
      user_tasks[ index ].argument
    );
    _Assert( rtems_is_status_successful( return_value ) );
    (void) return_value;
  }
}
