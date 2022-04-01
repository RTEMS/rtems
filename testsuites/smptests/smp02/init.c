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

#include <stdio.h>
#include <inttypes.h>

const char rtems_test_name[] = "SMP 2";

static void success(void)
{
  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int               i;
  char              ch;
  uint32_t          cpu_num;
  rtems_id          id;
  rtems_status_code status;
  char              str[80];

  TEST_BEGIN();

  locked_print_initialize();

  if ( rtems_scheduler_get_processor_maximum() == 1 ) {
    success();
  }

  /* Create/verify synchronisation semaphore */
  status = rtems_semaphore_create(
    rtems_build_name ('S', 'E', 'M', '1'),
    1,                                             
    RTEMS_LOCAL                   |
    RTEMS_SIMPLE_BINARY_SEMAPHORE |
    RTEMS_PRIORITY,
    1,
    &Semaphore);
  directive_failed( status, "rtems_semaphore_create" );

  /* Lock semaphore */
  status = rtems_semaphore_obtain( Semaphore, RTEMS_WAIT, 0);
  directive_failed( status,"rtems_semaphore_obtain of SEM1\n");

  for ( i=1; i < rtems_scheduler_get_processor_maximum(); i++ ){

    /* Create and start tasks for each CPU */
    ch = '0' + i;

    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch, ' ' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );

    cpu_num = rtems_scheduler_get_processor();
    locked_printf(" CPU %" PRIu32 " start task TA%c\n", cpu_num, ch);
    status = rtems_task_start( id, Test_task, i+1 );
    directive_failed( status, str );
  }

  /*
   * Release the semaphore, allowing the blocked tasks to start.
   */
  status = rtems_semaphore_release( Semaphore );
  directive_failed( status,"rtems_semaphore_release of SEM1\n");
  

  /* 
   * Wait for log full. print the log and end the program.
   */  
  while (Log_index < LOG_SIZE)
    ;
 
  for (i=0; i< LOG_SIZE; i++) {
    if ( Log[i].IsLocked ) {
      locked_printf(
        " CPU %d Task TA%" PRIu32 " Obtain\n", 
        Log[i].cpu_num,
        Log[i].task_index
      );
    } else {
      locked_printf(
        " CPU %d Task TA%" PRIu32 " Release\n", 
        Log[i].cpu_num,
        Log[i].task_index
      );
    }
  }

  success();
}
