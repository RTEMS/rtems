/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief CPU_Spin_wait() Test 1
 *
 * @note This test verifies the basic functionality of CPU_Spin_wait() by performing 
 * a number of spin waits across multiple CPU cores, ensuring that the 
 * spin wait functionality works correctly under SMP conditions.
 *
 * @ingroup SMP
 */

/*
 * Copyright (C) 2026 Wayne Michael Thornton <wmthornton-dev@outlook.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
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

#include <rtems.h>
#include <rtems/score/cpu.h>
#include <rtems/test.h>
#include <rtems/test-info.h>
#include <tmacros.h>

const char rtems_test_name[] = "SMPSPINWAIT 1";

#define RING_PASS_ROUNDS 10U

typedef struct {
  rtems_id barrier_id;

  /* Phase 1: Shared read contention */
  volatile uint32_t broadcast_flag;
  volatile uint32_t phase1_acks;

  /* 
   * Phase 2: Intentionally non-volatile. Proves _CPU_Spin_wait() 
   * enforces a compiler memory barrier across SMP core boundaries.
   */
  uint32_t          non_volatile_flag;
  volatile uint32_t phase2_acks;

  /* Phase 3: Cache-line bouncing ring token */
  volatile uint32_t ring_token;
} TestContext;

static TestContext test_instance;

static void execute_ring_stress(
  TestContext *ctx,
  uint32_t     cpu_index,
  uint32_t     cpu_count
)
{
  uint32_t total_steps;
  uint32_t step;

  total_steps = RING_PASS_ROUNDS * cpu_count;

  /*
   * Each core waits for the token to equal its designated step, increments it,
   * and hands ownership to the next core in the ring. This forces intense
   * cache-line invalidation while executing _CPU_Spin_wait().
   */
  for ( step = cpu_index; step < total_steps; step += cpu_count ) {
    while ( __atomic_load_n( &ctx->ring_token, __ATOMIC_ACQUIRE ) != step ) {
      _CPU_Spin_wait();
    }
    __atomic_store_n( &ctx->ring_token, step + 1, __ATOMIC_RELEASE );
  }
}

static void test_worker_task( rtems_task_argument arg )
{
  TestContext *ctx;
  uint32_t     cpu_index;
  uint32_t     cpu_count;

  ctx = (TestContext *) arg;
  cpu_index = rtems_scheduler_get_processor();
  cpu_count = rtems_scheduler_get_processor_maximum();

  /* --- Phase 1: Read-Shared Bus Contention --- */
  rtems_barrier_wait( ctx->barrier_id, RTEMS_NO_TIMEOUT );

  while ( __atomic_load_n( &ctx->broadcast_flag, __ATOMIC_ACQUIRE ) == 0 ) {
    _CPU_Spin_wait();
  }
  __atomic_add_fetch( &ctx->phase1_acks, 1, __ATOMIC_RELEASE );

  /* --- Phase 2: Compiler Memory Barrier Verification --- */
  rtems_barrier_wait( ctx->barrier_id, RTEMS_NO_TIMEOUT );

  while ( ctx->non_volatile_flag == 0 ) {
    _CPU_Spin_wait();
  }
  __atomic_add_fetch( &ctx->phase2_acks, 1, __ATOMIC_RELEASE );

  /* --- Phase 3: MESI Cache-Line Bouncing Stress --- */
  rtems_barrier_wait( ctx->barrier_id, RTEMS_NO_TIMEOUT );

  execute_ring_stress( ctx, cpu_index, cpu_count );

  rtems_task_exit();
}

