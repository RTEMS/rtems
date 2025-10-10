/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 Aeroflex Gaisler AB.  All rights reserved.
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <rtems/score/atomic.h>
#include <rtems/score/smpbarrier.h>
#include <rtems/score/smpimpl.h>
#include <rtems.h>
#include <limits.h>
#include <setjmp.h>
#include <string.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPCACHE 1";

#define CPU_COUNT 32

#define WORKER_PRIORITY 100

typedef struct {
  SMP_barrier_Control barrier;
  bool do_longjmp[CPU_COUNT];
  jmp_buf instruction_invalidate_return_context[CPU_COUNT];
} test_context;

static test_context ctx = {
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER,
};

static void function_to_flush( void )
{
  /* Does nothing. Used to give a pointer to instruction address space. */
}

typedef void ( *test_case )( void );

static void test_cache_invalidate_entire_instruction( void )
{
  rtems_cache_invalidate_entire_instruction();
}

static void test_cache_invalidate_multiple_instruction_lines( void )
{
  uint32_t self = rtems_scheduler_get_processor();

  ctx.do_longjmp[self] = true;

  if (setjmp(ctx.instruction_invalidate_return_context[self]) == 0) {
    rtems_cache_invalidate_multiple_instruction_lines( &function_to_flush,
        4 /* arbitrary size */ );
  }

  ctx.do_longjmp[self] = false;
}

static void barrier( SMP_barrier_State *bs )
{
  _SMP_barrier_Wait(
    &ctx.barrier,
    bs,
    rtems_scheduler_get_processor_maximum()
  );
}

static test_case test_cases[] = {
  test_cache_invalidate_entire_instruction,
  test_cache_invalidate_multiple_instruction_lines
};

static void call_tests( SMP_barrier_State *bs )
{
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE( test_cases ); ++i) {
    barrier( bs );
    ( *test_cases[ i ] )();
    barrier( bs );
  }
}

static void call_tests_isr_disabled( SMP_barrier_State *bs )
{
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE( test_cases ); ++i) {
    ISR_Level isr_level;

    _ISR_Local_disable( isr_level );
    barrier( bs );
    ( *test_cases[ i ] )();
    _ISR_Local_enable( isr_level );
    barrier( bs );
  }
}

static void call_tests_with_thread_dispatch_disabled( SMP_barrier_State *bs )
{
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE( test_cases ); ++i) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable();
    barrier( bs );
    ( *test_cases[ i ] )();
    barrier( bs );
    _Thread_Dispatch_enable( cpu_self );
  }
}

static void cmlog(  const char* str )
{
  if ( rtems_scheduler_get_processor() == 0 )
    printf( "%s", str );
}

static void all_tests( void )
{
  SMP_barrier_State bs = SMP_BARRIER_STATE_INITIALIZER;

  /* Call test cases */
  cmlog( "Calling test cases. " );
  call_tests( &bs );
  cmlog( "Done!\n");

  /* Call test cases with ISR disabled */
  cmlog( "Calling test cases with ISR disabled. " );
  call_tests_isr_disabled( &bs );
  cmlog( "Done!\n" );

  /* Call test cases with thread dispatch disabled */
  cmlog( "Calling test cases with thread_dispatch_disabled. ");
  call_tests_with_thread_dispatch_disabled( &bs );
  cmlog( "Done!\n");

  /* Done. Free up memory. */
  _SMP_barrier_Wait(
    &ctx.barrier,
    &bs,
    rtems_scheduler_get_processor_maximum()
  );
}

static void worker_task(rtems_task_argument arg)
{
  (void) arg;

  rtems_status_code sc;

  all_tests();

  sc = rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_smp_cache_manager( void )
{
  rtems_status_code sc;
  size_t worker_index;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();

  for (worker_index = 1; worker_index < cpu_count; ++worker_index) {
    rtems_id worker_id;

    sc = rtems_task_create(
      rtems_build_name('W', 'R', 'K', '0'+worker_index),
      WORKER_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &worker_id
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    sc = rtems_task_start( worker_id, worker_task, 0 );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  }

  all_tests();
}


static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  test_smp_cache_manager();

  TEST_END();
  rtems_test_exit(0);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{
  (void) always_set_to_false;
  (void) error;

  uint32_t self = rtems_scheduler_get_processor();

  if (source == RTEMS_FATAL_SOURCE_EXCEPTION && ctx.do_longjmp[self]) {
    _ISR_Set_level(0);
    longjmp(ctx.instruction_invalidate_return_context[self], 1);
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
