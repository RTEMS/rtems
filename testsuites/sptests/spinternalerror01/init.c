/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#include "tmacros.h"

#include <bsp.h>
#include <bsp/bootcard.h>

const char rtems_test_name[] = "SPINTERNALERROR 1";

#define FATAL_SOURCE 0xdeadbeef

#define FATAL_ERROR 0x600df00d

void boot_card( const char *cmdline )
{
  _Terminate( FATAL_SOURCE, FATAL_ERROR );
}

static void fatal_extension(
  Internal_errors_Source source,
  bool always_set_to_false,
  Internal_errors_t error
)
{
  TEST_BEGIN();

  if (
    source == FATAL_SOURCE
      && !always_set_to_false
      && error == FATAL_ERROR
  ) {
    TEST_END();
  }
}

static void *idle_body(uintptr_t ignored)
{
  while ( true ) {
    /* Do nothing */
  }

  return NULL;
}

static void *stack_allocate(size_t size)
{
  (void) size;
  return NULL;
}

static void stack_free(void *ptr)
{
  (void) ptr;
}

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_SCHEDULER_USER

#define CONFIGURE_SCHEDULER

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES { }

#define CONFIGURE_TASK_STACK_ALLOCATOR stack_allocate

#define CONFIGURE_TASK_STACK_DEALLOCATOR stack_free

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#define CONFIGURE_IDLE_TASK_BODY idle_body

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
