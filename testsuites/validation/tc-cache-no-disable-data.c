/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsCacheValCacheNoDisableData
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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
#include <setjmp.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsCacheValCacheNoDisableData \
 *   spec:/rtems/cache/val/cache-no-disable-data
 *
 * @ingroup TestsuitesValidationCache
 *
 * @brief Tests some @ref RTEMSAPIClassicCache directives.
 *
 * This test case performs the following actions:
 *
 * - Call the rtems_cache_disable_data() directive.
 *
 *   - Check that the right fatal error occurred.
 *
 * - Call the rtems_cache_invalidate_entire_data() directive.
 *
 * - Call the rtems_cache_invalidate_entire_data() directive with maskable
 *   interrupts disabled.
 *
 * @{
 */

static jmp_buf fatal_before;

static Atomic_Uint fatal_counter;

static rtems_fatal_source fatal_source;

static rtems_fatal_code fatal_code;

static void FatalRecordAndJump(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  (void) arg;

  fatal_source = source;
  fatal_code = code;
  _Atomic_Fetch_add_uint( &fatal_counter, 1, ATOMIC_ORDER_RELAXED );
  _ISR_Set_level( 0 );
  longjmp( fatal_before, 1 );
}

/**
 * @brief Call the rtems_cache_disable_data() directive.
 */
static void RtemsCacheValCacheNoDisableData_Action_0( void )
{
  SetFatalHandler( FatalRecordAndJump, NULL );

  if ( setjmp( fatal_before ) == 0 ) {
    rtems_cache_disable_data();
  }

  SetFatalHandler( NULL, NULL );

  /*
   * Check that the right fatal error occurred.
   */
  T_eq_uint(
    _Atomic_Load_uint( &fatal_counter, ATOMIC_ORDER_RELAXED ),
    1
  );
  T_eq_int( fatal_source, INTERNAL_ERROR_CORE );
  T_eq_ulong(
    fatal_code,
    INTERNAL_ERROR_CANNOT_DISABLE_DATA_CACHE
  );
}

/**
 * @brief Call the rtems_cache_invalidate_entire_data() directive.
 */
static void RtemsCacheValCacheNoDisableData_Action_1( void )
{
  rtems_cache_invalidate_entire_data();
}

/**
 * @brief Call the rtems_cache_invalidate_entire_data() directive with maskable
 *   interrupts disabled.
 */
static void RtemsCacheValCacheNoDisableData_Action_2( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  rtems_cache_invalidate_entire_data();
  rtems_interrupt_local_enable(level);
}

/**
 * @fn void T_case_body_RtemsCacheValCacheNoDisableData( void )
 */
T_TEST_CASE( RtemsCacheValCacheNoDisableData )
{
  RtemsCacheValCacheNoDisableData_Action_0();
  RtemsCacheValCacheNoDisableData_Action_1();
  RtemsCacheValCacheNoDisableData_Action_2();
}

/** @} */
