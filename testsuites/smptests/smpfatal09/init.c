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

#include <rtems.h>
#include <rtems/score/percpudata.h>
#include <rtems/score/memory.h>
#include <rtems/sysinit.h>

#include <tmacros.h>

static PER_CPU_DATA_ITEM(int, i) = 123;

const char rtems_test_name[] = "SMPFATAL 9";

static void Init( rtems_task_argument arg )
{
  (void) arg;
}

static void consume_all_memory( void )
{
  const Memory_Information *mem;
  size_t                    i;

  mem = _Memory_Get();

  for ( i = 0; i < _Memory_Get_count( mem ); ++i ) {
    Memory_Area *area;

    area = _Memory_Get_area( mem, i );
    _Memory_Consume( area, _Memory_Get_free_size( area ) );
  }
}

static void begin_test( void )
{
  TEST_BEGIN();
  consume_all_memory();
}

RTEMS_SYSINIT_ITEM(
  begin_test,
  RTEMS_SYSINIT_PER_CPU_DATA,
  RTEMS_SYSINIT_ORDER_FIRST
);

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  if (
    source == INTERNAL_ERROR_CORE
      && !always_set_to_false
      && code == INTERNAL_ERROR_NO_MEMORY_FOR_PER_CPU_DATA
  ) {
    TEST_END();
  }
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
