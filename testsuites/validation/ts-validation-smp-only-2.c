/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesValidationSmpOnly2
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

#include "ts-config.h"

#include <rtems/test.h>

/**
 * @defgroup TestsuitesValidationSmpOnly2 \
 *   spec:/testsuites/validation-smp-only-2
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This SMP-only test suite validates the clustered scheduler
 *   configuration through an application configuration with a processor
 *   maximum of two.  The second processor has a optional scheduler assigned
 *   and fails to start.
 *
 * @{
 */

const char rtems_test_name[] = "TestsuitesValidationSmpOnly2";

bool __wrap__CPU_SMP_Start_processor( uint32_t cpu_index );

bool __wrap__CPU_SMP_Start_processor( uint32_t cpu_index )
{
  (void) cpu_index;
  return false;
}

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#include <rtems/score/scheduleredfsmp.h>

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP( a );

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP( a, TEST_SCHEDULER_A_NAME )

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN( 0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY ), \
  RTEMS_SCHEDULER_ASSIGN( 0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL )

#include "ts-default.h"

/** @} */
