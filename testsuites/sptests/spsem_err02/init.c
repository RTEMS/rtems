/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2014.
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

const char rtems_test_name[] = "SP SEMAPHORE ERROR 02";

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;
  
  TEST_BEGIN();
   
  Semaphore_name[ 1 ]  =  rtems_build_name( 'S', 'M', '1', ' ' );
  Semaphore_name[ 2 ]  =  rtems_build_name( 'S', 'M', '2', ' ' );
  Task_name[ 2 ]       =  rtems_build_name( 'T', 'A', '2', ' ' );

  status = rtems_task_create(
    Task_name[ 2 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );
  puts( "TA1 - rtems_task_create - TA2 created - RTEMS_SUCCESSFUL" );
  
  puts( "TA1 - rtems_task_start - start TA2 - RTEMS_SUCCESSFUL" );
  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" ); 

  /* OK */
  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 1 ]
  );
  directive_failed( status, "rtems_semaphore_create" );
  puts( "TA1 - rtems_semaphore_create - 1 - RTEMS_SUCCESSFUL" );

  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create" );
  puts( "TA1 - rtems_semaphore_create - 2 - RTEMS_SUCCESSFUL" );

  status = rtems_semaphore_obtain(
    100,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_obtain with illegal id"
  );
  puts( "TA1 - rtems_semaphore_obtain - RTEMS_INVALID_ID" );

  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain successful" );
  puts( "TA1 - rtems_semaphore_obtain - got sem 1 - RTEMS_SUCCESSFUL" );

  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_semaphore_obtain not available"
  );
  puts( "TA1 - rtems_semaphore_obtain - RTEMS_UNSATISFIED" );

  puts( "TA1 - rtems_semaphore_obtain - timeout in 3 seconds" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    3 * rtems_clock_get_ticks_per_second()
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_semaphore_obtain timeout"
  );
  puts( "TA1 - rtems_semaphore_obtain - woke up with RTEMS_TIMEOUT" );

  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  fatal_directive_status(
    status,
    RTEMS_NOT_OWNER_OF_RESOURCE,
    "rtems_semaphore_release and not owner"
  );
  puts( "TA1 - rtems_semaphore_release - RTEMS_NOT_OWNER_OF_RESOURCE" );

  status = rtems_semaphore_release( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_release with illegal id"
  );
  puts( "TA1 - rtems_semaphore_release - RTEMS_INVALID_ID" );

  puts( "TA1 - rtems_task_wake_after - yield processor - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after (yield)" );

  puts( "TA1 - rtems_semaphore_delete - delete sem 1 - RTEMS_SUCCESSFUL" );
  status = rtems_semaphore_delete( Semaphore_id[ 1 ] );
  directive_failed( status, "rtems_semaphore_delete of SM1" );

  puts( "TA1 - rtems_semaphore_obtain - binary semaphore" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain");

  puts( "TA1 - rtems_semaphore_delete - delete sem 2 - RTEMS_RESOURCE_IN_USE" );

  status = rtems_semaphore_delete( Semaphore_id[ 2 ] );
  fatal_directive_status(
     status,
     RTEMS_RESOURCE_IN_USE,
     "rtems_semaphore_delete of SM2"
  );

  puts( "TA1 - rtems_task_wake_after - yield processor - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after (yield)" );
  
  TEST_END();
}
