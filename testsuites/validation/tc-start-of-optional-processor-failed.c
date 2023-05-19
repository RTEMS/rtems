/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSmpValStartOfOptionalProcessorFailed
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

#include <rtems/score/percpu.h>

#include <rtems/test.h>

/**
 * @defgroup ScoreSmpValStartOfOptionalProcessorFailed \
 *   spec:/score/smp/val/start-of-optional-processor-failed
 *
 * @ingroup TestsuitesValidationSmpOnly2
 *
 * @brief Tests the state of an optional processor which failed to start.
 *
 * This test case performs the following actions:
 *
 * - The test action was carried by the system initialization.  If we execute
 *   this test case, then the failed start of an optional processor did not
 *   abort the system initialization.
 *
 *   - Check that the boot processor is online.
 *
 *   - Check that the optional processor which failed to start is not online.
 *
 * @{
 */

/**
 * @brief The test action was carried by the system initialization.  If we
 *   execute this test case, then the failed start of an optional processor did
 *   not abort the system initialization.
 */
static void ScoreSmpValStartOfOptionalProcessorFailed_Action_0( void )
{
  /* Nothing to do */

  /*
   * Check that the boot processor is online.
   */
  T_step_true(
    0,
    _Per_CPU_Is_processor_online( _Per_CPU_Get_by_index( 0 ) )
  );

  /*
   * Check that the optional processor which failed to start is not online.
   */
  T_step_false(
    1,
    _Per_CPU_Is_processor_online( _Per_CPU_Get_by_index( 1 ) )
  );
}

/**
 * @fn void T_case_body_ScoreSmpValStartOfOptionalProcessorFailed( void )
 */
T_TEST_CASE( ScoreSmpValStartOfOptionalProcessorFailed )
{
  T_plan( 2 );

  ScoreSmpValStartOfOptionalProcessorFailed_Action_0();
}

/** @} */
