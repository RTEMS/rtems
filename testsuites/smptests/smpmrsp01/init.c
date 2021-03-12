/*
 * Copyright (c) 2014-2015 embedded brains GmbH.  All rights reserved.
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

#include <sys/param.h>

#include <stdio.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/score/schedulersmpimpl.h>
#include <rtems/score/smpbarrier.h>
#include <rtems/score/smplock.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPMRSP 1";

#define CPU_COUNT 32

#define MRSP_COUNT 32

#define SWITCH_EVENT_COUNT 32

typedef struct {
  uint32_t sleep;
  uint32_t timeout;
  uint32_t obtain[MRSP_COUNT];
  uint32_t cpu[CPU_COUNT];
} counter;

typedef struct {
  uint32_t cpu_index;
  const Thread_Control *executing;
  const Thread_Control *heir;
  const Thread_Control *heir_node;
  Priority_Control heir_priority;
} switch_event;

typedef struct {
  rtems_id main_task_id;
  rtems_id migration_task_id;
  rtems_id low_task_id[2];
  rtems_id high_task_id[2];
  rtems_id timer_id;
  rtems_id counting_sem_id;
  rtems_id mrsp_ids[MRSP_COUNT];
  rtems_id scheduler_ids[CPU_COUNT];
  rtems_id worker_ids[2 * CPU_COUNT];
  volatile bool stop_worker[2 * CPU_COUNT];
  counter counters[2 * CPU_COUNT];
  uint32_t migration_counters[CPU_COUNT];
  Thread_Control *worker_task;
  SMP_barrier_Control barrier;
  SMP_lock_Control switch_lock;
  size_t switch_index;
  switch_event switch_events[32];
  volatile bool high_run[2];
  volatile bool low_run[2];
} test_context;

static test_context test_instance = {
  .switch_lock = SMP_LOCK_INITIALIZER("test instance switch lock")
};

static void busy_wait(void)
{
  rtems_interval later = rtems_clock_tick_later(2);

  while (rtems_clock_tick_before(later)) {
    /* Wait */
  }
}

static void barrier_init(test_context *ctx)
{
  _SMP_barrier_Control_initialize(&ctx->barrier);
}

static void barrier(test_context *ctx, SMP_barrier_State *bs)
{
  _SMP_barrier_Wait(&ctx->barrier, bs, 2);
}

static void barrier_and_delay(test_context *ctx, SMP_barrier_State *bs)
{
  barrier(ctx, bs);
  busy_wait();
}

