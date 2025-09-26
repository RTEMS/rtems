/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include "pmacros.h"
#include <pthread.h>  /* thread facilities */

const char rtems_test_name[] = "PSXCLEANUP 1";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

static rtems_id main_task_id;

static rtems_id restart_task_id;

static volatile rtems_task_argument restart_cleanup_arg;

static void wake_up_main(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_send(main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void wait_for_restart_task(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void restart_cleanup(void *arg)
{
  rtems_test_assert(restart_task_id == rtems_task_self());

  restart_cleanup_arg = (rtems_task_argument) arg;

  wake_up_main();
}

static void restart_task(rtems_task_argument arg)
{
  pthread_cleanup_push(restart_cleanup, (void *) arg);

  wake_up_main();

  rtems_test_assert(0);

  pthread_cleanup_pop(0);
}

static void test_restart_with_cleanup(void)
{
  rtems_status_code sc;
  rtems_id id;
  rtems_task_priority prio = 1;

  main_task_id = rtems_task_self();

  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('R', 'E', 'S', 'T'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  restart_task_id = id;

  sc = rtems_task_start(id, restart_task, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wait_for_restart_task();

  sc = rtems_task_restart(id, 2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wait_for_restart_task();

  rtems_test_assert(restart_cleanup_arg == 1);

  wait_for_restart_task();

  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wait_for_restart_task();

  rtems_test_assert(restart_cleanup_arg == 2);
}

static void cleaner(void *arg)
{
  (void) arg;

  puts( "clean was not supposed to run" );
  rtems_test_assert(0);
}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  TEST_BEGIN();

  test_restart_with_cleanup();

  puts( "Init - pthread_cleanup_push - a routine we will not execute" );
  pthread_cleanup_push(cleaner, NULL);

  puts( "Init - pthread_cleanup_pop - do not execute" );
  pthread_cleanup_pop(0);

  TEST_END();
  rtems_test_exit(0);
}


/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
