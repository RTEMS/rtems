/* SPDX-License-Identifier: BSD-2-Clause */

/*  Test_task
 *
 *  This task tests global semaphore operations.  It verifies that
 *  global semaphore errors are correctly returned.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
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

rtems_task Test_task(
  rtems_task_argument argument
)
{
  uint32_t    count;
  rtems_status_code status;

  puts( "Getting SMID of semaphore" );

  do {
    status = rtems_semaphore_ident(
      Semaphore_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Semaphore_id[ 1 ]
    );
  } while ( !rtems_is_status_successful( status ) );

  if ( rtems_object_get_local_node() == 2 ) {
    status = rtems_semaphore_delete( Semaphore_id[ 1 ] );
    fatal_directive_status(
      status,
      RTEMS_ILLEGAL_ON_REMOTE_OBJECT,
      "rtems_semaphore_delete did not return RTEMS_ILLEGAL_ON_REMOTE_OBJECT"
    );
    puts(
      "rtems_semaphore_delete correctly returned RTEMS_ILLEGAL_ON_REMOTE_OBJECT"
    );
  }

  count = 0;            /* number of times node 1 releases semaphore */
  while ( FOREVER ) {
    put_dot( 'p' );
    status = rtems_semaphore_obtain(
      Semaphore_id[ 1 ],
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
    if ( status != RTEMS_SUCCESSFUL ) {
      fatal_directive_status(
        status,
        RTEMS_OBJECT_WAS_DELETED,
        "rtems_semaphore_obtain"
      );
      puts( "\nGlobal semaphore deleted" );
      puts( "*** END OF TEST 8 ***" );
      rtems_test_exit( 0 );
    }

    if ( rtems_object_get_local_node() == 1 && ++count == 1000 ) {
      status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
      directive_failed( status, "rtems_task_wake_after" );

      puts( "\nDeleting global semaphore" );
      status = rtems_semaphore_delete( Semaphore_id[ 1 ] );
      directive_failed( status, "rtems_semaphore_delete" );

      puts( "*** END OF TEST 8 ***" );
      rtems_test_exit( 0 );
    }
    else {
      put_dot( 'v' );
      status = rtems_semaphore_release( Semaphore_id[ 1 ] );
      directive_failed( status, "rtems_semaphore_release FAILED!!" );
    }
  }
}
