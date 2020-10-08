/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

/*
 * Do not manually edit this file.  It is part of the RTEMS quality process
 * and was automatically generated.
 *
 * If you find something that needs to be fixed or worded better please
 * post a report to an RTEMS mailing list or raise a bug report:
 *
 * https://docs.rtems.org/branches/master/user/support/bugs.html
 *
 * For information on updating and regenerating please refer to:
 *
 * https://docs.rtems.org/branches/master/eng/req/howto.html
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <rtems/test-info.h>
#include <rtems/testopts.h>

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestSuiteTestsuitesValidation0 spec:/testsuites/validation-0
 *
 * @ingroup RTEMSTestSuites
 *
 * @brief This general purpose validation test suite provides enough resources
 *   to run basic tests for all specified managers and functions.
 *
 * In SMP configurations, up to three scheduler instances using the SMP EDF
 * scheduler are provided using up to four processors.
 *
 * @{
 */

const char rtems_test_name[] = "Validation0";

static char buffer[ 512 ];

static const T_action actions[] = {
  T_report_hash_sha256,
  T_check_task_context,
  T_check_rtems_barriers,
  T_check_rtems_extensions,
  T_check_rtems_message_queues,
  T_check_rtems_partitions,
  T_check_rtems_periods,
  T_check_rtems_semaphores,
  T_check_rtems_tasks,
  T_check_rtems_timers
};

static const T_config test_config = {
  .name = rtems_test_name,
  .buf = buffer,
  .buf_size = sizeof( buffer ),
  .putchar = rtems_put_char,
  .verbosity = RTEMS_TEST_VERBOSITY,
  .now = T_now_clock,
  .action_count = T_ARRAY_SIZE( actions ),
  .actions = actions
};

static void runner_task( rtems_task_argument arg )
{
  int exit_code;

  (void) arg;

  rtems_test_begin( rtems_test_name, TEST_STATE );
  T_register();
  exit_code = T_main( &test_config );

  if ( exit_code == 0 ) {
    rtems_test_end( rtems_test_name );
  }

  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, (uint32_t) exit_code );
}

#define MAX_TLS_SIZE RTEMS_ALIGN_UP( 64, RTEMS_TASK_STORAGE_ALIGNMENT )

#define ATTRIBUTES RTEMS_FLOATING_POINT

RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT )
static char runner_task_storage[
  RTEMS_TASK_STORAGE_SIZE(
    MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE,
    ATTRIBUTES
  )
];

static const rtems_task_config runner_task_config = {
  .name = rtems_build_name( 'R', 'U', 'N', ' ' ),
  .initial_priority = 1,
  .storage_area = runner_task_storage,
  .storage_size = sizeof( runner_task_storage ),
  .maximum_thread_local_storage_size = MAX_TLS_SIZE,
  .initial_modes = RTEMS_DEFAULT_MODES,
  .attributes = ATTRIBUTES
};

static void init_runner_task(void)
{
  rtems_id id;
  rtems_status_code sc;

  sc = rtems_task_construct( &runner_task_config, &id );
  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, 1 );
  }

  sc = rtems_task_start( id, runner_task, 0 );
  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, 1 );
  }
}

RTEMS_SYSINIT_ITEM(
  init_runner_task,
  RTEMS_SYSINIT_CLASSIC_USER_TASKS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS 4

#define CONFIGURE_MAXIMUM_BARRIERS 3

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 3

#define CONFIGURE_MAXIMUM_PARTITIONS 3

#define CONFIGURE_MAXIMUM_PERIODS 3

#define CONFIGURE_MAXIMUM_SEMAPHORES 3

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE \
  CONFIGURE_MAXIMUM_TASKS

#define CONFIGURE_MAXIMUM_TIMERS 3

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 3

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#define CONFIGURE_IDLE_TASK_BODY _CPU_Thread_Idle_body

#if defined(RTEMS_SMP)

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP(a);

RTEMS_SCHEDULER_EDF_SMP(b);

RTEMS_SCHEDULER_EDF_SMP(c);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(a, rtems_build_name('A', ' ', ' ', ' ')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(b, rtems_build_name('B', ' ', ' ', ' ')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(c, rtems_build_name('C', ' ', ' ', ' '))

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#endif /* RTEMS_SMP */

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/** @} */
