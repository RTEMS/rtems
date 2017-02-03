/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Test_task(
  rtems_task_argument unused
)
{
  rtems_id          tid;
  rtems_time_of_day time;
  uint32_t    task_index;
  rtems_status_code status;
  char              name[5];
  char             *p;

  /* Get the task name */
  p = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert( p != NULL );

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );
  task_index = task_number( tid );
  for ( ; ; ) {
    status = rtems_clock_get_tod( &time );
    rtems_test_assert( status == RTEMS_SUCCESSFUL );
    if ( time.second >= 35 ) {
      TEST_END();
      rtems_test_exit( 0 );
    }

    PrintTaskInfo( p, &time );
    status = rtems_task_wake_after(
      task_index * 5 * rtems_clock_get_ticks_per_second() );
    rtems_test_assert( status == RTEMS_SUCCESSFUL );
  }
}
