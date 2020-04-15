/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/counter.h>
#include <rtems/score/smpbarrier.h>
#include <rtems/score/smplock.h>

const char rtems_test_name[] = "SMPLOAD 1";

#define CPU_COUNT 32

#define MAX_INHERIT_OBTAIN_COUNT CPU_COUNT

#define SEM_WORKER_COUNT (3 * CPU_COUNT)

#define INIT_PRIO 1

#define INHERIT_RELEASE_PRIO_HIGH (INIT_PRIO + 1)

#define INHERIT_OBTAIN_PRIO_BASE (INHERIT_RELEASE_PRIO_HIGH + 1)

#define INHERIT_RELEASE_PRIO_LOW (INHERIT_OBTAIN_PRIO_BASE + MAX_INHERIT_OBTAIN_COUNT)

#define LOAD_PRIO (INHERIT_RELEASE_PRIO_LOW + 1)

#define SEM_WORKER_CEILING_PRIO (LOAD_PRIO + 1)

#define SEM_WORKER_PRIO_BASE (SEM_WORKER_CEILING_PRIO + 1)

typedef struct {
  rtems_id main_task_id;
  rtems_id inherit_release_task_id;
  rtems_id inherit_main_obtain_task_id;
  rtems_id sem_worker_sem_prio_inherit;
  rtems_id sem_worker_sem_prio_ceiling;
  rtems_id inherit_sem;
  rtems_counter_ticks inherit_obtain_delay;
  SMP_barrier_Control inherit_barrier;
  uint64_t inherit_obtain_counter[MAX_INHERIT_OBTAIN_COUNT];
  uint64_t inherit_release_counter;
  uint64_t sem_worker_counter[SEM_WORKER_COUNT];
} test_context;

static test_context test_instance = {
  .inherit_barrier = SMP_BARRIER_CONTROL_INITIALIZER
};

static uint32_t simple_random(uint32_t v)
{
  v *= 1664525;
  v += 1013904223;

  return v;
}

static void inherit_obtain_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  rtems_counter_ticks delay = (cpu_count - 1 - arg) * ctx->inherit_obtain_delay;

  while (true) {
    _SMP_barrier_Wait(&ctx->inherit_barrier, &barrier_state, cpu_count);

    rtems_counter_delay_ticks(delay);

    sc = rtems_semaphore_obtain(ctx->inherit_sem, RTEMS_WAIT, 1);
    rtems_test_assert(sc == RTEMS_TIMEOUT);

    _SMP_barrier_Wait(&ctx->inherit_barrier, &barrier_state, cpu_count);

    ++ctx->inherit_obtain_counter[arg];

    if (arg == 0) {
      rtems_task_priority prio = INHERIT_RELEASE_PRIO_HIGH;

      sc = rtems_task_set_priority(ctx->inherit_release_task_id, prio, &prio);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_event_transient_send(ctx->inherit_release_task_id);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }
  }
}