static void Init( rtems_task_argument arg )
{
  rtems_status_code sc;
  uint32_t          cpu_count;
  uint32_t          cpu_index;
  rtems_id          task_id;

  /* Silence -Werror=unused-parameter for standard task signature */
  (void) arg;

  TEST_BEGIN();

  cpu_count = rtems_scheduler_get_processor_maximum();

  /* Enforce SMP execution environment */
  if ( cpu_count < 2 ) {
    puts(
      "TEST SKIPPED: SMP spin-wait validation requires at least 2 processors."
    );
    TEST_END();
    rtems_test_exit( 0 );
  }

  printf(
    "Running SMP spin-wait validation across %" PRIu32 " cores...\n",
    cpu_count
  );

  sc = rtems_barrier_create(
    rtems_build_name( 'B', 'A', 'R', 'R' ),
    RTEMS_BARRIER_AUTOMATIC_RELEASE,
    cpu_count,
    &test_instance.barrier_id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  test_instance.broadcast_flag = 0;
  test_instance.phase1_acks = 0;
  test_instance.non_volatile_flag = 0;
  test_instance.phase2_acks = 0;
  test_instance.ring_token = 0;

  /* Spawn worker tasks pinned to secondary processors */
  for ( cpu_index = 1; cpu_index < cpu_count; ++cpu_index ) {
    cpu_set_t cpuset;
    CPU_ZERO( &cpuset );
    CPU_SET( cpu_index, &cpuset );

    sc = rtems_task_create(
      rtems_build_name( 'S', 'P', 'I', 'N' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    sc = rtems_task_set_affinity( task_id, sizeof( cpuset ), &cpuset );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    sc = rtems_task_start(
      task_id,
      test_worker_task,
      (rtems_task_argument) &test_instance
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  }

  /* --- Phase 1: Read-Shared Bus Contention --- */
  puts( "Phase 1: Testing Read-Shared bus contention and broadcast wake..." );
  rtems_barrier_wait( test_instance.barrier_id, RTEMS_NO_TIMEOUT );

  /* Let workers spin simultaneously on the shared cache line */
  rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 10 );
  __atomic_store_n( &test_instance.broadcast_flag, 1, __ATOMIC_RELEASE );

  while (
    __atomic_load_n( &test_instance.phase1_acks, __ATOMIC_ACQUIRE ) <
    ( cpu_count - 1 )
  ) {
    _CPU_Spin_wait();
  }

  /* --- Phase 2: Compiler Memory Barrier Verification --- */
  puts(
    "Phase 2: Testing compiler memory barrier on non-volatile shared flag..."
  );
  rtems_barrier_wait( test_instance.barrier_id, RTEMS_NO_TIMEOUT );

  rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 10 );
  test_instance.non_volatile_flag = 1;

  while (
    __atomic_load_n( &test_instance.phase2_acks, __ATOMIC_ACQUIRE ) <
    ( cpu_count - 1 )
  ) {
    _CPU_Spin_wait();
  }

  /* --- Phase 3: MESI Cache-Line Bouncing Stress --- */
  puts(
    "Phase 3: Testing cache-line invalidation stress via ring token pass..."
  );
  rtems_barrier_wait( test_instance.barrier_id, RTEMS_NO_TIMEOUT );

  /* Master core participates as CPU 0 in the ring pass */
  execute_ring_stress( &test_instance, 0, cpu_count );

  /* Wait for the secondary cores to complete the final steps of the ring */
  while (
    __atomic_load_n( &test_instance.ring_token, __ATOMIC_ACQUIRE ) <
    ( RING_PASS_ROUNDS * cpu_count )
  ) {
    _CPU_Spin_wait();
  }

  /* If ring token reached total steps, no core deadlocked during invalidation */
  rtems_test_assert(
    __atomic_load_n( &test_instance.ring_token, __ATOMIC_ACQUIRE ) ==
    ( RING_PASS_ROUNDS * cpu_count )
  );

  rtems_barrier_delete( test_instance.barrier_id );

  TEST_END();
  rtems_test_exit( 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_MAXIMUM_PROCESSORS
#define CONFIGURE_MAXIMUM_TASKS      CPU_MAXIMUM_PROCESSORS
#define CONFIGURE_MAXIMUM_BARRIERS   1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
