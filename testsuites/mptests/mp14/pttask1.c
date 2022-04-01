/* SPDX-License-Identifier: BSD-2-Clause */

/*  Partition_task
 *
 *  This task continuously gets a buffer from and returns that buffer
 *  to a global partition.
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

rtems_task Partition_task(
  rtems_task_argument argument
)
{
  uint32_t     count;
  rtems_status_code  status;
  uint32_t     yield_count;
  void              *buffer;

  puts( "Getting ID of partition" );
  while ( FOREVER ) {
    status = rtems_partition_ident(
      Partition_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Partition_id[ 1 ]
    );

    if ( status == RTEMS_SUCCESSFUL )
      break;

    puts( "rtems_partition_ident FAILED!!" );
    rtems_task_wake_after(2);
  }

  yield_count = 100;

  while ( Stop_Test == false ) {
    for ( count=PARTITION_DOT_COUNT ; Stop_Test == false && count ; count-- ) {
      status = rtems_partition_get_buffer( Partition_id[ 1 ], &buffer );
      directive_failed( status, "rtems_partition_get_buffer" );

      status = rtems_partition_return_buffer( Partition_id[ 1 ], buffer );
      directive_failed( status, "rtems_partition_return_buffer" );

      if (Stop_Test == false)
        if ( rtems_object_get_local_node() == 1 && --yield_count == 0 ) {
          status = rtems_task_wake_after( 1 );
          directive_failed( status, "rtems_task_wake_after" );
          yield_count = 100;
        }
    }
    put_dot( 'p' );
  }

  Exit_test();
}
