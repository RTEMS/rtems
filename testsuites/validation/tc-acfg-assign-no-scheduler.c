/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValSchedulerAssignNoScheduler
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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
 * @defgroup AcfgValSchedulerAssignNoScheduler \
 *   spec:/acfg/val/scheduler-assign-no-scheduler
 *
 * @ingroup TestsuitesValidationSmpOnly1
 *
 * @brief Tests the effect of RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER.
 *
 * This test case performs the following actions:
 *
 * - Check the effect of RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER.
 *
 *   - Check that the two configured processors are present.
 *
 *   - Check that the second processor has no scheduler assigned.
 *
 *   - Check that the second processor cannot be assigned to a scheduler.
 *
 * @{
 */

/**
 * @brief Check the effect of RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER.
 */
static void AcfgValSchedulerAssignNoScheduler_Action_0( void )
{
  rtems_status_code sc;
  rtems_id          id;

  /*
   * Check that the two configured processors are present.
   */
  T_eq_u32( rtems_scheduler_get_processor_maximum(), 2 );

  /*
   * Check that the second processor has no scheduler assigned.
   */
  sc = rtems_scheduler_ident_by_processor( 1, &id );
  T_rsc( sc, RTEMS_INCORRECT_STATE );

  /*
   * Check that the second processor cannot be assigned to a scheduler.
   */
  sc = rtems_scheduler_ident_by_processor( 0, &id );
  T_rsc_success( sc );

  sc = rtems_scheduler_add_processor( id, 1 );
  T_rsc( sc, RTEMS_NOT_CONFIGURED );
}

/**
 * @fn void T_case_body_AcfgValSchedulerAssignNoScheduler( void )
 */
T_TEST_CASE( AcfgValSchedulerAssignNoScheduler )
{
  AcfgValSchedulerAssignNoScheduler_Action_0();
}

/** @} */
