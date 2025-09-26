/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_1
 *
 *  This routine serves as a test task.  It verifies the semaphore manager.
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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_id          smid;
  rtems_status_code status;

  status = rtems_semaphore_ident(
    Semaphore_name[ 1 ],
    RTEMS_SEARCH_ALL_NODES,
    &smid
  );
  printf( "TA1 - rtems_semaphore_ident - smid => %08" PRIxrtems_id "\n", smid );
  directive_failed( status, "rtems_semaphore_ident of SM1" );

  puts( "TA1 - rtems_semaphore_obtain - wait forever on SM2" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain of SM2" );
  puts( "TA1 - got SM2" );

  puts( "TA1 - rtems_semaphore_obtain - wait forever on SM3" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 3 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain of SM3" );
  puts( "TA1 - got SM3" );

  puts( "TA1 - rtems_semaphore_obtain - get SM1 - RTEMS_NO_WAIT" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain of SM1" );
  puts( "TA1 - got SM1" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_semaphore_release - release SM1" );
  status = rtems_semaphore_release( Semaphore_id[ 1 ] );
  directive_failed( status, "rtems_semaphore_release of SM1" );

  puts(
    "TA1 - rtems_semaphore_obtain - waiting for SM1 with 10 second timeout"
  );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    10 * rtems_clock_get_ticks_per_second()
  );
  directive_failed( status, "rtems_semaphore_obtain of SM1" );
  puts( "TA1 - got SM1" );

  puts( "TA1 - rtems_semaphore_release - release SM2" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release of SM2" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_delete - delete TA3" );
  status = rtems_task_delete( Task_id[ 3 ] );
  directive_failed( status, "rtems_task_delete of TA3" );

  status = rtems_task_create(
    Task_name[ 4 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );

  status = rtems_task_create(
    Task_name[ 5 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 5 ]
   );
  directive_failed( status, "rtems_task_create of TA5" );

  status = rtems_task_start( Task_id[ 4 ], Task_4, 0 );
  directive_failed( status, "rtems_task_start of TA4" );

  status = rtems_task_start( Task_id[ 5 ], Task5, 0 );
  directive_failed( status, "rtems_task_start of TA5" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_delete - delete TA4" );
  status = rtems_task_delete( Task_id[ 4 ] );
  directive_failed( status, "rtems_task_delete of TA4" );

  puts( "TA1 - rtems_semaphore_release - release SM1" );
  status = rtems_semaphore_release( Semaphore_id[ 1 ] );
  directive_failed( status, "rtems_semaphore_release on SM1" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_semaphore_delete - delete SM1" );
  status = rtems_semaphore_delete( Semaphore_id[ 1 ] );
  directive_failed( status, "rtems_semaphore_delete of SM1" );

  puts( "TA1 - rtems_semaphore_delete - delete SM3" );
  status = rtems_semaphore_delete( Semaphore_id[ 3 ] );
  directive_failed( status, "rtems_semaphore_delete of SM3" );

  puts( "TA1 - rtems_task_exit" );
  rtems_task_exit();
}
