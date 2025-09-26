/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2013.
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

#include <rtems/btimer.h>

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "TIME TEST 19";

rtems_asr Process_asr_for_pass_1(
  rtems_signal_set signals
);

rtems_asr Process_asr_for_pass_2(
  rtems_signal_set signals
);

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

rtems_task Task_3(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;

  Print_Warning();

  TEST_BEGIN();

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TASK1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TASK1" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    (RTEMS_MAXIMUM_PRIORITY / 2),
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TASK2" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TASK2" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    (RTEMS_MAXIMUM_PRIORITY / 2u) - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TASK3" );

  status = rtems_task_start( Task_id[ 3 ], Task_3, 0 );
  directive_failed( status, "rtems_task_start of TASK3" );

  rtems_task_exit();
}

rtems_asr Process_asr_for_pass_1(
  rtems_signal_set signals
)
{
  (void) signals;

  end_time = benchmark_timer_read();

  put_time(
    "rtems_signal_send: signal to self",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
}

rtems_asr Process_asr_for_pass_2(
  rtems_signal_set signals
)
{
  (void) signals;

  rtems_status_code status;

  status = rtems_task_resume( Task_id[ 3 ] );
  directive_failed( status, "rtems_task_resume" );

  benchmark_timer_initialize();
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;

  benchmark_timer_initialize();
    (void) rtems_signal_catch( Process_asr_for_pass_1, RTEMS_DEFAULT_MODES );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_signal_catch: only case",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    rtems_signal_send( Task_id[ 2 ], 1 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_signal_send: returns to caller",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_signal_send( RTEMS_SELF, RTEMS_SIGNAL_1 );

  /* end time is done is RTEMS_ASR */

  end_time = benchmark_timer_read();

  put_time(
    "rtems_signal: exit ASR overhead returns to calling task",
    end_time,
    1,
    0,
    0
  );

  status = rtems_signal_catch( Process_asr_for_pass_2, RTEMS_NO_PREEMPT );
  directive_failed( status, "rtems_signal_catch" );

  benchmark_timer_initialize();
    (void) rtems_signal_send( RTEMS_SELF, RTEMS_SIGNAL_1 );
}

/* avoid warnings for no prototype */
rtems_asr Process_asr_for_task_2(
  rtems_signal_set signals
);

rtems_asr Process_asr_for_task_2(
  rtems_signal_set signals
)
{
  (void) signals;

  ;
}

rtems_task Task_2(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;

  status = rtems_signal_catch( Process_asr_for_task_2, RTEMS_DEFAULT_MODES );
  directive_failed( status, "rtems_signal_catch" );

  (void) rtems_task_suspend( RTEMS_SELF );
}

rtems_task Task_3(
  rtems_task_argument argument
)
{
  (void) argument;

  (void) rtems_task_suspend( RTEMS_SELF );

  end_time = benchmark_timer_read();

  put_time(
    "rtems_signal: exit ASR overhead returns to preempting task",
    end_time,
    1,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
