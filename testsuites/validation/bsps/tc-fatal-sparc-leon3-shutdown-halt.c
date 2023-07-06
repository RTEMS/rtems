/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValFatalShutdownHalt
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

#include <rtems/sysinit.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ValFatalShutdownHalt \
 *   spec:/bsp/sparc/leon3/val/fatal-shutdown-halt
 *
 * @ingroup TestsuitesBspsFatalSparcLeon3Shutdown
 *
 * @brief Tests the leon3 BSP family shutdown procedure.
 *
 * This test case performs the following actions:
 *
 * - Check the effects of the leon3 BSP family shutdown procedure.
 *
 *   - Check that no dynamic fatal error extension was invoked.  This shows
 *     that the leon3 BSP family shutdown procedure called the wrapped
 *     _CPU_Fatal_halt() function of the test suite.
 *
 * @{
 */

static Atomic_Uint dynamic_fatal_extension_counter;

static rtems_status_code status;

static unsigned int Add( Atomic_Uint *a, unsigned int b )
{
  return _Atomic_Fetch_add_uint( a, b, ATOMIC_ORDER_RELAXED );
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

static void InitBspSparcLeon3ValFatalShutdownHalt( void )
{
  rtems_extensions_table table = { .fatal = DynamicFatalHandler };
  rtems_id               id;

  status = rtems_extension_create( OBJECT_NAME, &table, &id );
}

RTEMS_SYSINIT_ITEM(
  InitBspSparcLeon3ValFatalShutdownHalt,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

/**
 * @brief Check the effects of the leon3 BSP family shutdown procedure.
 */
static void BspSparcLeon3ValFatalShutdownHalt_Action_0( void )
{
  uint32_t counter;

  /*
   * Check that no dynamic fatal error extension was invoked.  This shows that
   * the leon3 BSP family shutdown procedure called the wrapped
   * _CPU_Fatal_halt() function of the test suite.
   */
  T_step_rsc_success( 0, status );
  counter = Add( &dynamic_fatal_extension_counter, 0 );
  T_step_eq_u32( 1, counter, 0 );
}

/**
 * @fn void T_case_body_BspSparcLeon3ValFatalShutdownHalt( void )
 */
T_TEST_CASE( BspSparcLeon3ValFatalShutdownHalt )
{
  T_plan( 2 );

  BspSparcLeon3ValFatalShutdownHalt_Action_0();
}

/** @} */
