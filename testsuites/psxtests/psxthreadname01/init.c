/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#define _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "tmacros.h"

const char rtems_test_name[] = "PSXTHREADNAME 1";

#define MAX_NAME_SIZE 13

static void test(void)
{
  static const char no_name[] = "NO";
  static const char big_name[] = "abcdefghijklmnopqrstuvwxyz";
  static const char new_name[] = "new";
  char name[sizeof(big_name)];
  int eno;

  memcpy(name, no_name, sizeof(no_name));
  eno = pthread_getname_np(0xffffffff, name, MAX_NAME_SIZE);
  rtems_test_assert(eno == ESRCH);
  rtems_test_assert(strcmp(name, "") == 0);

  eno = pthread_setname_np(0xffffffff, name);
  rtems_test_assert(eno == ESRCH);

  memcpy(name, no_name, sizeof(no_name));
  eno = pthread_getname_np(pthread_self(), name, 0);
  rtems_test_assert(eno == ERANGE);
  rtems_test_assert(strcmp(name, "NO") == 0);

  memcpy(name, no_name, sizeof(no_name));
  eno = pthread_getname_np(pthread_self(), name, sizeof(name));
  rtems_test_assert(eno == 0);
  rtems_test_assert(strcmp(name, "UI1 ") == 0);

  eno = pthread_setname_np(pthread_self(), big_name);
  rtems_test_assert(eno == ERANGE);

  memcpy(name, no_name, sizeof(no_name));
  eno = pthread_getname_np(pthread_self(), name, sizeof(name));
  rtems_test_assert(eno == 0);
  rtems_test_assert(strcmp(name, "abcdefghijkl") == 0);

  eno = pthread_setname_np(pthread_self(), new_name);
  rtems_test_assert(eno == 0);

  memcpy(name, no_name, sizeof(no_name));
  eno = pthread_getname_np(pthread_self(), name, sizeof(name));
  rtems_test_assert(eno == 0);
  rtems_test_assert(strcmp(name, "new") == 0);

  memcpy(name, no_name, sizeof(no_name));
  eno = pthread_getname_np(pthread_self(), name, 3);
  rtems_test_assert(eno == ERANGE);
  rtems_test_assert(strcmp(name, "ne") == 0);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_THREAD_NAME_SIZE MAX_NAME_SIZE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
