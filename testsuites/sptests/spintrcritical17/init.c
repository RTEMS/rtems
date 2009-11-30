/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>
#include <intrcritical.h>

static rtems_id timer_0;
static rtems_id timer_1;
static rtems_id timer_2;

volatile bool case_hit;

static void never_callback(rtems_id timer, void *arg)
{
  rtems_test_assert(false);
}

static void reset_callback(rtems_id timer, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_timer_reset(timer_0);
  directive_failed_with_level(sc, "rtems_timer_reset", -1);

  sc = rtems_timer_reset(timer_1);
  directive_failed_with_level(sc, "rtems_timer_reset", -1);

  if (!case_hit) {
    case_hit = _Timer_server->insert_chain != NULL;
  }
}

static void trigger_callback(rtems_id timer, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (case_hit) {
    puts("*** END OF INTERRUPT CRITICAL SECTION 17 ***");

    rtems_test_exit(0);
  } else if (interrupt_critical_section_test_support_delay()) {
    puts("test case not hit, give up");

    rtems_test_exit(0);
  }

  sc = rtems_timer_reset(timer_2);
  directive_failed(sc, "rtems_timer_reset");
}

rtems_task Init( rtems_task_argument ignored )
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  puts("\n\n*** TEST INTERRUPT CRITICAL SECTION 17 ***");

  sc = rtems_timer_create(
    rtems_build_name('T', 'I', 'M', '0'),
    &timer_0
  );
  directive_failed(sc, "rtems_timer_create");

  sc = rtems_timer_create(
    rtems_build_name('T', 'I', 'M', '1'),
    &timer_1
  );
  directive_failed(sc, "rtems_timer_create");

  sc = rtems_timer_create(
    rtems_build_name('T', 'I', 'M', '2'),
    &timer_2
  );
  directive_failed(sc, "rtems_timer_create");

  sc = rtems_timer_initiate_server(
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  directive_failed(sc, "rtems_timer_initiate_server");

  sc = rtems_timer_server_fire_after(
    timer_0,
    2,
    never_callback,
    NULL
  );
  directive_failed(sc, "rtems_timer_server_fire_after");

  sc = rtems_timer_fire_after(
    timer_1,
    1,
    reset_callback,
    NULL
  );
  directive_failed(sc, "rtems_timer_fire_after");

  sc = rtems_timer_server_fire_after(
    timer_2,
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
#define CONFIGURE_MAXIMUM_TIMERS 3

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
