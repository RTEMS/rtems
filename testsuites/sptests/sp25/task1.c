/*  Task_1
 *
 *  This routine serves as a test task.  It tests the region manager.
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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  void              *address_1;
  void              *address_2;
  void              *address_3;
  void              *address_4;
  void              *address_5;
  void              *address_6;
  void              *address_7;
  void              *address_8;
  rtems_status_code  status;
  rtems_id           region_id;

  status = rtems_region_ident(Region_name[ 1 ], &region_id);
  printf( "TA1 - rtems_region_ident - 0x%08x\n", region_id );
  directive_failed(status, "rtems_region_ident of RN01");

  puts(
    "TA1 - rtems_region_get_segment - wait on 64 byte segment from region 1"
  );
  rtems_region_get_segment(
    Region_id[ 1 ],
    64,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_1
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( address_1 );
  new_line;

  puts(
    "TA1 - rtems_region_get_segment - wait on 128 byte segment from region 1"
  );
  rtems_region_get_segment(
    Region_id[ 1 ],
    128,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_2
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( address_2 );
  new_line;

  puts(
    "TA1 - rtems_region_get_segment - wait on 256 byte segment from region 1"
  );
  rtems_region_get_segment(
    Region_id[ 1 ],
    256,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_3
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( address_3 );
  new_line;

  puts(
    "TA1 - rtems_region_get_segment - wait on 512 byte segment from region 1"
  );
  rtems_region_get_segment(
    Region_id[ 1 ],
    512,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_4
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( address_4 );
  new_line;

  puts(
    "TA1 - rtems_region_get_segment - wait on 1024 byte segment from region 1"
  );
  rtems_region_get_segment(
    Region_id[ 1 ],
    1024,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_5
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( address_5 );
  new_line;

  puts(
    "TA1 - rtems_region_get_segment - wait on 2048 byte segment from region 1"
  );
  rtems_region_get_segment(
    Region_id[ 1 ],
    2048,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_6
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( address_6 );
  new_line;

  puts(
    "TA1 - rtems_region_get_segment - wait on 4096 byte segment from region 1"
  );
  rtems_region_get_segment(
    Region_id[ 1 ],
    4096,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_7
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( address_7 );
  new_line;

  puts(
    "TA1 - rtems_region_get_segment - wait on 8192 byte segment from region 1"
  );
  rtems_region_get_segment(
    Region_id[ 1 ],
    8192,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_8
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts_nocr( "TA1 - got segment from region 1 - " );
  Put_address_from_area_1( address_8 );
  new_line;

pause();

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( address_3 );
  status = rtems_region_return_segment( Region_id[ 1 ], address_3 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;
  address_3 = 0;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( address_4 );
  status = rtems_region_return_segment( Region_id[ 1 ], address_4 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;
  address_4 = 0;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( address_1 );
  status = rtems_region_return_segment( Region_id[ 1 ], address_1 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;
  address_1 = 0;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( address_2 );
  status = rtems_region_return_segment( Region_id[ 1 ], address_2 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;
  address_2 = 0;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( address_7 );
  status = rtems_region_return_segment( Region_id[ 1 ], address_7 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;
  address_7 = 0;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( address_6 );
  status = rtems_region_return_segment( Region_id[ 1 ], address_6 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;
  address_6 = 0;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( address_8 );
  status = rtems_region_return_segment( Region_id[ 1 ], address_8 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;
  address_8 = 0;

  puts_nocr(
    "TA1 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( address_5 );
  status = rtems_region_return_segment( Region_id[ 1 ], address_5 );
  directive_failed( status, "rtems_region_return_segment" );
  new_line;
  address_5 = 0;

  puts( "TA1 - rtems_region_delete - walks heap if debug enabled" );
  rtems_region_delete(Region_id[ 1 ]);

  puts( "*** END OF TEST 25 ***" );
  exit( 0 );
}
