/* SPDX-License-Identifier: BSD-2-Clause */

/*  Test_task
 *
 *  This task is used for three test tasks.  It obtains its task id and
 *  based upon that id, performs certain actions.
 *
 *       Task_1 delays 5 seconds and deletes itself.
 *       Task_2 delays 10 seconds and then loops until
 *             deleted by the third task.
 *       Task 3 delays 15 seconds, then deletes task 2 and itself.
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
  rtems_status_code status;
  rtems_id          tid;
  rtems_time_of_day time;

  status = rtems_task_ident( RTEMS_WHO_AM_I, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );

  put_name( Task_name[ task_number( tid ) ], FALSE );
  print_time( " - rtems_clock_get_tod - ", &time, "\n" );

  status = rtems_task_wake_after( task_number( tid ) * 1 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );
  put_name( Task_name[ task_number( tid ) ], FALSE );
  print_time( " - rtems_clock_get_tod - ", &time, "\n" );

  if ( task_number(tid) == 1 ) {          /* TASK 1 */
    put_name( Task_name[ 1 ], FALSE );
    printf( " - deleting self\n" );
    rtems_task_exit();
  }
  else if ( task_number(tid) == 2 ) {     /* TASK 2 */
    put_name( Task_name[ 2 ], FALSE );
    printf( " - waiting to be deleted by " );
    put_name( Task_name[ 3 ], TRUE );
    while ( FOREVER );
  }
  else {                                  /* TASK 3 */
    put_name( Task_name[ 3 ], FALSE );
    printf( " - getting TID of " );
    put_name( Task_name[ 2 ], TRUE );
    do {
      status = rtems_task_ident( Task_name[ 2 ], RTEMS_SEARCH_ALL_NODES, &tid );
    } while ( status != RTEMS_SUCCESSFUL );
    directive_failed( status, "rtems_task_ident" );

    put_name( Task_name[ 3 ], FALSE );
    printf( " - deleting " );
    put_name( Task_name[ 2 ], TRUE );
    status = rtems_task_delete( tid );
    directive_failed( status, "rtems_task_delete of Task 2" );

    puts( "*** END OF TEST 1 ***" );
    rtems_test_exit(0);
  }
}
