/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018 embedded brains GmbH
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

#include <future>
#include <new>

#include <rtems.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPCXX 1";

struct alignas(256) S {
  int i;
};

static void test_aligned_new(void)
{
  int *i = static_cast<decltype(i)>(
    ::operator new(sizeof(*i), std::align_val_t(256))
  );
  RTEMS_OBFUSCATE_VARIABLE(i);
  rtems_test_assert(i != nullptr);
  rtems_test_assert(reinterpret_cast<uintptr_t>(i) % 256 == 0);
  ::delete(i);

  S *s = new S;
  RTEMS_OBFUSCATE_VARIABLE(s);
  rtems_test_assert(s != nullptr);
  rtems_test_assert(reinterpret_cast<uintptr_t>(s) % 256 == 0);
  delete s;
}

static void test_future(void)
{
  std::future<int> f = std::async(std::launch::async, []{ return 12358; });
  rtems_test_assert(f.get() == 12358);
}

extern "C" void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_aligned_new();
  test_future();

  TEST_END();
  exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
