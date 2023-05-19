/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSupportValSupport
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

#include <rtems/test.h>

/**
 * @defgroup RtemsSupportValSupport spec:/rtems/support/val/support
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests some @ref RTEMSAPIClassicSupport directives.
 *
 * This test case performs the following actions:
 *
 * - Validates the characters returned by the rtems_name_to_characters()
 *   directive.
 *
 *   - Check that the first character is equal to 0x01.
 *
 *   - Check that the second character is equal to 0x02.
 *
 *   - Check that the third character is equal to 0x03.
 *
 *   - Check that the fourth character is equal to 0x04.
 *
 * @{
 */

/**
 * @brief Validates the characters returned by the rtems_name_to_characters()
 *   directive.
 */
static void RtemsSupportValSupport_Action_0( void )
{
  char c1;
  char c2;
  char c3;
  char c4;

  rtems_name_to_characters( 0x01020304, &c1, &c2, &c3, &c4 );

  /*
   * Check that the first character is equal to 0x01.
   */
  T_step_eq_char( 0, c1, 0x01 );

  /*
   * Check that the second character is equal to 0x02.
   */
  T_step_eq_char( 1, c2, 0x02 );

  /*
   * Check that the third character is equal to 0x03.
   */
  T_step_eq_char( 2, c3, 0x03 );

  /*
   * Check that the fourth character is equal to 0x04.
   */
  T_step_eq_char( 3, c4, 0x04 );
}

/**
 * @fn void T_case_body_RtemsSupportValSupport( void )
 */
T_TEST_CASE( RtemsSupportValSupport )
{
  T_plan( 4 );

  RtemsSupportValSupport_Action_0();
}

/** @} */
