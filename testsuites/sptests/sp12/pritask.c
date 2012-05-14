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

rtems_task Priority_task(
  rtems_task_argument its_index
)
{
  rtems_interval      timeout;
  rtems_task_priority its_priority;
  rtems_task_priority current_priority;
  rtems_status_code   status;
  uint32_t            index;

  its_priority = Task_priority[ its_index ];

  if ( its_priority < 3 )
    timeout = 5 * rtems_clock_get_ticks_per_second();
  else
    timeout = RTEMS_NO_TIMEOUT;

  put_name( Priority_task_name[ its_index ], FALSE );
  puts( " - rtems_semaphore_obtain - wait forever on SM2" );

  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    timeout
  );
  directive_failed( status, "rtems_semaphore_obtain of SM2" );

  if ( its_priority < PRIORITY_INHERIT_BASE_PRIORITY ) {
    printf(
      "PRI%" PRIdrtems_task_argument " - WHY AM I HERE? (pri=%"
        PRIdrtems_task_priority ")",
     its_index,
     its_priority
    );
    rtems_test_exit( 0 );
  }

  /* special case of setting priority while holding a resource */
  {
    rtems_task_priority priority;
    rtems_task_priority old_priority;

    puts( "Set priority of self while holding resource" );
    status =
      rtems_task_set_priority( RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority );
    directive_failed( status, "rtems_task_set_priority get current" );
    status = rtems_task_set_priority( RTEMS_SELF, priority, &old_priority );
    directive_failed( status, "rtems_task_set_priority with resource" );
    if ( priority != old_priority ) {
      printf(
        "priority != old_priority (%" PRIdrtems_task_priority
            " != %" PRIdrtems_task_priority ")\n",
         priority,
         old_priority
      );
      rtems_test_exit(0);
    }
  }

  if ( its_index == 5 )
    puts( "PRI5 - rtems_task_suspend - until all priority tasks blocked" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  puts( "PRI5 - rtems_task_delete - all tasks waiting on SM2" );
  for ( index = 1 ; index < 5 ; index++ ) {
    status = rtems_task_delete( Priority_task_id[ index ] );
    directive_failed( status, "rtems_task_delete loop" );
  }

  puts( "PRI5 - rtems_semaphore_obtain - nested" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    timeout
  );
  directive_failed( status, "rtems_semaphore_obtain nested" );

  puts( "PRI5 - rtems_semaphore_release - nested" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release nested " );

  puts( "PRI5 - rtems_semaphore_release - restore priority" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release" );

  status = rtems_task_set_priority(
    RTEMS_SELF,
    RTEMS_CURRENT_PRIORITY,
    &current_priority
  );
  directive_failed( status, "PRI5 rtems_task_set_priority CURRENT" );
  printf(
    "PRI5 - priority of PRI5 is %" PRIdrtems_task_priority "\n",
     current_priority
  );

  (void) rtems_task_suspend( RTEMS_SELF );
}
