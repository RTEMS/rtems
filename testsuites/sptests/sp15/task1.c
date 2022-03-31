/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

  TEST_END();
  rtems_test_exit( 0 );
}
