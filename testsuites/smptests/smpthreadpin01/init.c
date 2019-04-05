/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>
#include <rtems/thread.h>
#include <rtems/score/threadimpl.h>

#include <tmacros.h>

const char rtems_test_name[] = "SMPTHREADPIN 1";

#define CPU_COUNT 2

#define SCHED_A rtems_build_name(' ', ' ', ' ', 'A')

#define SCHED_B rtems_build_name(' ', ' ', ' ', 'B')

#define EVENT_WAKEUP_MASTER RTEMS_EVENT_0

#define EVENT_MTX_LOCK RTEMS_EVENT_1

#define EVENT_MTX_UNLOCK RTEMS_EVENT_2

#define EVENT_MOVE_BUSY_TO_CPU_0 RTEMS_EVENT_3

#define EVENT_MOVE_BUSY_TO_CPU_1 RTEMS_EVENT_4

#define EVENT_MOVE_SELF_TO_CPU_0 RTEMS_EVENT_5

#define EVENT_MOVE_SELF_TO_CPU_1 RTEMS_EVENT_6

#define EVENT_SET_SELF_PRIO_TO_LOW RTEMS_EVENT_7

#define EVENT_SET_BUSY_PRIO_TO_IDLE RTEMS_EVENT_8

#define EVENT_SET_FLAG RTEMS_EVENT_9

#define PRIO_IDLE 6

#define PRIO_VERY_LOW 5

#define PRIO_LOW 4

#define PRIO_MIDDLE 3

#define PRIO_HIGH 2

#define PRIO_VERY_HIGH 1

typedef struct {
  rtems_id master;
  rtems_id event;
  rtems_id event_2;
  rtems_id busy;
  rtems_id sched_a;
  rtems_id sched_b;
  rtems_mutex mtx;
  volatile bool flag;
} test_context;

static test_context test_instance;

