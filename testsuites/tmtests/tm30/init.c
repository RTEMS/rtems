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

#if !defined(OPERATION_COUNT)
#define OPERATION_COUNT 100
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"

const char rtems_test_name[] = "TIME TEST 30";

rtems_id barrier[ OPERATION_COUNT ];

rtems_task Init(
  rtems_task_argument argument
);

static void benchmark_barrier_create(
  int    iteration,
  void  *argument
)
{
  rtems_status_code status;

  status = rtems_barrier_create(
    iteration + 1,
    RTEMS_LOCAL | RTEMS_FIFO,
    2,
    &barrier[iteration]
  );
  directive_failed(status, "rtems_barrier_create");
}

static void benchmark_barrier_ident(
  int    iteration,
  void  *argument
)
{
  rtems_status_code status;
  rtems_id          id;

  status = rtems_barrier_ident( iteration+1, &id );
  directive_failed(status, "rtems_barrier_ident");
}

static void benchmark_barrier_delete(
  int    iteration,
  void  *argument
)
{
  rtems_status_code status;

  status = rtems_barrier_delete( barrier[iteration] );
  directive_failed(status, "rtems_barrier_delete");
}

rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();

  rtems_time_test_measure_operation(
    "rtems_barrier_create: only case",
    benchmark_barrier_create,
    NULL,
    OPERATION_COUNT,
    0
  );

  rtems_time_test_measure_operation(
    "rtems_barrier_ident: only case",
    benchmark_barrier_ident,
    NULL,
    OPERATION_COUNT,
    0
  );

  rtems_time_test_measure_operation(
    "rtems_barrier_delete: only case",
    benchmark_barrier_delete,
    NULL,
    OPERATION_COUNT,
    0
  );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_MAXIMUM_BARRIERS          OPERATION_COUNT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
