/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsObjectValObject
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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
 * @defgroup RTEMSTestCaseRtemsObjectValObject spec:/rtems/object/val/object
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 *
 * @brief Tests the rtems_build_name() macro.
 *
 * This test case performs the following actions:
 *
 * - Validate the results of rtems_build_name() for a sample set of parameters.
 *
 *   - Check that the accumulated name has the expected value.
 *
 * @{
 */

/**
 * @brief Validate the results of rtems_build_name() for a sample set of
 *   parameters.
 */
static void RtemsObjectValObject_Action_0( void )
{
  static const uint16_t chars[] = { 0, 255, 257 };
  rtems_name            accumulated_name;
  size_t                i;
  size_t                j;
  size_t                k;
  size_t                r;

  accumulated_name = 0;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( chars ); ++i ) {
    for ( j = 0; j < RTEMS_ARRAY_SIZE( chars ); ++j ) {
      for ( k = 0; k < RTEMS_ARRAY_SIZE( chars ); ++k ) {
        for ( r = 0; r < RTEMS_ARRAY_SIZE( chars ); ++r ) {
          unsigned char u1;
          unsigned char u2;
          unsigned char u3;
          unsigned char u4;
          signed char s1;
          signed char s2;
          signed char s3;
          signed char s4;
          rtems_name expected_name;
          rtems_name actual_name;

          expected_name = ( chars[ i ] % 256 ) * UINT32_C( 16777216 ) +
            ( chars[ j ] % 256 ) * UINT32_C( 65536 ) +
            ( chars[ k ] % 256 ) * UINT32_C( 256 ) +
            ( chars[ r ] % 256 );

          u1 = (unsigned char) chars[ i ];
          u2 = (unsigned char) chars[ j ];
          u3 = (unsigned char) chars[ k ];
          u4 = (unsigned char) chars[ r ];
          actual_name = rtems_build_name( u1, u2, u3, u4 );
          T_quiet_eq_u32( actual_name, expected_name )
          accumulated_name += actual_name;

          s1 = (signed char) u1;
          s2 = (signed char) u2;
          s3 = (signed char) u3;
          s4 = (signed char) u4;
          actual_name = rtems_build_name( s1, s2, s3, s4 );
          T_quiet_eq_u32( actual_name, expected_name )
          accumulated_name += actual_name;

          actual_name = rtems_build_name(
            chars[ i ],
            chars[ j ],
            chars[ k ],
            chars[ r ]
          );
          T_quiet_eq_u32( actual_name, expected_name );
          accumulated_name += actual_name;
        }
      }
    }
  }

  /*
   * Check that the accumulated name has the expected value.
   */
  T_step_eq_u32( 0, accumulated_name, 0x51515100 );
}

/**
 * @fn void T_case_body_RtemsObjectValObject( void )
 */
T_TEST_CASE( RtemsObjectValObject )
{
  T_plan( 1 );

  RtemsObjectValObject_Action_0();
}

/** @} */
