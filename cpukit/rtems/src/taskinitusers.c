/*
 *  RTEMS Task Manager
 *
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
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

/*PAGE
 *
 *  _RTEMS_tasks_Initialize_user_tasks
 *
 *  This routine creates and starts all configured user
 *  initialzation threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */

void _RTEMS_tasks_Initialize_user_tasks( void )
{
  unsigned32                        index;
  unsigned32                        maximum;
  rtems_id                          id;
  rtems_status_code                 return_value;
  rtems_initialization_tasks_table *user_tasks;

  /*
   *  NOTE:  This is slightly different from the Ada implementation.
   */

  user_tasks = _RTEMS_tasks_User_initialization_tasks;
  maximum    = _RTEMS_tasks_Number_of_initialization_tasks;

  if ( !user_tasks || maximum == 0 )
    return;

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
      _Internal_error_Occurred( INTERNAL_ERROR_RTEMS_API, TRUE, return_value );

    return_value = rtems_task_start(
      id,
      user_tasks[ index ].entry_point,
      user_tasks[ index ].argument
    );

    if ( !rtems_is_status_successful( return_value ) )
      _Internal_error_Occurred( INTERNAL_ERROR_RTEMS_API, TRUE, return_value );
  }
}
