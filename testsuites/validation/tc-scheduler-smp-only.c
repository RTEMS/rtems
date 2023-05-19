/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerValSmpOnly
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

#include <limits.h>
#include <rtems.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSchedulerValSmpOnly spec:/rtems/scheduler/val/smp-only
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief This test case collection provides validation test cases for SMP-only
 *   requirements of the @ref RTEMSAPIClassicScheduler.
 *
 * This test case performs the following actions:
 *
 * - Call rtems_scheduler_get_processor() on all online processors and check
 *   the returned value.
 *
 * - Call rtems_scheduler_get_processor_maximum() and check the returned value.
 *
 *   - Check that the returned value is greater than or equal to one.
 *
 *   - Check that the returned value is less than or equal to
 *     rtems_configuration_get_maximum_processors().
 *
 * - Call rtems_scheduler_ident() for each configured scheduler.
 *
 *   - Check that the object index of scheduler A has the expected value.
 *
 *   - Check that the object index of scheduler B has the expected value.
 *
 *   - Check that the object index of scheduler C has the expected value.
 *
 *   - Check that the object index of scheduler D has the expected value.
 *
 *   - Check that processor 0 has scheduler A assigned.
 *
 *   - Check that processor 1 has scheduler B assigned.
 *
 *   - Check that scheduler B has the maximum priority of the EDF SMP
 *     scheduler.
 *
 *   - Check that processor 2 has scheduler C assigned if it is present.
 *
 *   - Check that processor 3 has scheduler C assigned if it is present.
 *
 * @{
 */

/**
 * @brief Call rtems_scheduler_get_processor() on all online processors and
 *   check the returned value.
 */
static void RtemsSchedulerValSmpOnly_Action_0( void )
{
  rtems_id            scheduler_id;
  rtems_task_priority priority;
  uint32_t            cpu_index;
  uint32_t            cpu_max;

  scheduler_id = GetSelfScheduler();
  priority = GetSelfPriority();
  cpu_max = rtems_scheduler_get_processor_maximum();
  T_step_ge_u32( 0, cpu_max, 1 );

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    rtems_status_code sc;
    rtems_id          id;

    sc = rtems_scheduler_ident_by_processor( cpu_index, &id );
    T_quiet_rsc_success( sc );

    SetSelfScheduler( id, priority );
    SetSelfAffinityOne( cpu_index );

    T_quiet_eq_u32( rtems_scheduler_get_processor(), cpu_index );

    SetSelfAffinityAll();
  }

  SetSelfScheduler( scheduler_id, priority );
}

/**
 * @brief Call rtems_scheduler_get_processor_maximum() and check the returned
 *   value.
 */
static void RtemsSchedulerValSmpOnly_Action_1( void )
{
  uint32_t cpu_max;

  cpu_max = rtems_scheduler_get_processor_maximum();

  /*
   * Check that the returned value is greater than or equal to one.
   */
  T_step_ge_u32( 1, cpu_max, 1 );

  /*
   * Check that the returned value is less than or equal to
   * rtems_configuration_get_maximum_processors().
   */
  T_step_le_u32(
    2,
    cpu_max,
    rtems_configuration_get_maximum_processors()
  );
}

/**
 * @brief Call rtems_scheduler_ident() for each configured scheduler.
 */
static void RtemsSchedulerValSmpOnly_Action_2( void )
{
  rtems_status_code   sc;
  rtems_id            id[ 4 ];
  rtems_id            id_by_cpu;
  rtems_task_priority priority;

  sc = rtems_scheduler_ident( TEST_SCHEDULER_A_NAME, &id[ 0 ]);
  T_step_rsc_success( 3, sc );

  sc = rtems_scheduler_ident( TEST_SCHEDULER_B_NAME, &id[ 1 ]);
  T_step_rsc_success( 4, sc );

  sc = rtems_scheduler_ident( TEST_SCHEDULER_C_NAME, &id[ 2 ]);
  T_step_rsc_success( 5, sc );

  sc = rtems_scheduler_ident( TEST_SCHEDULER_D_NAME, &id[ 3 ]);
  T_step_rsc_success( 6, sc );

  /*
   * Check that the object index of scheduler A has the expected value.
   */
  T_step_eq_u16( 7, rtems_object_id_get_index( id[ 0 ] ), 1 );

  /*
   * Check that the object index of scheduler B has the expected value.
   */
  T_step_eq_u16( 8, rtems_object_id_get_index( id[ 1 ] ), 2 );

  /*
   * Check that the object index of scheduler C has the expected value.
   */
  T_step_eq_u16( 9, rtems_object_id_get_index( id[ 2 ] ), 3 );

  /*
   * Check that the object index of scheduler D has the expected value.
   */
  T_step_eq_u16( 10, rtems_object_id_get_index( id[ 3 ] ), 4 );

  /*
   * Check that processor 0 has scheduler A assigned.
   */
  sc = rtems_scheduler_ident_by_processor( 0, &id_by_cpu );
  T_step_rsc_success( 11, sc );
  T_step_eq_u32( 12, id[ 0 ], id_by_cpu );

  /*
   * Check that processor 1 has scheduler B assigned.
   */
  sc = rtems_scheduler_ident_by_processor( 1, &id_by_cpu );
  T_step_rsc_success( 13, sc );
  T_step_eq_u32( 14, id[ 1 ], id_by_cpu );

  /*
   * Check that scheduler B has the maximum priority of the EDF SMP scheduler.
   */
  sc = rtems_scheduler_get_maximum_priority( id_by_cpu, &priority );
  T_step_rsc_success( 15, sc );
  T_step_eq_u32( 16, priority, (uint32_t) INT_MAX );

  /*
   * Check that processor 2 has scheduler C assigned if it is present.
   */
  sc = rtems_scheduler_ident_by_processor( 2, &id_by_cpu );
  T_step_true( 17, sc == RTEMS_INVALID_NAME || id[ 2 ] == id_by_cpu );

  /*
   * Check that processor 3 has scheduler C assigned if it is present.
   */
  sc = rtems_scheduler_ident_by_processor( 3, &id_by_cpu );
  T_step_true( 18, sc == RTEMS_INVALID_NAME || id[ 2 ] == id_by_cpu );
}

/**
 * @fn void T_case_body_RtemsSchedulerValSmpOnly( void )
 */
T_TEST_CASE( RtemsSchedulerValSmpOnly )
{
  T_plan( 19 );

  RtemsSchedulerValSmpOnly_Action_0();
  RtemsSchedulerValSmpOnly_Action_1();
  RtemsSchedulerValSmpOnly_Action_2();
}

/** @} */
