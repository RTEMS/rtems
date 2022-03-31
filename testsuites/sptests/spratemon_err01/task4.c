/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_4
 *
 *  This routine serves as a test task.  Its only purpose in life is to
 *  generate the error where a rate monotonic period is accessed by a
 *  task other than its creator.
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

rtems_task Task_4(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  status = rtems_rate_monotonic_cancel( Period_id[ 1 ] );
  fatal_directive_status(
    status,
    RTEMS_NOT_OWNER_OF_RESOURCE,
    "rtems_rate_monotonic_cancel not the owner"
  );
  puts( "TA4 - rtems_rate_monotonic_cancel - RTEMS_NOT_OWNER_OF_RESOURCE" );

  status = rtems_rate_monotonic_period( Period_id[ 1 ], 5 );
  fatal_directive_status(
    status,
    RTEMS_NOT_OWNER_OF_RESOURCE,
    "rtems_rate_monotonic_period not the owner"
  );
  puts( "TA4 - rtems_rate_monotonic_period - RTEMS_NOT_OWNER_OF_RESOURCE" );

  puts( "TA4 - rtems_task_exit" );
  rtems_task_exit();
}
