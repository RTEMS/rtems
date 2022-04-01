/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_2
 *
 *  This routine serves as a test task.  It is just a CPU bound task
 *  requiring timesliced operation.
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

#include <rtems/cpuuse.h>

static void
showTaskSwitches (void)
{
  int i;
  int switches = taskSwitchLogIndex;

  for (i = 0 ; i < switches ; i++) {
      put_name( Task_name[taskSwitchLog[i].taskIndex], FALSE );
      print_time( "- ", &taskSwitchLog[i].when, "\n" );
  }
}

rtems_task Task_2(
  rtems_task_argument argument
)
{
  while ( !testsFinished );

  showTaskSwitches ();
  puts( "" );
  rtems_cpu_usage_report();
  puts( "" );
  puts( "TA2 - RESETTING USAGE STATISTICS" );
  rtems_cpu_usage_reset();
  puts( "" );
  rtems_cpu_usage_report();
  puts( "" );
  TEST_END();
  rtems_test_exit( 0 );
}
