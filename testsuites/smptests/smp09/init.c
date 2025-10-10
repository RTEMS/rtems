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

#include <tmacros.h>
#include "test_support.h"
#include <rtems/cpuuse.h>

const char rtems_test_name[] = "SMP 9";

volatile int killtime;

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Test_task(
  rtems_task_argument argument
);

rtems_task Test_task(
  rtems_task_argument argument
)
{
  (void) argument;

  while (1)
    ;
  /* XXX something goes here */
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  uint32_t           i;
  char               ch;
  uint32_t           cpu_num;
  rtems_id           id;
  rtems_status_code  status;

  TEST_BEGIN();

  locked_print_initialize();

  for ( killtime=0; killtime<1000000; killtime++ )
    ;
  
  for ( i=0; i<rtems_scheduler_get_processor_maximum() -1; i++ ) {
    ch = '1' + i;

    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch, ' ' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "task create" );

    cpu_num = rtems_scheduler_get_processor();
    locked_printf(" CPU %" PRIu32 " start task TA%c\n", cpu_num, ch);

    status = rtems_task_start( id, Test_task, i+1 );
    directive_failed( status, "task start" );
  }

  locked_printf(" kill 10 clock ticks\n" );
  while ( rtems_clock_get_ticks_since_boot() < 10 )
    ;

  rtems_cpu_usage_report();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS   4

#define CONFIGURE_MAXIMUM_TASKS            \
    (1 + CONFIGURE_MAXIMUM_PROCESSORS)
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
