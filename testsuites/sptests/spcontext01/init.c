/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

const char rtems_test_name[] = "SPCONTEXT 1";

#define ITERATION_COUNT 2000

#define PRIORITY_HIGH 2

#define PRIORITY_LOW 3

#define FINISH_EVENT RTEMS_EVENT_0

typedef struct {
  rtems_id control_task;
  rtems_id validate_tasks[2];
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
  rtems_task_priority priority
)
{
  rtems_status_code sc;

  sc = rtems_task_create(
    rtems_build_name('V', 'A', 'L', 'I'),
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
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
  size_t next = (index + 1) & 0x1;
  size_t task_high = index;
  size_t task_low = next;
  rtems_task_priority priority;

  self->task_index = next;

  sc = rtems_task_set_priority(
    self->validate_tasks[task_high],
    PRIORITY_HIGH,
    &priority
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_priority(
    self->validate_tasks[task_low],
    PRIORITY_LOW,
    &priority
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void clobber_and_switch_timer(rtems_id timer, void *arg)
{
  uintptr_t pattern = (uintptr_t) 0xffffffffffffffffU;
  test_context *self = arg;

  reset_timer_or_finish(self, timer);
  switch_priorities(self);

  _CPU_Context_volatile_clobber(pattern);
}

static void start_timer(test_context *self)
{
  rtems_status_code sc;
  rtems_id timer;

  sc = rtems_timer_create(rtems_build_name('C', 'L', 'S', 'W'), &timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_fire_after(timer, 2, clobber_and_switch_timer, self);
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

static void test(test_context *self)
{
  uintptr_t pattern_0 = (uintptr_t) 0xaaaaaaaaaaaaaaaaU;
  uintptr_t pattern_1 = (uintptr_t) 0x5555555555555555U;

  memset(self, 0, sizeof(*self));

  self->control_task = rtems_task_self();

  start_validate_task(&self->validate_tasks[0], pattern_0, PRIORITY_LOW);
  start_validate_task(&self->validate_tasks[1], pattern_1, PRIORITY_HIGH);
  start_timer(self);
  wait_for_finish();
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

static void Init(rtems_task_argument arg)
{
  test_context *self = &test_instance;

  TEST_BEGIN();

  test_context_is_executing();
  test(self);

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
