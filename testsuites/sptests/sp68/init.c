/**
 * @file
 *
 * @ingroup sptests
 *
 * @brief Test for timer server with blocking calls.
 */

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

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define TIMER_COUNT 6

#define OBTAIN 0
#define RELEASE 1
#define INTERRUPT 2
#define DELAYED 3
#define SERVER_TRIGGERED 4
#define INTERRUPT_TRIGGERED 5

#define T0 0
#define T1 1
#define T2 2
#define T3 3
#define T4 4
#define T5 5
#define T6 6

static volatile bool obtain_try;
static volatile bool obtain_done;
static volatile bool release_happened;
static volatile bool interrupt_happened;
static volatile bool delayed_happened;
static volatile bool server_triggered_happened;
static volatile bool interrupt_triggered_happened;

static rtems_id timer [TIMER_COUNT];

static rtems_id semaphore;
static rtems_id mutex;
static rtems_id message_queue;
static rtems_id region;
static rtems_id barrier;

static void *region_item;

static rtems_interval start;

static volatile enum resource_type {
  SEMAPHORE = 0,
  MUTEX,
  MESSAGE_QUEUE,
  REGION,
  EVENT,
  BARRIER,
  TASK_WAKE_AFTER
} resource_type;

static const char *resource_type_desc [] = {
  "SEMAPHORE",
  "MUTEX",
  "MESSAGE QUEUE",
  "REGION",
  "EVENT",
  "BARRIER",
  "TASK WAKE AFTER"
};

static void assert_time(rtems_interval expected)
{
  rtems_test_assert((rtems_clock_get_ticks_since_boot() - start) == expected);
}

