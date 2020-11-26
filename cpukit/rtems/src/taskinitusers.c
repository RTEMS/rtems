/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   _RTEMS_tasks_Initialize_user_task().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/interr.h>

void _RTEMS_tasks_Initialize_user_task( void )
{
  rtems_id                                id;
  rtems_status_code                       return_value;
  const rtems_initialization_tasks_table *user_task;

  user_task = &_RTEMS_tasks_User_task_table;
  return_value = rtems_task_create(
    user_task->name,
    user_task->initial_priority,
    user_task->stack_size,
    user_task->mode_set,
    user_task->attribute_set,
    &id
  );
  if ( !rtems_is_status_successful( return_value ) ) {
    _Internal_error( INTERNAL_ERROR_RTEMS_INIT_TASK_CREATE_FAILED );
  }

  return_value = rtems_task_start(
    id,
    user_task->entry_point,
    user_task->argument
  );
  _Assert( rtems_is_status_successful( return_value ) );
  (void) return_value;

  _Assert( _Thread_Global_constructor == 0 );
  _Thread_Global_constructor = id;
}
