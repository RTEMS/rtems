/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  rtems_task_priority the_priority;
  rtems_task_priority previous_priority;

  status = rtems_task_set_priority(
    RTEMS_SELF,
    RTEMS_CURRENT_PRIORITY,
    &the_priority
  );
  directive_failed( status, "rtems_task_set_priority" );

  while( FOREVER ) {
    if ( --the_priority == 0 ) {
      puts( "TA2 - rtems_task_suspend - suspend TA1" );
      status = rtems_task_suspend( Task_id[ 1 ] );
      directive_failed( status, "rtems_task_suspend" );

      puts( "TA2 - rtems_task_set_priority - set priority of TA1 ( blocked )" );
      status = rtems_task_set_priority( Task_id[ 1 ], 5, &previous_priority );
      directive_failed( status, "rtems_task_set_priority" );

      status = rtems_task_delete( Task_id[ 1 ] );          /* TA1 is blocked */
      directive_failed( status, "rtems_task_delete of TA1" );
      assert_extension_counts( &Task_deleted[ 0 ], 0x0 );

      status = rtems_task_delete( Task_id[ 3 ] );          /* TA3 is ready   */
      directive_failed( status, "rtems_task_delete of TA3" );
      assert_extension_counts( &Task_deleted[ 0 ], 0x0 );

      rtems_task_exit();
    } else {

      printf(
        "TA2 - rtems_task_set_priority - set TA1's priority: "
            "%02" PRIdrtems_task_priority "\n",
        the_priority
      );
      status = rtems_task_set_priority(
                 Task_id[ 1 ],
                 the_priority,
                 &previous_priority
      );
      directive_failed( status, "rtems_task_set_priority" );
    }
  }
}
