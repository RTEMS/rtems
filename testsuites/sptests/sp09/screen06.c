/*  Screen6
 *
 *  This routine generates error screen 6 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
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

void Screen6()
{
  rtems_status_code status;

  status = rtems_semaphore_obtain(
    100,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_obtain with illegal id"
  );
  puts( "TA1 - rtems_semaphore_obtain - RTEMS_INVALID_ID" );

  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain successful" );
  puts( "TA1 - rtems_semaphore_obtain - got sem 1 - RTEMS_SUCCESSFUL" );

  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_semaphore_obtain not available"
  );
  puts( "TA1 - rtems_semaphore_obtain - RTEMS_UNSATISFIED" );

  puts( "TA1 - rtems_semaphore_obtain - timeout in 3 seconds" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    3 * rtems_clock_get_ticks_per_second()
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_semaphore_obtain timeout"
  );
  puts( "TA1 - rtems_semaphore_obtain - woke up with RTEMS_TIMEOUT" );

  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  fatal_directive_status(
    status,
    RTEMS_NOT_OWNER_OF_RESOURCE,
    "rtems_semaphore_release and not owner"
  );
  puts( "TA1 - rtems_semaphore_release - RTEMS_NOT_OWNER_OF_RESOURCE" );

  status = rtems_semaphore_release( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_release with illegal id"
  );
  puts( "TA1 - rtems_semaphore_release - RTEMS_INVALID_ID" );

  puts( "TA1 - rtems_task_start - start TA2 - RTEMS_SUCCESSFUL" );
  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  puts( "TA1 - rtems_task_wake_after - yield processor - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after (yield)" );

  puts( "TA1 - rtems_semaphore_delete - delete sem 1 - RTEMS_SUCCESSFUL" );
  status = rtems_semaphore_delete( Semaphore_id[ 1 ] );
  directive_failed( status, "rtems_semaphore_delete of SM1" );

  puts( "TA1 - rtems_semaphore_obtain - binary semaphore" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain");

  puts( "TA1 - rtems_semaphore_delete - delete sem 2 - RTEMS_RESOURCE_IN_USE" );
  status = rtems_semaphore_delete( Semaphore_id[ 2 ] );
  fatal_directive_status(
     status,
     RTEMS_RESOURCE_IN_USE,
     "rtems_semaphore_delete of SM2"
  );

  puts( "TA1 - rtems_task_wake_after - yield processor - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after (yield)" );

  status = rtems_task_delete( Task_id[ 2 ] );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_delete after the task has been deleted"
  );

  puts( "TA1 - rtems_task_delete TA2 - already deleted RTEMS_INVALID_ID" );
}
