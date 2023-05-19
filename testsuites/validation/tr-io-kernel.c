/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIoValKernel
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

#include <rtems/bspIo.h>

#include "tr-io-kernel.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIoValKernel spec:/rtems/io/val/kernel
 *
 * @ingroup TestsuitesValidationIoKernel
 *
 * @brief Tests the functions referenced by BSP_output_char and BSP_poll_char.
 *
 * This test case performs the following actions:
 *
 * - Call the function referenced by BSP_output_char.
 *
 * - If BSP_poll_char references a function, then call it.
 *
 *   - Check that the returned value is minus one or an unsigned character
 *     value.
 *
 * @{
 */

/**
 * @brief Call the function referenced by BSP_output_char.
 */
static void RtemsIoValKernel_Action_0( void )
{
  T_report_hash_sha256_update( 'X' );
  ( *BSP_output_char )( 'X' );
  T_report_hash_sha256_update( '\n' );
  ( *BSP_output_char )( '\n' );
}

/**
 * @brief If BSP_poll_char references a function, then call it.
 */
static void RtemsIoValKernel_Action_1( void )
{
  BSP_polling_getchar_function_type poll_char;
  int c;

  poll_char = BSP_poll_char;

  if ( poll_char != NULL ) {
    c = ( *poll_char )();
  } else {
    c = -1;
  }

  /*
   * Check that the returned value is minus one or an unsigned character value.
   */
  T_step_true( 0, c == -1 || ( c & ~0xff ) == 0 );
}

void RtemsIoValKernel_Run( void )
{
  T_case_begin( "RtemsIoValKernel", &T_empty_fixture );

  T_plan( 1 );

  RtemsIoValKernel_Action_0();
  RtemsIoValKernel_Action_1();

  T_case_end();
}

/** @} */
