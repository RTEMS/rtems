/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerValNonSmp
 */

/*
 * Copyright (C) 2021, 2023 embedded brains GmbH & Co. KG
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
 * @defgroup RtemsSchedulerValNonSmp spec:/rtems/scheduler/val/non-smp
 *
 * @ingroup TestsuitesValidationNonSmp
 *
 * @brief This test case collection provides validation test cases for non-SMP
 *   requirements of the @ref RTEMSAPIClassicScheduler.
 *
 * This test case performs the following actions:
 *
 * - Assert that rtems_scheduler_get_processor() is a constant expression which
 *   evaluates to zero.
 *
 * - Check that calling rtems_scheduler_get_processor() returns zero.
 *
 * - Assert that rtems_scheduler_get_processor_maximum() is a constant
 *   expression which evaluates to zero.
 *
 * - Check that calling rtems_scheduler_get_processor_maximum() returns one.
 *
 * @{
 */

/**
 * @brief Assert that rtems_scheduler_get_processor() is a constant expression
 *   which evaluates to zero.
 */
static void RtemsSchedulerValNonSmp_Action_0( void )
{
  RTEMS_STATIC_ASSERT( rtems_scheduler_get_processor() == 0, GET_PROCESSOR );
}

/**
 * @brief Check that calling rtems_scheduler_get_processor() returns zero.
 */
static void RtemsSchedulerValNonSmp_Action_1( void )
{
  T_eq_u32( rtems_scheduler_get_processor(), 0 );
}

/**
 * @brief Assert that rtems_scheduler_get_processor_maximum() is a constant
 *   expression which evaluates to zero.
 */
static void RtemsSchedulerValNonSmp_Action_2( void )
{
  RTEMS_STATIC_ASSERT(
    rtems_scheduler_get_processor_maximum() == 1,
    GET_PROCESSOR_MAXIMUM
  );
}

/**
 * @brief Check that calling rtems_scheduler_get_processor_maximum() returns
 *   one.
 */
static void RtemsSchedulerValNonSmp_Action_3( void )
{
  T_eq_u32( rtems_scheduler_get_processor_maximum(), 1 );
}

/**
 * @fn void T_case_body_RtemsSchedulerValNonSmp( void )
 */
T_TEST_CASE( RtemsSchedulerValNonSmp )
{
  RtemsSchedulerValNonSmp_Action_0();
  RtemsSchedulerValNonSmp_Action_1();
  RtemsSchedulerValNonSmp_Action_2();
  RtemsSchedulerValNonSmp_Action_3();
}

/** @} */
