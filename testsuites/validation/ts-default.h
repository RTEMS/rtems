/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the default validation test suite runner
 *   and application configuration.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/chain.h>
#include <rtems/test-info.h>
#include <rtems/testopts.h>

#include <rtems/test.h>
#include <rtems/test-scheduler.h>

#include "ts-config.h"
#include "tx-support.h"

#if !defined( MAX_TLS_SIZE )
#define MAX_TLS_SIZE TEST_MAXIMUM_TLS_SIZE
#endif

#define MAX_TASKS ( TEST_MAXIMUM_TASKS - 1 )

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE \
  RTEMS_TASK_STORAGE_SIZE( \
    MAX_TLS_SIZE + TEST_MINIMUM_STACK_SIZE + \
      CPU_STACK_ALIGNMENT - CPU_HEAP_ALIGNMENT, \
    TASK_ATTRIBUTES \
  )

static char buffer[ 512 ];

static const T_action actions[] = {
  T_report_hash_sha256,
  T_memory_action,
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
#if defined(CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER)
  .now = T_now_clock,
#else
  .now = T_now_tick,
#endif
  .allocate = T_memory_allocate,
  .deallocate = T_memory_deallocate,
  .action_count = T_ARRAY_SIZE( actions ),
  .actions = actions
};

static rtems_chain_control free_task_storage =
  RTEMS_CHAIN_INITIALIZER_EMPTY( free_task_storage );

static union {
  RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT ) char
    storage[ TASK_STORAGE_SIZE ];
  rtems_chain_node node;
} task_storage[ MAX_TASKS ];

rtems_task_argument test_runner_argument;

rtems_task_priority test_runner_initial_priority;

rtems_mode test_runner_initial_modes;

static void Runner( rtems_task_argument arg )
{
  int exit_code;

  test_runner_argument = arg;

  (void) rtems_task_mode(
    RTEMS_ASR,
    RTEMS_ASR_MASK,
    &test_runner_initial_modes
  );

  (void) rtems_task_set_priority(
    RTEMS_SELF,
    PRIO_DEFAULT,
    &test_runner_initial_priority
  );

  rtems_chain_initialize(
    &free_task_storage,
    task_storage,
    RTEMS_ARRAY_SIZE( task_storage ),
    sizeof( task_storage[ 0 ] )
  );

  rtems_test_begin( rtems_test_name, TEST_STATE );
  T_register();
  exit_code = T_main( &test_config );

  if ( exit_code == 0 ) {
    rtems_test_end( rtems_test_name );
  }

  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, (uint32_t) exit_code );
}

void *test_task_stack_allocate( size_t size )
{
  if ( size > sizeof( task_storage[ 0 ] ) ) {
    return NULL;
  }

  T_quiet_ge_sz( size, TEST_MINIMUM_STACK_SIZE );

  return rtems_chain_get_unprotected( &free_task_storage );
}

void test_task_stack_deallocate( void *stack )
{
  rtems_chain_append_unprotected(
    &free_task_storage,
    (rtems_chain_node *) stack
  );
}

#if !defined( CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER )
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#endif

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE TEST_MINIMUM_STACK_SIZE

#define CONFIGURE_INTERRUPT_STACK_SIZE TEST_INTERRUPT_STACK_SIZE

#define CONFIGURE_MAXIMUM_BARRIERS TEST_MAXIMUM_BARRIERS

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES TEST_MAXIMUM_MESSAGE_QUEUES

#define CONFIGURE_MAXIMUM_PARTITIONS TEST_MAXIMUM_PARTITIONS

#define CONFIGURE_MAXIMUM_PERIODS TEST_MAXIMUM_PERIODS

#define CONFIGURE_MAXIMUM_SEMAPHORES TEST_MAXIMUM_SEMAPHORES

#define CONFIGURE_MAXIMUM_TASKS TEST_MAXIMUM_TASKS

#define CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE \
  CONFIGURE_MAXIMUM_TASKS

#define CONFIGURE_MAXIMUM_TIMERS TEST_MAXIMUM_TIMERS

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS TEST_MAXIMUM_USER_EXTENSIONS

#define CONFIGURE_MICROSECONDS_PER_TICK TEST_MICROSECONDS_PER_TICK

#define CONFIGURE_TICKS_PER_TIMESLICE TEST_TICKS_PER_TIMESLICE

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE MAX_TLS_SIZE

#define CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE

#define CONFIGURE_TASK_STACK_ALLOCATOR test_task_stack_allocate

#define CONFIGURE_TASK_STACK_DEALLOCATOR test_task_stack_deallocate

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ARGUMENTS TEST_RUNNER_ARGUMENT

