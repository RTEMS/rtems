/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesValidationIoKernel
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp/bootcard.h>
#include <rtems/test-info.h>
#include <rtems/test.h>
#include <rtems/testopts.h>

#include "tr-io-kernel.h"

#include <rtems/test.h>

/**
 * @defgroup TestsuitesValidationIoKernel spec:/testsuites/validation-io-kernel
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This validation test suite contains test cases which test the kernel
 *   character input/output device provided by the BSP before the system
 *   initialization is performed.
 *
 * @{
 */

const char rtems_test_name[] = "TestsuitesValidationIoKernel";

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
  .action_count = T_ARRAY_SIZE( actions ),
  .actions = actions
};

void boot_card( const char *cmdline )
{
  uint32_t exit_code;

  (void) cmdline;

  rtems_test_begin( rtems_test_name, TEST_STATE );
  T_run_initialize( &test_config );
  RtemsIoValKernel_Run();

  if ( T_run_finalize() ) {
    rtems_test_end( rtems_test_name );
    exit_code = 0;
  } else {
    exit_code = 1;
  }

  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, exit_code );
}

static void *IdleBody( uintptr_t ignored )
{
  (void) ignored;

  while ( true ) {
    /* Do nothing */
  }

  return NULL;
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_SCHEDULER_USER

#define CONFIGURE_SCHEDULER

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES { }

#define CONFIGURE_IDLE_TASK_STORAGE_SIZE RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#define CONFIGURE_IDLE_TASK_BODY IdleBody

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/** @} */
