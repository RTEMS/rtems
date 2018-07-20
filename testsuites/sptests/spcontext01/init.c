/*
 * Copyright (c) 2013-2015 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/cpuimpl.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPCONTEXT 1";

#define ITERATION_COUNT 2000

#define PRIORITY_HIGH 2

#define PRIORITY_LOW 3

#define FINISH_EVENT RTEMS_EVENT_0

typedef struct {
  rtems_id control_task;
  rtems_id validate_tasks[3];
  rtems_id timer;
  size_t task_index;
  int iteration_counter;
} test_context;

static test_context test_instance;

static void validate_task(rtems_task_argument arg)
{
  _CPU_Context_validate(arg);
  rtems_test_assert(0);
}

static void start_validate_task(
  rtems_id *id,
  uintptr_t pattern,
  rtems_task_priority priority,
  bool fp_unit
)
{
  rtems_status_code sc;
  rtems_attribute fpu_state;

  fpu_state = fp_unit ? RTEMS_FLOATING_POINT : RTEMS_DEFAULT_ATTRIBUTES;

  sc = rtems_task_create(
    rtems_build_name('V', 'A', 'L', 'I'),
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    fpu_state,
    id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(*id, validate_task, pattern);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void reset_timer_or_finish(test_context *self, rtems_id timer)
{
  rtems_status_code sc;
  int i = self->iteration_counter;

  if (i < ITERATION_COUNT) {
    self->iteration_counter = i + 1;

    sc = rtems_timer_reset(timer);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  } else {
    sc = rtems_event_send(self->control_task, FINISH_EVENT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void switch_priorities(test_context *self)
{
  rtems_status_code sc;
  size_t index = self->task_index;
  size_t next = (index + 1) % RTEMS_ARRAY_SIZE(self->validate_tasks);
  size_t task_current_high = index;
  size_t task_next_high = next;
  rtems_task_priority priority;

  self->task_index = next;

  sc = rtems_task_set_priority(
    self->validate_tasks[task_next_high],
    PRIORITY_HIGH,
    &priority
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_priority(
    self->validate_tasks[task_current_high],
    PRIORITY_LOW,
    &priority
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void clobber_and_switch_timer(rtems_id timer, void *arg)
{
  uintptr_t pattern = (uintptr_t) 0xffffffffffffffffU;
  test_context *self = arg;

  reset_timer_or_finish(self, self->timer);
  switch_priorities(self);

  _CPU_Context_volatile_clobber(pattern);
}

static void start_timer(test_context *self)
{
  rtems_status_code sc;

  sc = rtems_timer_create(rtems_build_name('C', 'L', 'S', 'W'), &self->timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_fire_after(self->timer, 2, clobber_and_switch_timer, self);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void wait_for_finish(void)
{
  rtems_status_code sc;
  rtems_event_set out;

  sc = rtems_event_receive(
    FINISH_EVENT,
    RTEMS_WAIT | RTEMS_EVENT_ALL,
    RTEMS_NO_TIMEOUT,
    &out
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(out == FINISH_EVENT);
}

static void test(
  test_context *self,
  bool task_0_fpu,
  bool task_1_fpu,
  bool task_2_fpu
)
{
  rtems_status_code sc;
  uintptr_t pattern_0 = (uintptr_t) 0xaaaaaaaaaaaaaaaaU;
  uintptr_t pattern_1 = (uintptr_t) 0x5555555555555555U;
  uintptr_t pattern_2 = (uintptr_t) 0x0000000000000000U;

  memset(self, 0, sizeof(*self));

  self->control_task = rtems_task_self();
  start_validate_task(
    &self->validate_tasks[0],
    pattern_0,
    PRIORITY_HIGH,
    task_0_fpu
  );
  start_validate_task(
    &self->validate_tasks[1],
    pattern_1,
    PRIORITY_LOW,
    task_1_fpu
  );
  start_validate_task(
    &self->validate_tasks[2],
    pattern_2,
    PRIORITY_LOW,
    task_2_fpu
  );
  start_timer(self);
  wait_for_finish();

  sc = rtems_task_delete(self->validate_tasks[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(self->validate_tasks[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(self->validate_tasks[2]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_delete(self->timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_context_is_executing(void)
{
#if defined(RTEMS_SMP)
  /*
   * Provide a stack area, since on some architectures the top/bottom of stack
   * is initialized by _CPU_Context_Initialize().
   */
  static char stack[1024];

  Context_Control context;
  bool is_executing;

  memset(&context, 0, sizeof(context));

  is_executing = _CPU_Context_Get_is_executing(&context);
  rtems_test_assert(!is_executing);

  _CPU_Context_Set_is_executing(&context, true);
  is_executing = _CPU_Context_Get_is_executing(&context);
  rtems_test_assert(is_executing);

  _CPU_Context_Set_is_executing(&context, false);
  is_executing = _CPU_Context_Get_is_executing(&context);
  rtems_test_assert(!is_executing);

  _CPU_Context_Set_is_executing(&context, true);
  _CPU_Context_Initialize(
    &context,
    (void *) &stack[0],
    sizeof(stack),
    0,
    NULL,
    false,
    NULL
  );
  is_executing = _CPU_Context_Get_is_executing(&context);
  rtems_test_assert(is_executing);
#endif
}

static bool is_fp(int i)
{
  return i != 0;
}

static const char *desc(int i)
{
  return is_fp(i) ? "F" : "N";
}

static void Init(rtems_task_argument arg)
{
  test_context *self = &test_instance;
  int i;
  int j;
  int k;

  TEST_BEGIN();

  test_context_is_executing();

  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 2; ++j) {
      for (k = 0; k < 2; ++k) {
        printf("Test configuration %s %s %s... ", desc(i), desc(j), desc(k));
        test(self, is_fp(i), is_fp(j), is_fp(k));
        printf("done\n");
      }
    }
  }

  TEST_END();

  rtems_test_exit(0);
}

static void switch_extension(Thread_Control *executing, Thread_Control *heir)
{
  uintptr_t pattern = (uintptr_t) 0xffffffffffffffffU;

  _CPU_Context_volatile_clobber(pattern);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 4
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .thread_switch = switch_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
