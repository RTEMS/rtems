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

#include <inttypes.h>

const char rtems_test_name[] = "SMP 1";

void Loop() {
  volatile int i;

  for (i=0; i<300000; i++);
}

rtems_task Init(
  rtems_task_argument argument
)
{
  uint32_t           i;
  char               ch;
  uint32_t           cpu_self;
  rtems_id           id;
  rtems_status_code  status;
  bool               allDone;

  cpu_self = rtems_scheduler_get_processor();

  /* XXX - Delay a bit to allow debug messages from
   * startup to print.  This may need to go away when
   * debug messages go away.
   */ 
  Loop();

  TEST_BEGIN();

  locked_print_initialize();

  /* Initialize the TaskRan array */
  for ( i=0; i<rtems_scheduler_get_processor_maximum() ; i++ ) {
    TaskRan[i] = false;
  }

  /* Create and start tasks for each processor */
  for ( i=0; i< rtems_scheduler_get_processor_maximum() ; i++ ) {
    if ( i != cpu_self ) {
      ch = '0' + i;

      status = rtems_task_create(
        rtems_build_name( 'T', 'A', ch, ' ' ),
        1,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES,
        &id
      );
      directive_failed( status, "task create" );

      locked_printf(" CPU %" PRIu32 " start task TA%c\n", cpu_self, ch);
      status = rtems_task_start( id, Test_task, i+1 );
      directive_failed( status, "task start" );

      Loop();
    }
  }
  
  /* Wait on the all tasks to run */
  while (1) {
    allDone = true;
    for ( i=0; i<rtems_scheduler_get_processor_maximum() ; i++ ) {
      if ( i != cpu_self && TaskRan[i] == false)
        allDone = false;
    }
    if (allDone) {
      TEST_END();
      rtems_test_exit( 0 );
    }
  }

}
