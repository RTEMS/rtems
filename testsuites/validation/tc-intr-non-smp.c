/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrValIntrNonSmp
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

#include <rtems.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrValIntrNonSmp spec:/rtems/intr/val/intr-non-smp
 *
 * @ingroup TestsuitesValidationNonSmp
 *
 * @brief Tests some @ref RTEMSAPIClassicIntr interfaces.
 *
 * This test case performs the following actions:
 *
 * - Validate some interrupt lock macros.
 *
 *   - Check that RTEMS_INTERRUPT_LOCK_DECLARE() expands to white space only.
 *
 *   - Check that RTEMS_INTERRUPT_LOCK_DEFINE() expands to white space only.
 *
 *   - Check that RTEMS_INTERRUPT_LOCK_MEMBER() expands to white space only.
 *
 *   - Check that RTEMS_INTERRUPT_LOCK_REFERENCE() expands to white space only.
 *
 *   - Check that rtems_interrupt_lock_destroy() expands to white space only.
 *
 *   - Check that RTEMS_INTERRUPT_LOCK_INITIALIZER() expands to an empty
 *     structure initializer.
 *
 *   - Check that rtems_interrupt_lock_initialize() expands to white space
 *     only.
 *
 *   - Check that rtems_interrupt_lock_acquire_isr() expands to a code block
 *     which marks the second parameter as used.
 *
 *   - Check that rtems_interrupt_lock_release_isr() expands to a code block
 *     which marks the second parameter as used.
 *
 * @{
 */

/**
 * @brief Validate some interrupt lock macros.
 */
static void RtemsIntrValIntrNonSmp_Action_0( void )
{
  const char *s;

  /*
   * Check that RTEMS_INTERRUPT_LOCK_DECLARE() expands to white space only.
   */
  s = RTEMS_XSTRING( RTEMS_INTERRUPT_LOCK_DECLARE( x, y ) );
  T_true( IsWhiteSpaceOnly( s ) );

  /*
   * Check that RTEMS_INTERRUPT_LOCK_DEFINE() expands to white space only.
   */
  s = RTEMS_XSTRING( RTEMS_INTERRUPT_LOCK_DEFINE( x, y, z ) );
  T_true( IsWhiteSpaceOnly( s ) );

  /*
   * Check that RTEMS_INTERRUPT_LOCK_MEMBER() expands to white space only.
   */
  s = RTEMS_XSTRING( RTEMS_INTERRUPT_LOCK_MEMBER( x ) );
  T_true( IsWhiteSpaceOnly( s ) );

  /*
   * Check that RTEMS_INTERRUPT_LOCK_REFERENCE() expands to white space only.
   */
  s = RTEMS_XSTRING( RTEMS_INTERRUPT_LOCK_REFERENCE( x, y ) );
  T_true( IsWhiteSpaceOnly( s ) );

  /*
   * Check that rtems_interrupt_lock_destroy() expands to white space only.
   */
  s = RTEMS_XSTRING( rtems_interrupt_lock_destroy( x ) );
  T_true( IsWhiteSpaceOnly( s ) );

  /*
   * Check that RTEMS_INTERRUPT_LOCK_INITIALIZER() expands to an empty
   * structure initializer.
   */
  s = RTEMS_XSTRING( RTEMS_INTERRUPT_LOCK_INITIALIZER( x ) );
  T_true( IsEqualIgnoreWhiteSpace( s, "{}" ) );

  /*
   * Check that rtems_interrupt_lock_initialize() expands to white space only.
   */
  s = RTEMS_XSTRING( rtems_interrupt_lock_initialize( x, y ) );
  T_true( IsWhiteSpaceOnly( s ) );

  /*
   * Check that rtems_interrupt_lock_acquire_isr() expands to a code block
   * which marks the second parameter as used.
   */
  s = RTEMS_XSTRING( rtems_interrupt_lock_acquire_isr( x, y ) );
  T_true( IsEqualIgnoreWhiteSpace( s, "do{(void)y;}while(0)" ) );

  /*
   * Check that rtems_interrupt_lock_release_isr() expands to a code block
   * which marks the second parameter as used.
   */
  s = RTEMS_XSTRING( rtems_interrupt_lock_release_isr( x, y ) );
  T_true( IsEqualIgnoreWhiteSpace( s, "do{(void)y;}while(0)" ) );
}

/**
 * @fn void T_case_body_RtemsIntrValIntrNonSmp( void )
 */
T_TEST_CASE( RtemsIntrValIntrNonSmp )
{
  RtemsIntrValIntrNonSmp_Action_0();
}

/** @} */
