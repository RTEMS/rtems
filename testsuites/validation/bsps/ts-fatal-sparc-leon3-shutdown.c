/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesBspsFatalSparcLeon3Shutdown
 */

/*
 * Copyright (C) 2021, 2022 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/test-info.h>
#include <rtems/test.h>
#include <rtems/testopts.h>
#include <rtems/score/smpimpl.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup TestsuitesBspsFatalSparcLeon3Shutdown \
 *   spec:/testsuites/bsps/fatal-sparc-leon3-shutdown
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This validation test suite provides an application configuration to
 *   perform a shutdown.
 *
 * @{
 */

const char rtems_test_name[] = "TestsuitesBspsFatalSparcLeon3Shutdown";

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

void __real__CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr code );

void __wrap__CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr code );

void __wrap__CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr code )
{
  int exit_code;

  T_register();
  exit_code = T_main( &test_config );

  if ( exit_code == 0 ) {
    rtems_test_end( rtems_test_name );
  }

#if defined(RTEMS_GCOV_COVERAGE)
  rtems_test_gcov_dump_info();
#endif
  __real__CPU_Fatal_halt( source, code );
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#if defined(RTEMS_SMP)
#define CONFIGURE_MAXIMUM_PROCESSORS 2

#include <rtems/score/scheduleredfsmp.h>

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP( a );

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP( a, TEST_SCHEDULER_A_NAME )

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN( 0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY ), \
  RTEMS_SCHEDULER_ASSIGN( 0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY )
#endif /* RTEMS_SMP */

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_IDLE_TASK_STORAGE_SIZE RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

static void *ShutdownIdleBody( uintptr_t arg )
{
#if defined(RTEMS_SMP)
  if ( rtems_scheduler_get_processor() == 0 ) {
    rtems_test_begin( rtems_test_name, TEST_STATE );
    rtems_fatal( RTEMS_FATAL_SOURCE_SMP, SMP_FATAL_SHUTDOWN );
  }

  return _CPU_Thread_Idle_body( arg );
#else
  rtems_test_begin( rtems_test_name, TEST_STATE );
  rtems_fatal( RTEMS_FATAL_SOURCE_APPLICATION, 123 );
#endif
}

#define CONFIGURE_IDLE_TASK_BODY ShutdownIdleBody

#define CONFIGURE_INITIAL_EXTENSIONS { .fatal = FatalInitialExtension }

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/** @} */
