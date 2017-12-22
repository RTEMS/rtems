/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

const char rtems_test_name[] = "SMPFATAL 3";

static void bad( rtems_id timer_id, void *arg )
{
  rtems_id *sem_id;

  sem_id = arg;

  rtems_semaphore_obtain( *sem_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  rtems_test_assert( 0 );
}

static void Init( rtems_task_argument arg )
{
  rtems_status_code sc;
  rtems_id          timer_id;
  rtems_id          sem_id;

  TEST_BEGIN();

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    1,
    &sem_id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_timer_create(
    rtems_build_name( 'E', 'V', 'I', 'L' ),
    &timer_id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_semaphore_obtain( sem_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_timer_fire_after( timer_id, 1, bad, &sem_id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rtems_task_wake_after( 2 );
  rtems_test_assert( 0 );
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  if (
    source == INTERNAL_ERROR_CORE
      && !always_set_to_false
      && code == INTERNAL_ERROR_THREAD_QUEUE_ENQUEUE_STICKY_FROM_BAD_STATE
  ) {
    TEST_END();
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_TIMERS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 1
#define CONFIGURE_MAXIMUM_MRSP_SEMAPHORES 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
