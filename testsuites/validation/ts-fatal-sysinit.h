/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides a configurable validation test suite runner
 *   and application configuration for fatal error tests which occur during
 *   system initialization.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <rtems/test-info.h>
#include <rtems/testopts.h>
#include <rtems/score/atomic.h>

#include <rtems/test.h>

#include "tx-support.h"

static char buffer[ 512 ];

static const T_action actions[] = {
  T_report_hash_sha256
};

static const T_config test_config = {
  .name = rtems_test_name,
  .buf = buffer,
  .buf_size = sizeof( buffer ),
  .putchar = rtems_put_char,
  .verbosity = RTEMS_TEST_VERBOSITY,
#if defined(CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER)
  .now = T_now_clock,
#else
  .now = T_now_tick,
#endif
  .allocate = T_memory_allocate,
  .deallocate = T_memory_deallocate,
  .action_count = T_ARRAY_SIZE( actions ),
  .actions = actions
};

static Atomic_Uint counter;

static void TestSuiteFatalExtension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  rtems_fatal_code exit_code;

  (void) always_set_to_false;

  if ( source == RTEMS_FATAL_SOURCE_EXIT ) {
    return;
  }

  if ( _Atomic_Fetch_add_uint( &counter, 1, ATOMIC_ORDER_RELAXED ) != 0 ) {
    return;
  }

  T_make_runner();
  FATAL_SYSINIT_RUN( source, code );

  if ( T_run_finalize() ) {
    rtems_test_end( rtems_test_name );
    exit_code = 0;
  } else {
    exit_code = 1;
  }

#if defined(FATAL_SYSINIT_EXIT)
  FATAL_SYSINIT_EXIT( exit_code );
#else
  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, exit_code );
#endif
}

static void TestRunInitialize( void )
{
  rtems_test_begin( rtems_test_name, TEST_STATE );
  T_run_initialize( &test_config );
}

RTEMS_SYSINIT_ITEM(
  TestRunInitialize,
  RTEMS_SYSINIT_BSP_EARLY,
  RTEMS_SYSINIT_ORDER_FIRST
);

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#ifdef FATAL_SYSINIT_INITIAL_EXTENSION
#define OPTIONAL_FATAL_SYSINIT_INITIAL_EXTENSION FATAL_SYSINIT_INITIAL_EXTENSION,
#else
#define OPTIONAL_FATAL_SYSINIT_INITIAL_EXTENSION
#endif

#define CONFIGURE_INITIAL_EXTENSIONS \
  OPTIONAL_FATAL_SYSINIT_INITIAL_EXTENSION \
  { .fatal = FatalInitialExtension }, \
  { .fatal = TestSuiteFatalExtension }

#define CONFIGURE_IDLE_TASK_STORAGE_SIZE RTEMS_MINIMUM_STACK_SIZE

#if !defined(CONFIGURE_RTEMS_INIT_TASKS_TABLE)

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#if !defined(CONFIGURE_IDLE_TASK_BODY)

#define CONFIGURE_IDLE_TASK_BODY IdleBody

void *IdleBody( uintptr_t ignored )
{
  (void) ignored;

  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, 1 );
}

#endif /* CONFIGURE_IDLE_TASK_BODY */

#endif /* CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION */

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
