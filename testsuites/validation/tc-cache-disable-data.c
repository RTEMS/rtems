/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsCacheValCacheDisableData
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

#include <rtems/test.h>

/**
 * @defgroup RtemsCacheValCacheDisableData \
 *   spec:/rtems/cache/val/cache-disable-data
 *
 * @ingroup TestsuitesValidationCache
 *
 * @brief Tests some @ref RTEMSAPIClassicCache directives.
 *
 * This test case performs the following actions:
 *
 * - Call the rtems_cache_disable_data() and rtems_cache_enable_data()
 *   directives.
 *
 * - Call the rtems_cache_disable_data() and rtems_cache_enable_data()
 *   directives with maskable interrupts disabled.
 *
 * - Call the rtems_cache_invalidate_entire_data() directive with maskable
 *   interrupts disabled.
 *
 * @{
 */

/**
 * @brief Call the rtems_cache_disable_data() and rtems_cache_enable_data()
 *   directives.
 */
static void RtemsCacheValCacheDisableData_Action_0( void )
{
  T_step( 0 );
  rtems_cache_disable_data();
  T_step( 1 );
  rtems_cache_enable_data();
  T_step( 2 );
}

/**
 * @brief Call the rtems_cache_disable_data() and rtems_cache_enable_data()
 *   directives with maskable interrupts disabled.
 */
static void RtemsCacheValCacheDisableData_Action_1( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  T_step( 3 );
  rtems_cache_disable_data();
  T_step( 4 );
  rtems_cache_enable_data();
  T_step( 5 );
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_invalidate_entire_data() directive with maskable
 *   interrupts disabled.
 */
static void RtemsCacheValCacheDisableData_Action_2( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  T_step( 6 );
  rtems_cache_disable_data();
  T_step( 7 );
  rtems_cache_invalidate_entire_data();
  T_step( 8 );
  rtems_cache_enable_data();
  T_step( 9 );
  rtems_interrupt_local_enable(level);
}

/**
 * @fn void T_case_body_RtemsCacheValCacheDisableData( void )
 */
T_TEST_CASE( RtemsCacheValCacheDisableData )
{
  T_plan( 10 );

  RtemsCacheValCacheDisableData_Action_0();
  RtemsCacheValCacheDisableData_Action_1();
  RtemsCacheValCacheDisableData_Action_2();
}

/** @} */
