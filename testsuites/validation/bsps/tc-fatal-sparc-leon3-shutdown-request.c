/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValFatalShutdownRequest
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
 * @defgroup BspSparcLeon3ValFatalShutdownRequest \
 *   spec:/bsp/sparc/leon3/val/fatal-shutdown-request
 *
 * @ingroup TestsuitesBspsFatalSparcLeon3Shutdown
 *
 * @brief Tests the leon3 BSP family SMP-specific shutdown procedure.
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
 * @{
 */

static uint32_t mpstat_during_sysinit;

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

static void InitBspSparcLeon3ValFatalShutdownRequest( void )
{
  irqamp                *regs;

  regs = LEON3_IrqCtrl_Regs;
  mpstat_during_sysinit = grlib_load_32( &regs->mpstat );
  SetFatalHandler( ShutdownFatalHandler, NULL );
}

RTEMS_SYSINIT_ITEM(
  InitBspSparcLeon3ValFatalShutdownRequest,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

/**
 * @brief Check the effects of the leon3 BSP family shutdown procedure.
 */
static void BspSparcLeon3ValFatalShutdownRequest_Action_0( void )
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
}

/**
 * @fn void T_case_body_BspSparcLeon3ValFatalShutdownRequest( void )
 */
T_TEST_CASE( BspSparcLeon3ValFatalShutdownRequest )
{
  T_plan( 3 );

  BspSparcLeon3ValFatalShutdownRequest_Action_0();
}

/** @} */
