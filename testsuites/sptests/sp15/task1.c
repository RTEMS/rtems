/*  Task_1
 *
 *  This routine serves as a test task.  It tests the partition manager.
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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id           ptid_1;
  rtems_id           ptid_2;
  void              *buffer_address_1;
  void              *buffer_address_2;
  void              *buffer_address_3;
  void              *buffer_address_4;
  rtems_status_code  status;

  puts_nocr( "TA1 - rtems_partition_ident - partition 1 id = " );
  status = rtems_partition_ident(
    Partition_name[ 1 ],
    RTEMS_SEARCH_ALL_NODES,
    &ptid_1
  );
  directive_failed( status, "rtems_partition_ident of PT1" );
  printf( "%08" PRIxrtems_id "\n", ptid_1 );

  puts_nocr( "TA1 - rtems_partition_ident - partition 2 id = " );
  status = rtems_partition_ident(
    Partition_name[ 2 ],
    RTEMS_SEARCH_ALL_NODES,
    &ptid_2
  );
  directive_failed( status, "rtems_partition_ident of PT2" );
  printf( "%08" PRIxrtems_id "\n", ptid_2 );

  puts_nocr(
    "TA1 - rtems_partition_get_buffer - buffer 1 from partition 1  - "
  );
  status = rtems_partition_get_buffer( ptid_1, &buffer_address_1 );
  directive_failed( status, "rtems_partition_get_buffer" );
  Put_address_from_area_1( buffer_address_1 );
  new_line;

  puts_nocr(
    "TA1 - rtems_partition_get_buffer - buffer 2 from partition 1  - "
  );
  status = rtems_partition_get_buffer( ptid_1, &buffer_address_2 );
  directive_failed( status, "rtems_partition_get_buffer" );
  Put_address_from_area_1( buffer_address_2 );
  new_line;

  puts_nocr(
    "TA1 - rtems_partition_get_buffer - buffer 1 from partition 2  - "
  );
  status = rtems_partition_get_buffer( ptid_2, &buffer_address_3 );
  directive_failed( status, "rtems_partition_get_buffer" );
  Put_address_from_area_2( buffer_address_3 );
  new_line;

  puts_nocr(
    "TA1 - rtems_partition_get_buffer - buffer 2 from partition 2  - "
  );
  status = rtems_partition_get_buffer( ptid_2, &buffer_address_4 );
  directive_failed( status, "rtems_partition_get_buffer" );
  Put_address_from_area_2( buffer_address_4 );
  new_line;

  puts_nocr(
    "TA1 - rtems_partition_return_buffer - buffer 1 to partition 1 - "
  );
  Put_address_from_area_1( buffer_address_1 );
  new_line;
  status = rtems_partition_return_buffer( ptid_1, buffer_address_1 );
  directive_failed( status, "rtems_partition_return_buffer" );

  puts_nocr(
    "TA1 - rtems_partition_return_buffer - buffer 2 to partition 1 - "
  );
  Put_address_from_area_1( buffer_address_2 );
  new_line;
  status = rtems_partition_return_buffer( ptid_1, buffer_address_2 );
  directive_failed( status, "rtems_partition_return_buffer" );

  puts_nocr(
    "TA1 - rtems_partition_return_buffer - buffer 1 to partition 2 - "
  );
  Put_address_from_area_2( buffer_address_3 );
  new_line;
  status = rtems_partition_return_buffer( ptid_2, buffer_address_3 );
  directive_failed( status, "rtems_partition_return_buffer" );

  puts_nocr(
    "TA1 - rtems_partition_return_buffer - buffer 2 to partition 2 - "
  );
  Put_address_from_area_2( buffer_address_4 );
  new_line;
  status = rtems_partition_return_buffer( ptid_2, buffer_address_4 );
  directive_failed( status, "rtems_partition_return_buffer" );

  puts( "TA1 - rtems_partition_delete - delete partition 1"
  );
  status = rtems_partition_delete( ptid_1 );
  directive_failed( status, "rtems_partition_delete" );

  puts( "TA1 - rtems_partition_delete - delete partition 2"
  );
  status = rtems_partition_delete( ptid_2 );
  directive_failed( status, "rtems_partition_delete" );

  puts( "*** END OF TEST 15 ***" );
  rtems_test_exit( 0 );
}
