/*  Task_1
 *
 *  This routine serves as a test task.  It tests the region manager.
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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id           rnid;
  void              *segment_address_1;
  void              *segment_address_2;
  void              *segment_address_3;
  void              *segment_address_4;
  rtems_status_code  status;

  status = rtems_region_ident( Region_name[ 1 ], &rnid );
  printf( "TA1 - rtems_region_ident - rnid => %08" PRIxrtems_id "\n", rnid );
  directive_failed( status, "rtems_region_ident of RN1" );

  puts(
    "TA1 - rtems_region_get_segment - wait on 100 byte segment from region 2"
  );
  status = rtems_region_get_segment(
    Region_id[ 2 ],
    100,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_1
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 2 - " );
  Put_address_from_area_2( segment_address_1 );
  new_line;

  puts( "TA1 - rtems_region_get_segment - wait on 3K segment from region 3" );
  status = rtems_region_get_segment(
    Region_id[ 3 ],
    3072,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_2
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 3 - " );
  Put_address_from_area_3( segment_address_2 );
  new_line;

  puts_nocr( "TA1 - rtems_region_get_segment - get 3080 byte segment " );
  puts     ( "from region 1 - NO_WAIT" );
  status = rtems_region_get_segment(
    Region_id[ 1 ],
    3080,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &segment_address_3
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( segment_address_3 );
  new_line;

  puts( "TA1 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

rtems_test_pause();

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( segment_address_3 );
  status = rtems_region_return_segment( Region_id[ 1 ], segment_address_3 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;

  puts(
    "TA1 - rtems_region_get_segment - wait 10 seconds for 3K "
      "segment from region 1"
  );
  status = rtems_region_get_segment(
    Region_id[ 1 ],
    3072,
    RTEMS_DEFAULT_OPTIONS,
    10 * rtems_clock_get_ticks_per_second(),
    &segment_address_4
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( segment_address_4 );
  new_line;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 2 - "
  );
  Put_address_from_area_2( segment_address_1 );
  new_line;
  status = rtems_region_return_segment( Region_id[ 2 ], segment_address_1 );
  directive_failed( status, "rtems_region_return_segment" );

  puts( "TA1 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_delete - delete TA3" );
  status = rtems_task_delete( Task_id[ 3 ] );
  directive_failed( status, "rtems_task_delete of TA3" );

rtems_test_pause();

  status = rtems_task_create(
    Task_name[ 4 ],
    BASE_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );

  status = rtems_task_create(
    Task_name[ 5 ],
    BASE_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 5 ]
  );
  directive_failed( status, "rtems_task_create of TA5" );

  status = rtems_task_start( Task_id[ 4 ], Task_4, 0 );
  directive_failed( status, "rtems_task_start of TA4" );

  status = rtems_task_start( Task_id[ 5 ], Task5, 0 );
  directive_failed( status, "rtems_task_start of TA5" );

  puts( "TA1 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( 1 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( segment_address_4 );
  status = rtems_region_return_segment( Region_id[ 1 ], segment_address_4 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;

  puts( "TA1 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( 1 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts_nocr( "TA1 - rtems_region_get_segment - wait 10 seconds for 3K " );
  puts     ( "segment from region 1");
  status = rtems_region_get_segment(
    Region_id[ 1 ],
    3072,
    RTEMS_DEFAULT_OPTIONS,
    10 * rtems_clock_get_ticks_per_second(),
    &segment_address_4
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( segment_address_4 );
  new_line;

  puts( "TA1 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( 1 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_delete - delete TA4" );
  status = rtems_task_delete( Task_id[ 4 ] );
  directive_failed( status, "rtems_task_delete of TA4" );

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( segment_address_4 );
  status = rtems_region_return_segment( Region_id[ 1 ], segment_address_4 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;

  puts( "TA1 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

  puts( "TA1 - rtems_region_delete - delete region 1" );
  status = rtems_region_delete( Region_id[ 1 ] );
  directive_failed( status, "rtems_region_delete" );

  puts( "TA1 - rtems_region_get_segment - get 3K segment from region 4" );
  status = rtems_region_get_segment(
    Region_id[ 4 ],
    3072,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_1
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 4 - " );
  Put_address_from_area_4( segment_address_1 );
  new_line;

  puts(
   "TA1 - rtems_region_get_segment - attempt to get 3K segment from region 4"
  );
  status =  rtems_region_get_segment(
    Region_id[ 4 ],
    3072,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &segment_address_2
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_region_get_segment with no memory left"
  );
  puts( "TA1 - rtems_region_get_segment - RTEMS_UNSATISFIED" );

  puts( "TA1 - rtems_region_extend - extend region 4 by 1" );
  status = rtems_region_extend(
    Region_id[ 4 ],
    &Area_4[4096],
    1
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_extend with too small memory area"
  );
  puts( "TA1 - rtems_region_extend - RTEMS_INVALID_ADDRESS" );

  puts( "TA1 - rtems_region_extend - extend region 4 by 4K - 1" );
  status = rtems_region_extend(
    Region_id[ 4 ],
    (char *) &Area_4[4096] + 1,
    4096 - 1
  );
  directive_failed( status, "rtems_region_extend" );

  puts(
   "TA1 - rtems_region_get_segment - attempt to get 3K segment from region 4"
  );
  status = rtems_region_get_segment(
    Region_id[ 4 ],
    3072,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &segment_address_3
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got 3K segment from region 4 - " );
  Put_address_from_area_4( segment_address_3 );
  new_line;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 4 - "
  );
  Put_address_from_area_4( segment_address_1 );
  status = rtems_region_return_segment( Region_id[ 4 ], segment_address_1 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 4 - "
  );
  Put_address_from_area_4( segment_address_3 );
  status = rtems_region_return_segment( Region_id[ 4 ], segment_address_3 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;

  puts( "TA1 - rtems_region_delete - delete region 4" );
  status = rtems_region_delete( Region_id[ 4 ] );
  directive_failed( status, "rtems_region_delete" );

  puts( "*** END OF TEST 16 ***" );
  rtems_test_exit( 0 );
}
