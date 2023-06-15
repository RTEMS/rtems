/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValFatalShutdown
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

#include <bsp/leon3.h>
#include <rtems/sysinit.h>
#include <rtems/score/smpimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ValFatalShutdown \
 *   spec:/bsp/sparc/leon3/val/fatal-shutdown
 *
 * @ingroup TestsuitesBspsFatalSparcLeon3Shutdown
 *
 * @brief Tests the leon3 BSP family shutdown procedure.
 *
 * This test case performs the following actions:
 *
 * - Check the effects of the leon3 BSP family shutdown procedure.
 *
 *   - Check that the second processor was not powered down during system
 *     initialization.
 *
 *   - Wait until the second processor is powered down.
 *
 *   - Check that the RTEMS_FATAL_SOURCE_SMP with SMP_FATAL_SHUTDOWN_RESPONSE
 *     fatal error occurred exactly once.
 *
 *   - Check that the RTEMS_FATAL_SOURCE_SMP with SMP_FATAL_SHUTDOWN_RESPONSE
 *     fatal error occurred on the second processor.
 *
 *   - Check that no dynamic fatal error extension was invoked.  This shows
 *     that the leon3 BSP family shutdown procedure called the wrapped
 *     _CPU_Fatal_halt() function of the test suite.
 *
 * @{
 */

static uint32_t mpstat_during_sysinit;

static Atomic_Uint shutdown_response_counter;

static uint32_t shutdown_response_cpu_index = UINT32_MAX;

static Atomic_Uint dynamic_fatal_extension_counter;

static rtems_status_code status;

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

static void DynamicFatalHandler(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  (void) source;
  (void) code;
  (void) always_set_to_false;
  (void) Add( &dynamic_fatal_extension_counter, 1 );
}

static void InitTestCase( void )
{
  rtems_extensions_table table = { .fatal = DynamicFatalHandler };
  irqamp                *regs;
  rtems_id               id;

  regs = LEON3_IrqCtrl_Regs;
  mpstat_during_sysinit = grlib_load_32( &regs->mpstat );
  SetFatalHandler( ShutdownFatalHandler, NULL );
  status = rtems_extension_create( OBJECT_NAME, &table, &id );
}

RTEMS_SYSINIT_ITEM(
  InitTestCase,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

/**
 * @brief Check the effects of the leon3 BSP family shutdown procedure.
 */
static void BspSparcLeon3ValFatalShutdown_Action_0( void )
{
  irqamp  *regs;
  uint32_t counter;

  regs = LEON3_IrqCtrl_Regs;

  /*
   * Check that the second processor was not powered down during system
   * initialization.
   */
  T_step_eq_u32( 0, mpstat_during_sysinit & 0x2, 0 );

  /*
   * Wait until the second processor is powered down.
   */
  while ( ( grlib_load_32( &regs->mpstat ) & 0x2 ) != 0x2U ) {
    /* Wait */
  }

  /*
   * Check that the RTEMS_FATAL_SOURCE_SMP with SMP_FATAL_SHUTDOWN_RESPONSE
   * fatal error occurred exactly once.
   */
  counter = Add( &shutdown_response_counter, 0 );
  T_step_eq_uint( 1, counter, 1 );

  /*
   * Check that the RTEMS_FATAL_SOURCE_SMP with SMP_FATAL_SHUTDOWN_RESPONSE
   * fatal error occurred on the second processor.
   */
  T_step_eq_u32( 2, shutdown_response_cpu_index, 1 );

  /*
   * Check that no dynamic fatal error extension was invoked.  This shows that
   * the leon3 BSP family shutdown procedure called the wrapped
   * _CPU_Fatal_halt() function of the test suite.
   */
  T_step_rsc_success( 3, status );
  counter = Add( &dynamic_fatal_extension_counter, 0 );
  T_step_eq_u32( 4, dynamic_fatal_extension_counter, 0 );
}

/**
 * @fn void T_case_body_BspSparcLeon3ValFatalShutdown( void )
 */
T_TEST_CASE( BspSparcLeon3ValFatalShutdown )
{
  T_plan( 5 );

  BspSparcLeon3ValFatalShutdown_Action_0();
}

/** @} */
