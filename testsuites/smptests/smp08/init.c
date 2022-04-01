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

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SMP 8";

void PrintTaskInfo(
  const char         *task_name,
  rtems_time_of_day  *_tb 
)
{
  uint32_t cpu_num;

  cpu_num = rtems_scheduler_get_processor();

  /* Print the cpu number and task name */
  locked_printf(
    "  CPU %" PRIu32 " running task %s - rtems_clock_get_tod "
    "%02" PRId32 ":%02" PRId32 ":%02" PRId32 "   %02" PRId32 
        "/%02" PRId32 "/%04" PRId32 "\n",
    cpu_num,
    task_name,
    _tb->hour, _tb->minute, _tb->second, 
    _tb->month, _tb->day, _tb->year
  ); 
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_time_of_day time;
  uint32_t          i;
  char              ch[4];
  rtems_id          id;

  TEST_BEGIN();
 
  locked_print_initialize();

  time.year   = 1988;
  time.month  = 12;
  time.day    = 31;
  time.hour   = 9;
  time.minute = 0;
  time.second = 0;
  time.ticks  = 0;

  status = rtems_clock_set( &time );

  /* Create/verify synchronisation semaphore */
  status = rtems_semaphore_create(
    rtems_build_name ('S', 'E', 'M', '1'),
    1,                                             
    RTEMS_LOCAL                   |
    RTEMS_SIMPLE_BINARY_SEMAPHORE |
    RTEMS_PRIORITY,
    1,
    &Semaphore
  );
  directive_failed( status, "rtems_semaphore_create" );

  /* Show that the init task is running on this cpu */
  PrintTaskInfo( "Init", &time );

  for ( i=1; i <= rtems_scheduler_get_processor_maximum() *3; i++ ) {

    sprintf(ch, "%02" PRIu32, i );
    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch[0], ch[1] ),
      2,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "task create" );

    status = rtems_task_start( id, Test_task, i+1 );
    directive_failed( status, "task start" );
  }

  rtems_task_exit();
}
