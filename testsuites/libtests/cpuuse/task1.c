/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_1
 *
 *  This test serves as a test task.  It verifies timeslicing activities
 *  and tswitch extension processing.
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
  uint32_t    seconds;
  uint32_t    old_seconds;
  rtems_mode        previous_mode;
  rtems_time_of_day time;
  rtems_status_code status;
  uint32_t    start_time;
  uint32_t    end_time;

  puts( "TA1 - rtems_task_suspend - on Task 2" );
  status = rtems_task_suspend( Task_id[ 2 ] );
  directive_failed( status, "rtems_task_suspend of TA2" );

  puts( "TA1 - rtems_task_suspend - on Task 3" );
  status = rtems_task_suspend( Task_id[ 3 ] );
  directive_failed( status, "rtems_task_suspend of TA3" );

  status = rtems_clock_get_seconds_since_epoch( &start_time );
  directive_failed( status, "rtems_clock_get_seconds_since_epoch" );

  puts( "TA1 - killing time" );

  for ( ; ; ) {
    status = rtems_clock_get_seconds_since_epoch( &end_time );
    directive_failed( status, "rtems_clock_get_seconds_since_epoch" );

    if ( end_time > (start_time + 2) )
      break;
  }

  puts( "TA1 - rtems_task_resume - on Task 2" );
  status = rtems_task_resume( Task_id[ 2 ] );
  directive_failed( status, "rtems_task_resume of TA2" );

  puts( "TA1 - rtems_task_resume - on Task 3" );
  status = rtems_task_resume( Task_id[ 3 ] );
  directive_failed( status, "rtems_task_resume of TA3" );

  while ( FOREVER ) {
    if ( Run_count[ 1 ] == 3 ) {
      puts( "TA1 - rtems_task_mode - change mode to NO RTEMS_PREEMPT" );

      status = rtems_task_mode(
        RTEMS_NO_PREEMPT,
        RTEMS_PREEMPT_MASK,
        &previous_mode
      );
      directive_failed( status, "rtems_task_mode" );

      status = rtems_clock_get_tod( &time );
      directive_failed( status, "rtems_clock_get_tod" );

      old_seconds = time.second;

      for ( seconds = 0 ; seconds < 6 ; ) {
        status = rtems_clock_get_tod( &time );
        directive_failed( status, "rtems_clock_get_tod" );

        if ( time.second != old_seconds ) {
          old_seconds = time.second;
          seconds++;
          print_time( "TA1 - ", &time, "\n" );
        }
      }

      puts( "TA1 - rtems_task_mode - change mode to RTEMS_PREEMPT" );
      status = rtems_task_mode(
        RTEMS_PREEMPT,
        RTEMS_PREEMPT_MASK,
        &previous_mode
      );
      directive_failed( status, "rtems_task_mode" );

      while ( FOREVER );
    }
  }
}
