/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_1
 *
 *  This routine serves as a test task.  It verifies that tasks can
 *  be suspended and resumed.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  NOTE: The rtems_task_suspend() directives fail on the first iteration.
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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id          tid2;
  rtems_id          tid3;
  uint32_t    pass;
  rtems_status_code status;

  status = rtems_task_ident( Task_name[ 2 ], 1, &tid2 );
  directive_failed( status, "rtems_task_ident of TA2" );

  status = rtems_task_ident( Task_name[ 3 ], 1, &tid3 );
  directive_failed( status, "rtems_task_ident of TA3" );

  for ( pass=1 ; pass <= 3 ; pass++ ) {

    puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
    status = rtems_task_wake_after( 5*rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after of TA1" );

    puts( "TA1 - rtems_task_suspend - suspend TA3" );
    status = rtems_task_suspend( tid3 );
    if ( pass == 1 ) {
      fatal_directive_status(
         status,
         RTEMS_ALREADY_SUSPENDED,
         "rtems_task_suspend of TA3"
      );
    } else {
      directive_failed( status, "rtems_task_suspend of TA3" );
    }

    puts( "TA1 - rtems_task_resume - resume TA2" );
    status = rtems_task_resume( tid2 );
    directive_failed( status, "rtems_task_resume of TA2" );

    puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
    status = rtems_task_wake_after( 5*rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "TA1 - rtems_task_suspend - suspend TA2" );
    status = rtems_task_suspend( tid2 );
    directive_failed( status, "rtems_task_suspend of TA2" );

    puts( "TA1 - rtems_task_resume - resume TA3" );
    status = rtems_task_resume( tid3 );
    directive_failed( status, "rtems_task_resume" );
  }

  TEST_END();
  rtems_test_exit( 0 );
}
