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

#include <rtems/rtems/tasksimpl.h>

const char rtems_test_name[] = "TASK ERROR 04";

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code   status;
  
  TEST_BEGIN();

  Task_id[ 0 ] = rtems_task_self();

  Task_name[ 1 ]       =  rtems_build_name( 'T', 'A', '1', ' ' );

  /* priority of 0 error */
  status = rtems_task_create(
     Task_name[1],
     0,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES,
     &Task_id[ 1 ]
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_PRIORITY,
    "rtems_task_create with illegal priority"
  );
  puts( "INIT - rtems_task_create - priority of 0 - RTEMS_INVALID_PRIORITY" );

  /* priority > 255 error */
  status = rtems_task_create(
     Task_name[1],
     UINT32_C(0x80000000),
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES,
     &Task_id[ 1 ]
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_PRIORITY,
    "rtems_task_create with illegal priority"
  );
  puts(
    "INIT - rtems_task_create - priority too high - RTEMS_INVALID_PRIORITY"
  );

  status = rtems_task_create(
    Task_name[ 1 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_restart( Task_id[ 1 ], 0 );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_task_restart of DORMANT task"
  );
  puts( "INIT - rtems_task_restart - RTEMS_INCORRECT_STATE" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  rtems_task_exit();
}
