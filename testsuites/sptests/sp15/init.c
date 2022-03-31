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

const char rtems_test_name[] = "SP 15";

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  TEST_BEGIN();

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );

  Partition_name[ 1 ] =  rtems_build_name( 'P', 'T', '1', ' ' );
  Partition_name[ 2 ] =  rtems_build_name( 'P', 'T', '2', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    4u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  puts("INIT - rtems_partition_create - partition 1");
  status = rtems_partition_create(
    Partition_name[ 1 ],
    Area_1,
    4096,
    512,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Partition_id[ 1 ]
  );
  directive_failed( status, "rtems_partition_create of PT1" );

  puts("INIT - rtems_partition_create - partition 2");
  status = rtems_partition_create(
    Partition_name[ 2 ],
    Area_2,
    274,
    128,
    RTEMS_GLOBAL,
    &Partition_id[ 2 ]
  );
  directive_failed( status, "rtems_partition_create of PT2" );

  rtems_task_exit();
}