static rtems_task_priority get_prio(rtems_id task_id)
{
  rtems_status_code sc;
  rtems_task_priority prio;

  sc = rtems_task_set_priority(task_id, RTEMS_CURRENT_PRIORITY, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return prio;
}

static void wait_for_prio(rtems_id task_id, rtems_task_priority prio)
{
  while (get_prio(task_id) != prio) {
    /* Wait */
  }
}

static void assert_prio(rtems_id task_id, rtems_task_priority expected_prio)
{
  rtems_test_assert(get_prio(task_id) == expected_prio);
}

static void change_prio(rtems_id task_id, rtems_task_priority prio)
{
  rtems_status_code sc;

  sc = rtems_task_set_priority(task_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void assert_executing_worker(test_context *ctx)
{
  rtems_test_assert(
    _CPU_Context_Get_is_executing(&ctx->worker_task->Registers)
  );
}

static void switch_extension(Thread_Control *executing, Thread_Control *heir)
{
  test_context *ctx = &test_instance;
  SMP_lock_Context lock_context;
  size_t i;

  _SMP_lock_ISR_disable_and_acquire(&ctx->switch_lock, &lock_context);

  i = ctx->switch_index;
  if (i < SWITCH_EVENT_COUNT) {
    switch_event *e = &ctx->switch_events[i];
    Scheduler_SMP_Node *node = _Scheduler_SMP_Thread_get_node(heir);

    e->cpu_index = rtems_scheduler_get_processor();
    e->executing = executing;
    e->heir = heir;
    e->heir_node = _Scheduler_Node_get_owner(&node->Base);
    e->heir_priority = node->priority;

    ctx->switch_index = i + 1;
  }

  _SMP_lock_Release_and_ISR_enable(&ctx->switch_lock, &lock_context);
}

static void reset_switch_events(test_context *ctx)
{
  SMP_lock_Context lock_context;

  _SMP_lock_ISR_disable_and_acquire(&ctx->switch_lock, &lock_context);
  ctx->switch_index = 0;
  _SMP_lock_Release_and_ISR_enable(&ctx->switch_lock, &lock_context);
}

static size_t get_switch_events(test_context *ctx)
{
  SMP_lock_Context lock_context;
  size_t events;

  _SMP_lock_ISR_disable_and_acquire(&ctx->switch_lock, &lock_context);
  events = ctx->switch_index;
  _SMP_lock_Release_and_ISR_enable(&ctx->switch_lock, &lock_context);

  return events;
}

static void print_switch_events(test_context *ctx)
{
  size_t n = get_switch_events(ctx);
  size_t i;

  for (i = 0; i < n; ++i) {
    switch_event *e = &ctx->switch_events[i];
    char ex[5];
    char hr[5];
    char hn[5];

    rtems_object_get_name(e->executing->Object.id, sizeof(ex), &ex[0]);
    rtems_object_get_name(e->heir->Object.id, sizeof(hr), &hr[0]);
    rtems_object_get_name(e->heir_node->Object.id, sizeof(hn), &hn[0]);

    printf(
      "[%" PRIu32 "] %4s -> %4s (prio %3" PRIu64 ", node %4s)\n",
      e->cpu_index,
      &ex[0],
      &hr[0],
      e->heir_priority,
      &hn[0]
    );
  }
}

static void create_timer(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_timer_create(
    rtems_build_name('T', 'I', 'M', 'R'),
    &ctx->timer_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void delete_timer(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_timer_delete(ctx->timer_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void fire_timer(
  test_context *ctx,
  rtems_interval interval,
  rtems_timer_service_routine_entry routine
)
{
  rtems_status_code sc;

  sc = rtems_timer_fire_after(ctx->timer_id, interval, routine, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void create_mrsp_sema(
  test_context *ctx,
  rtems_id *id,
  rtems_task_priority prio
)
{
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t index;
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    prio,
    id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (index = 1; index < cpu_count; index = ((index + 2) & ~UINT32_C(1))) {
    rtems_task_priority old_prio;

    old_prio = 1;
    sc = rtems_semaphore_set_priority(
      *id,
      ctx->scheduler_ids[index],
      prio,
      &old_prio
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(old_prio == 0);
  }
}

static void run_task(rtems_task_argument arg)
{
  volatile bool *run = (volatile bool *) arg;

  *run = true;

  while (true) {
    /* Do nothing */
  }
}

static void obtain_and_release_worker(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;

  ctx->worker_task = _Thread_Get_executing();

  assert_prio(RTEMS_SELF, 4);

  /* Obtain with timeout (A) */
  barrier(ctx, &barrier_state);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, 4);
  rtems_test_assert(sc == RTEMS_TIMEOUT);

  assert_prio(RTEMS_SELF, 4);

  /* Obtain with priority change and timeout (B) */
  barrier(ctx, &barrier_state);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, 4);
  rtems_test_assert(sc == RTEMS_TIMEOUT);

  assert_prio(RTEMS_SELF, 2);

  /* Restore priority (C) */
  barrier(ctx, &barrier_state);

  /* Obtain without timeout (D) */
  barrier(ctx, &barrier_state);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 4);

  /* Obtain and help with timeout (E) */
  barrier(ctx, &barrier_state);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, 4);
  rtems_test_assert(sc == RTEMS_TIMEOUT);

  assert_prio(RTEMS_SELF, 4);

  sc = rtems_task_suspend(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Worker done (H) */
  barrier(ctx, &barrier_state);

  while (true) {
    /* Wait for termination */
  }
}

static void test_mrsp_obtain_and_release(test_context *ctx)
{
  rtems_status_code sc;
  rtems_task_priority prio;
  rtems_id scheduler_id;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;

  puts("test MrsP obtain and release");

  change_prio(RTEMS_SELF, 3);

  barrier_init(ctx);
  reset_switch_events(ctx);

  ctx->high_run[0] = false;

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', '0'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high_task_id[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Check executing task parameters */

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(ctx->scheduler_ids[0] == scheduler_id);

  /* Create a MrsP semaphore object and lock it */

  create_mrsp_sema(ctx, &ctx->mrsp_ids[0], 2);

  assert_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 2);

  /*
   * The ceiling priority values per scheduler are equal to the value specified
   * for object creation.
   */

  prio = RTEMS_CURRENT_PRIORITY;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[0],
    ctx->scheduler_ids[0],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 2);

  /* Check the old value and set a new ceiling priority for scheduler B */

  prio = 3;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[0],
    ctx->scheduler_ids[1],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 2);

  /* Check the ceiling priority values */

  prio = RTEMS_CURRENT_PRIORITY;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[0],
    ctx->scheduler_ids[0],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 2);

  prio = RTEMS_CURRENT_PRIORITY;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[0],
    ctx->scheduler_ids[1],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 3);

  /* Check that a thread waiting to get ownership remains executing */

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    255,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_ids[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(ctx->worker_ids[0], ctx->scheduler_ids[1], 4);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->worker_ids[0], obtain_and_release_worker, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Obtain with timeout (A) */
  barrier_and_delay(ctx, &barrier_state);

  assert_prio(ctx->worker_ids[0], 3);
  assert_executing_worker(ctx);

  /* Obtain with priority change and timeout (B) */
  barrier_and_delay(ctx, &barrier_state);

  assert_prio(ctx->worker_ids[0], 3);
  change_prio(ctx->worker_ids[0], 2);
  assert_executing_worker(ctx);

  /* Restore priority (C) */
  barrier(ctx, &barrier_state);

  assert_prio(ctx->worker_ids[0], 2);
  change_prio(ctx->worker_ids[0], 4);

  /* Obtain without timeout (D) */
  barrier_and_delay(ctx, &barrier_state);

  assert_prio(ctx->worker_ids[0], 3);
  assert_executing_worker(ctx);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Check that a timeout works in case the waiting thread actually helps */

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Obtain and help with timeout (E) */
  barrier_and_delay(ctx, &barrier_state);

  sc = rtems_task_start(
    ctx->high_task_id[0],
    run_task,
    (rtems_task_argument) &ctx->high_run[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  while (rtems_scheduler_get_processor() != 0) {
    /* Wait */
  }

  rtems_test_assert(ctx->high_run[0]);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  print_switch_events(ctx);

  /* Worker done (H) */
  barrier(ctx, &barrier_state);

  sc = rtems_task_delete(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void obtain_after_migration_worker(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;

  assert_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Worker done (K) */
  barrier(ctx, &barrier_state);

  while (true) {
    /* Wait for termination */
  }
}

static void obtain_after_migration_high(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;

  assert_prio(RTEMS_SELF, 2);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[1], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Obtain done (I) */
  barrier(ctx, &barrier_state);

  /* Ready to release (J) */
  barrier(ctx, &barrier_state);

  sc = rtems_semaphore_release(ctx->mrsp_ids[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void test_mrsp_obtain_after_migration(test_context *ctx)
{
  rtems_status_code sc;
  rtems_task_priority prio;
  rtems_id scheduler_id;
  SMP_barrier_State barrier_state;

  puts("test MrsP obtain after migration");

  change_prio(RTEMS_SELF, 3);

  barrier_init(ctx);
  reset_switch_events(ctx);

  /* Create tasks */

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', '0'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high_task_id[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    255,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_ids[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(ctx->worker_ids[0], ctx->scheduler_ids[1], 3);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Create a MrsP semaphore objects */

  create_mrsp_sema(ctx, &ctx->mrsp_ids[0], 3);
  create_mrsp_sema(ctx, &ctx->mrsp_ids[1], 2);
  create_mrsp_sema(ctx, &ctx->mrsp_ids[2], 1);

  prio = 4;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[2],
    ctx->scheduler_ids[1],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 1);

  /* Check executing task parameters */

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(ctx->scheduler_ids[0] == scheduler_id);

  assert_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 3);

  /* Start other tasks */

  sc = rtems_task_start(ctx->worker_ids[0], obtain_after_migration_worker, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->high_task_id[0], obtain_after_migration_high, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  /* Obtain done (I) */
  _SMP_barrier_State_initialize(&barrier_state);
  barrier(ctx, &barrier_state);

  sc = rtems_task_suspend(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  /*
   * Obtain second MrsP semaphore and ensure that we change the priority of our
   * own scheduler node and not the one we are currently using.
   */

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[2], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 1);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  sc = rtems_semaphore_release(ctx->mrsp_ids[2]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_resume(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Ready to release (J) */
  barrier(ctx, &barrier_state);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  /* Prepare barrier for worker */
  barrier_init(ctx);
  _SMP_barrier_State_initialize(&barrier_state);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  print_switch_events(ctx);

  /* Worker done (K) */
  barrier(ctx, &barrier_state);

  sc = rtems_task_delete(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(ctx->mrsp_ids[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(ctx->mrsp_ids[2]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_flush_error(test_context *ctx)
{
  rtems_status_code sc;
  rtems_id id;

  puts("test MrsP flush error");

  create_mrsp_sema(ctx, &id, 1);

  sc = rtems_semaphore_flush(id);
  rtems_test_assert(sc == RTEMS_NOT_DEFINED);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_initially_locked(void)
{
  rtems_status_code sc;
  rtems_id id;

  puts("test MrsP initially locked");

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    0,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    1,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_nested_obtain_error(test_context *ctx)
{
  rtems_status_code sc;
  rtems_id id;

  puts("test MrsP nested obtain error");

  create_mrsp_sema(ctx, &id, 1);

  sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);

  sc = rtems_semaphore_release(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void deadlock_timer(rtems_id timer_id, void *arg)
{
  test_context *ctx = arg;

  change_prio(ctx->main_task_id, 1);
}

static void deadlock_worker(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[1], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  fire_timer(ctx, 2, deadlock_timer);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(ctx->mrsp_ids[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_send(ctx->main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void test_mrsp_deadlock_error(test_context *ctx)
{
  rtems_status_code sc;
  rtems_task_priority prio = 2;

  puts("test MrsP deadlock error");

  change_prio(RTEMS_SELF, prio);

  create_timer(ctx);
  create_mrsp_sema(ctx, &ctx->mrsp_ids[0], prio);
  create_mrsp_sema(ctx, &ctx->mrsp_ids[1], prio);

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    prio,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_ids[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->worker_ids[0], deadlock_worker, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  prio = 1;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[1],
    ctx->scheduler_ids[0],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 2);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[1], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);

  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[1],
    ctx->scheduler_ids[0],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(ctx->mrsp_ids[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  delete_timer(ctx);
}

static void test_mrsp_multiple_obtain(test_context *ctx)
{
  rtems_status_code sc;
  rtems_id sem_a_id;
  rtems_id sem_b_id;
  rtems_id sem_c_id;

  puts("test MrsP multiple obtain");

  change_prio(RTEMS_SELF, 4);

  create_mrsp_sema(ctx, &sem_a_id, 3);
  create_mrsp_sema(ctx, &sem_b_id, 2);
  create_mrsp_sema(ctx, &sem_c_id, 1);

  assert_prio(RTEMS_SELF, 4);

  sc = rtems_semaphore_obtain(sem_a_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_obtain(sem_b_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 2);

  sc = rtems_semaphore_obtain(sem_c_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 1);

  sc = rtems_semaphore_release(sem_c_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 2);

  sc = rtems_semaphore_release(sem_b_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_release(sem_a_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 4);

  sc = rtems_semaphore_obtain(sem_a_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_obtain(sem_b_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 2);

  sc = rtems_semaphore_obtain(sem_c_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 1);
  change_prio(RTEMS_SELF, 3);
  assert_prio(RTEMS_SELF, 1);

  sc = rtems_semaphore_release(sem_c_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 2);

  sc = rtems_semaphore_release(sem_b_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_release(sem_a_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_delete(sem_a_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(sem_b_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(sem_c_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void ready_unlock_worker(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;

  assert_prio(RTEMS_SELF, 4);

  /* Obtain (F) */
  barrier(ctx, &barrier_state);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 4);

  /* Done (G) */
  barrier(ctx, &barrier_state);

  while (true) {
    /* Do nothing */
  }
}

static void unblock_ready_timer(rtems_id timer_id, void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  sc = rtems_task_start(
    ctx->high_task_id[0],
    run_task,
    (rtems_task_argument) &ctx->high_run[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_resume(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /*
   * At this point the scheduler node of the main thread is in the
   * SCHEDULER_SMP_NODE_READY state and a _Scheduler_SMP_Unblock() operation is
   * performed.
   */
  sc = rtems_event_transient_send(ctx->main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void unblock_ready_owner(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 3);

  fire_timer(ctx, 2, unblock_ready_timer);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(!ctx->high_run[0]);
}

static void unblock_owner_before_rival_timer(rtems_id timer_id, void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  sc = rtems_task_suspend(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(ctx->high_task_id[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void unblock_owner_after_rival_timer(rtems_id timer_id, void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  sc = rtems_task_suspend(ctx->high_task_id[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void various_block_unblock(test_context *ctx)
{
  rtems_status_code sc;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;

  /* Worker obtain (F) */
  barrier_and_delay(ctx, &barrier_state);

  sc = rtems_task_suspend(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  busy_wait();

  sc = rtems_task_start(
    ctx->high_task_id[1],
    run_task,
    (rtems_task_argument) &ctx->high_run[1]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (!ctx->high_run[1]) {
    /* Do noting */
  }

  sc = rtems_task_resume(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Try to schedule a blocked active rival */

  sc = rtems_task_suspend(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(ctx->high_task_id[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_resume(ctx->high_task_id[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_resume(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  /* Use node of the active rival */

  sc = rtems_task_suspend(ctx->high_task_id[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_resume(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  sc = rtems_task_suspend(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_resume(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /*
   * Try to schedule an active rival with an already scheduled active owner
   * user.
   */

  fire_timer(ctx, 2, unblock_owner_before_rival_timer);

  /* This will take the processor away from us, the timer will help later */
  sc = rtems_task_resume(ctx->high_task_id[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /*
   * Try to schedule an active owner with an already scheduled active rival
   * user.
   */

  sc = rtems_task_resume(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  fire_timer(ctx, 2, unblock_owner_after_rival_timer);

  /* This will take the processor away from us, the timer will help later */
  sc = rtems_task_resume(ctx->high_task_id[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  assert_prio(RTEMS_SELF, 4);

  /* Worker done (G) */
  barrier(ctx, &barrier_state);
}

static void start_low_task(test_context *ctx, size_t i)
{
  rtems_status_code sc;

  sc = rtems_task_create(
    rtems_build_name('L', 'O', 'W', '0' + i),
    255,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->low_task_id[i]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(ctx->low_task_id[i], ctx->scheduler_ids[i], 5);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    ctx->low_task_id[i],
    run_task,
    (rtems_task_argument) &ctx->low_run[i]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_various_block_and_unblock(test_context *ctx)
{
  rtems_status_code sc;

  puts("test MrsP various block and unblock");

  change_prio(RTEMS_SELF, 4);

  barrier_init(ctx);
  reset_switch_events(ctx);

  ctx->low_run[0] = false;
  ctx->low_run[1] = false;
  ctx->high_run[0] = false;
  ctx->high_run[1] = false;

  create_mrsp_sema(ctx, &ctx->mrsp_ids[0], 3);
  assert_prio(RTEMS_SELF, 4);

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', '0'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high_task_id[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', '1'),
    255,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high_task_id[1]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(ctx->high_task_id[1], ctx->scheduler_ids[1], 2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    255,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_ids[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(ctx->worker_ids[0], ctx->scheduler_ids[1], 4);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->worker_ids[0], ready_unlock_worker, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  create_timer(ctx);

  /* In case these tasks run, then we have a MrsP protocol violation */
  start_low_task(ctx, 0);
  start_low_task(ctx, 1);

  unblock_ready_owner(ctx);
  various_block_unblock(ctx);

  rtems_test_assert(!ctx->low_run[0]);
  rtems_test_assert(!ctx->low_run[1]);

  print_switch_events(ctx);
  delete_timer(ctx);

  sc = rtems_task_delete(ctx->high_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->high_task_id[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->low_task_id[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->low_task_id[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_obtain_and_sleep_and_release(test_context *ctx)
{
  rtems_status_code sc;
  rtems_id sem_id;
  rtems_id run_task_id;
  volatile bool run = false;

  puts("test MrsP obtain and sleep and release");

  change_prio(RTEMS_SELF, 1);

  reset_switch_events(ctx);

  sc = rtems_task_create(
    rtems_build_name(' ', 'R', 'U', 'N'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &run_task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(run_task_id, run_task, (rtems_task_argument) &run);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  create_mrsp_sema(ctx, &sem_id, 1);

  rtems_test_assert(!run);

  sc = rtems_task_wake_after(2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(run);
  run = false;

  sc = rtems_semaphore_obtain(sem_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(!run);

  sc = rtems_task_wake_after(2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(!run);

  sc = rtems_semaphore_release(sem_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  print_switch_events(ctx);

  sc = rtems_semaphore_delete(sem_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(run_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void help_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (true) {
    /* Do nothing */
  }
}

static void test_mrsp_obtain_and_release_with_help(test_context *ctx)
{
  rtems_status_code sc;
  rtems_id help_task_id;
  rtems_id run_task_id;
  volatile bool run = false;

  puts("test MrsP obtain and release with help");

  change_prio(RTEMS_SELF, 3);

  reset_switch_events(ctx);

  create_mrsp_sema(ctx, &ctx->mrsp_ids[0], 2);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 2);

  sc = rtems_task_create(
    rtems_build_name('H', 'E', 'L', 'P'),
    255,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &help_task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(
    help_task_id,
    ctx->scheduler_ids[1],
    3
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(help_task_id, help_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name(' ', 'R', 'U', 'N'),
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &run_task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(run_task_id, run_task, (rtems_task_argument) &run);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wait_for_prio(help_task_id, 2);

  sc = rtems_task_wake_after(2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 0);
  rtems_test_assert(!run);

  change_prio(run_task_id, 1);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  while (!run) {
    /* Wait */
  }

  sc = rtems_task_wake_after(2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  change_prio(run_task_id, 4);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  /*
   * With this operation the scheduler instance 0 has now only the main and the
   * idle threads in the ready set.
   */
  sc = rtems_task_suspend(run_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  change_prio(RTEMS_SELF, 1);
  change_prio(RTEMS_SELF, 3);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  assert_prio(RTEMS_SELF, 3);

  wait_for_prio(help_task_id, 3);

  print_switch_events(ctx);

  sc = rtems_semaphore_delete(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(help_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(run_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static uint32_t simple_random(uint32_t v)
{
  v *= 1664525;
  v += 1013904223;

  return v;
}

static rtems_interval timeout(uint32_t v)
{
  return (v >> 23) % 4;
}

static void load_worker(rtems_task_argument index)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  uint32_t v = index;

  while (!ctx->stop_worker[index]) {
    uint32_t i = (v >> 13) % MRSP_COUNT;

    assert_prio(RTEMS_SELF, 3 + CPU_COUNT + index);

    if ((v >> 7) % 1024 == 0) {
      /* Give some time to the lower priority tasks */

      ++ctx->counters[index].sleep;

      sc = rtems_task_wake_after(1);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      ++ctx->counters[index].cpu[rtems_scheduler_get_processor()];
    } else {
      uint32_t n = (v >> 17) % (i + 1);
      uint32_t s;
      uint32_t t;

      /* Nested obtain */
      for (s = 0; s <= n; ++s) {
        uint32_t k = i - s;

        sc = rtems_semaphore_obtain(ctx->mrsp_ids[k], RTEMS_WAIT, timeout(v));
        if (sc == RTEMS_SUCCESSFUL) {
          ++ctx->counters[index].obtain[n];

          assert_prio(RTEMS_SELF, 3 + k);
        } else {
          rtems_test_assert(sc == RTEMS_TIMEOUT);

          ++ctx->counters[index].timeout;

          break;
        }

        ++ctx->counters[index].cpu[rtems_scheduler_get_processor()];

        v = simple_random(v);
      }

      /* Release in reverse obtain order */
      for (t = 0; t < s; ++t) {
        uint32_t k = i + t - s + 1;

        sc = rtems_semaphore_release(ctx->mrsp_ids[k]);
        rtems_test_assert(sc == RTEMS_SUCCESSFUL);

        ++ctx->counters[index].cpu[rtems_scheduler_get_processor()];
      }
    }

    v = simple_random(v);
  }

  sc = rtems_semaphore_release(ctx->counting_sem_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void migration_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t v = 0xdeadbeef;

  while (true) {
    uint32_t cpu_index = (v >> 5) % cpu_count;

    sc = rtems_task_set_scheduler(RTEMS_SELF, ctx->scheduler_ids[cpu_index], 2);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    ++ctx->migration_counters[rtems_scheduler_get_processor()];

    v = simple_random(v);
  }
}

static void test_mrsp_load(test_context *ctx)
{
  rtems_status_code sc;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t index;

  puts("test MrsP load");

  change_prio(RTEMS_SELF, 2);

  sc = rtems_task_create(
    rtems_build_name('M', 'I', 'G', 'R'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->migration_task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->migration_task_id, migration_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'Y', 'N', 'C'),
    0,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &ctx->counting_sem_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (index = 0; index < MRSP_COUNT; ++index) {
    create_mrsp_sema(ctx, &ctx->mrsp_ids[index], 3 + index);
  }

  for (index = 0; index < cpu_count; ++index) {
    uint32_t a = 2 * index;
    uint32_t b = a + 1;

    sc = rtems_task_create(
      'A' + a,
      255,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->worker_ids[a]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(
      ctx->worker_ids[a],
      ctx->scheduler_ids[index],
      3 + MRSP_COUNT + a
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(
      ctx->worker_ids[a],
      load_worker,
      (rtems_task_argument) a
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_create(
      'A' + b,
      255,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->worker_ids[b]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(
      ctx->worker_ids[b],
      ctx->scheduler_ids[index],
      3 + MRSP_COUNT + b
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(
      ctx->worker_ids[b],
      load_worker,
      (rtems_task_argument) b
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_task_wake_after(30 * rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (index = 0; index < 2 * cpu_count; ++index) {
    ctx->stop_worker[index] = true;
  }

  for (index = 0; index < 2 * cpu_count; ++index) {
    sc = rtems_semaphore_obtain(
      ctx->counting_sem_id,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (index = 0; index < 2 * cpu_count; ++index) {
    sc = rtems_task_delete(ctx->worker_ids[index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (index = 0; index < MRSP_COUNT; ++index) {
    sc = rtems_semaphore_delete(ctx->mrsp_ids[index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_semaphore_delete(ctx->counting_sem_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->migration_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (index = 0; index < 2 * cpu_count; ++index) {
    uint32_t nest_level;
    uint32_t cpu_index;

    printf(
      "worker[%" PRIu32 "]\n"
        "  sleep = %" PRIu32 "\n"
        "  timeout = %" PRIu32 "\n",
      index,
      ctx->counters[index].sleep,
      ctx->counters[index].timeout
    );

    for (nest_level = 0; nest_level < MRSP_COUNT; ++nest_level) {
      printf(
        "  obtain[%" PRIu32 "] = %" PRIu32 "\n",
        nest_level,
        ctx->counters[index].obtain[nest_level]
      );
    }

    for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
      printf(
        "  cpu[%" PRIu32 "] = %" PRIu32 "\n",
        cpu_index,
        ctx->counters[index].cpu[cpu_index]
      );
    }
  }

  for (index = 0; index < cpu_count; ++index) {
    printf(
      "migrations[%" PRIu32 "] = %" PRIu32 "\n",
      index,
      ctx->migration_counters[index]
    );
  }
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_resource_snapshot snapshot;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t cpu_index;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  ctx->main_task_id = rtems_task_self();

  for (cpu_index = 0; cpu_index < MIN(2, cpu_count); ++cpu_index) {
    sc = rtems_scheduler_ident(cpu_index, &ctx->scheduler_ids[cpu_index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (cpu_index = 2; cpu_index < cpu_count; ++cpu_index) {
    sc = rtems_scheduler_ident(
      cpu_index / 2 + 1,
      &ctx->scheduler_ids[cpu_index]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  test_mrsp_flush_error(ctx);
  test_mrsp_initially_locked();
  test_mrsp_nested_obtain_error(ctx);
  test_mrsp_deadlock_error(ctx);
  test_mrsp_multiple_obtain(ctx);

  if (cpu_count > 1) {
    test_mrsp_various_block_and_unblock(ctx);
    test_mrsp_obtain_after_migration(ctx);
    test_mrsp_obtain_and_sleep_and_release(ctx);
    test_mrsp_obtain_and_release_with_help(ctx);
    test_mrsp_obtain_and_release(ctx);
    test_mrsp_load(ctx);
  }

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS (2 * CPU_COUNT + 2)
#define CONFIGURE_MAXIMUM_SEMAPHORES (MRSP_COUNT + 1)
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_SIMPLE_SMP(0);
RTEMS_SCHEDULER_SIMPLE_SMP(1);
RTEMS_SCHEDULER_SIMPLE_SMP(2);
RTEMS_SCHEDULER_SIMPLE_SMP(3);
RTEMS_SCHEDULER_SIMPLE_SMP(4);
RTEMS_SCHEDULER_SIMPLE_SMP(5);
RTEMS_SCHEDULER_SIMPLE_SMP(6);
RTEMS_SCHEDULER_SIMPLE_SMP(7);
RTEMS_SCHEDULER_SIMPLE_SMP(8);
RTEMS_SCHEDULER_SIMPLE_SMP(9);
RTEMS_SCHEDULER_SIMPLE_SMP(10);
RTEMS_SCHEDULER_SIMPLE_SMP(11);
RTEMS_SCHEDULER_SIMPLE_SMP(12);
RTEMS_SCHEDULER_SIMPLE_SMP(13);
RTEMS_SCHEDULER_SIMPLE_SMP(14);
RTEMS_SCHEDULER_SIMPLE_SMP(15);
RTEMS_SCHEDULER_SIMPLE_SMP(16);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(0, 0), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(1, 1), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(2, 2), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(3, 3), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(4, 4), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(5, 5), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(6, 6), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(7, 7), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(8, 8), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(9, 9), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(10, 10), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(11, 11), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(12, 12), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(13, 13), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(14, 14), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(15, 15), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(16, 16)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(3, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(3, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(4, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(4, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(5, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(5, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(6, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(6, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(7, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(7, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(8, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(8, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(9, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(9, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(10, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(10, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(11, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(11, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(12, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(12, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(13, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(13, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(14, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(14, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(15, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(15, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(16, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(16, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .thread_switch = switch_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_NAME rtems_build_name('M', 'A', 'I', 'N')
#define CONFIGURE_INIT_TASK_PRIORITY 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
