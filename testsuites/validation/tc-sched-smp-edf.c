/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSchedSmpEdfValEdf
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
 * @defgroup ScoreSchedSmpEdfValEdf spec:/score/sched/smp/edf/val/edf
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests for operations of the EDF SMP scheduler.
 *
 * This test case performs the following actions:
 *
 * - Validate a set affinity error case with an unsupported subset.
 *
 * @{
 */

/**
 * @brief Validate a set affinity error case with an unsupported subset.
 */
static void ScoreSchedSmpEdfValEdf_Action_0( void )
{
  if ( rtems_scheduler_get_processor_maximum() >= 3 ) {
    rtems_status_code sc;
    cpu_set_t         affinity;

    CPU_ZERO( &affinity );
    CPU_SET( 0, &affinity );
    CPU_SET( 1, &affinity );

    RemoveProcessor( SCHEDULER_B_ID, 1 );
    RemoveProcessor( SCHEDULER_C_ID, 2 );
    AddProcessor( SCHEDULER_A_ID, 1 );
    AddProcessor( SCHEDULER_B_ID, 2 );

    sc = rtems_task_set_affinity( RTEMS_SELF, sizeof( affinity), &affinity );
    T_rsc( sc, RTEMS_INVALID_NUMBER );

    RemoveProcessor( SCHEDULER_A_ID, 1 );
    RemoveProcessor( SCHEDULER_B_ID, 2 );
    AddProcessor( SCHEDULER_B_ID, 1 );
    AddProcessor( SCHEDULER_C_ID, 2 );
  }
}

/**
 * @fn void T_case_body_ScoreSchedSmpEdfValEdf( void )
 */
T_TEST_CASE( ScoreSchedSmpEdfValEdf )
{
  ScoreSchedSmpEdfValEdf_Action_0();
}

/** @} */
