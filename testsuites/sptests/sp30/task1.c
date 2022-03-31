/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_1_through_3
 *
 *  This task is a cyclic version of test1 to asssure that the times
 *  displayed are not skewed as in test1.  "TA1" is printed once every
 *  5 seconds, "TA2" is printed once every 10 seconds, and "TA3" is
 *  printed once every 15 seconds.  The times displayed should be
 *  in multiples of 5, 10, and 15 for TA1, TA2, and TA3 respectively.
 *  If the times are skewed from these values, then the calendar time
 *  does not correspond correctly with the number of ticks.
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

rtems_task Task_1_through_3(
  rtems_task_argument argument
)
{
  rtems_id          tid;
  rtems_time_of_day time;
  rtems_status_code status;

  status = rtems_task_ident( RTEMS_WHO_AM_I, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident of self" );

  while ( FOREVER )  {
    status = rtems_timer_server_fire_after(
      Timer_id[ argument ],
      (task_number( tid ) - 1) * 5 * rtems_clock_get_ticks_per_second(),
      Resume_task,
      (void *) &tid
    );
    directive_failed( status, "rtems_timer_server_fire_after failed" );

    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod failed" );

    if ( time.second >= 35 ) {
      TEST_END();
      rtems_test_exit( 0 );
    }

    put_name( Task_name[ task_number( tid ) - 1 ], FALSE );
    print_time( " - rtems_clock_get_tod - ", &time, "\n" );

    status = rtems_task_suspend( RTEMS_SELF );
    directive_failed( status, "rtems_task_suspend" );
  }
}
