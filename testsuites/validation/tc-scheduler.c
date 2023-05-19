/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerValScheduler
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
 * @defgroup RtemsSchedulerValScheduler spec:/rtems/scheduler/val/scheduler
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief This test case collection provides validation test cases for general
 *   requirements of the @ref RTEMSAPIClassicScheduler.
 *
 * This test case performs the following actions:
 *
 * - Validate rtems_scheduler_get_processor().
 *
 *   - Check that the values returned by rtems_scheduler_get_processor() and
 *     rtems_scheduler_get_processor() are equal.
 *
 * - Validate rtems_scheduler_get_processor_maximum().
 *
 *   - Check that the values returned by
 *     rtems_scheduler_get_processor_maximum() and
 *     rtems_scheduler_get_processor_maximum() are equal.
 *
 * @{
 */

static uint32_t GetProcessorMaximumMacro( void )
{
  return rtems_scheduler_get_processor_maximum();
}

#undef rtems_scheduler_get_processor_maximum

static uint32_t GetProcessorMaximum( void )
{
  return rtems_scheduler_get_processor_maximum();
}

static uint32_t GetProcessorMacro( void )
{
  return rtems_scheduler_get_processor();
}

#undef rtems_scheduler_get_processor

static uint32_t GetProcessor( void )
{
  return rtems_scheduler_get_processor();
}

/**
 * @brief Validate rtems_scheduler_get_processor().
 */
static void RtemsSchedulerValScheduler_Action_0( void )
{
  uint32_t cpu_index;
  uint32_t cpu_index_macro;

  cpu_index = GetProcessor();
  cpu_index_macro = GetProcessorMacro();

  /*
   * Check that the values returned by rtems_scheduler_get_processor() and
   * rtems_scheduler_get_processor() are equal.
   */
  T_step_eq_u32( 0, cpu_index, cpu_index_macro );
}

/**
 * @brief Validate rtems_scheduler_get_processor_maximum().
 */
static void RtemsSchedulerValScheduler_Action_1( void )
{
  uint32_t cpu_max;
  uint32_t cpu_max_macro;

  cpu_max = GetProcessorMaximum();
  cpu_max_macro = GetProcessorMaximumMacro();

  /*
   * Check that the values returned by rtems_scheduler_get_processor_maximum()
   * and rtems_scheduler_get_processor_maximum() are equal.
   */
  T_step_eq_u32( 1, cpu_max, cpu_max_macro );
}

/**
 * @fn void T_case_body_RtemsSchedulerValScheduler( void )
 */
T_TEST_CASE( RtemsSchedulerValScheduler )
{
  T_plan( 2 );

  RtemsSchedulerValScheduler_Action_0();
  RtemsSchedulerValScheduler_Action_1();
}

/** @} */
