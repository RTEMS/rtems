/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValGr712rc
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

#include <bsp.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ValGr712rc spec:/bsp/sparc/leon3/val/gr712rc
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @brief This test case collection provides validation test cases for the
 *   ``sparc/gr712rc`` BSP.
 *
 * This test case performs the following actions:
 *
 * - Validate the use of the ``-mfix-gr712rc`` compiler option.
 *
 *   - Check that the compiler built-in define ``__FIX_LEON3FT_B2BST`` is
 *     defined.
 *
 *   - Check that the compiler built-in define ``__FIX_LEON3FT_TN0018`` is
 *     defined.
 *
 *   - Check that the ``SPARC_LEON3FT_B2BST_NOP`` define expands to a ``nop``
 *     instruction.
 *
 * @{
 */

/**
 * @brief Validate the use of the ``-mfix-gr712rc`` compiler option.
 */
static void BspSparcLeon3ValGr712rc_Action_0( void )
{
  const char *s;

  /*
   * Check that the compiler built-in define ``__FIX_LEON3FT_B2BST`` is
   * defined.
   */
  #if !defined(__FIX_LEON3FT_B2BST)
  #error "__FIX_LEON3FT_B2BST is not defined"
  #endif

  /*
   * Check that the compiler built-in define ``__FIX_LEON3FT_TN0018`` is
   * defined.
   */
  #if !defined(__FIX_LEON3FT_TN0018)
  #error "__FIX_LEON3FT_TN0018 is not defined"
  #endif

  /*
   * Check that the ``SPARC_LEON3FT_B2BST_NOP`` define expands to a ``nop``
   * instruction.
   */
  s = RTEMS_XSTRING( SPARC_LEON3FT_B2BST_NOP );
  T_true( IsEqualIgnoreWhiteSpace( s, "nop" ) );
}

/**
 * @fn void T_case_body_BspSparcLeon3ValGr712rc( void )
 */
T_TEST_CASE( BspSparcLeon3ValGr712rc )
{
  BspSparcLeon3ValGr712rc_Action_0();
}

/** @} */
