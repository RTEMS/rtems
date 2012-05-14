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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  void                   *address_1;
  void                   *address_2;
  void                   *address_3;
  void                   *address_4;
  void                   *address_5;
  void                   *address_6;
  void                   *address_7;
  void                   *address_8;
  rtems_id                region_id;
  Heap_Information_block  region_info;
  uintptr_t               segment_size;
  rtems_status_code       status;

  /* Obtain the region id */
  status = rtems_region_ident(Region_name[ 1 ], &region_id);
  printf( "TA1 - rtems_region_ident - 0x%08" PRIxrtems_id "\n", region_id );
  directive_failed(status, "rtems_region_ident of RN01");

  /* Get a 64 byte segment from the region */
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

  /* Get the size of segment_1 */
  puts_nocr( "TA1 - rtems_region_get_segment_size from - " );
  Put_address_from_area_1( address_1 );
  new_line;
  status = rtems_region_get_segment_size(region_id, address_1, &segment_size);
  directive_failed(status, "rtems_region_get_segment_size of segment 1");
  printf( "TA1 - got segment size of %" PRIuPTR "\n", segment_size );

  /* Get a 128 byte segment from the region */
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

  /* Get a 256 byte segment from the region */
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

  /* Get a 512 byte segment from the region */
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

  /* Get a 1024 byte segment from the region */
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

  /* Get a 2048 byte segment from the region */
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

  /* Get a 4096 byte segment from the region */
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

  /* Get a 8192 byte segment from the region */
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

  /* Get the size of segment_8 */
  puts_nocr( "TA1 - rtems_region_get_segment_size from - " );
  Put_address_from_area_1( address_8 );
  new_line;
  status = rtems_region_get_segment_size(region_id, address_8, &segment_size);
  directive_failed(status, "rtems_region_get_segment_size of segment 8");
  printf( "TA1 - got segment size of %" PRIuPTR "\n", segment_size );

  /* Get information about the region */
  printf(
    "TA1 - rtems_region_get_information - 0x%08" PRIxrtems_id "\n",
    region_id
  );
  status = rtems_region_get_information(region_id, &region_info);
  directive_failed(status, "rtems_region_get_information of RN01");
  printf(
    "TA1 - got information - free = %" PRIu32 ", used = %" PRIu32 "\n",
    region_info.Free.number,
    region_info.Used.number
  );

  printf(
    "TA1 - rtems_region_get_free_information - 0x%08" PRIxrtems_id "\n",
    region_id
  );
  status = rtems_region_get_free_information(region_id, &region_info);
  directive_failed(status, "rtems_region_get_free_information of RN01");
  printf(
    "TA1 - got free information - free = %" PRIu32 ", used = %" PRIu32 "\n",
    region_info.Free.number,
    region_info.Used.number
  );

rtems_test_pause();

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
  rtems_test_exit( 0 );
}
