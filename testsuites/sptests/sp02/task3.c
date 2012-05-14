/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task_3(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "TA3 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5*rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after in TA3" );

  puts( "TA3 - rtems_task_delete - delete self" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of TA3" );
}
