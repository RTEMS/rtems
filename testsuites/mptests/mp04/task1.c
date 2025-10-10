/* SPDX-License-Identifier: BSD-2-Clause */

/*  Test_task
 *
 *  This task tests the rtems_task_set_priority directive on a remote task.
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
#include "tmacros.h"

rtems_task Test_task(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_id            tid;
  rtems_status_code   status;
  uint32_t      remote_node;
  rtems_id            remote_tid;
  rtems_task_priority previous_priority;
  rtems_task_priority previous_priority_1;

  status = rtems_task_ident( RTEMS_WHO_AM_I, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  puts( "Getting TID of remote task" );
  remote_node = (rtems_object_get_local_node() == 1) ? 2 : 1;
  puts_nocr( "Remote task's name is : " );
  put_name( Task_name[ remote_node ], TRUE );

  do {
      status = rtems_task_ident(
          Task_name[ remote_node ],
          RTEMS_SEARCH_ALL_NODES,
          &remote_tid
          );
  } while ( status != RTEMS_SUCCESSFUL );

  directive_failed( status, "rtems_task_ident" );

  status = rtems_task_set_priority(
    remote_tid,
    rtems_object_get_local_node(),
    &previous_priority
  );
  directive_failed( status, "rtems_task_set_priority" );

  if ( previous_priority != remote_node ) {
    printf(
      "Remote priority (0x%" PRIxrtems_task_priority ") does not match remote node (0x%" PRIx32 ")!!!\n",
      previous_priority,
      remote_node
    );
    rtems_test_exit( 0xf0000 );
  }

  do {
    status = rtems_task_set_priority(
      RTEMS_SELF,
      RTEMS_CURRENT_PRIORITY,
      &previous_priority_1
    );
    directive_failed( status, "rtems_task_set_priority" );
  } while ( previous_priority_1 != remote_node );

  puts( "Local task priority has been set" );

  puts( "*** END OF TEST 4 ***" );
  rtems_test_exit( 0 );
}
