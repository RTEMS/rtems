/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_2
 *
 *  This routine serves as a test task.  It simply obtains semaphores
 *  1 and 2, the later when it is a high priority task.
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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  rtems_task_priority previous_priority;

  puts( "TA2 - rtems_semaphore_obtain - wait forever on SM1" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  puts( "TA2 - got SM1" );
  directive_failed( status, "rtems_semaphore_obtain on SM1" );

  puts( "TA2 - rtems_semaphore_release - release SM1" );
  status = rtems_semaphore_release( Semaphore_id[ 1 ] );
  directive_failed( status, "rtems_semaphore_release on SM1" );

  puts( "TA2 - rtems_task_set_priority - make self highest priority task" );
  status = rtems_task_set_priority( RTEMS_SELF, 3, &previous_priority );
  directive_failed( status, "rtems_task_set_priority on TA2" );

  puts( "TA2 - rtems_semaphore_obtain - wait forever on SM2" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  puts( "TA2 - got SM2" );
  directive_failed( status, "rtems_semaphore_obtain on SM2" );

  puts( "TA2 - rtems_semaphore_release - release SM2" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release on SM2" );

  puts( "TA2 - rtems_task_exit" );
  rtems_task_exit();
}