static void obtain_callback(rtems_id timer_id, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  char buf [1];
  size_t size = sizeof(buf);
  void *new_region_item = NULL;
  rtems_event_set events = 0;

  assert_time(T1);

  rtems_test_assert(
    !release_happened
      && !interrupt_happened
      && !delayed_happened
      && !interrupt_triggered_happened
      && !server_triggered_happened
  );

  obtain_try = true;

  switch (resource_type) {
    case SEMAPHORE:
      sc = rtems_semaphore_obtain(semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      break;
    case MUTEX:
      sc = rtems_semaphore_obtain(mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      break;
    case MESSAGE_QUEUE:
      sc = rtems_message_queue_receive(
        message_queue, buf, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      break;
    case REGION:
      sc = rtems_region_get_segment(
        region, 1, RTEMS_WAIT, RTEMS_NO_TIMEOUT, &new_region_item);
      break;
    case EVENT:
      sc = rtems_event_receive(
        RTEMS_EVENT_0, RTEMS_EVENT_ALL | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &events);
      break;
    case BARRIER:
      sc = rtems_barrier_wait(barrier, RTEMS_NO_TIMEOUT);
      break;
    case TASK_WAKE_AFTER:
      sc = rtems_task_wake_after(T4 - T1);
      break;
    default:
      rtems_test_assert(false);
      break;
  }
  directive_failed(sc, "obtain");

  obtain_done = true;
}

static void release_callback(rtems_id timer_id, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  char buf [1];
  size_t size = sizeof(buf);
  uint32_t released = 0;

  assert_time(T4);

  rtems_test_assert(
    obtain_try
      && interrupt_happened
      && !delayed_happened
      && !interrupt_triggered_happened
      && !server_triggered_happened
  );

  switch (resource_type) {
    case SEMAPHORE:
      sc = rtems_semaphore_release(semaphore);
      break;
    case MUTEX:
      sc = rtems_semaphore_release(mutex);
      break;
    case MESSAGE_QUEUE:
      sc = rtems_message_queue_send(message_queue, buf, size);
      break;
    case EVENT:
      sc = rtems_event_send(_Timer_server->thread->Object.id, RTEMS_EVENT_0);
      break;
    case BARRIER:
      sc = rtems_barrier_release(barrier, &released);
      break;
    case TASK_WAKE_AFTER:
      sc = RTEMS_SUCCESSFUL;
      break;
    default:
      rtems_test_assert(false);
      break;
  }
  directive_failed_with_level(sc, "release", 1);

  release_happened = true;
}

static void interrupt_triggered_callback(rtems_id timer_id, void *arg)
{
  /*
   * This callback is scheduled to fire at T3, but is delayed due to the
   * blocked obtain callback.
   */
  assert_time(T4);

  rtems_test_assert(
    obtain_done
      && release_happened
      && interrupt_happened
      && !server_triggered_happened
  );

  interrupt_triggered_happened = true;
}

static void interrupt_callback(rtems_id timer_id, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  assert_time(T2);

  rtems_test_assert(
    obtain_try
      && !obtain_done
      && !release_happened
      && !delayed_happened
      && !interrupt_triggered_happened
      && !server_triggered_happened
  );

  sc = rtems_timer_server_fire_after(
    timer [INTERRUPT_TRIGGERED],
    T3 - T2,
    interrupt_triggered_callback,
    NULL
  );
  directive_failed_with_level(sc, "rtems_timer_server_fire_after", -1);

  interrupt_happened = true;
}

static void server_triggered_callback(rtems_id timer_id, void *arg)
{
  assert_time(T5);

  rtems_test_assert(
    obtain_done
      && release_happened
      && interrupt_happened
      && delayed_happened
      && interrupt_triggered_happened
  );

  server_triggered_happened = true;
}

static void delayed_callback(rtems_id timer_id, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  assert_time(T4);

  rtems_test_assert(
    obtain_done
      && release_happened
      && interrupt_happened
      && !server_triggered_happened
  );

  sc = rtems_timer_server_fire_after(
    timer [SERVER_TRIGGERED],
    T5 - T4,
    server_triggered_callback,
    NULL
  );
  directive_failed(sc, "rtems_timer_server_fire_after");

  delayed_happened = true;
}

static void test_reset(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  obtain_try = false;
  obtain_done = false;
  release_happened = false;
  interrupt_happened = false;
  delayed_happened = false;
  interrupt_triggered_happened = false;
  server_triggered_happened = false;

  /* Synchronize with tick */
  sc = rtems_task_wake_after(1);
  directive_failed(sc, "rtems_task_wake_after");

  start = rtems_clock_get_ticks_since_boot();
}

static void test_case(enum resource_type rt)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  printf("test case: %s\n", resource_type_desc [rt]);

  resource_type = rt;

  test_reset();

  sc = rtems_timer_server_fire_after(
    timer [OBTAIN],
    T1 - T0,
    obtain_callback,
    NULL
  );
  directive_failed(sc, "rtems_timer_server_fire_after");

  sc = rtems_timer_fire_after(
    timer [INTERRUPT],
    T2 - T0,
    interrupt_callback,
    NULL
  );
  directive_failed(sc, "rtems_timer_fire_after");

  sc = rtems_timer_server_fire_after(
    timer [DELAYED],
    T3 - T0,
    delayed_callback,
    NULL
  );
  directive_failed(sc, "rtems_timer_server_fire_after");

  if (resource_type != REGION) {
    sc = rtems_timer_fire_after(
      timer [RELEASE],
      T4 - T0,
      release_callback,
      NULL
    );
    directive_failed(sc, "rtems_timer_fire_after");

    assert_time(T0);

    sc = rtems_task_wake_after(T6 - T0);
    directive_failed(sc, "task_wake_after");
  } else {
    sc = rtems_task_wake_after(T4 - T0);
    directive_failed(sc, "task_wake_after");

    assert_time(T4);

    rtems_test_assert(
      obtain_try
        && interrupt_happened
        && !delayed_happened
        && !interrupt_triggered_happened
        && !server_triggered_happened
    );

    sc = rtems_region_return_segment(region, region_item);
    directive_failed(sc, "rtems_region_return_segment");

    release_happened = true;

    sc = rtems_task_wake_after(T6 - T4);
    directive_failed(sc, "task_wake_after");
  }

  assert_time(T6);

  rtems_test_assert(
    obtain_done
      && interrupt_happened
      && release_happened
      && delayed_happened
      && interrupt_triggered_happened
      && server_triggered_happened
  );
}

rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  char region_area [256];
  enum resource_type rt = SEMAPHORE;
  void *new_region_item = NULL;
  size_t i = 0;

  puts("\n\n*** TEST 68 ***");

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

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    0,
    RTEMS_LOCAL | RTEMS_FIFO | RTEMS_COUNTING_SEMAPHORE,
    0,
    &semaphore
  );
  directive_failed(sc, "rtems_semaphore_create");

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'U', 'T', 'X'),
    0,
    RTEMS_LOCAL | RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &mutex
  );
  directive_failed(sc, "rtems_semaphore_create");

  sc = rtems_message_queue_create(
    rtems_build_name('M', 'S', 'G', 'Q'),
    1,
    1,
    RTEMS_LOCAL | RTEMS_FIFO,
    &message_queue
  );
  directive_failed(sc, "rtems_message_queue_create");

  sc = rtems_region_create(
    rtems_build_name('R', 'E', 'G', 'I'),
    region_area,
    sizeof(region_area),
    1,
    RTEMS_LOCAL | RTEMS_FIFO,
    &region
  );
  directive_failed(sc, "rtems_region_create");

  do {
    region_item = new_region_item;
    sc = rtems_region_get_segment(
        region, 1, RTEMS_NO_WAIT, 0, &new_region_item);
  } while (sc == RTEMS_SUCCESSFUL);

  sc = rtems_barrier_create(
    rtems_build_name('B', 'A', 'R', 'R'),
    RTEMS_LOCAL | RTEMS_FIFO,
    2,
    &barrier
  );
  directive_failed(sc, "rtems_barrier_create");

  while (rt <= TASK_WAKE_AFTER) {
    test_case(rt);
    ++rt;
  }

  puts("*** END OF TEST 68 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_TIMERS TIMER_COUNT
#define CONFIGURE_MAXIMUM_SEMAPHORES 2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 1
#define CONFIGURE_MAXIMUM_REGIONS 1
#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
