/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
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
#include "pritime.h"

const char rtems_test_name[] = "SP 50";

/* forward declarations to avoid warnings */
rtems_timer_service_routine Timer_Routine(rtems_id id, void *ignored);
rtems_task Init(rtems_task_argument argument);

volatile int   Fired;
volatile bool  timerRan;

rtems_timer_service_routine Timer_Routine(rtems_id id, void *ignored)
{
  rtems_status_code status;

  Fired++;
  timerRan = true;

  status = rtems_timer_server_fire_after(
    id,
    rtems_clock_get_ticks_per_second(),
    Timer_Routine,
    NULL
  );
  directive_failed( status, "fire after" );

}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code sc;
  rtems_id          timer1;
  struct timespec   uptime;

  TEST_BEGIN();

  sc = rtems_timer_initiate_server(
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  directive_failed( sc, "rtems_timer_initiate_server" );

  sc = rtems_timer_create(rtems_build_name( 'T', 'M', 'R', '1' ), &timer1);
  directive_failed( sc, "rtems_timer_create" );

  Fired = 0;
  timerRan = false;

  Timer_Routine(timer1, NULL);

  while (1) {
    sc = rtems_task_wake_after( 10 );
    directive_failed( sc, "rtems_task_wake_after" );

    if ( timerRan == true ) {
      timerRan = false;

      sc = rtems_clock_get_uptime( &uptime );
      directive_failed( sc, "rtems_clock_get_uptime" );

      printf( "Timer fired at %" PRIdtime_t "\n", uptime.tv_sec );
    }

    if ( Fired >= 10 ) {
      TEST_END();
      rtems_test_exit( 0 );
    }
    /* technically the following is a critical section */
  }
}


/**************** START OF CONFIGURATION INFORMATION ****************/

#define CONFIGURE_INIT
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         2
#define CONFIGURE_MAXIMUM_TIMERS        1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/****************  END OF CONFIGURATION INFORMATION  ****************/
