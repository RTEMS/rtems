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

#include <rtems.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulersmpimpl.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDULER 3";

static void task(rtems_task_argument arg)
{
  rtems_test_assert(0);
}

static void test_case(
  Thread_Control *executing,
  Scheduler_SMP_Node *node,
  Scheduler_SMP_Node_state start_state,
  Priority_Control prio,
  bool prepend_it,
  Scheduler_SMP_Node_state new_state
)
{
  switch (start_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      _Thread_Change_priority(executing, 1, true);
      break;
    case SCHEDULER_SMP_NODE_READY:
      _Thread_Change_priority(executing, 4, true);
      break;
    default:
      rtems_test_assert(0);
      break;
  }
  rtems_test_assert(node->state == start_state);

  _Thread_Change_priority(executing, prio, prepend_it);
  rtems_test_assert(node->state == new_state);
}

static const Scheduler_SMP_Node_state states[2] = {
  SCHEDULER_SMP_NODE_SCHEDULED,
  SCHEDULER_SMP_NODE_READY
};

static const Priority_Control priorities[2] = { 2, 5 };

static const bool prepend_it[2] = { true, false };

static void test(void)
{
  rtems_status_code sc;
  rtems_id task_id;
  Thread_Control *executing;
  Scheduler_SMP_Node *node;
  size_t i;
  size_t j;
  size_t k;

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    3,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_id, task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  _Thread_Disable_dispatch();

  executing = _Thread_Executing;
  node = _Scheduler_SMP_Node_get( executing );

  for (i = 0; i < RTEMS_ARRAY_SIZE(states); ++i) {
    for (j = 0; j < RTEMS_ARRAY_SIZE(priorities); ++j) {
      for (k = 0; k < RTEMS_ARRAY_SIZE(prepend_it); ++k) {
        test_case(
          executing,
          node,
          states[i],
          priorities[j],
          prepend_it[k],
          states[j]
        );
      }
    }
  }

  _Thread_Change_priority(executing, 1, true);
  rtems_test_assert(node->state == SCHEDULER_SMP_NODE_SCHEDULED);

  _Thread_Enable_dispatch();

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS 1

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