static rtems_task_priority set_prio(rtems_id id, rtems_task_priority prio)
{
  rtems_status_code sc;
  rtems_task_priority old_prio;

  old_prio = 0xffffffff;
  sc = rtems_task_set_priority(id, prio, &old_prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return old_prio;
}

static void set_affinity(rtems_id task, uint32_t cpu_index)
{
  rtems_status_code sc;
  rtems_id sched_cpu;
  rtems_id sched_task;
  cpu_set_t set;

  sc = rtems_scheduler_ident_by_processor(cpu_index, &sched_cpu);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_get_scheduler(task, &sched_task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  if (sched_task != sched_cpu) {
    rtems_task_priority prio;

    CPU_FILL(&set);
    sc = rtems_task_set_affinity(task, sizeof(set), &set);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    prio = set_prio(task, RTEMS_CURRENT_PRIORITY);
    sc = rtems_task_set_scheduler(task, sched_cpu, prio);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  CPU_ZERO(&set);
  CPU_SET((int) cpu_index, &set);
  sc = rtems_task_set_affinity(task, sizeof(set), &set);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void send_events(rtems_id task, rtems_event_set events)
{
  rtems_status_code sc;

  sc = rtems_event_send(task, events);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static rtems_event_set wait_for_events(void)
{
  rtems_event_set events;
  rtems_status_code sc;

  sc = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_EVENT_ANY | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return events;
}

static void pin(bool blocked)
{
  Per_CPU_Control *cpu_self;
  Thread_Control *executing;

  cpu_self = _Thread_Dispatch_disable();
  executing = _Per_CPU_Get_executing(cpu_self);

  if (blocked) {
    _Thread_Set_state(executing, STATES_SUSPENDED);
  }

  _Thread_Pin(executing);

  if (blocked) {
    _Thread_Clear_state(executing, STATES_SUSPENDED);
  }

  _Thread_Dispatch_enable(cpu_self);
}

static void unpin(bool blocked)
{
  Per_CPU_Control *cpu_self;
  Thread_Control *executing;

  cpu_self = _Thread_Dispatch_disable();
  executing = _Per_CPU_Get_executing(cpu_self);

  if (blocked) {
    _Thread_Set_state(executing, STATES_SUSPENDED);
  }

  _Thread_Unpin(executing, cpu_self);

  if (blocked) {
    _Thread_Clear_state(executing, STATES_SUSPENDED);
  }

  _Thread_Dispatch_enable(cpu_self);
}

static void event_task(rtems_task_argument arg)
{
  test_context *ctx;

  ctx = (test_context *) arg;

  while (true) {
    rtems_event_set events;

    events = wait_for_events();

    /*
     * The order of event processing is important!
     */

    if ((events & EVENT_MTX_LOCK) != 0) {
      rtems_mutex_lock(&ctx->mtx);
    }

    if ((events & EVENT_MTX_UNLOCK) != 0) {
      rtems_mutex_unlock(&ctx->mtx);
    }

    if ((events & EVENT_MOVE_BUSY_TO_CPU_0) != 0) {
      set_affinity(ctx->busy, 0);
    }

    if ((events & EVENT_MOVE_BUSY_TO_CPU_1) != 0) {
      set_affinity(ctx->busy, 1);
    }

    if ((events & EVENT_MOVE_SELF_TO_CPU_0) != 0) {
      set_affinity(RTEMS_SELF, 0);
    }

    if ((events & EVENT_MOVE_SELF_TO_CPU_1) != 0) {
      set_affinity(RTEMS_SELF, 1);
    }

    if ((events & EVENT_SET_SELF_PRIO_TO_LOW) != 0) {
      set_prio(RTEMS_SELF, PRIO_LOW);
    }

    if ((events & EVENT_SET_BUSY_PRIO_TO_IDLE) != 0) {
      set_prio(ctx->busy, PRIO_IDLE);
    }

    if ((events & EVENT_SET_FLAG) != 0) {
      ctx->flag = true;
    }

    if ((events & EVENT_WAKEUP_MASTER) != 0) {
      send_events(ctx->master, EVENT_WAKEUP_MASTER);
    }
  }
}

static void busy_task(rtems_task_argument arg)
{
  (void) arg;

  _CPU_Thread_Idle_body(0);
}

static const char *blocked_or_ready(bool blocked)
{
  return blocked ? "blocked" : "ready";
}

static void reconfigure_scheduler(test_context *ctx)
{
  rtems_status_code sc;

  puts("reconfigure scheduler");

  set_prio(ctx->master, PRIO_MIDDLE);
  set_prio(ctx->event, PRIO_LOW);
  set_prio(ctx->event_2, PRIO_VERY_LOW);
  set_prio(ctx->busy, PRIO_IDLE);

  set_affinity(ctx->master, 0);
  set_affinity(ctx->event, 0);
  set_affinity(ctx->event_2, 0);
  set_affinity(ctx->busy, 0);

  sc = rtems_scheduler_remove_processor(ctx->sched_a, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_add_processor(ctx->sched_b, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_simple_pin_unpin(test_context *ctx, int run)
{
  Per_CPU_Control *cpu_self;
  Thread_Control *executing;

  printf("test simple wait unpin (run %i)\n", run);

  set_affinity(ctx->busy, 0);
  set_prio(ctx->busy, PRIO_IDLE);
  set_prio(RTEMS_SELF, PRIO_MIDDLE);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  cpu_self = _Thread_Dispatch_disable();
  executing = _Per_CPU_Get_executing(cpu_self);
  _Thread_Pin(executing);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  _Thread_Unpin(executing, cpu_self);
  _Thread_Dispatch_enable(cpu_self);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);
}

static void test_pin_wait_unpin(test_context *ctx, bool blocked, int run)
{
  printf("test pin wait unpin (%s, run %i)\n", blocked_or_ready(blocked), run);

  set_affinity(ctx->busy, 0);
  set_prio(ctx->busy, PRIO_IDLE);
  set_prio(RTEMS_SELF, PRIO_MIDDLE);
  set_prio(ctx->event, PRIO_LOW);
  set_affinity(ctx->event, 1);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  pin(blocked);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  send_events(ctx->event, EVENT_WAKEUP_MASTER);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);
  wait_for_events();
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  set_prio(ctx->busy, PRIO_HIGH);
  set_affinity(ctx->busy, 0);
  unpin(blocked);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);
}

static void test_pin_preempt_unpin(test_context *ctx, bool blocked, int run)
{
  printf(
    "test pin preempt unpin (%s, run %i)\n",
    blocked_or_ready(blocked),
    run
  );

  set_prio(RTEMS_SELF, PRIO_MIDDLE);
  set_prio(ctx->event, PRIO_VERY_HIGH);
  set_prio(ctx->busy, PRIO_HIGH);
  set_affinity(ctx->event, 0);
  set_affinity(ctx->busy, 0);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  pin(blocked);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  ctx->flag = false;
  send_events(
    ctx->event,
    EVENT_MOVE_BUSY_TO_CPU_1 | EVENT_SET_SELF_PRIO_TO_LOW
      | EVENT_SET_BUSY_PRIO_TO_IDLE | EVENT_SET_FLAG
  );

  while (!ctx->flag) {
    rtems_test_assert(rtems_scheduler_get_processor() == 1);
  }

  set_affinity(ctx->busy, 0);
  unpin(blocked);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);
}

static void test_pin_home_no_help_unpin(
  test_context *ctx,
  bool blocked,
  int run
)
{
  rtems_status_code sc;

  printf(
    "test pin home no help unpin (%s, run %i)\n",
    blocked_or_ready(blocked),
    run
  );

  set_affinity(ctx->busy, 1);
  set_prio(ctx->busy, PRIO_IDLE);
  set_prio(RTEMS_SELF, PRIO_MIDDLE);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  pin(blocked);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  sc = rtems_task_set_scheduler(RTEMS_SELF, ctx->sched_b, 1);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);

  rtems_mutex_lock(&ctx->mtx);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  set_affinity(ctx->event, 1);
  set_prio(ctx->event, PRIO_MIDDLE);

  send_events(ctx->event, EVENT_MTX_LOCK);
  set_prio(ctx->event_2, PRIO_LOW);
  set_affinity(ctx->event_2, 1);
  send_events(ctx->event_2, EVENT_WAKEUP_MASTER);
  wait_for_events();

  /* Now the event task can help us */
  rtems_test_assert(ctx->mtx._Queue._heads != NULL);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  set_affinity(ctx->event_2, 0);
  set_affinity(ctx->busy, 1);
  set_prio(ctx->busy, PRIO_HIGH);
  send_events(
    ctx->event_2,
    EVENT_MOVE_BUSY_TO_CPU_0 | EVENT_MOVE_SELF_TO_CPU_1
      | EVENT_SET_SELF_PRIO_TO_LOW | EVENT_SET_BUSY_PRIO_TO_IDLE
  );
  set_prio(ctx->event_2, PRIO_VERY_HIGH);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  rtems_mutex_unlock(&ctx->mtx);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  send_events(ctx->event, EVENT_WAKEUP_MASTER | EVENT_MTX_UNLOCK);
  wait_for_events();
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  unpin(blocked);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);
}

static void test_pin_foreign_no_help_unpin(
  test_context *ctx,
  bool blocked,
  int run
)
{
  printf(
    "test pin foreign no help unpin (%s, run %i)\n",
    blocked_or_ready(blocked),
    run
  );

  set_affinity(ctx->busy, 1);
  set_prio(ctx->busy, PRIO_IDLE);
  set_prio(RTEMS_SELF, PRIO_MIDDLE);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  rtems_mutex_lock(&ctx->mtx);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  set_affinity(ctx->event, 1);
  set_prio(ctx->event, PRIO_MIDDLE);
  send_events(ctx->event, EVENT_MTX_LOCK);
  set_prio(ctx->event_2, PRIO_LOW);
  set_affinity(ctx->event_2, 1);
  send_events(ctx->event_2, EVENT_WAKEUP_MASTER);
  wait_for_events();

  /* Now the event task can help us */
  rtems_test_assert(ctx->mtx._Queue._heads != NULL);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  /* Request help */
  set_affinity(ctx->busy, 0);
  set_prio(ctx->busy, PRIO_HIGH);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  /* Pin while using foreign scheduler */
  pin(blocked);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  set_affinity(ctx->event_2, 1);
  send_events(
    ctx->event_2,
    EVENT_MOVE_BUSY_TO_CPU_1 | EVENT_MOVE_SELF_TO_CPU_0
      | EVENT_SET_SELF_PRIO_TO_LOW | EVENT_SET_BUSY_PRIO_TO_IDLE
  );
  set_prio(ctx->event_2, PRIO_VERY_HIGH);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  unpin(blocked);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  set_prio(ctx->busy, PRIO_IDLE);
  rtems_mutex_unlock(&ctx->mtx);
  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  send_events(ctx->event, EVENT_WAKEUP_MASTER | EVENT_MTX_UNLOCK);
  wait_for_events();
  rtems_test_assert(rtems_scheduler_get_processor() == 0);
}

static void test(test_context *ctx)
{
  rtems_status_code sc;
  int run;

  ctx->master = rtems_task_self();

  rtems_mutex_init(&ctx->mtx, "test");

  sc = rtems_scheduler_ident(SCHED_A, &ctx->sched_a);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_ident(SCHED_B, &ctx->sched_b);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('B', 'U', 'S', 'Y'),
    PRIO_HIGH,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->busy
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->busy, busy_task, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  set_affinity(ctx->busy, 0);
  set_prio(ctx->busy, PRIO_IDLE);
  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  sc = rtems_task_create(
    rtems_build_name('E', 'V', 'T', '1'),
    PRIO_LOW,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->event
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->event, event_task, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  send_events(ctx->event, EVENT_WAKEUP_MASTER);
  wait_for_events();

  sc = rtems_task_create(
    rtems_build_name('E', 'V', 'T', '2'),
    PRIO_LOW,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->event_2
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->event_2, event_task, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  send_events(ctx->event_2, EVENT_WAKEUP_MASTER);
  wait_for_events();

  for (run = 1; run <= 3; ++run) {
    test_simple_pin_unpin(ctx, run);
    test_pin_wait_unpin(ctx, true, run);
    test_pin_wait_unpin(ctx, false, run);
    test_pin_preempt_unpin(ctx, true, run);
    test_pin_preempt_unpin(ctx, false, run);
  }

  reconfigure_scheduler(ctx);

  for (run = 1; run <= 3; ++run) {
    test_pin_home_no_help_unpin(ctx, true, run);
    test_pin_home_no_help_unpin(ctx, false, run);
    test_pin_foreign_no_help_unpin(ctx, true, run);
    test_pin_foreign_no_help_unpin(ctx, false, run);
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  if (rtems_scheduler_get_processor_maximum() == CPU_COUNT) {
    test(&test_instance);
  } else {
    puts("warning: wrong processor count to run the test");
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS 4

#define CONFIGURE_INIT_TASK_PRIORITY PRIO_MIDDLE

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP(a, CONFIGURE_MAXIMUM_PROCESSORS);

RTEMS_SCHEDULER_EDF_SMP(b, CONFIGURE_MAXIMUM_PROCESSORS);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(a, SCHED_A), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(b, SCHED_B)  \

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
