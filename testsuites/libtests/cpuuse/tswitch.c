/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_switch
 *
 *  This routine is the tswitch user extension.  It determines which
 *  task is being switched to and displays a message indicating the
 *  time and date that it gained control.
 *
 *  Input parameters:
 *    unused  - pointer to currently running TCB
 *    heir    - pointer to heir TCB
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

#include "system.h"

/*
 *  Only require 8 task switches on sis/gdb.  --joel 2 Feb 2011
 */
struct taskSwitchLog taskSwitchLog[25];
int taskSwitchLogIndex;
volatile int testsFinished;

void Task_switch(
  rtems_tcb *unused,
  rtems_tcb *heir
)
{
  uint32_t    index;
  rtems_time_of_day time;
  rtems_status_code status;

  index = task_number( heir->Object.id );

  switch( index ) {
    case 1:
    case 2:
    case 3:
      Run_count[ index ] += 1;

      status = rtems_clock_get_tod( &time );
      fatal_directive_check_status_only( status, RTEMS_SUCCESSFUL,
                                         "rtems_clock_get_tod" );

      if (taskSwitchLogIndex <
          (sizeof taskSwitchLog / sizeof taskSwitchLog[0])) {
        taskSwitchLog[taskSwitchLogIndex].taskIndex = index;
        taskSwitchLog[taskSwitchLogIndex].when = time;
        taskSwitchLogIndex++;
      }
      if ( time.second >= 16 )
        testsFinished = 1;

      break;

    case 0:
    default:
      break;
  }
}

