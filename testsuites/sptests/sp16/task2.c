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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

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

  puts("TA2 - rtems_region_get_segment - wait on 3968 byte segment");
  status = rtems_region_get_segment(
    Region_id[ 2 ],
    3968,
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