static void inherit_release_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(
    ctx->inherit_sem,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_send(ctx->main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (true) {
    rtems_task_priority prio = INHERIT_RELEASE_PRIO_LOW;

    sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_semaphore_release(ctx->inherit_sem);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    ++ctx->inherit_release_counter;

    sc = rtems_semaphore_obtain(
      ctx->inherit_sem,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_event_transient_send(ctx->inherit_main_obtain_task_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void load_task(rtems_task_argument arg)
{
  size_t data_size;
  volatile int *data;
  volatile int dummy;
  size_t n;

  data_size = rtems_cache_get_data_cache_size(0);
  if (data_size > 0) {
    data = malloc(data_size);
    rtems_test_assert(data != NULL);
  } else {
    data_size = sizeof(dummy);
    data = &dummy;
  }

  n = data_size / sizeof(*data);
  while (true) {
    size_t i;

    for (i = 0; i < n; ++i) {
      data[i] = i;
    }
  }
}

static void sem_worker_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  uint32_t v = arg;

  while (true) {
    rtems_status_code sc;
    rtems_id id;

    v = simple_random(v);

    if ((v & 0x80000000) != 0) {
      id = ctx->sem_worker_sem_prio_inherit;
    } else {
      id = ctx->sem_worker_sem_prio_ceiling;
    }

    sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_wake_after(1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_semaphore_release(id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    ++ctx->sem_worker_counter[arg];
  }
}

static int cmp(const void *ap, const void *bp)
{
  const rtems_counter_ticks *a = ap;
  const rtems_counter_ticks *b = bp;

  return *a - *b;
}

static void get_obtain_delay_estimate(test_context *ctx)
{
  rtems_counter_ticks t[32];
  SMP_lock_Control lock;
  ISR_Level level;
  size_t n = RTEMS_ARRAY_SIZE(t);
  size_t i;

  _SMP_lock_Initialize(&lock, "test");

  _ISR_Local_disable(level);

  for (i = 0; i < n; ++i) {
    SMP_lock_Context lock_context;
    rtems_counter_ticks a;
    rtems_counter_ticks b;

    a = rtems_counter_read();
    _SMP_lock_ISR_disable_and_acquire(&lock, &lock_context);
    b = rtems_counter_read();
    _SMP_lock_Release_and_ISR_enable(&lock, &lock_context);

    t[i] = rtems_counter_difference(b, a);
  }

  _ISR_Local_enable(level);

  _SMP_lock_Destroy(&lock);

  qsort(&t[0], n, sizeof(t[0]), cmp);

  ctx->inherit_obtain_delay = t[n / 2];
}

static void test(void)
{
  test_context *ctx = &test_instance;
  uint32_t i;
  rtems_status_code sc;
  rtems_id id;

  ctx->main_task_id = rtems_task_self();

  get_obtain_delay_estimate(ctx);

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'I'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &ctx->sem_worker_sem_prio_inherit
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'C'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    SEM_WORKER_CEILING_PRIO,
    &ctx->sem_worker_sem_prio_ceiling
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('I', 'N', 'H', 'E'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &ctx->inherit_sem
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < SEM_WORKER_COUNT; ++i) {
    sc = rtems_task_create(
      rtems_build_name('S', 'E', 'M', 'W'),
      SEM_WORKER_PRIO_BASE + i,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(id, sem_worker_task, i);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_task_create(
    rtems_build_name('L', 'O', 'A', 'D'),
    LOAD_PRIO,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, load_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('I', 'N', 'H', 'R'),
    INHERIT_RELEASE_PRIO_LOW,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  ctx->inherit_release_task_id = id;

  sc = rtems_task_start(id, inherit_release_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < rtems_scheduler_get_processor_maximum(); ++i) {
    sc = rtems_task_create(
      rtems_build_name('I', 'N', 'H', 'O'),
      INHERIT_OBTAIN_PRIO_BASE + i,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    if (i == 0) {
      ctx->inherit_main_obtain_task_id = id;
    }

    sc = rtems_task_start(id, inherit_obtain_task, i);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_task_wake_after(30 * rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < SEM_WORKER_COUNT; ++i) {
    printf(
      "semaphore worker count %2" PRIu32 ": %" PRIu64 "\n",
      i,
      ctx->sem_worker_counter[i]
    );
  }

  printf(
    "priority inheritance release count: %" PRIu64 "\n",
    ctx->inherit_release_counter
  );

  for (i = 0; i < rtems_scheduler_get_processor_maximum(); ++i) {
    printf(
      "priority inheritance obtain count %2" PRIu32 ": %" PRIu64 "\n",
      i,
      ctx->inherit_obtain_counter[i]
    );
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS \
  (1 + MAX_INHERIT_OBTAIN_COUNT + 1 + 1 + SEM_WORKER_COUNT)

#define CONFIGURE_MAXIMUM_SEMAPHORES 3

#define CONFIGURE_INIT_TASK_PRIORITY INIT_PRIO

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
