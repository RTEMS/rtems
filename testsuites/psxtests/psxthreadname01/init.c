/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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
