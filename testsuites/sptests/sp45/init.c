/* SPDX-License-Identifier: BSD-2-Clause */

/*
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

#include <tmacros.h>

const char rtems_test_name[] = "SP 45";

rtems_task Init(
  rtems_task_argument argument
);
rtems_timer_service_routine Malloc_From_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_id   Timer_id[ 3 ];    /* array of timer ids */
rtems_name Timer_name[ 3 ];  /* array of timer names */

volatile int TSR_fired;
volatile void *TSR_malloc_ptr;

rtems_timer_service_routine Malloc_From_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code  status;

  TSR_fired = 2;
  puts( "TSR: calling malloc" );
  TSR_malloc_ptr = malloc( 64 );

  puts( "TSR: calling free" );
  free( (void *) TSR_malloc_ptr );

  puts( "TSR: delaying with rtems_task_wake_after" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 2 );
  directive_failed( status, "rtems_task_wake_after" );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code  status;

  TEST_BEGIN();

  status = rtems_timer_initiate_server(
    RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  directive_failed( status, "rtems_timer_initiate_server" );

  /*
   * Initialize Timers
   */

  Timer_name[ 1 ] = rtems_build_name( 'T', 'M', '1', ' ' );
  Timer_name[ 2 ] = rtems_build_name( 'T', 'M', '2', ' ' );

  puts( "INIT - rtems_timer_create - creating timer 1" );
  status = rtems_timer_create( Timer_name[ 1 ], &Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_create" );
  printf( "INIT - timer 1 has id (0x%" PRIxrtems_id ")\n", Timer_id[ 1 ] );

  puts( "INIT - rtems_timer_create - creating timer 2" );
  status = rtems_timer_create( Timer_name[ 2 ], &Timer_id[ 2 ] );
  directive_failed( status, "rtems_timer_create" );
  printf( "INIT - timer 2 has id (0x%" PRIxrtems_id ")\n", Timer_id[ 2 ] );

  /*
   *  Schedule malloc TSR for 1 second from now
   */

  TSR_fired = 0;
  TSR_malloc_ptr = (void *) 0xa5a5a5;
  puts( "TA1 - rtems_timer_server_fire_after - timer 1 in 1 seconds" );
  status = rtems_timer_server_fire_after(
    Timer_id[ 1 ],
    1 * rtems_clock_get_ticks_per_second(),
    Malloc_From_TSR,
    NULL
  );
  directive_failed( status, "rtems_timer_server_fire_after" );

  puts( "TA1 - rtems_task_wake_after - 2 second" );
  status = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  if ( TSR_fired == 2 &&
       (TSR_malloc_ptr && TSR_malloc_ptr != (void *)0xa5a5a5) )
    puts( "TSR appears to have executed OK" );
  else {
    printf( "FAILURE ptr=%p TSR_fired=%d\n", TSR_malloc_ptr, TSR_fired );
    rtems_test_exit( 0 );
  }

  /*
   *  Delete timer and exit test
   */
  puts( "TA1 - timer_deleting - timer 1" );
  status = rtems_timer_delete( Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_delete" );


  TEST_END();
  rtems_test_exit( 0 );
}

#define CONFIGURE_INIT
/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* Two Tasks: Init and Timer Server */
#define CONFIGURE_MAXIMUM_TASKS           2
#define CONFIGURE_MAXIMUM_TIMERS          2
#define CONFIGURE_INIT_TASK_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS       (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

