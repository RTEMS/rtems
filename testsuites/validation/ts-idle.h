/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides a configurable validation test suite runner
 *   and application configuration for tests which should run within an idle
 *   task without a user initialization task.
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
#include <rtems/test-info.h>
#include <rtems/testopts.h>

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
  .now = T_now_tick,
  .allocate = T_memory_allocate,
  .deallocate = T_memory_deallocate,
  .action_count = T_ARRAY_SIZE( actions ),
  .actions = actions
};

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = FatalInitialExtension }

#ifndef CONFIGURE_IDLE_TASK_STORAGE_SIZE
#define CONFIGURE_IDLE_TASK_STORAGE_SIZE RTEMS_MINIMUM_STACK_SIZE
#endif

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

void *IdleBody( uintptr_t ignored )
{
  int exit_code;

  (void) ignored;

  rtems_test_begin( rtems_test_name, TEST_STATE );
  T_register();
  exit_code = T_main( &test_config );

  if ( exit_code == 0 ) {
    rtems_test_end( rtems_test_name );
  }

  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, (uint32_t) exit_code );
}

#define CONFIGURE_IDLE_TASK_BODY IdleBody

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