#define CONFIGURE_INIT_TASK_ATTRIBUTES ( RTEMS_SYSTEM_TASK | TASK_ATTRIBUTES )

#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE TASK_STORAGE_SIZE

#define CONFIGURE_INIT_TASK_ENTRY_POINT Runner

#define CONFIGURE_INIT_TASK_INITIAL_MODES TEST_RUNNER_INITIAL_MODES

#define CONFIGURE_INIT_TASK_NAME TEST_RUNNER_NAME

#define CONFIGURE_INIT_TASK_PRIORITY 0

#if !defined( CONFIGURE_INITIAL_EXTENSIONS )
#define CONFIGURE_INITIAL_EXTENSIONS { .fatal = FatalInitialExtension }
#endif

#if defined( RTEMS_SMP ) && \
  ( CONFIGURE_MAXIMUM_PROCESSORS == 4 || CONFIGURE_MAXIMUM_PROCESSORS == 5 )

#include <rtems/score/scheduleredfsmp.h>

const Scheduler_Operations
T_scheduler_operations[ CONFIGURE_MAXIMUM_PROCESSORS ] = {
   SCHEDULER_EDF_SMP_ENTRY_POINTS,
   SCHEDULER_EDF_SMP_ENTRY_POINTS,
   SCHEDULER_EDF_SMP_ENTRY_POINTS,
#if CONFIGURE_MAXIMUM_PROCESSORS >= 5
   SCHEDULER_EDF_SMP_ENTRY_POINTS,
#endif
   SCHEDULER_EDF_SMP_ENTRY_POINTS
};

#undef SCHEDULER_EDF_SMP_ENTRY_POINTS

#define SCHEDULER_EDF_SMP_ENTRY_POINTS T_SCHEDULER_ENTRY_POINTS

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP( a );

RTEMS_SCHEDULER_EDF_SMP( b );

RTEMS_SCHEDULER_EDF_SMP( c );

RTEMS_SCHEDULER_EDF_SMP( d );

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP( a, TEST_SCHEDULER_A_NAME ), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP( b, TEST_SCHEDULER_B_NAME ), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP( c, TEST_SCHEDULER_C_NAME ), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP( d, TEST_SCHEDULER_D_NAME )

#if CONFIGURE_MAXIMUM_PROCESSORS == 5
#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER
#else
#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)
#endif

#elif !defined( CONFIGURE_SCHEDULER_ASSIGNMENTS )

#include <rtems/score/schedulerpriority.h>

#undef CONFIGURE_MAXIMUM_PROCESSORS
#define CONFIGURE_MAXIMUM_PROCESSORS 1

const Scheduler_Operations
T_scheduler_operations[ CONFIGURE_MAXIMUM_PROCESSORS ] = {
   SCHEDULER_PRIORITY_ENTRY_POINTS
};

#undef SCHEDULER_PRIORITY_ENTRY_POINTS

#define SCHEDULER_PRIORITY_ENTRY_POINTS T_SCHEDULER_ENTRY_POINTS

#define CONFIGURE_SCHEDULER_PRIORITY

#if defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_PRIORITY( a, 64 );

#else /* CONFIGURE_SCHEDULER_TABLE_ENTRIES */

#define CONFIGURE_SCHEDULER_NAME TEST_SCHEDULER_A_NAME

#define CONFIGURE_MAXIMUM_PRIORITY 127

#endif /* CONFIGURE_SCHEDULER_TABLE_ENTRIES */

#endif

#define CONFIGURE_IDLE_TASK_STACK_SIZE TEST_IDLE_STACK_SIZE

static char test_idle_stacks[ CONFIGURE_MAXIMUM_PROCESSORS ][
  RTEMS_ALIGN_UP(
    MAX_TLS_SIZE + TEST_IDLE_STACK_SIZE + CPU_IDLE_TASK_IS_FP * CONTEXT_FP_SIZE,
    CPU_INTERRUPT_STACK_ALIGNMENT
  )
]
RTEMS_ALIGNED( CPU_INTERRUPT_STACK_ALIGNMENT )
RTEMS_SECTION( ".rtemsstack.idle" );

void *test_idle_task_stack_allocate( uint32_t cpu_index, size_t *size )
{
  if ( *size > sizeof( test_idle_stacks[ 0 ] ) ) {
    rtems_fatal( RTEMS_FATAL_SOURCE_APPLICATION, 0xABAD1DEA );
  }

  return &test_idle_stacks[ cpu_index ][0];
}

#define CONFIGURE_TASK_STACK_ALLOCATOR_FOR_IDLE test_idle_task_stack_allocate

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
