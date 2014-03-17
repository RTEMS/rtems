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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <rtems.h>
#include <rtems/profiling.h>

const char rtems_test_name[] = "SMPLOAD 1";

#define CPU_COUNT 32

#define WORKER_COUNT (3 * CPU_COUNT)

struct {
  rtems_id sema_prio_inherit;
  rtems_id sema_prio_ceiling;
} test_context;

static uint32_t simple_random(uint32_t v)
{
	v *= 1664525;
	v += 1013904223;

	return v;
}

static void worker_task(rtems_task_argument arg)
{
  uint32_t v = arg;

  while (true) {
    rtems_status_code sc;
    rtems_id id;

    v = simple_random(v);

    if ((v & 0x80000000) != 0) {
      id = test_context.sema_prio_inherit;
    } else {
      id = test_context.sema_prio_ceiling;
    }

    sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_wake_after(1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_semaphore_release(id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void test(void)
{
  uint32_t i;
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_semaphore_create(
    rtems_build_name('I', 'N', 'H', 'R'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &test_context.sema_prio_inherit
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('C', 'E', 'I', 'L'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    2,
    &test_context.sema_prio_ceiling
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < WORKER_COUNT; ++i) {
    sc = rtems_task_create(
      rtems_build_name('W', 'O', 'R', 'K'),
      3 + i,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(id, worker_task, i);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_task_wake_after(10 * rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_profiling_report_xml("SMPLOAD 1", rtems_printf_plugin, NULL, 1, "  ");
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

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS (1 + WORKER_COUNT)
#define CONFIGURE_MAXIMUM_SEMAPHORES 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
