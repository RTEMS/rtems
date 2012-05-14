/*  Task_4
 *
 *  This routine serves as a test task.  It competes with the other tasks
 *  for region resources.
 *
 *  Input parameters:
 *    argument - task argument
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

rtems_task Task_4(
  rtems_task_argument argument
)
{
  rtems_status_code  status;
  void              *segment_address_1;
  void              *segment_address_2;

  puts( "TA4 - rtems_region_get_segment - wait on 1.5K segment from region 1" );
  status = rtems_region_get_segment(
    Region_id[ 1 ],
    1536,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_1
  );
  directive_failed( status, "rtems_region_get_segment" );

  status = rtems_region_return_segment( Region_id[ 1 ], segment_address_1 );
  directive_failed( status, "rtems_region_return_segment" );
  puts_nocr( "TA4 - got and returned " );
  Put_address_from_area_1( segment_address_1 );
  new_line;

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA4 - rtems_region_get_segment - wait on 3K segment from region 1" );
  status = rtems_region_get_segment(
    Region_id[ 1 ],
    3072,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_2
  );
  directive_failed( status, "rtems_region_get_segment" );
}
