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

#include <rtems/score/sysstate.h>

const char rtems_test_name[] = "SP 8";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define PRIxrtems_mode 	PRIx32

#define put_mode( _comment, _output_mode ) \
   printf( "%s %08" PRIxrtems_mode "\n", _comment, _output_mode );

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_mode        previous_mode;
  rtems_status_code status;

  TEST_BEGIN();

/* BEGINNING OF RTEMS_ASR */

  status = rtems_task_mode( RTEMS_ASR, RTEMS_ASR_MASK, &previous_mode );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_ASR                  - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_ASR,
    RTEMS_ASR_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_ASR               - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_ASR,
    RTEMS_ASR_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_ASR               - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_ASR,
    RTEMS_ASR_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_ASR                  - previous mode: ",
    previous_mode
  );

/* END OF RTEMS_ASR */

/* BEGINNING OF RTEMS_TIMESLICE */

  status = rtems_task_mode(
    RTEMS_NO_TIMESLICE,
    RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_TIMESLICE         - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_TIMESLICE,
    RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_TIMESLICE            - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_TIMESLICE,
    RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_TIMESLICE            - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_TIMESLICE,
    RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_TIMESLICE         - previous mode: ",
    previous_mode
  );

/* END OF RTEMS_TIMESLICE */

/* BEGINNING OF RTEMS_PREEMPT */

  status = rtems_task_mode(
    RTEMS_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_PREEMPT              - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_PREEMPT           - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_PREEMPT           - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_PREEMPT              - previous mode: ",
    previous_mode
  );

/* END OF RTEMS_PREEMPT */

/* BEGINNING OF RTEMS_INTERRUPT_LEVEL */
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == FALSE
  status = rtems_task_mode(
    RTEMS_INTERRUPT_LEVEL(3),
    RTEMS_INTERRUPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_INTERRUPT_LEVEL( 3 ) - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_INTERRUPT_LEVEL(5),
    RTEMS_INTERRUPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_INTERRUPT_LEVEL( 5 ) - previous mode: ",
    previous_mode
  );
#else
  status = rtems_task_mode(
    RTEMS_INTERRUPT_LEVEL( 1 ),
    RTEMS_INTERRUPT_MASK,
    &previous_mode
  );
  fatal_directive_status( status, RTEMS_NOT_IMPLEMENTED, "rtems_task_mode" );
#endif
/* END OF RTEMS_INTERRUPT_LEVEL */

/* BEGINNING OF COMBINATIONS */

  status = rtems_task_mode(
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
    RTEMS_INTERRUPT_LEVEL(0) |
#else
    RTEMS_INTERRUPT_LEVEL(3) |
#endif
      RTEMS_NO_ASR |
      RTEMS_TIMESLICE | RTEMS_NO_PREEMPT,
    RTEMS_INTERRUPT_MASK | RTEMS_ASR_MASK |
      RTEMS_TIMESLICE_MASK | RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - set all modes        - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
    RTEMS_INTERRUPT_LEVEL(0) |
#else
    RTEMS_INTERRUPT_LEVEL(3) |
#endif
      RTEMS_NO_ASR |
      RTEMS_TIMESLICE | RTEMS_NO_PREEMPT,
    RTEMS_INTERRUPT_MASK | RTEMS_ASR_MASK |
      RTEMS_TIMESLICE_MASK | RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - set all modes        - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_INTERRUPT_LEVEL(0) | RTEMS_ASR | RTEMS_NO_TIMESLICE | RTEMS_PREEMPT,
    RTEMS_INTERRUPT_MASK | RTEMS_ASR_MASK |
    RTEMS_TIMESLICE_MASK | RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - clear all modes      - previous mode: ",
    previous_mode
  );

/* END OF COMBINATIONS */

/* BEGINNING OF CURRENT MODE */

  status = rtems_task_mode(
    RTEMS_CURRENT_MODE,
    RTEMS_CURRENT_MODE,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - get current mode     - previous mode: ",
    previous_mode
  );

/* END OF CURRENT MODE */

/* TEST BEING IN A SYSTEM STATE OTHER THAN UP */
  puts( "TA1 - FORCE system state to before multitasking" );
  _System_state_Set( SYSTEM_STATE_BEFORE_MULTITASKING );

  puts( "TA1 - enable preemption mode" );
  status = rtems_task_mode(
    RTEMS_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_PREEMPT              - previous mode: ",
    previous_mode
  );

  puts( "TA1 - disable preemption mode" );
  status = rtems_task_mode(
    RTEMS_NO_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );

  puts( "TA1 - Restore system state" );
  _System_state_Set( SYSTEM_STATE_UP );

/* END OF TEST BEING IN A SYSTEM STATE OTHER THAN UP */

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
