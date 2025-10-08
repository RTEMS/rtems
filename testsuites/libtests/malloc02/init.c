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

const char rtems_test_name[] = "MALLOC 2";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_timer_service_routine test_operation_from_isr(rtems_id timer, void *arg);

volatile bool operation_performed_from_tsr;

void *Pointer1;

rtems_timer_service_routine test_operation_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  (void) timer;
  (void) arg;

  /* free memory from ISR so it is deferred */
  free( Pointer1 );

  operation_performed_from_tsr = true;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code     status;
  rtems_id              timer;
  void                 *pointer2;

  TEST_BEGIN();

  puts( "malloc memory to free from ISR" );
  Pointer1 = malloc( 20 );

  /*
   *  Timer used in multiple ways
   */
  status = rtems_timer_create( rtems_build_name('T', 'M', 'R', '0'), &timer );
  directive_failed( status, "rtems_timer_create" );

  operation_performed_from_tsr = false;

  /*
   * Test Operation from ISR
   */
  status = rtems_timer_fire_after( timer, 10, test_operation_from_isr, NULL );
  directive_failed( status, "timer_fire_after failed" );

  /* delay to let timer fire */
  status = rtems_task_wake_after( 20 );
  directive_failed( status, "timer_wake_after failed" );

  if ( !operation_performed_from_tsr ) {
    puts( "Operation from ISR did not get processed\n" );
    rtems_test_exit( 0 );
  }

  puts( "Free from ISR successfully processed" );
  puts( "Now malloc'ing more memory to process the free" );
  pointer2 = malloc(20);
  rtems_test_assert( pointer2 );

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MALLOC_DIRTY

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_MAXIMUM_TIMERS            1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */

