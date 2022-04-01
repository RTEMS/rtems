/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_1
 *
 *  This test serves as a test task.  It creates and deletes
 *  the remaining tasks, twice so that the tasks displayed by
 *  the top command grow past the max tasks displayed then shrink
 *  back down.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
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

#include "system.h"

static void create_and_start( uint32_t i )
{
  rtems_status_code status;
  char              str[30];
  char              name[6];

  sprintf(name, "TA%02" PRId32 " ", i);

  Task_name[ i ] =  rtems_build_name(
    name[0], name[1], name[2], name[3]
  );

  status = rtems_task_create(
    Task_name[ i ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_TIMESLICE,
    RTEMS_FLOATING_POINT,
    &Task_id[ i ]
  );
  sprintf(str,"rtems_task_create of %s", name);
  directive_failed( status, str );

  status = rtems_task_start( Task_id[ i ], Task_3, i );
  sprintf(str, "rtems_task_start of %s", name);
  directive_failed( status, str);
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  char              str[80];
  uint32_t          i,j;

  for(j=0; j<2; j++) {

    for( i=3; i<TASK_COUNT; i++) {
      create_and_start(i);
      status = rtems_task_wake_after (TicksPerSecond * 5);
      directive_failed( status, "rtems_task_wake_after" );
    }

    status = rtems_task_wake_after (TicksPerSecond * 10);
    directive_failed( status, "rtems_task_wake_after" );

    for( i=3; i<TASK_COUNT; i++) {
      status = rtems_task_delete( Task_id[i] );
      sprintf(str, "task delete TA%02" PRId32 "", i);
      directive_failed( status, str );
      status = rtems_task_wake_after (TicksPerSecond * 5);
      directive_failed( status, "rtems_task_wake_after" );
    }
  }

  TEST_END();
  rtems_test_exit( 0 );
}
