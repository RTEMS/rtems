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
#include <rtems/counter.h>
#include <rtems/test.h>
#include <rtems/score/smpbarrier.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>

#include <assert.h>
#include <stdlib.h>

const char rtems_test_name[] = "SMPFATAL 3";

#define CPU_COUNT 2

static uint32_t main_cpu;

static SMP_barrier_Control giant_barrier = SMP_BARRIER_CONTROL_INITIALIZER;

static SMP_barrier_Control fatal_barrier = SMP_BARRIER_CONTROL_INITIALIZER;

static void acquire_giant_and_fatal_task(rtems_task_argument arg)
{
  SMP_barrier_State state = SMP_BARRIER_STATE_INITIALIZER;
  int i;

  for (i = 0; i < 13; ++i) {
    _Giant_Acquire();
  }

  _SMP_barrier_Wait(&giant_barrier, &state, CPU_COUNT);

  /*
   * Now we have to wait some time so that the other thread can actually start
   * with the _Giant_Acquire() procedure.
   */
  rtems_counter_delay_nanoseconds(1000000);

  rtems_fatal(RTEMS_FATAL_SOURCE_APPLICATION, 0xdeadbeef);
}

static void wait_for_giant(void)
{
  SMP_barrier_State state = SMP_BARRIER_STATE_INITIALIZER;

  _SMP_barrier_Wait(&giant_barrier, &state, CPU_COUNT);

  _Giant_Acquire();
}

static void Init(rtems_task_argument arg)
{
  uint32_t self = rtems_get_current_processor();
  uint32_t cpu_count = rtems_get_processor_count();

  rtems_test_begink();

  main_cpu = self;

  if (cpu_count >= CPU_COUNT) {
    rtems_status_code sc;
    rtems_id id;

    sc = rtems_task_create(
      rtems_build_name( 'W', 'A', 'I', 'T' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(id, acquire_giant_and_fatal_task, 0);
    assert(sc == RTEMS_SUCCESSFUL);

    wait_for_giant();
  } else {
    rtems_test_endk();
    exit(0);
  }
}

static void fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
)
{
  if (
    source == RTEMS_FATAL_SOURCE_APPLICATION
      || source == RTEMS_FATAL_SOURCE_SMP
  ) {
    uint32_t self = rtems_get_current_processor();
    SMP_barrier_State state = SMP_BARRIER_STATE_INITIALIZER;

    assert(!is_internal);

    if (self == main_cpu) {
      assert(source == RTEMS_FATAL_SOURCE_SMP);
      assert(code == SMP_FATAL_SHUTDOWN_RESPONSE);
    } else {
      assert(source == RTEMS_FATAL_SOURCE_APPLICATION);
      assert(code == 0xdeadbeef);
    }

    _SMP_barrier_Wait(&fatal_barrier, &state, CPU_COUNT);

    if (self == 0) {
      rtems_test_endk();
    }

    _SMP_barrier_Wait(&fatal_barrier, &state, CPU_COUNT);
  }
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
