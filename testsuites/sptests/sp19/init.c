/* SPDX-License-Identifier: BSD-2-Clause */

/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
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

const char rtems_test_name[] = "SP 19";

void thread_switch_extension( Thread_Control *executing, Thread_Control *heir )
{
  (void) executing;

  if ( heir->is_fp ) {
    rtems_print_printer_fprintf_putc( &rtems_test_printer );
  } else {
    rtems_print_printer_printk( &rtems_test_printer );
  }
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;

  TEST_BEGIN();

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] =  rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] =  rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] =  rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ] =  rtems_build_name( 'T', 'A', '5', ' ' );
  Task_name[ 6 ] =  rtems_build_name( 'F', 'P', '1', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
    Task_name[ 2 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_create(
    Task_name[ 3 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_create(
    Task_name[ 4 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );

  status = rtems_task_create(
    Task_name[ 5 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &Task_id[ 5 ]
  );
  directive_failed( status, "rtems_task_create of TA5" );

  status = rtems_task_create(
    Task_name[ 6 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &Task_id[ 6 ]
  );
  directive_failed( status, "rtems_task_create of FP1" );

  status = rtems_task_start( Task_id[ 6 ], First_FP_task, 0 );
  directive_failed( status, "rtems_task_start of FP1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  status = rtems_task_start( Task_id[ 4 ], FP_task, 0 );
  directive_failed( status, "rtems_task_start of TA4" );

  status = rtems_task_start( Task_id[ 5 ], FP_task, 0 );
  directive_failed( status, "rtems_task_create of TA5" );

  /*
   *  Load "task dependent factors" in the context areas
   */


  FP_factors[0] =    0.0;
  FP_factors[1] = 1000.1;
  FP_factors[2] = 2000.2;
  FP_factors[3] = 3000.3;
  FP_factors[4] = 4000.4;
  FP_factors[5] = 5000.5;
  FP_factors[6] = 6000.6;
  FP_factors[7] = 7000.7;
  FP_factors[8] = 8000.8;
  FP_factors[9] = 9000.9;

  INTEGER_factors[0] = 0x0000;
  INTEGER_factors[1] = 0x1000;
  INTEGER_factors[2] = 0x2000;
  INTEGER_factors[3] = 0x3000;
  INTEGER_factors[4] = 0x4000;
  INTEGER_factors[5] = 0x5000;
  INTEGER_factors[6] = 0x6000;
  INTEGER_factors[7] = 0x7000;
  INTEGER_factors[8] = 0x8000;
  INTEGER_factors[9] = 0x9000;

  rtems_task_exit();
}
