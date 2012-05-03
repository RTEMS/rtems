/*  Task_2
 *
 *  This routine serves as a test task.  It competes with the other tasks
 *  for region resources.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code    status;
  rtems_task_priority  previous_priority;
  void                *segment_address_1;
  void                *segment_address_2;

  puts( "TA2 - rtems_region_get_segment - wait on 2K segment from region 1" );
  status = rtems_region_get_segment(
    Region_id[ 1 ],
    2048,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_1
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA2 - got segment from region 1 - " );
  Put_address_from_area_1( segment_address_1 );
  new_line;

  puts_nocr(
    "TA2 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( segment_address_1 );
  status = rtems_region_return_segment( Region_id[ 1 ], segment_address_1 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;

  puts( "TA2 - rtems_task_set_priority - make self highest priority task" );
  status = rtems_task_set_priority(
    RTEMS_SELF,
    BASE_PRIORITY-1,
    &previous_priority
  );
  directive_failed( status, "rtems_task_set_priority" );

  puts("TA2 - rtems_region_get_segment - wait on 3950 byte segment");
  status = rtems_region_get_segment(
    Region_id[ 2 ],
    3950,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_2
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA2 - got segment from region 2 - ");
  Put_address_from_area_2( segment_address_2 );
  new_line;

  puts_nocr(
    "TA2 - rtems_region_return_segment - return segment to region 2 - "
  );
  Put_address_from_area_2( segment_address_2 );
  status = rtems_region_return_segment( Region_id[ 2 ], segment_address_2 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;

  puts( "TA2 - rtems_task_delete - delete self" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
