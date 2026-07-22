/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2026 Abdullah Wasiq (aftwasiq@gmail.com)
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

#include <rtems.h>
#include <tmacros.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <rtems/score/threadimpl.h>

const char rtems_test_name[] = "SPCPUUSAGE 1";

rtems_task Task01( rtems_task_argument ignored );
rtems_task Task02( rtems_task_argument ignored );
rtems_task Init( rtems_task_argument ); 

rtems_id    Task_id[ 3 ];
rtems_name  Task_name[ 3 ];

rtems_task Task01( rtems_task_argument ignored )
{
  (void) ignored;

  rtems_status_code status;
  struct timespec ts;

  for ( ;; ) {
    status = rtems_clock_get_uptime( &ts );
    directive_failed( status, "get uptime" );
    
    if ( ts.tv_nsec > 300000000L ) {
      status = rtems_task_get_cpu_usage( Task_id[ 1 ], &ts );
      directive_failed( status, "cpu usage" );

      /* assert usage remains under or equivalent to 200ms from t=100ms to t=300ms */
      assert( 200 >= ((uint32_t) ts.tv_nsec / 1000000) );

      rtems_task_suspend( RTEMS_SELF );
    }
  }
}

rtems_task Task02( rtems_task_argument ignored )
{
  (void) ignored;

  rtems_status_code status;
  struct timespec ts;
  bool acquired = false;

  for ( ;; ) {
    status = rtems_clock_get_uptime( &ts );
    directive_failed( status, "get uptime" );

    if ( !acquired && ts.tv_nsec >= 100000000L ) {
      acquired = true;
      
      status = rtems_task_get_cpu_usage( Task_id[ 2 ], &ts );
      directive_failed( status, "cpu usage" );

      assert( 100 >= ((uint32_t) ts.tv_nsec / 1000000) );

      status = rtems_task_start( Task_id[ 1 ], Task01, 0 );
      directive_failed( status, "start task" );
    }
    
    if ( ts.tv_nsec >= 300000000L ) {
      status = rtems_task_get_cpu_usage( Task_id[ 2 ], &ts );
      directive_failed( status, "cpu usage" );

      /*
       * The accumulated time after the interrupt should be about 0 milliseconds
       * or some value close to 0 depending on the rate of instruction execution
       *
       * this means the same assert (under or equal to 100ms) from before the
       * interrupt should hold true even after the interrupt
       */ 
      assert( 100 >= ((uint32_t) ts.tv_nsec / 1000000) );

      TEST_END();
      rtems_test_exit( 0 );
    }
  }
}

rtems_task Init( rtems_task_argument ignored )
{
  (void) ignored;

  TEST_BEGIN();

  rtems_status_code status;

  /* error handling for an invalid timespec address */
  struct timespec *t = NULL;
  status = rtems_task_get_cpu_usage( Task_id[ 1 ], t );
  assert( status == RTEMS_INVALID_ADDRESS );

  /* error handling for an invalid task id */
  struct timespec ts;
  rtems_id test = 0xFFFF;
  status = rtems_task_get_cpu_usage( test, &ts ); 
  assert( status == RTEMS_INVALID_ID );

  rtems_time_of_day time;

  time.year = 2026;
  time.month = 6;
  time.day = 15;
  time.hour = 8;
  time.minute = 0;
  time.second = 0;
  time.ticks = 0;

  status = rtems_clock_set( &time );
  directive_failed( status, "get tod" );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  
  status = rtems_task_create(
    Task_name[ 1 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &Task_id[ 1 ]
  );
  
  directive_failed( status, "create task" );

  status = rtems_task_create(
    Task_name[ 2 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT,
    RTEMS_FLOATING_POINT,
    &Task_id[ 2 ]
  );

  directive_failed( status, "create task" );

  status = rtems_task_start( Task_id[ 2 ], Task02, 0 );
  directive_failed( status, "start task" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "delete task" );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_PRIORITY 2

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
