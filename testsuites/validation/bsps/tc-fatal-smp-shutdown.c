/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspValFatalSmpShutdown
 */

/*
 * Copyright (C) 2021, 2024 embedded brains GmbH & Co. KG
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

#include <rtems/sysinit.h>
#include <rtems/score/smpimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup BspValFatalSmpShutdown spec:/bsp/val/fatal-smp-shutdown
 *
 * @ingroup TestsuitesBspsFatalExtension
 *
 * @brief Tests the BSP-specific fatal extension shutdown procedure.
 *
 * This test case performs the following actions:
 *
 * - Check the effects of the BSP-specific fatal extension.
 *
 *   - Wait until the second processor is idle.
 *
 *   - Check that the RTEMS_FATAL_SOURCE_SMP with SMP_FATAL_SHUTDOWN_RESPONSE
 *     fatal error occurred exactly once.
 *
 *   - Check that the RTEMS_FATAL_SOURCE_SMP with SMP_FATAL_SHUTDOWN_RESPONSE
 *     fatal error occurred on the second processor.
 *
 * @{
 */

static Atomic_Uint shutdown_response_counter;

static uint32_t shutdown_response_cpu_index = UINT32_MAX;

static unsigned int Add( Atomic_Uint *a, unsigned int b )
{
  return _Atomic_Fetch_add_uint( a, b, ATOMIC_ORDER_RELAXED );
}

static void ShutdownFatalHandler(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  T_null( arg );

  if (
    source == RTEMS_FATAL_SOURCE_SMP &&
    code == SMP_FATAL_SHUTDOWN_RESPONSE
  ) {
    (void) Add( &shutdown_response_counter, 1 );
    shutdown_response_cpu_index = rtems_scheduler_get_processor();
  }
}

static void InitBspValFatalSmpShutdown( void )
{
  SetFatalHandler( ShutdownFatalHandler, NULL );
}

RTEMS_SYSINIT_ITEM(
  InitBspValFatalSmpShutdown,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

static Atomic_Uint idle_counter;

void *__real__CPU_Thread_Idle_body( void *arg );

void *__wrap__CPU_Thread_Idle_body( void *arg );

void *__wrap__CPU_Thread_Idle_body( void *arg )
{
  (void) Add( &idle_counter, 1 );
  return __real__CPU_Thread_Idle_body( arg );
}

/**
 * @brief Check the effects of the BSP-specific fatal extension.
 */
static void BspValFatalSmpShutdown_Action_0( void )
{
  unsigned int counter;

  /*
   * Wait until the second processor is idle.
   */
  do {
    counter = _Atomic_Load_uint( &idle_counter, ATOMIC_ORDER_RELAXED );
  } while ( counter != 2U );

  /*
   * Check that the RTEMS_FATAL_SOURCE_SMP with SMP_FATAL_SHUTDOWN_RESPONSE
   * fatal error occurred exactly once.
   */
  counter = Add( &shutdown_response_counter, 0 );
  T_step_eq_uint( 0, counter, 1 );

  /*
   * Check that the RTEMS_FATAL_SOURCE_SMP with SMP_FATAL_SHUTDOWN_RESPONSE
   * fatal error occurred on the second processor.
   */
  T_step_eq_u32( 1, shutdown_response_cpu_index, 1 );
}

/**
 * @fn void T_case_body_BspValFatalSmpShutdown( void )
 */
T_TEST_CASE( BspValFatalSmpShutdown )
{
  T_plan( 2 );

  BspValFatalSmpShutdown_Action_0();
}

/** @} */
