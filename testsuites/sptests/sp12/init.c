/* SPDX-License-Identifier: BSD-2-Clause */

/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
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

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP 12";

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  TEST_BEGIN();

  Task_name[ 1 ]          = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ]          = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ]          = rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ]          = rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ]          = rtems_build_name( 'T', 'A', '5', ' ' );

  Priority_task_name[ 1 ] = rtems_build_name( 'P', 'R', 'I', '1' );
  Priority_task_name[ 2 ] = rtems_build_name( 'P', 'R', 'I', '2' );
  Priority_task_name[ 3 ] = rtems_build_name( 'P', 'R', 'I', '3' );
  Priority_task_name[ 4 ] = rtems_build_name( 'P', 'R', 'I', '4' );
  Priority_task_name[ 5 ] = rtems_build_name( 'P', 'R', 'I', '5' );

  Semaphore_name[ 1 ]     = rtems_build_name( 'S', 'M', '1', ' ' );
  Semaphore_name[ 2 ]     = rtems_build_name( 'S', 'M', '2', ' ' );
  Semaphore_name[ 3 ]     = rtems_build_name( 'S', 'M', '3', ' ' );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 1 ]
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );

  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    0,
    RTEMS_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create of SM2" );

  status = rtems_semaphore_create(
    Semaphore_name[ 3 ],
    1,
    RTEMS_GLOBAL,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 3 ]
  );
  directive_failed( status, "rtems_semaphore_create of SM3" );

  puts( "INIT - Forward priority queue test" );
  Priority_test_driver( 0 );

  puts( "INIT - Backward priority queue test" );
  Priority_test_driver( (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u );

  puts( "INIT - Binary Semaphore and Priority Inheritance Test" );

  status = rtems_semaphore_delete( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_delete of SM2 #1" );

  puts( "INIT - rtems_semaphore_create - allocated binary semaphore" );
  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    0,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create of priority inherit SM2" );

  puts( "INIT - rtems_semaphore_release - allocated binary semaphore" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release of SM2" );

  puts( "INIT - rtems_semaphore_delete - allocated binary semaphore" );
  status = rtems_semaphore_delete( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_delete of SM2 #2" );

  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create of priority inherit SM2" );

  Priority_test_driver( PRIORITY_INHERIT_BASE_PRIORITY );

  status = rtems_semaphore_delete( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_delete of SM2 #3" );

  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    0,
    RTEMS_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create of priority SM2" );

  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release of SM2" );

  status = rtems_task_create(
    Task_name[ 1 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
    Task_name[ 2 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_create(
    Task_name[ 3 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_3, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  rtems_task_exit();
}
