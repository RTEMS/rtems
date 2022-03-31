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

void Priority_test_driver(
  rtems_task_priority priority_base
)
{
  rtems_task_priority previous_priority;
  uint32_t            index;
  rtems_status_code   status;

  for ( index = 1 ; index <= 5 ; index++ ) {
    switch ( index ) {
       case 1:
       case 2:
       case 3:
         Task_priority[ index ] = priority_base + index;
         break;
       default:
         Task_priority[ index ] = priority_base + 3;
         break;
    }

    status = rtems_task_create(
      Priority_task_name[ index ],
      Task_priority[ index ],
      RTEMS_MINIMUM_STACK_SIZE * 2,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Priority_task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );

  }

  if ( priority_base == 0 ) {
    for ( index = 1 ; index <= 5 ; index++ ) {
      status = rtems_task_start(
        Priority_task_id[ index ],
        Priority_task,
        index
      );
      directive_failed( status, "rtems_task_start loop" );
    }
  } else {
    for ( index = 5 ; index >= 1 ; index-- ) {
      status = rtems_task_start(
        Priority_task_id[ index ],
        Priority_task,
        index
      );
      directive_failed( status, "rtems_task_start loop" );

      status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
      directive_failed( status, "rtems_task_wake_after loop" );

      if ( priority_base == PRIORITY_INHERIT_BASE_PRIORITY ) {
        if ( index == 4 ) {
          status = rtems_task_set_priority(
            Priority_task_id[ 5 ],
            priority_base + 4,
            &previous_priority
          );
          printf(
	    "PDRV - change priority of PRI5 from %" PRIdrtems_task_priority
              " to %" PRIdrtems_task_priority "\n",
             previous_priority,
             priority_base + 4
          );
          directive_failed( status, "PDRV rtems_task_set_priority" );
        }
        status = rtems_task_set_priority(
          Priority_task_id[ 5 ],
          RTEMS_CURRENT_PRIORITY,
          &previous_priority
        );
        directive_failed( status, "PDRV rtems_task_set_priority CURRENT" );
        printf(
          "PDRV - priority of PRI5 is %" PRIdrtems_task_priority "\n",
           previous_priority
        );
      }
    }
  }

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after after loop" );

  if ( priority_base == 0 ) {
    for ( index = 1 ; index <= 5 ; index++ ) {
      status = rtems_semaphore_release( Semaphore_id[ 2 ] );
      directive_failed( status, "rtems_semaphore_release loop" );
    }
  }

  if ( priority_base == PRIORITY_INHERIT_BASE_PRIORITY ) {
    puts( "PDRV - rtems_task_resume - PRI5" );
    status = rtems_task_resume( Priority_task_id[ 5 ] );
    directive_failed( status, "rtems_task_resume" );

    status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after so PRI5 can run" );

    status = rtems_task_delete( Priority_task_id[ 5 ] );
    directive_failed( status, "rtems_task_delete of PRI5" );
  } else {
    for ( index = 1 ; index <= 5 ; index++ ) {
      status = rtems_task_delete( Priority_task_id[ index ] );
      directive_failed( status, "rtems_task_delete loop" );
    }
  }
}
