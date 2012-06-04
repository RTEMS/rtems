/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <intrcritical.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define TIMER_COUNT 4

#define TIMER_TRIGGER 0
#define TIMER_RESET 1
#define TIMER_NEVER_INTERVAL 2
#define TIMER_NEVER_TOD 3

static rtems_id timer [TIMER_COUNT];

static rtems_time_of_day tod;

static volatile bool case_hit;

static void never_callback(rtems_id timer, void *arg)
{
  rtems_test_assert(false);
}

static void reset_tod_timer(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_timer_server_fire_when(
    timer [TIMER_NEVER_TOD],
    &tod,
    never_callback,
    NULL
  );
  directive_failed_with_level(sc, "rtems_timer_server_fire_after", -1);
}

static void reset_callback(rtems_id timer_id, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_timer_reset(timer [TIMER_RESET]);
  directive_failed_with_level(sc, "rtems_timer_reset", -1);

  sc = rtems_timer_reset(timer [TIMER_NEVER_INTERVAL]);
  directive_failed_with_level(sc, "rtems_timer_reset", -1);

  reset_tod_timer();

  if (!case_hit) {
    case_hit = _Timer_server->insert_chain != NULL;
  }
}

static void trigger_callback(rtems_id timer_id, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (case_hit) {
    puts("*** END OF INTERRUPT CRITICAL SECTION 17 ***");

    rtems_test_exit(0);
  } else if (interrupt_critical_section_test_support_delay()) {
    puts("test case not hit, give up");

    rtems_test_exit(0);
  }

  sc = rtems_timer_reset(timer [TIMER_TRIGGER]);
  directive_failed(sc, "rtems_timer_reset");
}

rtems_task Init( rtems_task_argument ignored )
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t i = 0;

  puts("\n\n*** TEST INTERRUPT CRITICAL SECTION 17 ***");

  build_time(&tod, 4, 12, 2009, 9, 34, 11, 0);
  sc = rtems_clock_set(&tod);
  directive_failed(sc, "rtems_clock_set");

  ++tod.year;

  for (i = 0; i < TIMER_COUNT; ++i) {
    sc = rtems_timer_create(
      rtems_build_name('T', 'I', 'M', '0' + i),
      &timer [i]
    );
    directive_failed(sc, "rtems_timer_create");
  }

  sc = rtems_timer_initiate_server(
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  directive_failed(sc, "rtems_timer_initiate_server");

  sc = rtems_timer_server_fire_after(
    timer [TIMER_NEVER_INTERVAL],
    2,
    never_callback,
    NULL
  );
  directive_failed(sc, "rtems_timer_server_fire_after");

  reset_tod_timer();

  sc = rtems_timer_fire_after(
    timer [TIMER_RESET],
    1,
    reset_callback,
    NULL
  );
  directive_failed(sc, "rtems_timer_fire_after");

  sc = rtems_timer_server_fire_after(
    timer [TIMER_TRIGGER],
    1,
    trigger_callback,
    NULL
  );
  directive_failed(sc, "rtems_timer_server_fire_after");

  interrupt_critical_section_test_support_initialize(NULL);

  rtems_task_delete(RTEMS_SELF);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_TIMERS 4

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
