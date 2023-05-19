/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskValSmp
 */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskValSmp spec:/rtems/task/val/smp
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief This test case collection provides SMP-specific validation test cases
 *   for requirements of the @ref RTEMSAPIClassicTasks.
 *
 * This test case performs the following actions:
 *
 * - Validate the home scheduler of tasks created by rtems_task_create() and
 *   constructed by rtems_task_construct() on scheduler B.
 *
 *   - Move runner from scheduler A to B.
 *
 *   - Create a task.  Check that the home scheduler of the created task is
 *     scheduler B.
 *
 *   - Construct a task.  Check that the home scheduler of the constructed task
 *     is scheduler B.
 *
 *   - Restore runner scheduler.
 *
 * @{
 */

/**
 * @brief Validate the home scheduler of tasks created by rtems_task_create()
 *   and constructed by rtems_task_construct() on scheduler B.
 */
static void RtemsTaskValSmp_Action_0( void )
{
  rtems_status_code sc;
  rtems_status_code id;

  /*
   * Move runner from scheduler A to B.
   */
  T_step_eq_u32( 0, GetSelfScheduler(), SCHEDULER_A_ID );
  SetSelfScheduler( SCHEDULER_B_ID, 1 );

  /*
   * Create a task.  Check that the home scheduler of the created task is
   * scheduler B.
   */
  sc = rtems_task_create(
    OBJECT_NAME,
    1,
    TEST_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  T_step_rsc_success( 1, sc );

  T_step_eq_u32( 2, GetScheduler( id ), SCHEDULER_B_ID );
  DeleteTask( id );

  /*
   * Construct a task.  Check that the home scheduler of the constructed task
   * is scheduler B.
   */
  sc = rtems_task_construct( &DefaultTaskConfig, &id );
  T_step_rsc_success( 3, sc );

  T_step_eq_u32( 4, GetScheduler( id ), SCHEDULER_B_ID );
  DeleteTask( id );

  /*
   * Restore runner scheduler.
   */
  RestoreRunnerScheduler();
}

/**
 * @fn void T_case_body_RtemsTaskValSmp( void )
 */
T_TEST_CASE( RtemsTaskValSmp )
{
  T_plan( 5 );

  RtemsTaskValSmp_Action_0();
}

/** @} */
